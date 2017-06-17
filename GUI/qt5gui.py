import sys,re
from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QFileDialog,QTabBar,QTabWidget, QSplitter,QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget,QHBoxLayout)
from PyQt5.QtGui import QIcon, QPalette
from PyQt5.QtCore import Qt,QFileInfo, QThreadPool
sys.path.append("../")
from GUI.Settings import Settings
from SolverHelper import SolverHelper
from GUI.Editor import CodeEditor
from GUI.ConfigDialog import ConfigDialog
from GUI.TabEditor import TabEditor, FileType
from Generator.GeneratorDialog import GeneratorDialog

class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()

        self.settings = Settings()
        self.__initToolbar()
        self.__initText()
        self.__initUI()


    def __initUI(self):

        self.setWindowIcon(QIcon('./Icons/logo.png'))
        self.setToolButtonStyle(Qt.ToolButtonFollowStyle)
        self.setPalette(self.settings.getPallete())
        self.setMinimumSize(300,300)
        self.center()
        self.showMaximized()
        self.show()


    def __initToolbar(self):

        self.statusBar()

        self.newFileButton = QAction(QIcon('Icons/newFile.png'), 'New File', self)
        self.newFileButton.setShortcut('Ctrl+N')
        self.newFileButton.setStatusTip('Create new file')
        self.newFileButton.triggered.connect(self.newFile)

        self.openFileButton = QAction(QIcon('Icons/openFile.png'), 'Open', self)
        self.openFileButton.setShortcut('Ctrl+O')
        self.openFileButton.setStatusTip('Open new File')
        self.openFileButton.triggered.connect(self.openFile)

        self.saveFileButton = QAction(QIcon('Icons/saveFile.png'), 'Save', self)
        self.saveFileButton.setShortcut('Ctrl+S')
        self.saveFileButton.setStatusTip('Save File')
        self.saveFileButton.triggered.connect(self.saveFile)

        self.stopButton = QAction(QIcon('Icons/stop-flat.png'), 'Stop session', self)
        self.stopButton.setShortcut('F7')
        self.stopButton.setStatusTip('Stop session')
        self.stopButton.triggered.connect(self.stop)
        self.stopButton.setEnabled(False)

        self.executeButton = QAction(QIcon('Icons/execute.png'),'Execute',self)
        self.executeButton.setShortcut('F5')
        self.executeButton.setStatusTip('Solve clause')
        self.executeButton.triggered.connect(self.solve)

        self.exportDimacs = QAction(QIcon('Icons/export_dimacs.png'),'Export dimacs',self)
        self.exportDimacs.setStatusTip('Export clause to dimacs')
        self.exportDimacs.triggered.connect(self.__export)

        self.settingsButton = QAction(QIcon('Icons/settings.png'), 'Settings', self)
        self.settingsButton.setStatusTip('Settings')
        self.settingsButton.triggered.connect(self.__openSettings)

        self.generateButton = QAction('Generate',self)
        self.generateButton.setStatusTip('Generate GOPR')
        self.generateButton.triggered.connect(self.__generate)

        self.solversChoice = QComboBox()

        for item in SolverHelper.get_solvers():
            self.solversChoice.addItem(item)


        menubar = self.menuBar()
        self.fileMenu = menubar.addMenu('&File')
        self.fileMenu.addAction(self.newFileButton)
        self.fileMenu.addAction(self.openFileButton)
        self.fileMenu.addAction(self.saveFileButton)
        self.fileMenu.addAction(self.exportDimacs)

        toolbar = self.addToolBar('Toolbar')
        toolbar.addAction(self.newFileButton)
        toolbar.addAction(self.openFileButton)
        toolbar.addAction(self.saveFileButton)
        toolbar.addAction(self.exportDimacs)
        toolbar.addSeparator()
        toolbar.addAction(self.executeButton)
        toolbar.addAction(self.stopButton)
        toolbar.addWidget(self.solversChoice)
        toolbar.addSeparator()
        toolbar.addAction(self.settingsButton)
        toolbar.addAction(self.generateButton)



    def removeTab(self,index):
        if self.tab.widget(index).textChanged():
            reply = QMessageBox.question(self, 'Message',
                                         "Save file?",
                                         QMessageBox.Save | QMessageBox.No, QMessageBox.Save)
            if reply == QMessageBox.Save:
                self.tab.widget(index).saveFile()
        self.tab.removeTab(index)

    def __initText(self):

        tab = QTabWidget()
        tab.setTabsClosable(True)

        if self.settings.lastOpenFile is not None:
            for file in self.settings.lastOpenFile:

                if QFileInfo(file).exists():
                    filename = file
                    tabWidget = TabEditor(self, self.settings, self.executeButton, filename)
                    # tabWidget.loadFile()

                    tab.addTab(tabWidget, tabWidget.title)


        tab.tabCloseRequested.connect(self.removeTab)

        self.setCentralWidget(tab)
        self.tab = tab
        self.__setTitle()
        tab.currentChanged.connect(self.__setTitle)


    def __setTitle(self):
        if self.tab.count() == 0:
            shownName = ""
        else:
            shownName = self.tab.currentWidget().title

        self.setWindowTitle("%s[*] - Application" % shownName)

    def center(self):

        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())
    def __generate(self):
        result,dimacs = GeneratorDialog.getDialog(self)
        if result != "" and result != None:
            if dimacs:
                self.addDimacsFile(result)
            else:
                self.newFile()
                self.tab.currentWidget().textEdit.setText(result)
            self.tab.currentWidget().textEdit.text_was_changed = True


    def __openSettings(self):
        dialog = ConfigDialog(self.settings, self)

        dialog.exec()

        self.setPalette(self.settings.getPallete())
        self.settings.updateParserSettings()
        if self.tab.currentWidget() is not None:
            self.tab.currentWidget().highlightCurrentLine()

    def newFile(self):
        tabWidget = TabEditor(self,self.settings,self.executeButton)
        self.tab.addTab(tabWidget,tabWidget.title)
        self.tab.setCurrentIndex(self.tab.count()-1)
        self.__setTitle()

    def addDimacsFile(self, text):
        self.newFile()
        self.tab.currentWidget().textEdit.setText(text)
        self.tab.currentWidget().fileType = FileType.Dimacs

    def openFile(self):

        fname = QFileDialog.getOpenFileName(self, 'Open file', '',"Text Files (*.txt);;Cnf Files (*.cnf);;All Files (*)")
        for i in range (0,self.tab.count()):
            if self.tab.widget(i).file == fname[0]:
                self.tab.setCurrentIndex(i)
                return

        if fname[0] != "":
            tabWidget = TabEditor(self,self.settings,self.executeButton,fname[0])
            self.tab.addTab(tabWidget,tabWidget.title)
            self.tab.setCurrentIndex(self.tab.count()-1)
            self.__setTitle()


    def saveFile(self):
        self.tab.currentWidget().saveFile()
        self.tab.setTabText(self.tab.currentIndex(),self.tab.currentWidget().title)
        self.__setTitle()

    def __export(self):
        self.tab.currentWidget().export2dimacs()

    def solve(self):
        success = self.tab.currentWidget().solveC(self.solversChoice.currentText())
        if success:
            self.fileMenu.setEnabled(False)
            self.solversChoice.setEnabled(False)
            self.openFileButton.setEnabled(False)
            self.newFileButton.setEnabled(False)
            self.saveFileButton.setEnabled(False)
            self.exportDimacs.setEnabled(False)
            self.settingsButton.setEnabled(False)
            self.generateButton.setEnabled(False)
            self.stopButton.setEnabled(True)
            self.tab.currentWidget().setReadOnly(True)

    def stop(self):
        self.fileMenu.setEnabled(True)
        self.solversChoice.setEnabled(True)
        self.openFileButton.setEnabled(True)
        self.newFileButton.setEnabled(True)
        self.saveFileButton.setEnabled(True)
        self.exportDimacs.setEnabled(True)
        self.settingsButton.setEnabled(True)
        self.generateButton.setEnabled(True)
        self.tab.currentWidget().setReadOnly(False)
        self.stopButton.setEnabled(False)
        self.tab.currentWidget().clearAssigments()

    def closeEvent(self, event):
        if self.tab.count() == 0:
            self.__updateSettings()
            event.accept()
        else:

            reply = QMessageBox.question(self, 'Message',
                                         "Are you sure to quit without saving?", QMessageBox.Save | QMessageBox.Cancel |
                                         QMessageBox.Yes, QMessageBox.Cancel)

            if reply == QMessageBox.Yes:
                self.__updateSettings()
                event.accept()
            elif reply == QMessageBox.Save:
                self.__updateSettings()
                self.saveFile()
                event.accept()
            else:
                event.ignore()

    def __updateSettings(self):

        files = []
        for i in range(0,self.tab.count()):
            files.append(self.tab.widget(i).file)

        self.settings.qsettings.setValue(self.settings.section,files)
        self.settings.updateValue('Pallete', 'PalleteType',str(self.settings.palleteType))
        self.settings.closeEvent()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())