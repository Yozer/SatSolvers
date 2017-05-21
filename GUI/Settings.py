from PyQt5 import QtCore, QtGui
from PyQt5.QtGui import QColor
from PyQt5.QtCore import QDir, Qt
from configparser import  ConfigParser


class ParserSettings():

    Equal = '<=>'
    Implies = '=>'
    Not = '~'
    Or = '|'
    And = '&'
    Xor = '^'

    def __init__(self):
        pass

    def to_default(self):
        self.Equal = ParserSettings.Equal
        self.Implies = ParserSettings.Implies
        self.Not = ParserSettings.Not
        self.Or = ParserSettings.Or
        self.And = ParserSettings.And
        self.Xor = ParserSettings.Xor

    #TODO zwraca liste operatorów, zrobić aby można było wstawić do regexa
    def operators(self):
        return [self.Equal,self.Implies,self.Not,self.Or,self.And,self.Xor]

    '''
    Tworzy mape dla parsera z obecnymi ustawieniami

    :returns: dictionary
    '''
    def changeMap(self):
        result = dict()
        result[self.Equal] = ParserSettings.Equal
        result[self.Implies] = ParserSettings.Implies
        result[self.Not] = ParserSettings.Not
        result[self.Or] = ParserSettings.Or
        result[self.And] = ParserSettings.And
        result[self.Xor] = ParserSettings.Xor
        return result



class PalleteType():
    Default = 1
    Dark = 2

# TODO poprawić pallete żeby, ładnie wyglądała, wywalić nie potrzebne,
class Settings():

    clauseTextFont = "{} {} {}".format('arial',14,'normal')
    clauseTextHeight = 5
    clauseTextWidth = 20


    resultTextFont = "{} {} {}".format('arial',14,'normal')
    resultTextHeight = 2
    resultTextWidth = 20


    lastOpenFile = ""
    defaultFile = ""

    __defaultPallete = QtGui.QPalette()
    __palette = QtGui.QPalette()
    __config = ConfigParser()
    __settings = ""

    palleteType = 1


    @staticmethod
    def initialize(settings = './settings.ini'):
        #print (QDir.currentPath())

        Settings.__initPallete()
        Settings.__settings = settings
        Settings.__config.read(settings)

        Settings.lastOpenFile = Settings.__config.get('File','LastOpenFile')
        Settings.defaultFile = Settings.__config.get('File','DefaultFile')
        Settings.palleteType = Settings.__config.getint('Pallete','PalleteType')


    def updateValue(self,section,where,value):
        self.__config.set(section,where,value)



    def closeEvent(self):
        with open(Settings.__settings,'w') as configFile:
            self.__config.write(configFile)

    @staticmethod
    def __initPallete():
        Settings.__palette.setColor(QtGui.QPalette.Window, QtGui.QColor(53, 53, 53))
        Settings.__palette.setColor(QtGui.QPalette.WindowText, QtCore.Qt.white)
        Settings.__palette.setColor(QtGui.QPalette.Base, QtGui.QColor(15, 15, 15))
        Settings.__palette.setColor(QtGui.QPalette.AlternateBase, QtGui.QColor(53, 53, 53))
        Settings.__palette.setColor(QtGui.QPalette.ToolTipBase, QtCore.Qt.white)
        Settings.__palette.setColor(QtGui.QPalette.ToolTipText, QtCore.Qt.white)
        Settings.__palette.setColor(QtGui.QPalette.Text, QtCore.Qt.white)
        Settings.__palette.setColor(QtGui.QPalette.Button, QtGui.QColor(53, 53, 53))
        Settings.__palette.setColor(QtGui.QPalette.ButtonText, QtCore.Qt.white)
        Settings.__palette.setColor(QtGui.QPalette.BrightText, QtCore.Qt.red)
        Settings.__palette.setColor(QtGui.QPalette.Highlight, QtGui.QColor(142, 45, 197).lighter())
        Settings.__palette.setColor(QtGui.QPalette.HighlightedText, QtCore.Qt.black)

    def getPallete(self):
        if self.palleteType == PalleteType.Dark:
            return self.__palette
        else:
            return self.__defaultPallete

    def getHighlight(self):
        if self.palleteType == PalleteType.Dark:
            return self.darkHighlight
        else:
            return self.lightHighlight

    def __init__(self):
        self.parser = ParserSettings()

        self.parser.Equal = self.__config.get('ParserSettings','Equal')
        self.parser.Implies = self.__config.get('ParserSettings','Implies')
        self.parser.Not = self.__config.get('ParserSettings','Not')
        self.parser.Or = self.__config.get('ParserSettings','Or')
        self.parser.And = self.__config.get('ParserSettings','And')
        self.parser.Xor = self.__config.get('ParserSettings','Xor')

        self.darkHighlight = QColor("#323232")
        self.lightHighlight = QColor("#e8f2fe")
        self.errorHighlight = QColor("#3a2323")
        self.errorColor = QColor("#e6413c")
        self.commentColor = QColor("#42647a")
        self.commentChar = '#'

    def updateParserSettings(self):
        self.__config.set('ParserSettings', 'Equal', self.parser.Equal)
        self.__config.set('ParserSettings', 'Implies', self.parser.Implies)
        self.__config.set('ParserSettings', 'Not', self.parser.Not)
        self.__config.set('ParserSettings', 'Or', self.parser.Or)
        self.__config.set('ParserSettings', 'And', self.parser.And)
        self.__config.set('ParserSettings', 'Xor', self.parser.Xor)

