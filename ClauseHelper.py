import re as re
from pyeda.inter import  *

class ClauseHelper(object):

    @staticmethod
    def parse_to_dimacs(s):
        v = set()
        clauses = []
        if s.count('&') == 0:
            s += '&'

        for line in s.split("&"):
            m = re.findall('(~?)x([0-9]+)', line)
            if len(m) == 0:
                continue

            data = [-int(b) if a == '~' else int(b) for (a, b) in m]
            v.update([abs(x) for x in data])
            data.append(0)
            clauses.append(" ".join([str(x) for x in data]))

        variables = list(v)
        variables.sort()
        return "p cnf " + str(max(v)) + " " + str(len(clauses)) + "\n" + "\n".join(clauses), variables

    @staticmethod
    def check_clause(s):
        if (s == ""):
            return False
        else:
            return True

    @staticmethod
    def is_cnf(s):
        return expr(s).is_cnf()

    @staticmethod
    def parse_to_cnf(s):
        clause = expr(s)
        return clause.to_cnf()

    @staticmethod
    def parse_to_dimacs_pyeda(expression):

        return expr2dimacscnf(expression.to_cnf())