from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtWidgets import QWidget, QApplication, QLineEdit, QCompleter, QPushButton

import util
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
        self.username = QLineEdit("Username....", self)
        self.username.setFixedWidth(100)
        self.username.move(0, 10)
        self.username_completer = QCompleter(userlist, self.username)
        self.username_completer.setCompletionMode(QCompleter.InlineCompletion)
        self.username.setCompleter(self.username_completer)
        self.fight_state = QLineEdit("won", self)
        self.fight_state.setValidator(QFightStateValidator())
        self.fight_state.setFixedWidth(100)
        self.fight_state.setCompleter(QCompleter(["won", "lost"], self.fight_state))
        self.fight_state.move(0,40)

        insert_button = QPushButton("Register", self)
        insert_button.setFixedWidth(80)
        insert_button.move(0, 80)
        insert_button.clicked.connect(self.on_submitToDatabaseClick)

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
    def on_submitToDatabaseClick(self):
        user = self.username.text()
        state = self.fight_state.text()
        dbg(f"inserting user '{user}' with fightstate '{state}'")
        self.SQLite.insert(user, state)