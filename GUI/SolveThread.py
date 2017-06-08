from PyQt5.QtCore import QThread, QObject,pyqtSignal, QRunnable
from PyQt5.Qt import QApplication
import sys

sys.path.append("../")
from SolverHelper import SolverHelper
from Solvers import GlucoseSolver

class ResultSignal(QObject):

    resultSignal = pyqtSignal(str)

    def conn(self,slot):
        self.resultSignal.connect(slot)

    def emit(self,s):
        self.resultSignal.emit(s)

class SolveThread(QRunnable):

    def __init__(self,clause,solver,settings,Dimacs = False):
        QThread.__init__(self)
        self.clause = clause
        self.solver = solver
        self.settings = settings
        self.dimacs = Dimacs
        self.signal = ResultSignal()


    def solveDimacs(self,s):
        solver = GlucoseSolver()
        model = solver.solve(s)


        if solver.is_Sat():

            result = "SAT\n"
            '''
            for x in model.keys():
                result+=str(mapa[x]) + "=" + str(model[x]) + "\n"
            '''
            for x in model.keys():
                result += str(x) + '=' + str(model[x]) + '\n'


            return result
        else:
            return "UNSAT"

    def run(self):

        if self.dimacs:
            result = SolverHelper.solveDimacs(self.clause,self.solver)
        else:
            result = SolverHelper.solve(self.clause,self.settings,self.solver)

        self.signal.emit(result)