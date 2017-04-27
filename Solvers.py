from AbstractSolver import AbstractSolver
import abc as abc
import subprocess
from enum import Enum


class RissSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/riss/riss"
        self._params = ["-config=505-M"]
        self._file = False


    def __str__(self):
        return "Riss"


class GlucoseSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/glucose/simp/glucose_static"
        self._params = ["-model"]
        self._file = False



    def __str__(self):
        return "Glucose"


class LingelingSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/lingeling/lingeling"
        self._params = ["-q"]
        self._file = False

    def __str__(self):
        return "Lingeling"


class MiniSatSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/minisat/simp/minisat_static"
        self._params = ["-verb=0"]
        self._file = False

    def __str__(self):
        return "MiniSat"

class LimmatSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/limmat/limmat"
        self._params = []
        self._file = False

    def __str__(self):
        return "Limmat"


class JeruSatSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/jerusat/Jerusat1.3"
        self._params = []
        self._file = True

    def __str__(self):
        return "JeruSat"


class RSatSolver(AbstractSolver):
    def __init__(self):
        self._exe = "../solvers/rsat/rsat"
        self._params = ["-s"]
        self._file = True

    def __str__(self):
        return "RSat"

