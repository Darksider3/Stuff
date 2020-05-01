import sys
from config import *
from dbg import *
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QLineEdit
from PyQt5.QtCore import Qt
import sqlite3

class sqliteHandler():
    Con: sqlite3.Connection
    Curs: sqlite3.Cursor

    def __init__(self, path: str=DB_PATH):
        try:
            self.Con = sqlite3.connect(path)
            self.Curs = self.Con.cursor()
        except sqlite3.Error as e:
            print("Couldn't connect to database...")

    def insert(self, against: str, outcome: str):
        insertion: str = "INSERT INTO WinRate (againstName, outcome) VALUES (?, ?);"
        self.Curs.execute(insertion, tuple([against, outcome]))
        self.Con.commit()

    def __del__(self):
        try:
            self.Con.commit()
            self.Con.close()
        except sqlite3.Error as e:
            print("Couldn't close DB")

class MainWindow(QWidget):
    def __init__(self, app: QApplication):
        super().__init__()
        self.app = app
        self.Layout = QVBoxLayout()
        self.Layout.addWidget(QPushButton("Push"))
        self.setLayout(self.Layout)
        self.SQLite = sqliteHandler()
        #self.SQLite.insert("testuser", "won")

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            dbg("Escape pressed, exiting...")
            self.app.exit()
        else:
            dbg(f"irrelevant keypress gets passed through with key {event.key()}")
            super().keyPressEvent(event)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow(app)
    window.show()
    app.exec_()