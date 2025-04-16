import sys
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QLineEdit,
    QPushButton, QVBoxLayout, QHBoxLayout, QMessageBox
)

class FrictionCalculator(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("마찰계수 계산기")
        self.init_ui()

    def init_ui(self):
        # 라벨과 입력창
        self.label_speed = QLabel("속도 (m/s):")
        self.input_speed = QLineEdit()

        self.label_distance = QLabel("제동 거리 (m):")
        self.input_distance = QLineEdit()

        self.result_label = QLabel("결과가 여기에 표시됩니다.")

        # 계산 버튼
        self.calc_button = QPushButton("계산하기")
        self.calc_button.clicked.connect(self.calculate_mu)

        # 레이아웃 구성
        layout = QVBoxLayout()

        row1 = QHBoxLayout()
        row1.addWidget(self.label_speed)
        row1.addWidget(self.input_speed)

        row2 = QHBoxLayout()
        row2.addWidget(self.label_distance)
        row2.addWidget(self.input_distance)

        layout.addLayout(row1)
        layout.addLayout(row2)
        layout.addWidget(self.calc_button)
        layout.addWidget(self.result_label)

        self.setLayout(layout)

    def calculate_mu(self):
        try:
            v = float(self.input_speed.text())
            d = float(self.input_distance.text())

            if d == 0:
                raise ValueError("제동 거리는 0이 될 수 없습니다.")

            g = 9.81
            mu = (v ** 2) / (2 * g * d)
            self.result_label.setText(f"계산된 마찰계수 μ: {mu:.4f}")
        except ValueError as e:
            QMessageBox.critical(self, "입력 오류", str(e))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = FrictionCalculator()
    window.show()
    sys.exit(app.exec_())


