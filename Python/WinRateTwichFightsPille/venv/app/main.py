import sys
from config import *
from dbg import *
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QLineEdit
from PyQt5.QtCore import Qt



class MainWindow(QWidget):
    def __init__(self, app: QApplication):
        super().__init__()
        self.app = app
        self.Layout = QVBoxLayout()
        self.Layout.addWidget(QPushButton("Push"))
        self.setLayout(self.Layout)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            dbg("Escape press, exiting...")
            self.app.exit()
        else:
            dbg(f"irrelevant keypress gets passed through with key {event.key()}")
            super().keyPressEvent(event)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    dbg("Yo")
    window = MainWindow(app)
    window.show()
    app.exec_()