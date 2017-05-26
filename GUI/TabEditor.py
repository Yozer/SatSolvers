import sys,re
from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QFileDialog,QTabBar,QTabWidget, QSplitter,QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget,QHBoxLayout)
from PyQt5.QtGui import QIcon, QPalette
from PyQt5.QtCore import Qt,QFileInfo
from Editor import CodeEditor
sys.path.append("../")
from GUI.Settings import Settings
from SolverHelper import SolverHelper
from GUI.Editor import CodeEditor
from GUI.ConfigDialog import ConfigDialog
from GUI.SolveThread import SolveThread



class TabEditor(QSplitter):

    def __init__(self,parent,settings,executeButton,filename = ""):
        super(self.__class__, self).__init__()
        self.textEdit = CodeEditor(parent,settings)
        self.resultText = QTextEdit()
        self.resultText.setReadOnly(True)

        self.addWidget(self.textEdit)
        self.addWidget(self.resultText)
        self.setStretchFactor(0, 10)
        self.setStretchFactor(1, 1)
        self.settings = settings
        self.executeButton = executeButton
        self.__updateFile(filename)
        self.__loadFile(filename)


    def __updateFile(self,filename):
        if filename=="":
            self.file = ""
            self.fileType = FileType.CNF
            self.title = "untitled.txt"
        else:
            self.file = filename
            if re.match(r'.*\.cnf', filename):
                self.fileType = FileType.Dimacs
            else:
                self.fileType = FileType.CNF
            self.title = QFileInfo(filename).fileName()
        print(self.title)

    def saveFile(self):
        if self.file != "":
            f = open(self.file, 'w')
            with f:
                f.write(self.textEdit.toPlainText())
            f.close()

        else:
            fname = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "Text Files (*.txt);;CNF Files (*.cnf);;All Files (*)")
            if fname[0]:
                f = open(fname[0], 'w')
                with f:
                    f.write(self.textEdit.toPlainText())
                    f.close()
            self.__updateFile(fname[0])
        self.textEdit.text_was_changed = False

    def __loadFile(self,filename):
        if filename == "":
            self.__setText("")
        else:
            f = open(filename, 'r')
            with f:
                data = f.read()
                self.__setText(data)
            f.close()
        self.textEdit.text_was_changed = False

    def __setText(self,s):
        self.textEdit.setText(s)

    def export2dimacs(self):
        dimacs = SolverHelper.toDimacs(self.textEdit.toPlainTextForParser(),self.settings.parser)
        if dimacs[0] == 'p':
            fname = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "",
                                                "Dimacs (*.cnf)")
            if fname[0]:
                f = open(fname[0]+'.cnf', 'w')
                with f:
                    f.write(dimacs)
                f.close()
        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Critical)
            msg.setText("Error")
            msg.setInformativeText(dimacs)
            msg.setWindowTitle("Error")
            msg.exec_()


    def solveC(self,solver):
        self.resultText.clear()

        if self.fileType == FileType.Dimacs:
            clause = self.textEdit.toPlainText()
            thread = SolveThread(clause,solver,self.settings.parser,True)
        else:
            clause = self.textEdit.toPlainTextForParser()
            thread = SolveThread(clause,solver,self.settings.parser)

        print("start")
        #thread.signal.connect(self.__setResult)
        thread.signal.conn(self.__setResult)
        thread.start()
        print("finished")

    def solve(self,solver):
        clause = self.textEdit.toPlainTextForParser()
        self.resultText.clear()
        if self.fileType == FileType.Dimacs:
            result = SolverHelper.solveDimacs(self.textEdit.toPlainText(),solver)
        else:
            result = SolverHelper.solve(clause,self.settings.parser,solver)

        if result[0] == 'S' or result[0] == 'U':
            self.resultText.setPlainText(result)

        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Critical)
            msg.setText("Error")
            msg.setInformativeText(result)
            msg.setWindowTitle("Error")
            msg.exec_()

    def __setResult(self,result):
        print("in setResult")
        if result[0] == 'S' or result[0] == 'U':
            self.resultText.setPlainText(result)

        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Critical)
            msg.setText("Error")
            msg.setInformativeText(result)
            msg.setWindowTitle("Error")
            msg.exec_()
        self.executeButton.setEnabled(True)

    def highlightCurrentLine(self):
        self.textEdit.highlightCurrentLine()

    def textChanged(self):
        return  self.textEdit.text_was_changed
class FileType():
    CNF = 1
    Dimacs = 2
    Other = 3