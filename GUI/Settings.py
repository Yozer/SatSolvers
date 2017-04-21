from PyQt5 import QtCore, QtGui
from configparser import  ConfigParser



# TODO poprawić pallete żeby, ładnie wyglądała, wywalić nie potrzebne,
# dodać last open file z pliku ---- albo wszystkie settings do pliku
class GUI_Settings():

    clauseTextFont = "{} {} {}".format('arial',14,'normal')
    clauseTextHeight = 5
    clauseTextWidth = 20


    resultTextFont = "{} {} {}".format('arial',14,'normal')
    resultTextHeight = 2
    resultTextWidth = 20


    lastOpenFile = ""
    defaultFile = ""


    palette = QtGui.QPalette()
    __config = ConfigParser()
    __settings = ""

    setPallete = True

    @staticmethod
    def initialize(settings = './settings.ini'):
        GUI_Settings.__initPallete()
        GUI_Settings.__settings = settings
        GUI_Settings.__config.read(settings)

        GUI_Settings.lastOpenFile = GUI_Settings.__config.get('File','LastOpenFile')
        GUI_Settings.defaultFile = GUI_Settings.__config.get('File','DefaultFile')
        GUI_Settings.setPallete = GUI_Settings.__config.getboolean('Pallete','DefaultPallete')


    @staticmethod
    def updateValue(section,where,value):
        GUI_Settings.__config.set(section,where,value)


    @staticmethod
    def closeEvent():
        with open(GUI_Settings.__settings,'w') as configFile:
            GUI_Settings.__config.write(configFile)

    @staticmethod
    def __initPallete():
        GUI_Settings.palette.setColor(QtGui.QPalette.Window, QtGui.QColor(53, 53, 53))
        GUI_Settings.palette.setColor(QtGui.QPalette.WindowText, QtCore.Qt.white)
        GUI_Settings.palette.setColor(QtGui.QPalette.Base, QtGui.QColor(15, 15, 15))
        GUI_Settings.palette.setColor(QtGui.QPalette.AlternateBase, QtGui.QColor(53, 53, 53))
        GUI_Settings.palette.setColor(QtGui.QPalette.ToolTipBase, QtCore.Qt.white)
        GUI_Settings.palette.setColor(QtGui.QPalette.ToolTipText, QtCore.Qt.white)
        GUI_Settings.palette.setColor(QtGui.QPalette.Text, QtCore.Qt.white)
        GUI_Settings.palette.setColor(QtGui.QPalette.Button, QtGui.QColor(53, 53, 53))
        GUI_Settings.palette.setColor(QtGui.QPalette.ButtonText, QtCore.Qt.white)
        GUI_Settings.palette.setColor(QtGui.QPalette.BrightText, QtCore.Qt.red)

        GUI_Settings.palette.setColor(QtGui.QPalette.Highlight, QtGui.QColor(142, 45, 197).lighter())
        GUI_Settings.palette.setColor(QtGui.QPalette.HighlightedText, QtCore.Qt.black)

from enum import Enum

class ParserSettings():

    Equal = '<=>'
    Implies = '=>'
    Not = '~'
    Or = '|'
    And = '&'
    Xor = '^'

    def __init__(self):
        pass


class PalleteType(Enum):

    Default = 1
    Dark = 2


class Settings():


    def __init__(self):
        self.parser = ParserSettings()
        self.pallete = PalleteType.Default
