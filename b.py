from PyQt5.QtWidgets import QApplication, QLabel, QWidget

app = QApplication([])
window = QWidget()
window.setWindowTitle('PyQt5 설치 확인')
window.resize(300, 200)

label = QLabel('✅ PyQt5 작동 완료!', parent=window)
label.move(80, 90)

window.show()
app.exec_()

