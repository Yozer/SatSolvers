from PyQt5 import QtCore, QtGui
from configparser import  ConfigParser

'''
try:
    from configparser import ConfigParser
except ImportError:
    from ConfigParser import ConfigParser  # ver. < 3.0

# instantiate
config = ConfigParser()

# parse existing file
config.read('settings.ini')

# read values from a section
string_val = config.get('section_a', 'string_val')
bool_val = config.getboolean('section_a', 'bool_val')
int_val = config.getint('section_a', 'int_val')
float_val = config.getfloat('section_a', 'pi_val')

# update existing value
config.set('section_a', 'string_val', 'world')

# add a new section and some values
config.add_section('section_b')
config.set('section_b', 'meal_val', 'spam')
config.set('section_b', 'not_found_val', 404)

# save to a file
with open('test_update.ini', 'w') as configfile:
    config.write(configfile)

'''

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


    @staticmethod
    def initialize(settings = './settings.ini'):
        GUI_Settings.__initPallete()
        GUI_Settings.__settings = settings
        GUI_Settings.__config.read(settings)

        GUI_Settings.lastOpenFile = GUI_Settings.__config.get('File','LastOpenFile')
        GUI_Settings.defaultFile = GUI_Settings.__config.get('File','DefaultFile')

        '''
        string_val = GUI_Settings.__config.get('section_a', 'string_val')
        bool_val = GUI_Settings.__config.getboolean('section_a', 'bool_val')
        int_val = GUI_Settings.__config.getint('section_a', 'int_val')
        float_val = GUI_Settings.__config.getfloat('section_a', 'pi_val')

        # update existing value
        GUI_Settings.__config.set('section_a', 'string_val', 'world')

        # add a new section and some values
        GUI_Settings.__config.add_section('section_b')
        GUI_Settings.__config.set('section_b', 'meal_val', 'spam')
        GUI_Settings.__config.set('section_b', 'not_found_val', 404)

        # save to a file
        with open('test_update.ini', 'w') as configfile:
            config.write(configfile)
        '''

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