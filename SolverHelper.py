from ClauseHelper import ClauseHelper
from Solvers import *
from pyeda.boolalg import expr

# parser skraca formuły, np. x & ~x - da 0, czyli unsat, a y | (x & ~x) da tylko y
class SolverHelper():

    __solvers = {'Glucose':GlucoseSolver(),'Lingeling':LingelingSolver(),'MiniSat':MiniSatSolver(),'Riss':RissSolver(),
                 'JeruSat':JeruSatSolver(),'RSAT':RSatSolver(),"Limmat":LimmatSolver()}

    @staticmethod
    def toDimacs(s,settings):
        if ClauseHelper.check_clause(s):
            cnf = ClauseHelper.parse_to_cnf(s,settings)
            if False == isinstance(cnf[0],expr.Expression):
                return cnf
            if isinstance(cnf[0],expr._Zero):
                return "Pusta klauzula"

            mapa, dimacs = ClauseHelper.parse_to_dimacs_pyeda(cnf[0])

            return dimacs.__str__()
        else:
            return "Pusta klauzula"


    @staticmethod
    def solveDimacs(s, solver = 'Glucose'):
        model = SolverHelper.__solvers[solver].solve(s)

        if SolverHelper.__solvers[solver].is_Sat():

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



    @staticmethod
    def solve(s, settings, solver = 'Glucose'):
        if ClauseHelper.check_clause(s):
            cnf = ClauseHelper.parse_to_cnf(s,settings)
            if False == isinstance(cnf[0],expr.Expression):
                return cnf
            if isinstance(cnf[0],expr._Zero):
                return "UNSAT"

            mapa, dimacs = ClauseHelper.parse_to_dimacs_pyeda(cnf[0])
            dimacs = dimacs.__str__()
            model = SolverHelper.__solvers[solver].solve(dimacs.__str__())

            if SolverHelper.__solvers[solver].is_Sat():

                result = "SAT\n"
                '''
                for x in model.keys():
                    result+=str(mapa[x]) + "=" + str(model[x]) + "\n"
                '''

                for x in cnf[1]:
                    if expr.expr(x) in mapa:
                        result += str(x) + "=" + str(model[mapa[expr.expr(x)]]) + "\n"
                    else:
                        result += str(x) + "=" + str(True) + "\n"

                return result
            else:
                return "UNSAT"

        else:
            return "Bad clause"


    @staticmethod
    def get_solvers():
        return SolverHelper.__solvers.keys()