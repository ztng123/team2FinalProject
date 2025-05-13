🚗붕붕 Team Final Project🚗
=============================
👁️EyesOn👁️\
: 운전자의 눈 감김 감지, 사람 인식을 통해
"모든 것을 보고 안전을 책임지는 차량"의 의미를 담고 있습니다.

# 🔔EyesOn 소개
RC Car에 아두오니와 센서 웹캠을 연결해서 운전 중 발생할 수 있는 **두 가지 주요 위험 상황**을 **감지**하고    
경고음이나 감속 같은 물리적인 반응과 함께 UI에서 경고 문구를 띄워주는 운전자 보조 시스템 구현이 목표 입니다.

# ⚙️기술 스택
- **Environment**

<img src="https://img.shields.io/badge/c++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/python-3776AB?style=for-the-badge&logo=python&logoColor=white">
<img src="https://img.shields.io/badge/github-181717?style=for-the-badge&logo=github&logoColor=white">
<img src="https://img.shields.io/badge/git-F05032?style=for-the-badge&logo=git&logoColor=white">
 <img src="https://img.shields.io/badge/jupyter-F37626?style=for-the-badge&logo=jupyter&logoColor=white">

- **Development**

<img src="https://img.shields.io/badge/opencv-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white"> <img src="https://img.shields.io/badge/arduino-5C3EE8?style=for-the-badge&logo=arduino&logoColor=white">
<img src="https://img.shields.io/badge/opencv-00878F?style=for-the-badge&logo=opencv&logoColor=white">
<img src="https://img.shields.io/badge/raspberrypi-A22846?style=for-the-badge&logo=raspberrypi&logoColor=white">
<img src="https://img.shields.io/badge/onnx-005CED?style=for-the-badge&logo=onnx&logoColor=white">
<img src="https://img.shields.io/badge/mariadb-003545?style=for-the-badge&logo=mariadb&logoColor=white">
<img src="https://img.shields.io/badge/mysql-003B57?style=for-the-badge&logo=mysql&logoColor=white">

- **Communication**

<img src="https://img.shields.io/badge/Notion-000000?style=for-the-badge&logo=notion&logoColor=white"> <img src="https://img.shields.io/badge/Kakaotalk-FFCD00?style=for-the-badge&logo=kakaotalk&logoColor=white"> <img src="https://img.shields.io/badge/Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white"> <img src="https://img.shields.io/badge/Slack-4A154B?style=for-the-badge&logo=slack&logoColor=white">

# 🔎Flow Chart
![image](https://github.com/user-attachments/assets/c40e1aed-893c-4555-a3af-6d7161cf2b55)

# Architecture
![붕붕팀_최종프로젝트-008](https://github.com/user-attachments/assets/01c0238b-b310-4f59-998a-c3fa8fd3d2e3)

# 🔧주요 기능

- 얼굴 인식 및 눈 감김 시간 분석 (OpenCV)
- 눈 감은 상태가 3초 이상 지속되면 경고 발생
- LED 깜빡임 + 부저 작동 + 모터 감속 제어 (아두이노 연동)
- 실시간 로그 기록 및 상태 표시

# ☁시현 
- 졸음 감지 및 부저 알림 + 객체 인식 후 브레이크
![시현-_2_](https://github.com/user-attachments/assets/6cf50057-3547-4a51-94cf-d017520c4083)

# ✒️회고
|직분|이름|회고 내용|
|--|--|--|
|팀장|류경미|상상하던 아이디어를 실물로 구현했을 때의 성취감이 가장 기억에 남습니다. 6개월동안 스마트 팩토리 7기 활동을 하며 배운 기술들을 종합적으로 활용해 하나의 시스템을 완성했다는 점에서 큰 뿌듯함을 느꼈습니다. 그리고 문제를 해결하면 새로운 문제가 계속 생겨 힘들었지만, 팀원들과 함께 의견을 나누고 도움을 받으며 끝까지 완성할 수 있어서 더욱 의미가 있었던 프로젝트였던 것 같습니다. 아쉬운 점은 전체 일정이 촉박하여 기능을 구현했지만 완성도가 낮아 아쉽습니다. 특히, 팀장으로서 일정 관리와 역할 분배를 더 체계적으로 하지 못한 것이 가장 아쉬움으로 남습니다. 이 프로젝트에서 그치지 않고 하드웨어 외관의 퀄리티를 높이고, 라즈베리파이 카메라 기능을 추가하여 사람 인식률을 보다 정확하게 개선하는 방향을 고민해보고 싶습니다.
|팀원|김성규|기본적인 기능 구현은 이루어졌지만, 아쉬운 점도 적지 않았습니다. 지도 스타일이 단조롭고, 밝은/어두운 테마나 반응형 UI 지원이 부족해 다양한 사용자 환경을 고려하지 못한 부분이 있었습니다. 또한, 상태 관리 구조도 복잡하게 얽혀 있어 코드 유지보수 측면에서 개선이 필요하다고 느꼈습니다.|
|팀원|노수현|UI, 아두이노, 라즈베리 파이, db가 하나의 시스템으로 연동될 수 있다는걸 알았고, 주제를 중간에 바꾸게 되었는데 오히려 도전의식이 더 느껴져서 긍정적인 영향을 끼쳤던 것 같습니다. 아쉬운점은 인식률 부분인데 이후에 학습량과 데이터를 다양하게 해 캠과 센서의 인식률을 높이고 결과물을 만들어 가야겠다.|
|팀원|이혜린|지도와 히스토그램을 활용해 직관적인 UI를 구현하고, 검색 방식을 고민하며 사용자 경험에 집중할 수 있어 좋았습니다. QGIS 데이터를 활용해 지도 시각화를 구현하여 동시에 SQL 연동이 어려워 CSV 파일만 사용한 점은 아쉬웠지만, 추후 기능 확장을 통해 보완하고싶습니다.|
|팀원|신예지|UI, 하드웨어, 센서를 하나로 연동해본 경험이 좋았지만 속도 측정이 어려워 제동 거리 계산을 못한 점은 아쉬웠습니다.추후에 휠 회전수를 활용해 속도를 구하고 제동 거리 계산해서 보완하고 싶습니다.|

# 👥 팀원 소개
|직분|이름|업무|Git 주소|
|--|--|--|--|
|팀장|류경미|하드웨어(RC카 제작, Arduino Uno), PPT, 발표|[![GitHub](https://img.shields.io/badge/GitHub-BunnyByee-black?logo=github)](https://github.com/ggmmi1)|
|팀원|김성규|UI(지도구현, 시뮬레이션, 문구 팝업, TTS)|[![GitHub](https://img.shields.io/badge/GitHub-hhhhhhyun-black?logo=github)](https://github.com/Seonggyu-art)|
|팀원|노수현|소프트웨어(AI 학습, OpenCV, MySQL) PPT 보조, README 작성|[![GitHub](https://img.shields.io/badge/GitHub-hhhhhhyun-black?logo=github)](https://github.com/hhhhhhyun)|
|팀원|이혜린|웹 페이지(차량번호 조회, 데이터 시각, 졸음운전 구역 지도 구현, 데이터 조사)|[![GitHub](https://img.shields.io/badge/GitHub-hhhhhhyun-black?logo=github)](https://github.com/hyerin00)|
|팀원|신예지|소프트웨어(AI 학습, OpenCV, MySQL, UI 연동), 하드웨어(Raspberry Pi)|[![GitHub](https://img.shields.io/badge/GitHub-hhhhhhyun-black?logo=github)](https://github.com/hyerin00)|

# 💬커밋 메시지 타입 목록

| 타입       | 설명                                                 |
| ---------- | ---------------------------------------------------- |
| `feat`     | 새로운 기능 추가                                     |
| `fix`      | 버그 수정                                            |
| `docs`     | 문서 수정 (README, 주석 등)                          |
| `style`    | 기능 변화 없는 코드 정리 (예: 들여쓰기, 세미콜론 등) |
| `refactor` | 리팩토링 (동작은 같지만 구조 개선)                   |
| `test`     | 테스트 코드 작성 또는 수정                           |

---

### 📝 커밋 메시지 예시

- [feat] 제동 수식 계산 함수 구현
- [fix] 시뮬레이션 결과 저장 오류 수정
- [docs] README에 개발 환경 추가
- [style] 변수명 통일 및 들여쓰기 정리
- [refactor] 제동 거리 계산 모듈 분리
- [test] 제동거리 수식 단위 테스트 추가
