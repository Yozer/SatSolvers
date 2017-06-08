

import sys,re,random
from pyeda.inter import  *
from pyeda.parsing import lex, boolexpr
from GUI.Settings import ParserSettings
from SolverHelper import SolverHelper

class Generator():

    constraints = {}
    avalanches = ['a1','a2','a3','a4','a5']
    wind = ['w1','w2','w3']
    fog = ['f1','f2','f3']
    temperature = ['t1','t2','t3']
    rain = ['r1','r2','r3']

    trails = { 1:1,2:1,3:2,4:2,5:3,6:3,7:3,8:3,9:3,10:4,11:4}


    constraints['E5'] = {}
    constraints['E5']['d4'] = ['w2', 'w3', 'f2', 'f3', 't2', 't3', 'r2', 'r3', 'a2', 'a3', 'a4', 'a5']
    constraints['E5']['d3'] = ['w2', 'w3', 'f2', 'f3', 't3', 'r2', 'r3', 'a3', 'a4', 'a5']
    constraints['E5']['d2'] = ['w3', 'f3', 't3', 'r3', 'a4', 'a5']
    constraints['E5']['d1'] = ['w3', 'f3', 'r3', 'a4', 'a5']
    constraints['E4'] = {}
    constraints['E4']['d4'] = ['w2', 'w3', 'f2', 'f3', 't3', 'r2', 'r3', 'a2', 'a3', 'a4', 'a5']
    constraints['E4']['d3'] = ['w2', 'w3', 'f2', 'f3', 't3', 'r3', 'a3', 'a4', 'a5']
    constraints['E4']['d2'] = ['w3', 'f3', 'r3', 'a4', 'a5']
    constraints['E4']['d1'] = ['w3', 'r3', 'a4', 'a5']
    constraints['E3'] = {}
    constraints['E3']['d4'] = ['w2', 'w3', 'f2', 'f3', 't3', 'r3', 'a2', 'a3', 'a4', 'a5']
    constraints['E3']['d3'] = ['w2', 'w3', 'f3', 't3', 'r3', 'a3', 'a4', 'a5']
    constraints['E3']['d2'] = ['w3', 'f3', 'a4', 'a5']
    constraints['E3']['d1'] = ['w3', 'a4', 'a5']
    constraints['E2'] = {}
    constraints['E2']['d4'] = ['w2', 'w3', 'f3', 't3', 'r3', 'a2', 'a3', 'a4', 'a5']
    constraints['E2']['d3'] = ['w2', 'w3', 'f3', 't3', 'a3', 'a4', 'a5']
    constraints['E2']['d2'] = ['w3', 'a4', 'a5']
    constraints['E2']['d1'] = ['a4', 'a5']

    @staticmethod
    def generateConditions():
        conditions = []
        random.seed()
        conditions.append(random.choice(Generator.avalanches))
        conditions.append(random.choice(Generator.wind))
        conditions.append(random.choice(Generator.fog))
        conditions.append(random.choice(Generator.temperature))
        conditions.append(random.choice(Generator.rain))

        return conditions

    @staticmethod
    def __addTrailNumber(list,trail):
        return [line+'z'+str(trail) for line in list]

    @staticmethod
    def generateTrail(trail,emergency):
        conditions = Generator.generateConditions()
        AndStr = ParserSettings.And
        OrStr = ParserSettings.Or

        trailDiff = 'd'+str(Generator.trails[trail])
        constraints = Generator.constraints['E'+str(emergency)][trailDiff]

        conditions = Generator.__addTrailNumber(conditions,trail)
        constraints = Generator.__addTrailNumber(constraints,trail)

        condStr = '(' + AndStr.join(map(str,conditions)) + ')'
        consStr = '(' + OrStr.join(map(str,constraints)) + ')'

        result = '(' + condStr + ParserSettings.Implies + trailDiff + AndStr + consStr + AndStr + 'o1)' + AndStr + condStr
        expression = expr(result,True)
        mapa, dimacs = expr2dimacscnf(expression.to_cnf())
        dimacs = dimacs.__str__()
        list = [variable for variable in mapa.keys()]
        variables = []
        for i in range(0,len(list),4):
            variables.append(list[i])

        dimacsStr = ""
        for var in variables:
            dimacsStr+='c '+str(mapa[var])+' = '+str(var) + '\n'

        dimacsStr = re.sub(r'z','@',dimacsStr)
        dimacsStr+=dimacs
        print(result)
        return dimacsStr

    @staticmethod
    def generateDiff(diff,emergency):
        AndStr = ParserSettings.And
        OrStr = ParserSettings.Or
        trails = []
        for key,value in Generator.trails.items():
            if value==diff:
                trails.append(key)

        diffStr = 'd' + str(diff)
        constraint = Generator.constraints['E'+str(emergency)][diffStr]

        constraints = {}
        conditions = {}
        for trail in trails:
            conditions[trail] = Generator.__addTrailNumber(Generator.generateConditions(),trail)
            constraints[trail] = Generator.__addTrailNumber(constraint,trail)

        condStr = []
        consStr = []
        for trail in trails:
            condStr.append('(' + AndStr.join(map(str,conditions[trail])) + ')')
            consStr.append('(' + OrStr.join(map(str,constraints[trail])) + ')')


        result = '(' + AndStr.join(map(str,condStr)) + ParserSettings.Implies + diffStr + AndStr + AndStr.join(map(str,consStr)) + AndStr + 'o1)' + AndStr + AndStr.join(map(str,condStr))
        expression = expr(result,True)
        mapa, dimacs = expr2dimacscnf(expression.to_cnf())
        dimacs = dimacs.__str__()
        list = [variable for variable in mapa.keys()]
        variables = []
        for i in range(0,len(list),4):
            variables.append(list[i])

        dimacsStr = ""
        for var in variables:
            dimacsStr+='c '+str(mapa[var])+' = '+str(var) + '\n'

        dimacsStr = re.sub(r'z','@',dimacsStr)
        dimacsStr+=dimacs
        print(result)
        return dimacsStr

    @staticmethod
    def generateTrailStr(trail,emergency):
        AndStr = ParserSettings.And
        OrStr = ParserSettings.Or

        trailDiff = 'd' + str(Generator.trails[trail])
        constraints = Generator.constraints['E' + str(emergency)][trailDiff]

        conditions = Generator.__addTrailNumber(Generator.generateConditions(), trail)
        constraints = Generator.__addTrailNumber(constraints, trail)

        condStr = '(' + AndStr.join(map(str, conditions)) + ')'
        consStr = '(' + OrStr.join(map(str, constraints)) + ')'

        result = '((' + condStr + ParserSettings.Implies + trailDiff + AndStr + consStr + AndStr + 'o{})'.format(trail) + AndStr + condStr + ')'

        return result

    @staticmethod
    def generateAll(emergency):
        AndStr = ParserSettings.And
        OrStr = ParserSettings.Or

        trailsStr = []

        for (trail,diff) in Generator.trails.items():
            trailsStr.append(Generator.generateTrailStr(trail,emergency))

        result = OrStr.join(map(str,trailsStr))
        for trail in trailsStr:
            expr(trail).to_cnf()


        '''
        expression = expr(result,True)
        mapa, dimacs = expr2dimacscnf(expression.to_cnf())
        dimacs = dimacs.__str__()
        list = [variable for variable in mapa.keys()]
        variables = []
        for i in range(0,len(list),4):
            variables.append(list[i])

        dimacsStr = ""
        for var in variables:
            dimacsStr+='c '+str(mapa[var])+' = '+str(var) + '\n'

        dimacsStr = re.sub(r'z','@',dimacsStr)
        dimacsStr+=dimacs
        '''
        return result








