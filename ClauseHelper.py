import re as re


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
