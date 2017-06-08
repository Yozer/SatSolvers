from PyQt5.QtCore import QThread, QObject,pyqtSignal, QRunnable
from PyQt5.Qt import QApplication
import sys


class ResultSignal(QObject):
    result = pyqtSignal(str)


class LoadFileThread(QRunnable):

    def __init__(self,filename):
        QThread.__init__(self)
        self.filename = filename
        self.signal = ResultSignal()


    def run(self):
        f = open(self.filename, 'r')
        with f:
            data = f.read()
        self.signal.result.emit(data)
        f.close()
