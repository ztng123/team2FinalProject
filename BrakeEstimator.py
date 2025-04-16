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

model = joblib.load("frictionl.pkl")
road_encoder = joblib.load("road.pkl")
cond_encoder = joblib.load("cond.pkl")


def predict_mu(road, cond, slope, speed, weight):
    road_encoded = road_encoder.transform([road])[0]
    cond_encoded = cond_encoder.transform([cond])[0]

    X_input = np.array([[road_encoded, cond_encoded, slope, speed, weight]])
    mu = model.predict(X_input)[0]
    return round(mu, 4)


# 위에서 정의한 함수들을 import했다고 가정
def get_friction_coefficient(env_condition):
    mapping = {"건조": 0.8, "습윤": 0.6, "결빙": 0.15, "눈길": 0.25}
    return mapping.get(env_condition, 0.6)  # 기본값: 습윤


# 마찰계수는 get_friction_coefficient 함수로 값 계산
# mu = get_friction_coefficient(condition)


def compute_braking(slope_percent, speed_kmh, mu):  # 변수 (경사도,속도,마찰계수)
    v = speed_kmh / 3.6  # 초기속도 km/h를 m/s로 변환
    g = 9.81  # 중력가속도
    theta = math.atan(slope_percent / 100.0)  # 도로 기울기 각도 변환

    distance = (v**2) / (
        2 * mu * g * math.cos(theta) + 2 * g * math.sin(theta)
    )  # 제동거리 공식
    time = v / (mu * g * math.cos(theta) + g * math.sin(theta))  # 제동시간 공식

    return round(distance, 2), round(time, 2)


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
            ["건조", "습윤", "결빙", "눈길"]
        )  # 선택할 수 있는 종류들

        self.slope_input = QLineEdit()
        self.speed_input = QLineEdit()
        self.weight_input = QLineEdit()

        layout.addWidget(QLabel("환경조건:"))  # QLabel() 통해 라벨 이름 작성
        layout.addWidget(self.env_combo)  # 환경조건 선택
        layout.addWidget(QLabel("경사도 (%):"))
        layout.addWidget(self.slope_input)  # 경사도 입력
        layout.addWidget(QLabel("차량 속도 (km/h):"))
        layout.addWidget(self.speed_input)  # 차량 속도 입력
        layout.addWidget(QLabel("차량 중량 (kg):"))
        layout.addWidget(self.weight_input)  # 차량 중량 입력

        self.result_label = QLabel("제동거리: -, 제동시간: -")  # 라벨 이름 작성
        layout.addWidget(self.result_label)

        calc_button = QPushButton("계산")  # 버튼 이름 작성
        calc_button.clicked.connect(self.calculate)  # 버튼을 누르면 calculate 함수 실행
        layout.addWidget(calc_button)  # 버튼 추가

        self.setLayout(layout)

    def calculate(self):
        try:
            slope = float(self.slope_input.text())  # UI에서 입력한 경사도 받아옴
            speed = float(self.speed_input.text())  # UI에서 입력한 속도 받아옴
            weight = float(self.weight_input.text())  # 현재 사용 X
        except ValueError:
            QMessageBox.warning(self, "입력 오류", "숫자를 제대로 입력해주세요.")
            return
        condition1 = self.road_combo.currentText()
        condition2 = (
            self.env_combo.currentText()
        )  # UI(.env_combo) 에서 선택한 Text 받아옴
        mu = predict_mu(condition1, condition2, slope, speed, weight)
        mu = get_friction_coefficient(
            condition
        )  # 그 Text 를 get_friction_coefficient()에 대입
        distance, time = compute_braking(slope, speed, mu)  # compute_braking() 계산

        self.result_label.setText(
            f"제동거리: {distance} m, 제동시간: {time} s, 마찰계수: {mu}"
        )  # 계산한 결과 UI에 표시


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BrakeCalcUI()
    window.show()
    sys.exit(app.exec_())
