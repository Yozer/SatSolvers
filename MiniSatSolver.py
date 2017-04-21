from AbstractSolver import AbstractSolver


class MiniSatSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/minisat/simp/minisat_static"
        self._params = ["-verb=0"]

    def __str__(self):
        return "MiniSat"
