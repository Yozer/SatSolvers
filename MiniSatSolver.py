from AbstractSolver import AbstractSolver


class MiniSatSolver(AbstractSolver):
    def __init__(self):
        AbstractSolver._exe = "minisat"
        AbstractSolver._params = ["-verb=0"]

    def __str__(self):
        return "MiniSat"
