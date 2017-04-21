from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QFileDialog, QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget,QHBoxLayout)
from PyQt5.QtGui import QIcon

# TODO stworzenie ładnego edytora
class Editor(QTextEdit):


    def __init__(self):
        super(self.__class__,self).__init__()


