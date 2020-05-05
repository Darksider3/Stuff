import typing
import sys
from dbg import *
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLineEdit, QCompleter
from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtGui import QValidator
from sqliteHandler import *


class QFightStateValidator(QValidator):
    def __init__(self):
        super().__init__()

    def validate(self, input_str: str, position: int) -> typing.Tuple['QValidator.State', str, int]:
        if input_str not in WIN_STR and input_str not in LOST_STR:
            return QValidator.Invalid, input_str, position
        else:
            return QValidator.Acceptable, input_str, position

    def fixup(self, a0: str) -> str:
        return ""


class MainWindow(QWidget):
    def __init__(self, qt_app: QApplication):
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
        self.fight_state = QLineEdit("won", self)
        self.fight_state.setValidator(QFightStateValidator())
        # @TODO: Validator. Just accept won/lost(or any other value set in cfg)
        self.fight_state.setFixedWidth(100)
        self.fight_state.setCompleter(QCompleter(["won", "lost"], self.fight_state))
        self.fight_state.move(0,40)

        insert_button = QPushButton("Register", self)
        insert_button.setFixedWidth(80)
        insert_button.move(0, 80)
        insert_button.clicked.connect(self.on_submitToDatabaseClick)

        self.app = qt_app
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
        dbg(f"inserting user '{user}' with fightstate '{state}'")
        self.SQLite.insert(user, state)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow(app)
    window.show()
    app.exec_()