from PyQt5.QtWidgets import QDialog,QVBoxLayout, QComboBox, QLabel, QSpinBox, QGridLayout, QHBoxLayout, QPushButton
from PyQt5.QtCore import QObject,pyqtSignal
from Generator.Generator import Generator


class ResultSignal(QObject):
    result = pyqtSignal(int,int,int,int)


class GeneratorDialog(QDialog):

    dimacs = ""

    def __init__(self,parent = None):
        super(self.__class__,self).__init__(parent=parent)
        self.setModal(True)

        typeBox = QComboBox()
        typeBox.addItem("Trail")
        typeBox.addItem("Difficulty")
        typeBox.addItem("All")
        self.typeBox =  typeBox
        self.typeBox.currentIndexChanged.connect(self.changeType)

        trailStr = QLabel("Trail number")
        trailVal = QSpinBox()
        trailVal.setSingleStep(1)
        trailVal.setRange(1,11)
        self.trailVal = trailVal

        diffStr = QLabel("Difficulty number")
        diffVal = QSpinBox()
        diffVal.setSingleStep(1)
        diffVal.setRange(1,4)
        self.diffVal = diffVal

        emergencyStr = QLabel("Emergency level")
        emergencyVal = QSpinBox()
        emergencyVal.setSingleStep(1)
        emergencyVal.setRange(2,5)
        self.emergencyVal = emergencyVal

        self.trailVal.setDisabled(False)
        self.diffVal.setDisabled(True)
        self.emergencyVal.setDisabled(False)

        vboxLayout = QVBoxLayout()
        vboxLayout.addWidget(typeBox)
        vboxLayout.addWidget(trailStr)
        vboxLayout.addWidget(trailVal)
        vboxLayout.addWidget(diffStr)
        vboxLayout.addWidget(diffVal)
        vboxLayout.addWidget(emergencyStr)
        vboxLayout.addWidget(emergencyVal)
        self.setLayout(vboxLayout)

        hboxLayout = QHBoxLayout()
        cancelButton  = QPushButton("Cancel")
        cancelButton.clicked.connect(self.reject)

        okButton = QPushButton("Ok")
        okButton.clicked.connect(self.resultAccepted)

        hboxLayout.addStretch(1)
        hboxLayout.addWidget(cancelButton)
        hboxLayout.addWidget(okButton)
        vboxLayout.addLayout(hboxLayout)
        self.resultSignal = ResultSignal()


    def resultAccepted(self):
        self.resultSignal.result.emit(self.typeBox.currentIndex(),self.trailVal.value(),self.diffVal.value(),self.emergencyVal.value())
        self.accept()


    @staticmethod
    def getDialog(parent):
        dialog = GeneratorDialog(parent)
        dialog.resultSignal.result.connect(GeneratorDialog.getDimacs)
        returnValue = dialog.exec_()
        if returnValue == QDialog.Rejected:
            return ""
        elif returnValue == QDialog.Accepted:
            return GeneratorDialog.dimacs
        print(dialog.trailVal.value())


    @staticmethod
    def getDimacs(type,trailVal, diffVal, emergencyVal):
        if type == GenType.Trail:
            GeneratorDialog.dimacs = Generator.generateTrail(trailVal,emergencyVal)
        elif type == GenType.Difficulty:
            GeneratorDialog.dimacs = Generator.generateDiff(diffVal,emergencyVal)
        else:
            GeneratorDialog.dimacs = Generator.generateAll(emergencyVal)


    def changeType(self,index):
        if index == GenType.Trail:
            self.trailVal.setDisabled(False)
            self.diffVal.setDisabled(True)
            self.emergencyVal.setDisabled(False)
        elif index == GenType.Difficulty:
            self.trailVal.setDisabled(True)
            self.diffVal.setDisabled(False)
            self.emergencyVal.setDisabled(False)
        elif index == GenType.All:
            self.trailVal.setDisabled(True)
            self.diffVal.setDisabled(True)
            self.emergencyVal.setDisabled(False)
        else:
            self.trailVal.setDisabled(True)
            self.diffVal.setDisabled(True)
            self.emergencyVal.setDisabled(True)


class GenType():
    Trail = 0
    Difficulty = 1
    All = 2