from PyQt5.QtCore import QDate, QSize, Qt
from PyQt5.QtGui import QIcon, QPixmap
from PyQt5.QtWidgets import (QApplication, QCheckBox, QComboBox, QDateTimeEdit,
        QDialog, QGridLayout, QGroupBox, QHBoxLayout, QLabel, QLineEdit,
        QListView, QListWidget, QListWidgetItem, QPushButton, QSpinBox,
        QStackedWidget, QVBoxLayout, QWidget, QRadioButton, QTableWidget,QTableWidgetItem,QAbstractItemView,
                             QAbstractScrollArea)

from GUI.Settings import ParserSettings

# TODO poprawić wygląd tabelki, ewentualnie dodać jeszcze czy parsować nową linie jako AND
class ParserSettingsPage(QWidget):
    def __init__(self,settings, parent=None):
        super(self.__class__, self).__init__(parent)
        self.settings = settings
        tableWidget = QTableWidget(6,1)
        tableWidget.setItem(0,0,QTableWidgetItem(settings.parser.Equal))
        tableWidget.setItem(0,1,QTableWidgetItem(settings.parser.Implies))
        tableWidget.setItem(0,2,QTableWidgetItem(settings.parser.Not))
        tableWidget.setItem(0,3,QTableWidgetItem(settings.parser.Or))
        tableWidget.setItem(0,4,QTableWidgetItem(settings.parser.And))
        tableWidget.setItem(0,5,QTableWidgetItem(settings.parser.Xor))

        tableWidget.setSizeAdjustPolicy(QAbstractScrollArea.AdjustToContents)
        headerLabels = ("Equal","Implies","Not","Or","And","Xor")
        tableWidget.setVerticalHeaderLabels(headerLabels)

        tableWidget.setEditTriggers(
            QAbstractItemView.DoubleClicked | QAbstractItemView.SelectedClicked)
        tableWidget.setSelectionBehavior(QAbstractItemView.SelectRows)
        tableWidget.resizeColumnsToContents()
        tableWidget.horizontalHeader().setVisible(False)

        resetButton = QPushButton("Reset")
        resetButton.clicked.connect(self.__reset)
        defaultButton = QPushButton("Default")
        defaultButton.clicked.connect(self.__default)
        self.tableWidget = tableWidget

        buttonLayout = QHBoxLayout()
        buttonLayout.addStretch(1)
        buttonLayout.addWidget(resetButton)
        buttonLayout.addWidget(defaultButton)
        buttonLayout.addSpacing(12)

        tableLayout = QHBoxLayout()
        tableLayout.addStretch(1)
        tableLayout.addWidget(tableWidget)
        tableLayout.addStretch(1)
        tableLayout.addSpacing(12)

        mainLayout = QVBoxLayout()
        mainLayout.addLayout(tableLayout)
        mainLayout.addLayout(buttonLayout)
        mainLayout.addStretch(1)

        self.setLayout(mainLayout)

    def __reset(self):
        self.tableWidget.setItem(0,0,QTableWidgetItem(self.settings.parser.Equal))
        self.tableWidget.setItem(0,1,QTableWidgetItem(self.settings.parser.Implies))
        self.tableWidget.setItem(0,2,QTableWidgetItem(self.settings.parser.Not))
        self.tableWidget.setItem(0,3,QTableWidgetItem(self.settings.parser.Or))
        self.tableWidget.setItem(0,4,QTableWidgetItem(self.settings.parser.And))
        self.tableWidget.setItem(0,5,QTableWidgetItem(self.settings.parser.Xor))

    def __default(self):
        self.tableWidget.setItem(0,0,QTableWidgetItem(ParserSettings.Equal))
        self.tableWidget.setItem(0,1,QTableWidgetItem(ParserSettings.Implies))
        self.tableWidget.setItem(0,2,QTableWidgetItem(ParserSettings.Not))
        self.tableWidget.setItem(0,3,QTableWidgetItem(ParserSettings.Or))
        self.tableWidget.setItem(0,4,QTableWidgetItem(ParserSettings.And))
        self.tableWidget.setItem(0,5,QTableWidgetItem(ParserSettings.Xor))


    # TODO zapisywanie ustawień parsera i sprawdzanie poprawności, konfliktów
    def save(self):
        pass

class ThemesPage(QWidget):
    def __init__(self, settings, parent=None):
        super(ThemesPage, self).__init__(parent)
        self.settings = settings
        themeGroup = QGroupBox("Themes")

        layout = QGridLayout()

        radiobutton = QRadioButton("Default")
        radiobutton.type = 1

        if settings.palleteType == 1:
            radiobutton.setChecked(True)
            self.palleteType = 1

        radiobutton.toggled.connect(self.__on_toggle)
        layout.addWidget(radiobutton, 0, 0)

        radiobutton = QRadioButton("Dark")
        radiobutton.type = 2

        if settings.palleteType == 2:
            radiobutton.setChecked(True)
            self.palleteType = 2

        radiobutton.toggled.connect(self.__on_toggle)
        layout.addWidget(radiobutton, 0, 1)

        pixmap = QPixmap('./images/defaultPallete1.png')
        lbl = QLabel(self)
        lbl.setPixmap(pixmap)
        layout.addWidget(lbl,1,0)

        pixmap = QPixmap('./images/darkPallete1.png')
        lbl = QLabel(self)
        lbl.setPixmap(pixmap)
        layout.addWidget(lbl,1,1)

        themeGroup.setLayout(layout)

        mainLayout = QVBoxLayout()
        mainLayout.addWidget(themeGroup)
        mainLayout.addSpacing(12)
        mainLayout.addStretch(1)

        self.setLayout(mainLayout)


    def __on_toggle(self):
        radiobutton = self.sender()

        if radiobutton.isChecked():
            self.palleteType = radiobutton.type

    def save(self):
        self.settings.palleteType = self.palleteType


class ConfigDialog(QDialog):
    def __init__(self, settings, parent=None):
        super(ConfigDialog, self).__init__(parent)

        self.settings = settings

        self.contentsWidget = QListWidget()
        self.contentsWidget.setViewMode(QListView.IconMode)
        self.contentsWidget.setIconSize(QSize(96, 84))
        self.contentsWidget.setMovement(QListView.Static)
        self.contentsWidget.setMaximumWidth(128)
        self.contentsWidget.setSpacing(12)

        self.pagesWidget = QStackedWidget()
        self.pagesWidget.addWidget(ParserSettingsPage(self.settings))
        self.pagesWidget.addWidget(ThemesPage(self.settings))


        closeButton = QPushButton("Close")
        closeButton.clicked.connect(self.close)

        self.createIcons()
        self.contentsWidget.setCurrentRow(0)

        saveButton = QPushButton("Save")
        saveButton.clicked.connect(self.__save)


        horizontalLayout = QHBoxLayout()
        horizontalLayout.addWidget(self.contentsWidget)
        horizontalLayout.addWidget(self.pagesWidget, 1)

        buttonsLayout = QHBoxLayout()
        buttonsLayout.addStretch(1)
        buttonsLayout.addWidget(saveButton)
        buttonsLayout.addWidget(closeButton)

        mainLayout = QVBoxLayout()
        mainLayout.addLayout(horizontalLayout)
        mainLayout.addStretch(1)
        #mainLayout.addSpacing(12)
        mainLayout.addLayout(buttonsLayout)

        self.setLayout(mainLayout)
        self.setFixedSize(564,311)


        #self.resizeEvent = self.onResize

        self.setWindowTitle("Config Dialog")

    def onResize(self,event):
        print (self.size())

    def __save(self):

        for x in range(self.contentsWidget.count()):
            self.pagesWidget.setCurrentIndex(x)
            self.pagesWidget.currentWidget().save()

        self.close()


    def changePage(self, current, previous):
        if not current:
            current = previous

        self.pagesWidget.setCurrentIndex(self.contentsWidget.row(current))

    def createIcons(self):
        configButton = QListWidgetItem(self.contentsWidget)
        configButton.setIcon(QIcon('./Icons/config.png'))
        configButton.setText("Parser \n Settings")
        configButton.setTextAlignment(Qt.AlignHCenter)
        configButton.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEnabled)

        themeButton = QListWidgetItem(self.contentsWidget)
        themeButton.setIcon(QIcon('./Icons/themes.png'))
        themeButton.setText("Themes")
        themeButton.setTextAlignment(Qt.AlignHCenter)
        themeButton.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEnabled)


        '''
        queryButton = QListWidgetItem(self.contentsWidget)
        #queryButton.setIcon(QIcon(':/images/query.png'))
        queryButton.setText("Query")
        queryButton.setTextAlignment(Qt.AlignHCenter)
        queryButton.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEnabled)


        '''


        self.contentsWidget.currentItemChanged.connect(self.changePage)

