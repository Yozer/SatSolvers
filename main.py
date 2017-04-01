from GlucoseSolver import GlucoseSolver
from MiniSatSolver import MiniSatSolver
from LingelingSolver import LingelingSolver
from ClauseHelper import ClauseHelper


clause = "(x1 | ~x5 | x4) & ( ~x1 | x3)"

#clause = "(x1) & (~x1)"
(dimacs, variables) = ClauseHelper.parse_to_dimacs(clause)
# (dimacs, variables) = ClauseHelper.parse_to_dimacs("(x1 | ~x5 | x4) & (~x1 | x5 | x3 | x4)")



solver = GlucoseSolver()
#solver = MiniSatSolver()
#solver = LingelingSolver()
model = solver.solve(dimacs)


if solver.is_Sat() :
    print ("SAT")

    for v in variables:
        print("x" + str(v) + "=" + str(model[v]))

else :
    print ("UNSAT")
