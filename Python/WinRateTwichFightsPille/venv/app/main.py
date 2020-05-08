import typing
import sys
from dbg import dbg
from config import *
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLineEdit, QCompleter, QMainWindow
from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtGui import QValidator
from sqliteHandler import *

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
matplotlib.use("Qt5Agg")


def outcomeToNumber(outcome: str):
    if outcome == WIN_STR:
        return 1
    else:
        return -1


def outcomePerUser(ll):
    key_list = list()
    value_list = list()

    for key in ll:
        if key["againstName"] not in key_list:
            key_list.append(key["againstName"])

    outcome_dict = dict((el,0) for el in key_list)

    for key in ll:
        user = key["againstName"]
        state = outcomeToNumber(key["outcome"])
        outcome_dict[user] += state

    for key in outcome_dict:
        value_list.append(outcome_dict[key])

    return outcome_dict, key_list, value_list


class QFightStateValidator(QValidator):
    def __init__(self):
        super().__init__()

    def validate(self, input_str: str, position: int) -> typing.Tuple['QValidator.State', str, int]:
        if input_str not in WIN_STR and input_str not in LOST_STR:
            return QValidator.Invalid, input_str, position
        else:
            return QValidator.Acceptable, input_str, position

    def fixup(self, fix_str: str) -> str:
        for x in range(len(WIN_STR)):
            if fix_str[x] in WIN_STR and fix_str[x] in LOST_STR:
                continue
            elif fix_str[x] in WIN_STR:
                return WIN_STR
            elif fix_str[x] in LOST_STR:
                return LOST_STR
            else:
                continue


class StatisticsWindow(QWidget):
    def __init__(self, qt_app: QApplication, usernames, outcomes):
        super(StatisticsWindow, self).__init__()
        plt.rcdefaults()
        fig, ax = plt.subplots()
        y_pos = np.arange(len(usernames))
        performance = outcomes
        error = 0

        ax.barh(y_pos, performance, align='center')
        ax.set_yticks(y_pos)
        ax.set_yticklabels(usernames)
        ax.invert_yaxis()  # labels read top-to-bottom
        ax.set_xlabel('Win-Loss-Ratio(-1 by loose, +1 by win)')
        ax.set_title('Win/Loss-Ratio per User fought already')

        plt.show()


class MainWindow(QWidget):
    def __init__(self, qt_app: QApplication):
        super().__init__()

        self.setWindowTitle("PilleFightInterface")
        self.setGeometry(10, 10, 300, 200)

        self.SQLite = SqliteHandler()
        outDict, userlist, valuelist = outcomePerUser(self.SQLite.getCompleteList())
        dbg(outDict)
        dbg(valuelist)
        dbg(userlist)
        # @TODO: Evaluate if better done in a separate Module, inheriting QLineEdit
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
        # self.SQLite.insert("testuser", "won")
        self.show()
        StatisticsWindow(qt_app, userlist, valuelist)

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
    app.exec_()