from AbstractSolver import AbstractSolver


class LingelingSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/lingeling/lingeling"
        self._params = ["-q"]

    def __str__(self):
        return "Lingeling"
