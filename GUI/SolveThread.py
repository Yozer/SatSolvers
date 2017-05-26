from PyQt5.QtCore import QThread, QObject,pyqtSignal
import sys

sys.path.append("../")
from SolverHelper import SolverHelper

class ResultSignal(QObject):

    resultSignal = pyqtSignal(str)

    def conn(self,slot):
        self.resultSignal.connect(slot)

    def emit(self,s):
        self.resultSignal.emit(s)
class SolveThread(QThread):

    def __init__(self,clause,solver,settings,Dimacs = False):
        QThread.__init__(self)
        self.clause = clause
        self.solver = solver
        self.settings = settings
        self.dimacs = Dimacs
        self.signal = ResultSignal()

    def __del__(self):
        self.wait()

    def run(self):
        if self.dimacs:
            result = SolverHelper.solveDimacs(self.clause,self.solver)
        else:
            result = SolverHelper.solve(self.clause,self.settings,self.solver)

        self.signal.emit(result)