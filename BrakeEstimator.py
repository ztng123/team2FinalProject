from PyQt5.QtWidgets import (
    QApplication,
    QWidget,
    QLabel,
    QVBoxLayout,
    QLineEdit,
    QPushButton,
    QMessageBox,
    QComboBox,
)
import sys
import math
import joblib
import numpy as np

# 도로 조건에 따른 평균 마찰계수 (250data.csv 기준으로 계산)
road_mu_avg = {
    "흙길": 0.5308,
    "자갈길": 0.6535,
    "콘크리트": 0.8026,
    "아스팔트": 0.7203,
    "눈길": 0.2473,
    "빙판길": 0.1427,
    "공항 활주로": 0.496,
    "고속도로": 0.7879,
    "시골 도로": 0.6618,
    "도시 도로": 0.7148,
    "산악 도로": 0.6127,
    "터널 내부": 0.6746,
}


# 마찰계수 예측
def predict_mu(road, cond, slope, speed, weight):
    key = f"{road}/{cond}".replace("/", "_")
    try:
        # 학습한 모델 불러오기
        model = joblib.load(f"group_models/model_{key}.pkl")
        road_enc = joblib.load(f"group_models/road_enc_{key}.pkl")
        cond_enc = joblib.load(f"group_models/cond_enc_{key}.pkl")
    except FileNotFoundError:
        return round(
            road_mu_avg.get(road, 0.65), 4
        )  # 해당하는 조건이 없다면 도로 조건을 기준으로

    try:
        road_enc_val = road_enc.transform([road])[0]
        cond_enc_val = cond_enc.transform([cond])[0]
    except:
        return round(road_mu_avg.get(road, 0.65), 4)

    X = np.array([[road_enc_val, cond_enc_val, slope, speed, weight]])
    return round(model.predict(X)[0], 4)


# 제동거리, 제동시간 계산 함수
def compute_braking(slope_percent, speed_kmh, mu):  # 변수 (경사도,속도,마찰계수)
    v = speed_kmh / 3.6  # 초기속도 km/h를 m/s로 변환
    g = 9.81  # 중력가속도
    theta = math.atan(slope_percent / 100.0)  # 도로 기울기 각도 변환

    distance = (v**2) / (
        2 * mu * g * math.cos(theta) + 2 * g * math.sin(theta)
    )  # 제동거리 공식
    time = v / (mu * g * math.cos(theta) + g * math.sin(theta))  # 제동시간 공식

    return round(distance, 2), round(time, 2)


# PyQT UI 설정
class BrakeCalcUI(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("제동 거리 계산기")
        self.init_ui()

    # UI 설정

    def init_ui(self):
        layout = QVBoxLayout()
        self.road_combo = QComboBox()
        self.road_combo.addItems(
            [
                "흙길",
                "자갈길",
                "콘크리트",
                "아스팔트",
                "눈길",
                "빙판길",
                "공항 활주로",
                "고속도로",
                "시골도로",
                "도시 도로",
                "산악 도로",
                "터널 내부",
            ]
        )
        # 여러 라벨 중 한가지를 선택 할 수 있는 Box 생성
        self.env_combo = QComboBox()
        # .addItems([])를 통해 선택 종류 작성
        self.env_combo.addItems(
            ["건조", "습윤", "고무 퇴적", "고무 제거"]
        )  # 선택할 수 있는 종류들

        self.slope_input = QLineEdit()
        self.speed_input = QLineEdit()
        self.weight_input = QLineEdit()

        layout.addWidget(QLabel("도로조건:"))  # QLabel() 통해 라벨 이름 작성
        layout.addWidget(self.road_combo)  # 도로조건 선택 위젯 추가
        layout.addWidget(QLabel("환경조건:"))
        layout.addWidget(self.env_combo)  # 환경조건 선택 위젯 추가
        layout.addWidget(QLabel("경사도 (%):"))
        layout.addWidget(self.slope_input)  # 경사도 입력 위젯 추가
        layout.addWidget(QLabel("차량 속도 (km/h):"))
        layout.addWidget(self.speed_input)  # 차량 속도 입력 위젯 추가
        layout.addWidget(QLabel("차량 중량 (kg):"))
        layout.addWidget(self.weight_input)  # 차량 중량 입력 위젯 추가

        self.result_label = QLabel(
            "제동거리: -, 제동시간: -, 마찰계수: -"
        )  # 라벨 이름 작성
        layout.addWidget(self.result_label)

        calc_button = QPushButton("계산")  # 버튼 이름 작성
        calc_button.clicked.connect(self.calculate)  # 버튼을 누르면 calculate 함수 실행
        layout.addWidget(calc_button)  # 버튼 추가

        self.setLayout(layout)
        self.road_combo.currentIndexChanged.connect(
            self.update_env_condition
        )  # 도로조건 변경시 환경조건 업데이트

    # 환경조건 설정
    def update_env_condition(self):
        road = self.road_combo.currentText()
        if road in ["눈길", "빙판길"]:  # 눈길, 빙판길은 환경조건 "-" 고정
            self.env_combo.clear()
            self.env_combo.addItem("-")
            self.env_combo.setEnabled(False)  # 다른 환경조건으로 변경 불가능하게 고정
        else:
            self.env_combo.setEnabled(True)  # 그 외는 다른 환경조건 자유롭게 선택 가능
            self.env_combo.clear()
            self.env_combo.addItems(["건조", "습윤", "고무 퇴적", "고무 제거"])

    def calculate(self):
        try:
            slope = float(self.slope_input.text())  # UI에서 입력한 경사도 받아옴
            speed = float(self.speed_input.text())  # UI에서 입력한 속도 받아옴
            weight = float(self.weight_input.text())  # UI에서 입력한 중량 받아옴
        except ValueError:
            QMessageBox.warning(self, "입력 오류", "숫자를 제대로 입력해주세요.")
            return
        condition1 = (
            self.road_combo.currentText()
        )  # UI 도로조건(.road_combo) 에서 선택한 텍스트 받아옴
        condition2 = (
            self.env_combo.currentText()
        )  # UI 환경조건(.env_combo) 에서 선택한 텍스트 받아옴
        mu = predict_mu(
            condition1, condition2, slope, speed, weight
        )  # predict_mu(도로조건,환경조건,경사,속도,무게) 마찰계수 계산
        distance, time = compute_braking(
            slope, speed, mu
        )  # compute_braking() 제동거리, 제동시간 계산

        self.result_label.setText(
            f"제동거리: {distance} m, 제동시간: {time} s, 마찰계수: {mu:.3f}"
        )  # 계산한 결과 UI에 표시


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BrakeCalcUI()
    window.show()
    sys.exit(app.exec_())
