from AbstractSolver import AbstractSolver


class GlucoseSolver(AbstractSolver):
    def __init__(self):
        AbstractSolver._exe = "glucose"
        AbstractSolver._params = ["-model"]

    def __str__(self):
        return "Glucose"
