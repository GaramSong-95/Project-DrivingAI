# Smart Surround
### **🏅 Intel Edge AI Academy 최종프로젝트 경진대회 – 최우수상 수상작**

도로 상에서 주변 차량의 운전 행태(과속, 졸음운전 등)를 인식 하고, 위험한 차량을 사전에 식별하여 사고를 예방할 수 있는 시스템을 구현

차량 전장 시스템(CAN통신) 연동

## 프로젝트 목표

* 주변 차량의 운전 행태 인식 (과속, 졸음운전 등)

* 딥러닝 기반 객체 추적 및 분석 알고리즘 구현

* CAN 통신을 통해 외부 시스템과 연동

* 실시간 위험 차량 탐지 시스템 구축

## 주제 선정 이유

* 사고를 사전에 감지하고 예방할 수 있는 AI 시스템의 필요성

* 향후 자율주행 기능과의 연계 가능성, 보다 견고한 자율주행 시스템 구축

* 차량 전장 시스템의 핵심인 CAN 통신을 실무적으로 경험하고자 함

### Team: Overflowers

* Members
  | Name | Role |
  |----|----|
  |송가람| 팀장, 프로젝트 총괄 및 AI 비전 알고리즘 개발 |
  |황치영| CAN통신 및 하드웨어 구현 |
  |신경임| CAN통신 및 하드웨어 구현 |

## High Level Design
### 개발환경
![개발환경2](https://github.com/user-attachments/assets/cba5cb68-62e0-474d-a945-9973c3bad590)


### 개발환경-하드웨어
![하드웨어](https://github.com/user-attachments/assets/3dfd577f-774a-4eb3-bf5c-2f20fae1c513)

### 시스템 구성도
![구성도2](https://github.com/user-attachments/assets/5cc5bea1-6f6e-4ae7-8000-a64c13109e2c)


### 흐름도
![흐름도](https://github.com/user-attachments/assets/144aba16-51e0-4736-b17e-dca0adff93c5)

## Clone code

```shell
git clone https://github.com/GaramSong-95/Project-DrivingAI.git
```
## Prerequite

```shell
sudo apt update
sudo apt install can-utils
python m venv .venv
pip install boxmot
pip install python-can
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
$ sudo ip link set can0 down  # 혹시 켜져있다면 먼저 끔
$ sudo ip link set can0 type can bitrate 500000
$ sudo ip link set can0 up
$ cd boxmot
$ python tracking/track.py --yolo-model yolov8n --source 영상경로 --device 0 --veiw --save 저장경로
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

<details>
  <summary>Tracking methods</summary>

  ```shell
$ python tracking/track.py --tracking-method deepocsort
                                             strongsort
                                             ocsort
                                             bytetrack
                                             botsort
                                             boosttrack
```

</details>

<details>
  <summary>Tracking sources</summary>
  
tracking can be run on most video formats
  ```shell
$ python tracking/track.py --source 0                               # webcam
                                    img.jpg                         # image
                                    vid.mp4                         # video
                                    path/                           # directory
                                    path/*.jpg                      # glob
                                    'https://youtu.be/Zgi9g1ksQHc'  # YouTube
                                    'rtsp://example.com/media.mp4'  # RTSP, RTMP, HTTP stream
```

</details>

<details>
  <summary>select ReID model</summary>
  
Some tracking methods combine appearance description and motion in the process of tracking. For those which use appearance, you can choose a ReID model based on your needs from this ReID model zoo. These model can be further optimized for you needs by the reid_export.py script
  ```shell
$ python tracking/track.py --source 0 --reid-model lmbn_n_cuhk03_d.pt               # lightweight
                                                   osnet_x0_25_market1501.pt
                                                   mobilenetv2_x1_4_msmt17.engine
                                                   resnet50_msmt17.onnx
                                                   osnet_x1_0_msmt17.pt
                                                   clip_market1501.pt               # heavy
                                                   clip_vehicleid.pt
                                                   ...
```

</details>

<details>
  <summary>Filter tracked classes</summary>
  
By default the tracker tracks all MS COCO classes.
If you want to track a subset of the classes that you model predicts, add their corresponding index after the classes flag,
  ```shell
python tracking/track.py --source 0 --yolo-model yolov8s.pt --classes 16 17  # COCO yolov8 model. Track cats and dogs, only
```
Here is a list of all the possible objects that a Yolov8 model trained on MS COCO can detect. Notice that the indexing for the classes in this repo starts at zero

</details>

<details>
  <summary>Evaluation</summary>
  
Evaluate a combination of detector, tracking method and ReID model on standard MOT dataset or you custom one by
  ```shell
$ python3 tracking/val.py --yolo-model yolov8n.pt --reid-model osnet_x0_25_msmt17.pt --tracking-method deepocsort --verbose --source ./assets/MOT17-mini/train
$ python3 tracking/val.py --yolo-model yolov8n.pt --reid-model osnet_x0_25_msmt17.pt --tracking-method ocsort     --verbose --source ./tracking/val_utils/MOT17/train
```
add --gsi to your command for postprocessing the MOT results by gaussian smoothed interpolation. Detections and embeddings are stored for the selected YOLO and ReID model respectively. They can then be loaded into any tracking algorithm. Avoiding the overhead of repeatedly generating this data.

</details>

<details>
  <summary>Evolution</summary>
  
We use a fast and elitist multiobjective genetic algorithm for tracker hyperparameter tuning. By default the objectives are: HOTA, MOTA, IDF1. Run it by
  ```shell
# saves dets and embs under ./runs/dets_n_embs separately for each selected yolo and reid model
$ python tracking/generate_dets_n_embs.py --source ./assets/MOT17-mini/train --yolo-model yolov8n.pt yolov8s.pt --reid-model weights/osnet_x0_25_msmt17.pt
# evolve parameters for specified tracking method using the selected detections and embeddings generated in the previous step
$ python tracking/evolve.py --dets yolov8n --embs osnet_x0_25_msmt17 --n-trials 9 --tracking-method botsort --source ./assets/MOT17-mini/train
```
The set of hyperparameters leading to the best HOTA result are written to the tracker's config file.

</details>

<details>
  <summary>Export</summary>
  
We support ReID model export to ONNX, OpenVINO, TorchScript and TensorRT
  ```shell
# export to ONNX
$ python3 boxmot/appearance/reid_export.py --include onnx --device cpu
# export to OpenVINO
$ python3 boxmot/appearance/reid_export.py --include openvino --device cpu
# export to TensorRT with dynamic input
$ python3 boxmot/appearance/reid_export.py --include engine --device 0 --dynamic
```

</details>

## Output

![보드사진](https://github.com/user-attachments/assets/72f5e2cc-a8dc-40fc-88d8-2f6280a11a10)
![차량사진 jpg](https://github.com/user-attachments/assets/2781f0cc-47c7-48df-8aa3-a2aa0787fdd1)
![차사진](https://github.com/user-attachments/assets/9281e447-e93f-401b-b8ec-ef2724b98659)

## ⚙️ 시스템 동작 방식
본 시스템은 영상 기반 객체 인식 및 추적 → 행태 분석 → 위험 감지 → CAN 통신 경고 전송의 순서로 동작합니다.

### 1. 영상 입력 및 객체 검출
* YOLOv8을 활용해 매 프레임에서 차량 객체를 검출합니다.

* 차량 객체의 위치, 크기, ID 정보를 받아옵니다.

### 2. 객체 추적
* BoxMOT 알고리즘으로 차량을 고유 ID 기반으로 지속적으로 추적합니다.

* 추적된 객체의 중심 좌표 및 박스 크기를 프레임별로 저장합니다.

### 3. 운전 행태 분석
과속 판단:

* 프레임 간 중심 좌표 간 거리로 상대 속도 추정

* 박스 크기(높이)가 작아질 때(멀어짐) + 속도가 임계값 이상일 때만 과속으로 판단

졸음운전 판단:

* 중심 좌표의 좌→우→좌 또는 우→좌→우로의 반복적인 흔들림이 감지될 때 졸음운전으로 판단

* 민감도를 낮춰 정상적인 차선 변경과 구분되도록 조절

* 옆 차선의 차량들은 오탐확률이 높아 픽셀을 제한하여 같은 차선의 차량만을 판단

### 4. 위험 차량 판단 및 CAN 전송
* 과속 or 졸음운전으로 판단된 차량에 대해 위험 차량으로 판단

* 위험 감지 시, **CAN 통신을 통해 보드(STM32F411RE)**로 경고 신호 전송

* 위험 ID, 상태 정보 등을 전송 가능

## 프로젝트 결과

* 차량 주변 객체 추적 및 운전 행태 인식 구현
* 상대속도 기반 과속 판단 알고리즘 구현
* 차량의 움직임 기반 졸음 운전 알고리즘 구현
* CAN 통신 연동 완료

### 기대효과

* 사고 예방을 위한 능동적 안전 시스템 실현

* 자율주행 시스템의 보조 판단 기능 강화

* 후방/측방도 추가하여 위험탐지 확장

## 시행착오 및 해결방안

  | 시행착오 | 원인 | 해결방안 |
  |----|----|----|
  |속도가 느린 차량도 위험운전으로 판단|Y축 이동만으로 판단하였음|상대적으로 멀이지는 차량만 체크|
  |차선 인식 모델 통합 어려움| 기존 시스템은 바운딩 박스 기반, 차선 인식은 세그멘테이션 혹은 폴리라인 방식|객체의 좌표값과 바운딩 박스만을 활용|
  |좌회전 및 우회전 시 졸음운전으로 감지|좌표 흔들릴 시 감지|좌우 방향 번갈아 흔들렸을 경우에만 감지|
  |CAN통신 송수신 문제|통신속도가 맞지 않았음|STM32보드의 프리스케일러를 조정하여 통신속도 500kbps로 맞춤|
  |JetsonNano에 boxmot 탑재 불가|Jet pack 4.6.3에서 지원하는 pytorch는 파이썬3.6 환경에서 작동되지만 yolov8은 파이썬3.7이상의 환경에서 작동되어 상충됨|PC에 탑재|




## Appendix

* boxmot
https://github.com/mikel-brostrom/boxmot?tab=readme-ov-file#rfdetr--yolox--yolov12-examples
