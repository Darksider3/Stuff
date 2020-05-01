import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QLineEdit
from PyQt5.QtCore import Qt

import SQL


class MainWindow(QWidget):
    def __init__(self, app: QApplication):
        super().__init__()
        self.app = app
        self.Layout = QVBoxLayout()
        self.Layout.addWidget(QPushButton("Push"))
        self.setLayout(self.Layout)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            print("Escape press, exiting...")
            self.app.exit()


if __name__ == '__main__':
    app = QApplication(sys.argv)

    window = MainWindow(app)
    window.show()
    app.exec_()