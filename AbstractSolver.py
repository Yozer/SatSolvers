import abc as abc
import subprocess
from enum import Enum
import os

class SolverResult(Enum):
    UNSAT = 1
    SAT = 2
    UNKNOWN = 3


class AbstractSolver(object, metaclass=abc.ABCMeta):
    _exe = ""
    _params = []
    _file = False
    __sat = SolverResult.UNKNOWN

    def solve(self, dimacs):
        if self._file:
            values = self.__call_file_process(dimacs)
        else:
            values = self.__call_process(dimacs)
        return {abs(x): x > 0 for x in values}

    def __call_file_process(self,dimacs):
        file = "tmpClause.cnf"

        f = open(file, 'w')
        with f:
            f.write(dimacs)

        proc_info = [self._exe,file] + self._params
        proc = subprocess.Popen(proc_info,
                                stdout=subprocess.PIPE)
        #proc.stdin.close()
        proc.wait()
        result = proc.stdout.read().decode('ascii')

        os.remove(file)

        return self.__parse_output(result)

    def __call_process(self, dimacs):
        proc_info = [self._exe] + self._params
        proc = subprocess.Popen(proc_info,
                                stdout=subprocess.PIPE,
                                stdin=subprocess.PIPE)

        proc.stdin.write(dimacs.encode('ascii'))
        proc.stdin.close()
        proc.wait()
        result = proc.stdout.read().decode('ascii')

        return self.__parse_output(result)

    def __parse_output(self, output):
        values = []

        for line in output.split("\n"):
            line = line.strip()
            first_two = line[:2]
            if len(line) == 0:
                continue

            if first_two == "s ":
                if "UNSATISFIABLE" in line or "UNSAT" in line:
                    self.__sat = SolverResult.UNSAT
                elif "SATISFIABLE" in line or "SAT" in line:
                    self.__sat = SolverResult.SAT
            elif first_two == "v ":
                for v in map(int, line[2:].split()):
                    if v != 0:
                        values.append(v)

        return values

    def is_Sat(self):
        return self.__sat == SolverResult.SAT

    @abc.abstractmethod
    def __str__(self):
        raise NotImplementedError('users must define __str__ to use this base class')