import sys
from dbg import *
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLineEdit, QCompleter
from PyQt5.QtCore import Qt, pyqtSlot
import sqlite3


class SqliteHandler:
    Con: sqlite3.Connection
    Curs: sqlite3.Cursor

    def __init__(self, path: str = DB_PATH):
        try:
            self.Con = sqlite3.connect(path)
            self.Curs = self.Con.cursor()
            self.Curs.row_factory = sqlite3.Row
        except sqlite3.Error as e:
            print(f"Couldn't connect to database... {e}")

    def insert(self, against: str, outcome: str):
        insertion: str = "INSERT INTO WinRate (againstName, outcome) VALUES (?, ?);"
        self.Curs.execute(insertion, tuple([against, outcome]))
        self.Con.commit()

    def getNameCompleteList(self):
        completer_list_query: str = "SELECT `againstName` FROM WinRate"
        completer_query_result = self.Curs.execute(completer_list_query)
        completer_query_result = completer_query_result.fetchall()
        self.Con.commit()
        return_list = list()
        for key in completer_query_result:
            if not key["againstName"] in return_list:
                return_list.append(key["againstName"])

        return return_list

    def __del__(self):
        try:
            self.Con.commit()
            self.Con.close()
        except sqlite3.Error as e:
            print(f"Couldn't close DB {e}")


class MainWindow(QWidget):
    def __init__(self, app: QApplication):
        super().__init__()
        self.setWindowTitle("PilleFightInterface")
        self.setGeometry(10, 10, 300, 200)

        self.SQLite = SqliteHandler()
        completer_list = self.SQLite.getNameCompleteList()
        dbg(completer_list)

        self.username = QLineEdit("Username....", self)
        self.username.setFixedWidth(100)
        self.username.move(0, 10)
        self.username.setCompleter(QCompleter(completer_list, self.username))
        self.fight_state = QLineEdit("won", self) # @TODO: Validator. Just accept won/lost(or any other value set in cfg)
        self.fight_state.setFixedWidth(100)
        self.fight_state.setCompleter(QCompleter(["won", "lost"], self.fight_state))
        self.fight_state.move(0,40)

        insert_button = QPushButton("Register", self)
        insert_button.setFixedWidth(80)
        insert_button.move(0, 80)
        insert_button.clicked.connect(self.on_submitToDatabaseClick)

        self.app = app
        """
        self.Layout = QVBoxLayout()
        self.Layout.addWidget(self.username)
        self.Layout.addWidget(self.fight_state)
        self.Layout.addWidget(insert_button)
        self.setLayout(self.Layout)
        """
        # self.SQLite.insert("testuser", "won")

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            dbg("Escape pressed, exiting...")
            self.app.exit()
        else:
            dbg(f"irrelevant keypress gets passed through with key {event.key()}")
            super().keyPressEvent(event)

    @pyqtSlot()
    def on_submitToDatabaseClick(self):
        user = self.username.text()
        state = self.fight_state.text()

        self.SQLite.insert(user, state)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow(app)
    window.show()
    app.exec_()