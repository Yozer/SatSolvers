import sys
from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QFileDialog, QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget,QHBoxLayout)
from PyQt5.QtGui import QIcon
from GUI_Settings import GUI_Settings

sys.path.append("../")
from SolverHelper import SolverHelper

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.__initToolbar()
        self.__initText()
        self.__initUI()

    def __initUI(self):

        #self.setCentralWidget(self.textEdit)

        #self.setPalette(GUI_Settings.palette)
        #self.setGeometry(300, 300)
        self.setMinimumSize(300,300)
        self.center()
        self.setWindowTitle('File dialog')
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

        # TODO dodanie rysowanie graphu z networkx i matplotlib
        printGraph = QAction(QIcon('Icons/graph.png'), 'Print graph', self)
        printGraph.setStatusTip('Print graph')


        self.solversChoice = QComboBox()

        for item in SolverHelper.get_solvers():
            self.solversChoice.addItem(item)

        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(newFile)
        fileMenu.addAction(openFile)
        fileMenu.addAction(saveFile)

        toolbar = self.addToolBar('Open')
        toolbar.addAction(newFile)
        toolbar.addAction(openFile)
        toolbar.addAction(saveFile)
        toolbar.addSeparator()
        toolbar.addAction(executeButton)
        toolbar.addWidget(self.solversChoice)
        toolbar.addAction(printGraph)


    def __initText(self):

        self.textEdit = QTextEdit()
        self.resultText = QTextEdit()
        #self.resultText.setFixedHeight(self.resultText.document().size().height() * 2 + self.resultText.contentsMargins().top() * 1)
        self.resultText.setFixedWidth(self.resultText.document().size().width()*10)

        self.resultText.setReadOnly(True)

        vbox = QHBoxLayout()
        vbox.addWidget(self.textEdit)
        vbox.addWidget(self.resultText)
        widg = QWidget()
        widg.setLayout(vbox)
        self.setCentralWidget(widg)

        if GUI_Settings.lastOpenFile == GUI_Settings.defaultFile:
            self.__openFile = ""
        else:
            try:
                f = open(GUI_Settings.lastOpenFile,'r')
                self.textEdit.setText(f.read())
                f.close()
                self.__openFile = GUI_Settings.lastOpenFile
            except IOError:
                f = open(GUI_Settings.lastOpenFile,'a')
                f.close()

    def center(self):

        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())


    def newFile(self):
        if self.__openFile != "":
            buttonReply = QMessageBox.question(self, 'Save file', "Save changes?",
                                               QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
            if buttonReply == QMessageBox.Yes:
                f = open(self.__openFile, 'w')
                with f:
                    f.write(self.textEdit.toPlainText())
                self.__openFile = ""
        elif self.textEdit.toPlainText() != "":
            buttonReply = QMessageBox.question(self, 'Save file', "Save file?",
                                               QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
            if buttonReply == QMessageBox.Yes:
                self.saveFile()
        else:
            self.textEdit.clear()

    def openFile(self):

        buttonReply = QMessageBox.question(self, 'Save file', "Save changes?",
                                           QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        if buttonReply == QMessageBox.Yes:
            self.saveFile()

        fname = QFileDialog.getOpenFileName(self, 'Open file', '',"Text Files (*.txt);;All Files (*)")
        self.__openFile = fname[0]
        if fname[0]:
            f = open(fname[0], 'r')

            with f:
                data = f.read()
                self.textEdit.setText(data)

    def saveFile(self):

        if self.__openFile != "":

            fname = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", self.__openFile, "Text Files (*.txt);;CNF Files (*.cnf);;All Files (*)")
            if fname[0]:
                f = open(fname[0], 'w')
                with f:
                    f.write(self.textEdit.toPlainText())
        else:
            fname = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "Text Files (*.txt);;CNF Files (*.cnf);;All Files (*)")
            if fname[0]:
                f = open(fname[0], 'w')
                with f:
                    f.write(self.textEdit.toPlainText())


    def solve(self):
        clause = self.textEdit.toPlainText()

        result = SolverHelper.solve(clause,self.solversChoice.currentText())

        if result[0] == 'S' or result[0] == 'U':
            self.resultText.setPlainText(result)

        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Critical)
            msg.setText("Error")
            msg.setInformativeText(result)
            msg.setWindowTitle("Error")
            msg.exec_()

    def closeEvent(self, event):

        reply = QMessageBox.question(self, 'Message',
                                     "Are you sure to quit without saving?", QMessageBox.Save | QMessageBox.No |
                                     QMessageBox.Yes, QMessageBox.No)

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
        if self.__openFile == "":
            GUI_Settings.updateValue('File','LastOpenFile',GUI_Settings.defaultFile)
        else:
            GUI_Settings.updateValue('File','LastOpenFile',str(self.__openFile))
        GUI_Settings.closeEvent()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    GUI_Settings.initialize()
    ex = MainWindow()
    #app.setStyle('Windows')
    #print (app.style().metaObject().className())
    #print (QStyleFactory.keys())
    sys.exit(app.exec_())