from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtWidgets import QWidget, QApplication, QLineEdit, QCompleter, QPushButton

import util
from config import USERNAME_PLACEHOLDER
from dbg import dbg
from stats_window import StatisticsWindow
from fightstatevalidator import QFightStateValidator
from sqliteHandler import SqliteHandler


class MainWindow(QWidget):
    def __init__(self, qt_app: QApplication):
        super().__init__()
        self.qt_app = qt_app

        self.setWindowTitle("PilleFightInterface")
        self.setGeometry(10, 10, 300, 200)

        self.SQLite = SqliteHandler()
        outDict, userlist, valuelist, percentWin = util.outcomePerUser(self.SQLite.getCompleteList())
        dbg(outDict)
        dbg(valuelist)
        dbg(userlist)
        dbg(percentWin)
        # @TODO: Evaluate if better done in a separate Module, inheriting QLineEdit
        # @TODO: Spaghettifull
        username = QLineEdit(USERNAME_PLACEHOLDER, self)
        username.setFixedWidth(100)
        username.move(0, 10)
        username_completer = QCompleter(userlist, username)
        username_completer.setCompletionMode(QCompleter.InlineCompletion)
        username.setCompleter(username_completer)
        fight_state = QLineEdit("won", self)
        fight_state.setValidator(QFightStateValidator())
        fight_state.setFixedWidth(100)
        fight_state.setCompleter(QCompleter(["won", "lost"], fight_state))
        fight_state.move(0,40)

        insert_button = QPushButton("Register", self)
        insert_button.setFixedWidth(80)
        insert_button.move(0, 80)
        insert_button.clicked.connect(lambda: self.on_submitToDatabaseClick(username.text(), fight_state.text()))

        update_button = QPushButton("Update", self)
        update_button.setFixedWidth(80)
        update_button.move(80, 80)
        update_button.clicked.connect(util.restart_program)

        # @TODO: On the right side of the window i'd like to have a textbox read-only there to allow printing stats

        # @TODO: Write a little statistical output module as well. Incorporate it onto the window title bar.
        # @TODO: Follow-up statistical: Show an HTML output an a QWebView of said statistics.

        self.app = qt_app
        """
        self.Layout = QVBoxLayout()
        self.Layout.addWidget(self.username)
        self.Layout.addWidget(self.fight_state)
        self.Layout.addWidget(insert_button)
        self.setLayout(self.Layout)
        """
        self.show()
        self.statistics = StatisticsWindow(qt_app, userlist, valuelist)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            dbg("Escape pressed, exiting...")
            self.app.exit()
        else:
            dbg(f"irrelevant keypress gets passed through with key {event.key()}")
            super().keyPressEvent(event)

    @pyqtSlot()
    def on_submitToDatabaseClick(self, user, state):
        dbg(f"inserting user '{user}' with fightstate '{state}'")
        self.SQLite.insert(user, state)