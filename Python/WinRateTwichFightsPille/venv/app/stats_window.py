import matplotlib
import numpy as np
from PyQt5.QtWidgets import QWidget, QApplication
from matplotlib import pyplot as plt

from dbg import dbg


class StatisticsWindow(QWidget):
    # @TODO: Overall very spaghetti, evaluate.
    def __init__(self, qt_app: QApplication, usernames, outcomes):
        self.app = qt_app
        super(StatisticsWindow, self).__init__()
        plt.rcdefaults()
        fig, ax = plt.subplots()
        fig.canvas.mpl_connect('key_press_event', self.keyPressEvent)
        plt.gcf().canvas.set_window_title("name")
        y_pos = np.arange(len(usernames))
        performance = outcomes
        error = 0

        ax.barh(y_pos, performance, align='center')
        ax.set_yticks(y_pos)
        ax.set_yticklabels(usernames)
        ax.invert_yaxis()  # labels read top-to-bottom
        ax.set_xlabel('Win-Loss-Ratio(-1 by loose, +1 by win)')
        ax.set_title('Win/Loss-Ratio per User fought already')

        plt.title = "PilleFightsFigure"
        plt.show()

    def keyPressEvent(self, event: matplotlib.backend_bases.KeyEvent):
        if event.key == "escape":
            dbg("Escape pressed, exiting...")
            self.app.exit(0)
        else:
            dbg(f"irrelevant keypress with key {event.key}")