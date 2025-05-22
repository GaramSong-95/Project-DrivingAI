# Mikel Broström 🔥 Yolo Tracking 🧾 AGPL-3.0 license

import argparse
import cv2
import numpy as np
from functools import partial
from pathlib import Path

import torch

import can

from boxmot import TRACKERS
from boxmot.tracker_zoo import create_tracker
from boxmot.utils import ROOT, WEIGHTS, TRACKER_CONFIGS
from boxmot.utils.checks import RequirementsChecker
from tracking.detectors import (get_yolo_inferer, default_imgsz,
                                is_ultralytics_model, is_yolox_model)

checker = RequirementsChecker()
checker.check_packages(('ultralytics @ git+https://github.com/mikel-brostrom/ultralytics.git', ))  # install

from ultralytics import YOLO
from ultralytics.utils.plotting import Annotator, colors
from ultralytics.data.utils import VID_FORMATS
from ultralytics.utils.plotting import save_one_box


#전역 초기화 (단 한 번만)
can_bus = can.interface.Bus(channel='can0', bustype='socketcan')


def send_can_message(event_type):
    """
    event_type: 'reckless' or 'drowsy'
    """
    if event_type == 'reckless':
        data = [0x01]
    elif event_type == 'drowsy':
        data = [0x02]
    else:
        return  # 유효하지 않은 타입 무시

    msg = can.Message(arbitration_id=0x123, data=data, is_extended_id=False)
    try:
        can_bus.send(msg)
        print(f"CAN 메시지 전송: {event_type}")
    except can.CanError:
        print("CAN 전송 실패!")



def on_predict_start(predictor, persist=False):
    """
    Initialize trackers for object tracking during prediction.

    Args:
        predictor (object): The predictor object to initialize trackers for.
        persist (bool, optional): Whether to persist the trackers if they already exist. Defaults to False.
    """

    assert predictor.custom_args.tracking_method in TRACKERS, \
        f"'{predictor.custom_args.tracking_method}' is not supported. Supported ones are {TRACKERS}"

    tracking_config = TRACKER_CONFIGS / (predictor.custom_args.tracking_method + '.yaml')
    trackers = []
    for i in range(predictor.dataset.bs):
        tracker = create_tracker(
            predictor.custom_args.tracking_method,
            tracking_config,
            predictor.custom_args.reid_model,
            predictor.device,
            predictor.custom_args.half,
            predictor.custom_args.per_class
        )
        # motion only modeles do not have
        if hasattr(tracker, 'model'):
            tracker.model.warmup()
        trackers.append(tracker)

    predictor.trackers = trackers


@torch.no_grad()
def run(args):

    if args.imgsz is None:
        args.imgsz = default_imgsz(args.yolo_model)
    yolo = YOLO(
        args.yolo_model if is_ultralytics_model(args.yolo_model)
        else 'yolov8n.pt',
    )

    results = yolo.track(
        source=args.source,
        conf=args.conf,
        iou=args.iou,
        agnostic_nms=args.agnostic_nms,
        show=False,
        stream=True,
        device=args.device,
        show_conf=args.show_conf,
        save_txt=args.save_txt,
        show_labels=args.show_labels,
        save=args.save,
        verbose=args.verbose,
        exist_ok=args.exist_ok,
        project=args.project,
        name=args.name,
        classes=args.classes,
        imgsz=args.imgsz,
        vid_stride=args.vid_stride,
        line_width=args.line_width
    )

    yolo.add_callback('on_predict_start', partial(on_predict_start, persist=True))

    if not is_ultralytics_model(args.yolo_model):
        # replace yolov8 model
        m = get_yolo_inferer(args.yolo_model)
        yolo_model = m(model=args.yolo_model, device=yolo.predictor.device,
                       args=yolo.predictor.args)
        yolo.predictor.model = yolo_model

        # If current model is YOLOX, change the preprocess and postprocess
        if not is_ultralytics_model(args.yolo_model):
            # add callback to save image paths for further processing
            yolo.add_callback(
                "on_predict_batch_start",
                lambda p: yolo_model.update_im_paths(p)
            )
            yolo.predictor.preprocess = (
                lambda imgs: yolo_model.preprocess(im=imgs))
            yolo.predictor.postprocess = (
                lambda preds, im, im0s:
                yolo_model.postprocess(preds=preds, im=im, im0s=im0s))

    # store custom args in predictor
    yolo.predictor.custom_args = args

    track_history = {}
    reckless_flags = {}
    swerve_counts = {}
    drowsy_flags = {}
    drowsy_display_counter = {} 

    for r in results:

        img = yolo.predictor.trackers[0].plot_results(r.orig_img, args.show_trajectories)

       
        boxes = r.boxes.xyxy.cpu().numpy()  # [x1, y1, x2, y2]
        ids = r.boxes.id.cpu().numpy().astype(int) if r.boxes.id is not None else []
        cls = r.boxes.cls.cpu().numpy().astype(int)  # 클래스 번호 (0=person, 1=bicycle, 2=car 등)

        current_ids = set()

        for i, track_id in enumerate(ids):
            if cls[i] != 2:
                continue
            current_ids.add(track_id)
            x1, y1, x2, y2 = boxes[i]
            cx = (x1 + x2) / 2
            cy = (y1 + y2) / 2

            if track_id not in track_history:
                track_history[track_id] = []
            h = y2 - y1
            track_history[track_id].append((cx, cy, h))

            if len(track_history[track_id]) > 30:
                track_history[track_id] = track_history[track_id][-30:]

            frame_height, frame_width = 384, 640 

            # 1. 과속 감지 (변화량 기반)
            if len(track_history[track_id]) >= 5:
                (prev_x, prev_y, prev_h), (curr_x, curr_y, curr_h) = track_history[track_id][-2], track_history[track_id][-1]
                dx, dy = curr_x - prev_x, curr_y - prev_y
                speed = (dx**2 + dy**2)**0.5

                if curr_h < prev_h and speed > 6:
                    reckless_flags[track_id] = True
                else:
                    reckless_flags[track_id] = False

            # 2. 졸음운전 감지 (전방 차량만, 좌우 흔들림 방향 반전 기반)
            if 50 <= cy <= 700 and 100 <= cx <= 700:  # 조건문에서 x, y 범위 바꿈
                if len(track_history[track_id]) >= 10:
                    # 좌우 흔들림을 x축(가로) 기준으로 계산해야 하니까 cy값(가로) 추출
                    cxs = [pos[1] for pos in track_history[track_id][-10:]]  # pos[1] = cy (가로 좌표)

                    directions = []

                    # 방향 기록
                    for i in range(1, len(cxs)):
                        diff = cxs[i] - cxs[i - 1]
                        if abs(diff) > 1:  # 너무 미세한 움직임 무시
                            directions.append(1 if diff > 0 else -1)

                    # 연속된 방향 중복 제거
                    compressed_dirs = []
                    for d in directions:
                        if len(compressed_dirs) == 0 or compressed_dirs[-1] != d:
                            compressed_dirs.append(d)

                    # 진짜로 '좌→우→좌' 또는 '우→좌→우' 패턴인지 체크
                    if len(compressed_dirs) >= 3:
                        recent3 = compressed_dirs[-3:]
                        if recent3 == [1, -1, 1] or recent3 == [-1, 1, -1]:
                            drowsy_display_counter[track_id] = 60  # 졸음운전 표시 유지 시간




        
         # 사라진 객체의 상태 제거
        removed_ids = set(track_history.keys()) - current_ids
        for rid in removed_ids:
            track_history.pop(rid, None)
            reckless_flags.pop(rid, None)
            swerve_counts.pop(rid, None)



        # 화면에 표시
        for i, track_id in enumerate(ids):
            if cls[i] != 2:
                continue
            x1, y1, x2, y2 = boxes[i]
            cx, cy = (x1 + x2) / 2, (y1 + y2) / 2

            # 프레임마다 남은 표시 시간 줄이기
            if track_id in drowsy_display_counter:
                drowsy_display_counter[track_id] -= 1
                if drowsy_display_counter[track_id] <= 0:
                    drowsy_display_counter.pop(track_id)

            info_lines = [f"Pos: ({int(cx)}, {int(cy)})"]
            if reckless_flags.get(track_id, False):
                info_lines.append("Reckless!")
                send_can_message('reckless')
            if track_id in drowsy_display_counter:
                info_lines.append("Drowsy!")
                send_can_message('drowsy')

            for j, line in enumerate(info_lines):
                if line == "Reckless!":
                    color = (0, 0, 255)
                elif line == "Drowsy!":
                    color = (0, 255, 255)
                else:
                    color = (255, 255, 255)
                y_offset = int(y1) - 30 - (j * 20)
                y_offset = max(y_offset, 0)
                cv2.putText(img, line, (int(x1), y_offset),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

  


            # 디버깅 출력 (원한다면 제거 가능)
            for track_id in current_ids:
                if track_id in track_history:
                    cx, cy, h = track_history[track_id][-1]
                    print(f"[Frame] Track ID: {track_id}, Center: ({cx:.1f}, {cy:.1f})")

        if args.show is True:
            cv2.imshow('BoxMOT', img)     
            key = cv2.waitKey(33) & 0xFF
            if key == ord(' ') or key == ord('q'):
                break


def parse_opt():
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--yolo-model', type=Path, default=WEIGHTS / 'yolov8n',
                        help='yolo model path')
    parser.add_argument('--reid-model', type=Path, default=WEIGHTS / 'osnet_x0_25_msmt17.pt',
                        help='reid model path')
    parser.add_argument('--tracking-method', type=str, default='deepocsort',
                        help='deepocsort, botsort, strongsort, ocsort, bytetrack, imprassoc, boosttrack')
    parser.add_argument('--source', type=str, default='0',
                        help='file/dir/URL/glob, 0 for webcam')
    parser.add_argument('--imgsz', '--img', '--img-size', nargs='+', type=int, default=None,
                        help='inference size h,w')
    parser.add_argument('--conf', type=float, default=0.5,
                        help='confidence threshold')
    parser.add_argument('--iou', type=float, default=0.7,
                        help='intersection over union (IoU) threshold for NMS')
    parser.add_argument('--device', default='',
                        help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    parser.add_argument('--show', action='store_true',
                        help='display tracking video results')
    parser.add_argument('--save', action='store_true',
                        help='save video tracking results')
    # class 0 is person, 1 is bycicle, 2 is car... 79 is oven
    parser.add_argument('--classes', nargs='+', type=int,
                        help='filter by class: --classes 0, or --classes 0 2 3')
    parser.add_argument('--project', default=ROOT / 'runs' / 'track',
                        help='save results to project/name')
    parser.add_argument('--name', default='exp',
                        help='save results to project/name')
    parser.add_argument('--exist-ok', action='store_true',
                        help='existing project/name ok, do not increment')
    parser.add_argument('--half', action='store_true',
                        help='use FP16 half-precision inference')
    parser.add_argument('--vid-stride', type=int, default=1,
                        help='video frame-rate stride')
    parser.add_argument('--show-labels', action='store_false',
                        help='either show all or only bboxes')
    parser.add_argument('--show-conf', action='store_false',
                        help='hide confidences when show')
    parser.add_argument('--show-trajectories', action='store_true',
                        help='show confidences')
    parser.add_argument('--save-txt', action='store_true',
                        help='save tracking results in a txt file')
    parser.add_argument('--save-id-crops', action='store_true',
                        help='save each crop to its respective id folder')
    parser.add_argument('--line-width', default=None, type=int,
                        help='The line width of the bounding boxes. If None, it is scaled to the image size.')
    parser.add_argument('--per-class', default=False, action='store_true',
                        help='not mix up classes when tracking')
    parser.add_argument('--verbose', default=True, action='store_true',
                        help='print results per frame')
    parser.add_argument('--agnostic-nms', default=False, action='store_true',
                        help='class-agnostic NMS')

    opt = parser.parse_args()
    return opt


if __name__ == "__main__":
    opt = parse_opt()
    run(opt)
