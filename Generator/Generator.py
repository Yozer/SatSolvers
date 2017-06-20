

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


    @staticmethod
    def oneConditon(trail):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        NotStr = ParserSettings.Not

        avalStr = Generator.notConditionsGenerate(Generator.__addTrailNumber(Generator.avalanches,trail))
        windStr = Generator.notConditionsGenerate(Generator.__addTrailNumber(Generator.wind,trail))
        fogStr = Generator.notConditionsGenerate(Generator.__addTrailNumber(Generator.fog,trail))
        tempStr = Generator.notConditionsGenerate(Generator.__addTrailNumber(Generator.temperature,trail))
        rainStr = Generator.notConditionsGenerate(Generator.__addTrailNumber(Generator.rain,trail))
        list = [avalStr,windStr,fogStr,tempStr,rainStr]

        return list

    @staticmethod
    def notConditionsGenerate(list):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        NotStr = ParserSettings.Not

        aval = []
        for a in list:
            tmpList = [a]
            for ab in list:
                if ab!=a:
                    tmpList.append(NotStr + ab)

            tmpStr = "(" + AndStr.join(map(str,tmpList)) + ")"
            aval.append(tmpStr)

        avalStr = "(" + OrStr.join(map(str,aval)) + ")"
        return avalStr

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
    def __toDimacs(result):
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

        return dimacsStr


    @staticmethod
    def toDimacs(text):
        return Generator.__toDimacs(text)


    @staticmethod
    def generateTrail(trail,emergency):
        result = Generator.generateTrailStr(trail,emergency,True)
        dimacsStr = Generator.__toDimacs(result)
        return dimacsStr

    @staticmethod
    def generateTrailStr(trail,emergency, forDimacs = False):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '

        trailDiff = 'd' + str(Generator.trails[trail])
        constraints = Generator.constraints['E' + str(emergency)][trailDiff]

        conditions = Generator.__addTrailNumber(Generator.generateConditions(), trail)
        constraints = Generator.__addTrailNumber(constraints, trail)

        condStr = '(' + AndStr.join(map(str, conditions)) + ')'
        consStr = '(' + OrStr.join(map(str, constraints)) + ')'

        result = '((' + condStr + ParserSettings.Implies + trailDiff + AndStr + consStr + AndStr + 'o{})'.format(trail) + AndStr + condStr + ')'

        if forDimacs:
            return result + AndStr + AndStr.join(map(str, Generator.oneConditon(trail)))
        else:
            return result + '\n' + '\n'.join(map(str, Generator.oneConditon(trail))) + '\n'

    @staticmethod
    def generateDiff(diff,emergency):
        result = Generator.generateDiffStr(diff,emergency,True)
        dimacsStr = Generator.__toDimacs(result)
        return dimacsStr


    @staticmethod
    def generateDiffStr(diff,emergency, forDimacs = False):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        trails = []
        for key, value in Generator.trails.items():
            if value == diff:
                trails.append(key)

        diffStr = 'd' + str(diff)
        constraint = Generator.constraints['E' + str(emergency)][diffStr]

        constraints = {}
        conditions = {}
        for trail in trails:
            conditions[trail] = Generator.__addTrailNumber(Generator.generateConditions(), trail)
            constraints[trail] = Generator.__addTrailNumber(constraint, trail)

        condStr = []
        consStr = []
        for trail in trails:
            condStr.append('(' + AndStr.join(map(str, conditions[trail])) + ')')
            consStr.append('(' + OrStr.join(map(str, constraints[trail])) + ')')

        result = '(' + AndStr.join(map(str, condStr)) + ParserSettings.Implies + diffStr + AndStr + AndStr.join(
            map(str, consStr)) + AndStr + 'o1)' + AndStr + AndStr.join(map(str, condStr))


        if forDimacs:
            trailStr = []
            for trail in trails:
                trailStr.append(AndStr.join(map(str,Generator.oneConditon(trail))))

            return result + AndStr + AndStr.join(map(str, trailStr))
        else:
            trailStr = []
            for trail in trails:
                trailStr.append('\n'.join(map(str,Generator.oneConditon(trail))))

            return result + '\n' + '\n'.join(map(str, trailStr)) + '\n'

    @staticmethod
    def generateAll(emergency):
        #result = Generator.generateAllStr(emergency,True)

        #dimacsStr = Generator.__toDimacs(result)


        return Generator.generateAllDimacs(emergency)

    @staticmethod
    def generateAllStr(emergency,forDimacs = False):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '

        trailsStr = []
        result,cond = Generator.genAllStr(emergency,forDimacs)
        if forDimacs:
            return '('+result+')&' + cond
        else:
            return result+'\n'+cond

    @staticmethod
    def genAllStr(emergency,forDimacs = True):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '

        trailsStr = []

        for (trail,diff) in Generator.trails.items():
            trailsStr.append(Generator.generateTrailStr2(trail,emergency,forDimacs))

        result = OrStr.join(map(str,trailsStr))
        cond = []
        for (trail,diff) in Generator.trails.items():
            conditions = Generator.__addTrailNumber(Generator.generateConditions(), trail)
            condStr = '(' + AndStr.join(map(str, conditions)) + ')'
            cond.append(condStr)

        if forDimacs:
            return result,AndStr.join(cond)
        else:
            return result, '\n'.join(cond)


    @staticmethod
    def generateTrailStr1(trail,emergency, forDimacs = False):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '

        trailDiff = 'd' + str(Generator.trails[trail])
        constraints = Generator.constraints['E' + str(emergency)][trailDiff]

        conditions = Generator.__addTrailNumber(Generator.generateConditions(), trail)
        constraints = Generator.__addTrailNumber(constraints, trail)

        condStr = '(' + AndStr.join(map(str, conditions)) + ')'
        consStr = '(' + OrStr.join(map(str, constraints)) + ')'

        result = '((' + condStr + ParserSettings.Implies + trailDiff + 'z{}'.format(trail) + AndStr + consStr + AndStr + 'o{})'.format(trail) + AndStr + condStr + ')'

        if forDimacs:
            return result
        else:
            return result + '\n' + '\n'.join(map(str, Generator.oneConditon(trail))) + '\n'

    @staticmethod
    def generateTrailStr2(trail,emergency, forDimacs = False):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '

        trailDiff = 'd' + str(Generator.trails[trail])
        constraints = Generator.constraints['E' + str(emergency)][trailDiff]

        conditions = Generator.__addTrailNumber(Generator.generateConditions(), trail)
        constraints = Generator.__addTrailNumber(constraints, trail)

        condStr = '(' + AndStr.join(map(str, conditions)) + ')'
        consStr = '(' + OrStr.join(map(str, constraints)) + ')'

        result = '('+ trailDiff + AndStr + consStr + AndStr + 'o{})'.format(trail)

        return result

    @staticmethod
    def generateAllDimacs(emergency):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        dnfAll = Generator.generateAllDnf()
        list = expr(dnfAll).to_dnf().__str__().split('And')
        variablesSet = set()
        ands = []

        for dnf in list:
            dnf = re.sub(r'\(', '', dnf)
            dnf = re.sub(r'\)', '', dnf)
            dnf = re.sub(r'Or', '', dnf)
            dnf = re.sub('[\s+]', '', dnf)
            if dnf != '':
                res = dnf.split(',')
                if res[len(res) - 1] == "":
                    res.pop()
                ands.append(res)

        for line in ands:
            for x in line:
                variablesSet.add(x)

        indexes = [0 for i in range(0, len(ands))]
        ors = []
        mainIndex = len(ands) - 1
        clauseNr = 0
        while True:
            clauseNr = clauseNr + 1
            variables = set()
            for i in range(0, len(ands)):
                variables.add(ands[i][indexes[i]])

            ors.append(variables)
            mainIndex = len(ands) - 1
            while mainIndex >= 0:
                indexes[mainIndex] = indexes[mainIndex] + 1
                if len(ands[mainIndex]) == indexes[mainIndex]:
                    mainIndex = mainIndex - 1
                else:
                    break
            if indexes[0] >= len(ands[0]):
                break

            for i in range(mainIndex + 1, len(ands)):
                indexes[i] = 0

        #orsStr = [' | '.join(map(str, line)) for line in ors]
        #orsStr = ['(' + line + ')' for line in orsStr]
        #result = '&'.join(map(str, orsStr))

        constraints = {}

        for (trail,diff) in Generator.trails.items():
            constraint = Generator.__addTrailNumber(Generator.constraints['E'+str(emergency)]['d'+str(diff)],trail)
            for x in constraint:
                variablesSet.add(x)
            #constraints[trail] = '(' + OrStr.join(map(str,constraint)) + ')'
            constraints['c'+str(trail)] = constraint
            variablesSet.remove('c'+str(trail))


        variablesNumber = {}
        count = 1
        for variable in variablesSet:
            variablesNumber[variable] = count
            count = count + 1

        dimacsStr = ""
        for (variable,nr) in variablesNumber.items():
            variable = re.sub(r'z', '@', variable)

            dimacsStr +='c {} = {}\n'.format(nr,variable)

        dimacsStr +='p cnf {} {} \n'.format(len(variablesNumber),clauseNr)
        for line in ors:
            for x in line:
                if x in variablesNumber:
                    dimacsStr +=str(variablesNumber[x])+' '
                else:
                    for z in constraints[x]:
                        dimacsStr +=str(variablesNumber[z]) + ' '
            dimacsStr +='0\n'

        return dimacsStr

    @staticmethod
    def generateAllDnfOnly3():
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        trailsStr = []

        ran = range(1,4)
        for trail in ran:
            diff = Generator.trails[trail]
            trailsStr.append('('+'d{}'.format(diff) +AndStr+ 'c{}'.format(trail) + AndStr + 'o{})'.format(trail))

        return '('+OrStr.join(trailsStr)+')'

    @staticmethod
    def generateAllDnf():
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        trailsStr = []

        for (trail, diff) in Generator.trails.items():
            trailsStr.append('('+'d{}'.format(diff) +AndStr+ 'c{}'.format(trail) + AndStr + 'o{})'.format(trail))

        return '('+OrStr.join(trailsStr)+')'


    @staticmethod
    def generateAllCnf(emergency,name):
        AndStr = ' ' + ParserSettings.And + ' '
        OrStr = ' ' + ParserSettings.Or + ' '
        dnfAll = Generator.generateAllDnf()
        list = expr(dnfAll).to_dnf().__str__().split('And')
        variablesSet = set()
        ands = []

        for dnf in list:
            dnf = re.sub(r'\(', '', dnf)
            dnf = re.sub(r'\)', '', dnf)
            dnf = re.sub(r'Or', '', dnf)
            dnf = re.sub('[\s+]', '', dnf)
            if dnf != '':
                res = dnf.split(',')
                if res[len(res) - 1] == "":
                    res.pop()
                ands.append(res)

        for line in ands:
            for x in line:
                variablesSet.add(x)

        indexes = [0 for i in range(0, len(ands))]
        ors = []
        mainIndex = len(ands) - 1
        clauseNr = 0
        while True:
            clauseNr = clauseNr + 1
            variables = set()
            for i in range(0, len(ands)):
                variables.add(ands[i][indexes[i]])

            ors.append(variables)
            mainIndex = len(ands) - 1
            while mainIndex >= 0:
                indexes[mainIndex] = indexes[mainIndex] + 1
                if len(ands[mainIndex]) == indexes[mainIndex]:
                    mainIndex = mainIndex - 1
                else:
                    break
            if indexes[0] >= len(ands[0]):
                break

            for i in range(mainIndex + 1, len(ands)):
                indexes[i] = 0

        #orsStr = [' | '.join(map(str, line)) for line in ors]
        #orsStr = ['(' + line + ')' for line in orsStr]
        #result = '&'.join(map(str, orsStr))

        constraints = {}

        ran = range(1,int(len(variablesSet)/3)+1)
        for trail in ran:
            diff = Generator.trails[trail]
            constraint = Generator.__addTrailNumber(Generator.constraints['E'+str(emergency)]['d'+str(diff)],trail)
            for x in constraint:
                variablesSet.add(x)
            #constraints[trail] = '(' + OrStr.join(map(str,constraint)) + ')'
            constraints['c'+str(trail)] = constraint
            variablesSet.remove('c'+str(trail))


        variablesNumber = {}
        count = 1
        for variable in variablesSet:
            variablesNumber[variable] = count
            count = count + 1

        f = open(name,'w')
        for (variable,nr) in variablesNumber.items():
            variable = re.sub(r'z', '@', variable)

            f.write('c {} = {}\n'.format(nr,variable))

        f.write('p cnf {} {} \n'.format(len(variablesNumber),clauseNr))
        for line in ors:
            for x in line:
                if x in variablesNumber:
                    f.write(str(variablesNumber[x])+' ')
                else:
                    for z in constraints[x]:
                        f.write(str(variablesNumber[z]) + ' ')
            f.write('0\n')

        f.close()
