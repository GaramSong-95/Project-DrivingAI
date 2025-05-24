# Smart Surround
전방 및 측면/후방 차량의 운전 패턴을 인식하여 졸음운전, 난폭운전 등을 실시간 탐지 (ADAS시스템)

차량 전장 시스템(CAN) 연동

### Team: Overflowers

* Members
  | Name | Role |
  |----|----|
  |송가람| 팀장, 프로젝트 총괄 및 AI 비전 알고리즘 개발 |
  |황치영| CAN통신 및 하드웨어 구현 |
  |신경임| CAN통신 및 하드웨어 구현 |

## High Level Design
### 개발환경
![개발환경](https://github.com/user-attachments/assets/c4819215-83e8-4d50-b16c-ad70cfc3f378)

### 개발환경-하드웨어
![개발환경-하드웨어](https://github.com/user-attachments/assets/d63a78cd-ccc1-41d5-bd3a-568e3696c252)
### 시스템 구성도
![시스템구성도](https://github.com/user-attachments/assets/2e7f8dbf-c287-43cd-9bc6-8ee93fb2c671)

## Clone code

```shell
git clone https://github.com/GaramSong-95/Project-DrivingAI.git
```
## Prerequite

```shell
python m venv .venv
pip install boxmot
```
If you want to run the RFDETR, YOLOX or YOLOv12 examples:
```shell
cd boxmot
pip install uv
uv sync --group yolo
activate .venv/bin/activate
```

## Steps to build

Project_STM 폴더를 STM32CUBEIDE 프로그램으로 import 후 빌드 진행 보드명은 STM32F411RE

## Step to run

```shell
cd boxmot
```

<details>
  <summary>Tracking</summary>
  
  ```shell
  
$ python tracking/track.py --yolo-model rf-detr-base.pt  # bboxes only
  python tracking/track.py --yolo-model yolox_s.pt       # bboxes only
  python tracking/track.py --yolo-model yolov10n         # bboxes only
  python tracking/track.py --yolo-model yolov9s          # bboxes only
  python tracking/track.py --yolo-model yolov8n          # bboxes only
                                        yolov8n-seg      # bboxes + segmentation masks 
                                        yolov8n-pose     # bboxes + pose estimation
```

</details>

## Output

## Appendix
