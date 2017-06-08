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
        #self.setGeometry(300, 300)
        self.setMinimumSize(300,300)
        self.center()
        #self.setWindowTitle('SatSolver')
        self.showMaximized()
        self.show()


    def __initToolbar(self):

        self.statusBar()

        newFile = QAction(QIcon('Icons/newFile.png'), 'New File', self)
        newFile.setShortcut('Ctrl+N')
        newFile.setStatusTip('Create new file')
        newFile.triggered.connect(self.newFile)

        openFile = QAction(QIcon('Icons/openFile.png'), 'Open', self)
        openFile.setShortcut('Ctrl+O')
        openFile.setStatusTip('Open new File')
        openFile.triggered.connect(self.openFile)

        saveFile = QAction(QIcon('Icons/saveFile.png'), 'Save', self)
        saveFile.setShortcut('Ctrl+S')
        saveFile.setStatusTip('Save File')
        saveFile.triggered.connect(self.saveFile)

        executeButton = QAction(QIcon('Icons/execute.png'),'Execute',self)
        executeButton.setShortcut('F5')
        executeButton.setStatusTip('Solve clause')
        executeButton.triggered.connect(self.solve)
        self.executeButton = executeButton

        exportDimacs = QAction(QIcon('Icons/export_dimacs.png'),'Export dimacs',self)
        exportDimacs.setStatusTip('Export clause to dimacs')
        exportDimacs.triggered.connect(self.__export)

        # TODO dodanie rysowanie graphu z networkx i matplotlib
        printGraph = QAction(QIcon('Icons/graph.png'), 'Print graph', self)
        printGraph.setStatusTip('Print graph')
        printGraph.triggered.connect(self.__printGraph)

        settingsButton = QAction(QIcon('Icons/settings.png'), 'Settings', self)
        settingsButton.setStatusTip('Settings')
        settingsButton.triggered.connect(self.__openSettings)

        generateButton = QAction('Generate',self)
        generateButton.setStatusTip('Generate GOPR')
        generateButton.triggered.connect(self.__generate)

        self.solversChoice = QComboBox()

        for item in SolverHelper.get_solvers():
            self.solversChoice.addItem(item)


        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(newFile)
        fileMenu.addAction(openFile)
        fileMenu.addAction(saveFile)
        fileMenu.addAction(exportDimacs)

        toolbar = self.addToolBar('Toolbar')
        toolbar.addAction(newFile)
        toolbar.addAction(openFile)
        toolbar.addAction(saveFile)
        toolbar.addAction(exportDimacs)
        toolbar.addSeparator()
        toolbar.addAction(executeButton)
        toolbar.addWidget(self.solversChoice)
        toolbar.addAction(printGraph)
        toolbar.addSeparator()
        toolbar.addAction(settingsButton)
        toolbar.addAction(generateButton)



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
        dialog = ConfigDialog(self.settings)

        dialog.exec_()
        dialog.show()

        self.setPalette(self.settings.getPallete())
        self.settings.updateParserSettings()
        self.tab.currentWidget().highlightCurrentLine()

    def __printGraph(self):
        self.tab.currentWidget().allAssigments(self.solversChoice.currentText())

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
            #tabWidget.loadFile()
            self.tab.addTab(tabWidget,tabWidget.title)
            self.tab.setCurrentIndex(self.tab.count()-1)
            self.__setTitle()


    def saveFile(self):
        self.tab.currentWidget().saveFile()
        self.tab.setTabText(self.tab.currentIndex(),self.tab.currentWidget().title)
        self.__setTitle()

    def __export(self):
        self.tab.currentWidget().export2dimacs()

    # TODO  kolorowania linii jesli błąd, lub na dole
    def solve(self):
        #self.tab.currentWidget().solveC(self.solversChoice.currentText())
        self.tab.currentWidget().solve(self.solversChoice.currentText())

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
    #Settings.initialize()
    ex = MainWindow()
    #app.setStyle('Windows')
    #print (app.style().metaObject().className())
    #print (QStyleFactory.keys())
    sys.exit(app.exec_())