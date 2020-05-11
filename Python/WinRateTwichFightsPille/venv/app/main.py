import sys

import matplotlib
from PyQt5.QtWidgets import QApplication

from mainwindow import MainWindow

matplotlib.use("Qt5Agg")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow(app)