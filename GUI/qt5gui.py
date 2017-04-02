import sys
from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QFileDialog, QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget)
from PyQt5.QtGui import QIcon
from GUI_Settings import GUI_Settings

sys.path.append("../")
from SolverHelper import SolverHelper

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.initUI()
        self.initText()

    def initUI(self):
        self.textEdit = QTextEdit()
        self.resultText = QTextEdit()
        self.resultText.setFixedHeight(self.resultText.document().size().height() * 2 + self.resultText.contentsMargins().top() * 1)
        self.resultText.setReadOnly(True)

        vbox = QVBoxLayout()
        vbox.addWidget(self.textEdit)
        vbox.addWidget(self.resultText)
        widg = QWidget()
        widg.setLayout(vbox)
        self.setCentralWidget(widg)
        #self.setCentralWidget(self.textEdit)
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

        #self.setPalette(GUI_Settings.palette)
        #self.setGeometry(300, 300)
        self.setMinimumSize(300,300)
        self.center()
        self.setWindowTitle('File dialog')
        self.show()

    def initText(self):
        try:
            f = open(GUI_Settings.lastClauseFile,'r')
            self.textEdit.setText(f.read())
            f.close()
        except IOError:
            f = open(GUI_Settings.lastClauseFile,'a')
            f.close()

    def center(self):

        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())


    def newFile(self):
        self.textEdit.clear()

    def openFile(self):

        fname = QFileDialog.getOpenFileName(self, 'Open file', '',"Text Files (*.txt);;All Files (*)")
        if fname[0]:
            f = open(fname[0], 'r')

            with f:
                data = f.read()
                self.textEdit.setText(data)

    def saveFile(self):
        fname = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "Text Files (*.txt);;CNF Files (*.cnf);;All Files (*)")
        if fname[0]:
            f = open(fname[0], 'w')
            with f:
                f.write(self.textEdit.toPlainText())

    def solve(self):
        clause = self.textEdit.toPlainText()

        result = SolverHelper.solve(clause,self.solversChoice.currentText())
        self.resultText.setPlainText(result)

    def closeEvent(self, event):

        reply = QMessageBox.question(self, 'Message',
                                     "Are you sure to quit?", QMessageBox.No |
                                     QMessageBox.Yes, QMessageBox.No)

        f = open(GUI_Settings.lastClauseFile,'w')
        with f:
            f.write(self.textEdit.toPlainText())

        f.close()

        if reply == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MainWindow()
    #app.setStyle('Windows')
    #print (app.style().metaObject().className())
    #print (QStyleFactory.keys())

    sys.exit(app.exec_())