from GlucoseSolver import GlucoseSolver
from MiniSatSolver import MiniSatSolver
from LingelingSolver import LingelingSolver
from ClauseHelper import ClauseHelper

from pyeda.boolalg import expr

# parser skraca formuły, np. x & ~x - da 0, czyli unsat, a y | (x & ~x) da tylko y
class SolverHelper():

    __solvers = {'Glucose':GlucoseSolver(),'Lingeling':LingelingSolver(),'MiniSat':MiniSatSolver()}

    @staticmethod
    def solve(s, solver = 'Glucose'):
        if ClauseHelper.check_clause(s):
            cnf = ClauseHelper.parse_to_cnf(s)
            if False == isinstance(cnf,expr.Expression):
                return cnf
            if isinstance(cnf,expr._Zero):
                return "UNSAT"


            mapa, dimacs = ClauseHelper.parse_to_dimacs_pyeda(cnf)
            dimacs = dimacs.__str__()
            model = SolverHelper.__solvers[solver].solve(dimacs.__str__())


            if SolverHelper.__solvers[solver].is_Sat():

                result = "SAT\n"

                for x in model.keys():
                    result+=str(mapa[x]) + "=" + str(model[x]) + "\n"

                return result
            else:
                return "UNSAT"

        else:
            return "Bad clause"


    @staticmethod
    def get_solvers():
        return SolverHelper.__solvers.keys()