from AbstractSolver import AbstractSolver


class LingelingSolver(AbstractSolver):
    def __init__(self):
        AbstractSolver._exe = "lingeling"
        AbstractSolver._params = ["-q"]

    def __str__(self):
        return "Lingeling"
