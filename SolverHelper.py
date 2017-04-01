from GlucoseSolver import GlucoseSolver
from MiniSatSolver import MiniSatSolver
from LingelingSolver import LingelingSolver
from ClauseHelper import ClauseHelper

from enum import Enum


class SolverHelper():

    __solvers = {'Glucose':GlucoseSolver(),'Lingeling':LingelingSolver(),'MiniSat':MiniSatSolver()}

    @staticmethod
    def solve(s, solver = 'Glucose'):
        if ClauseHelper.check_clause(s):
            cnf = ClauseHelper.parse_to_cnf(s)
            mapa, dimacs = ClauseHelper.parse_to_dimacs_pyeda(cnf)
            dimacs = dimacs.__str__()
            model = SolverHelper.__solvers[solver].solve(dimacs.__str__())


            if SolverHelper.__solvers[solver].is_Sat():
                print ("SAT")

                for x in model.keys():
                    print (str(mapa[x]) + "=" + str(model[x]))

            else:
                print ("UNSAT")

        else:
            print ("Bad clause")


    @staticmethod
    def get_solvers():
        return SolverHelper.__solvers.keys()