from AbstractSolver import AbstractSolver


class GlucoseSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/glucose/simp/glucose_static"
        self._params = ["-model"]



    def __str__(self):
        return "Glucose"
