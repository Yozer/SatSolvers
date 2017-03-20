from GlucoseSolver import GlucoseSolver
from MiniSatSolver import MiniSatSolver
from LingelingSolver import LingelingSolver
from ClauseHelper import ClauseHelper

(dimacs, variables) = ClauseHelper.parse_to_dimacs("(x1 | ~x5 | x4) ")
# (dimacs, variables) = ClauseHelper.parse_to_dimacs("(x1 | ~x5 | x4) & (~x1 | x5 | x3 | x4)")


solver = GlucoseSolver()
model = solver.solve(dimacs)
for v in variables:
    print("x" + str(v) + "=" + str(model[v]))
