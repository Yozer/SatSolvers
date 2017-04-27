/*------------------------------------------------------------------------*\
Jerusat 1.3 SAT Solver
(C)2001-2003, Nadel Alexander.

 All rights reserved. Redistribution and use in source and binary forms, with
 or without modification, are permitted provided that the following
 conditions are met:
 
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
  
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
   
 3. All advertising materials mentioning features or use of this software
	must display the following acknowledgement:
	
 This product includes software developed by Nadel Alexander
	 
	  
   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------*/

#ifndef CCC_GLOBALS_H
#define CCC_GLOBALS_H

#include <stdio.h>
#include "C4dataStruct.h"
#include "C4FndBg.h"

FILE *inFile, *outFile;
typedef char Boolean;
#define FALSE 0
#define TRUE 1

#ifdef _DEBUG
	#define MAX_BUFF_SZ 200000
	#define BUFF_PRINT_PORTION 200000
#endif

#ifndef _DEBUG
	#define MAX_BUFF_SZ 10000
	#define BUFF_PRINT_PORTION 10000
#endif

char buff[MAX_BUFF_SZ];
int buffInd;

typedef struct CnfInfo_
{
	char* clssBuff;
	unsigned long currOvBuffSize;
	unsigned long initVarsNum;
	unsigned long initClssNum;
	unsigned long sizeOfBuffForCnf;
} CnfInfo;

typedef enum Sat_
{
	unknown = 0,
	sat = 1,
	unsat = 2,
	cutoff = 3
} Sat;
//Cnf* BuildClausesFromFile(unsigned long overallLitsNum, unsigned long cutOff, double maxClsLenToRec, double maxCCBoxesInCC, unsigned long cbNumToRecAnyway, long heur, unsigned short qsNum, double maxInterval, double notToChkConfsFrom);
//Cnf* BuildClauses(CnfInfo* cnfInfo, unsigned long cutOff, double maxClsLenToRec, double maxCCBoxesInCC, unsigned long cbNumToRecAnyway, long heur, unsigned short qsNum, double maxInterval, double notToChkConfsFrom, double heurPartOfS1, unsigned long restartAft, unsigned long restartAdd, long clsDensFromNotInc, Boolean ifAdd1UipClause);
void FreeCnf(Cnf* cnf);

long RandFrom0ToNMin1(long n);

double GetLitsVal(Cnf* cnf, long litNum);
char GetVarsSign(Cnf* cnf, long litNum);

Sat AlgMain(Cnf* cnf, double initTime, long cutOffTime);
CCBox* PushD1Block(Cnf* cnf, CCBox* clsWithContr);
long IVSPushOrPrepush(struct Cnf_* cnf, long litNum);
char GetStatLetter(Cnf* cnf, long litNum, unsigned long implStat);
CCBox* Imply(Cnf* cnf);
void ExchWchInfo(Cnf* cnf, CCBox* wCCB, CCBox* newWCCB);
void IVSPushOrPrepushToBeImplied(Cnf* cnf);
void PrepVarToImpl(Cnf* cnf, long litNum, CCBox* implClsPtr, unsigned long implStat, long implLayer);

long* GetDelClsLit(CCBox* ccb);
CCArrowsBox* GetD1ArrFromCC(CCBox* ccb);
CCArrowsBox* GetDelUsedConf(CCBox* ccb);
CCArrowsBox* GetAddedCCList(CCBox* ccb);
CCArrowsBox* GetDelCls(CCBox* ccb);
CCArrowsBox* WchFromCcBox(CCBox* ccb, CCBox* wDownCcb);
CCArrowsBox* RfaisFromCcBox(CCBox* rfais2FromDownCCBox);
CCArrowsBox* Bot2FromCcBox(CCBox* bot2FromDownCCBox);

void InsWCCBoxIntoVarsWList(Cnf* cnf, CCBox* ccbToIns, CCBox* wDownCcb);
void InsCCIntoLstBeg(Cnf* cnf, CCBox** lstBeg, CCBox* ccb, CCArrowsBox*(*GetArrFromCC)(CCBox* ccb));
void RemWCCBoxFromVarsWList(Cnf* cnf, CCBox* ccbToRem, CCBox* wDownCcb);
void RemCCFromLst(Cnf* cnf, CCBox** lstBeg, CCBox* ccb, CCArrowsBox*(*GetArrFromCC)(CCBox* ccb));

void GetVarsInConf(Cnf* cnf, CCBox* clsWithContradiction);
CCBox* RecClsAndConf(Cnf* cnf, char/*Boolean*/* ifUpdAisAft);
CCBox* RecConf(Cnf* cnf, long addLitNum);
void UpdateAddImplStInfo(Cnf* cnf, CCBox* lastCCBox, long varNumToChk);
void RemFromAisOnUnimply(Cnf* cnf, long litUnimpl);
void PushIntoAddImplSt(Cnf* cnf, long litNum);
void PrepFirstConfForChk(Cnf* cnf, long headD2VarNum);
Boolean PassToNextConfAftFuthExSt(Cnf* cnf);
Boolean PassToNextConfBefFuthExSt(Cnf* cnf);
void AnalyzeNewConflict(Cnf* cnf, CCBox* clsWithContradiction, ConfCircum cc);

void PrintConfToBeChecked(Cnf* cnf, Var* var);
void DelClssFromList(Cnf* cnf, long litNum);

void UnimplyChangeSide(struct Cnf_* cnf, long litNum);
void UnimplyD1Var(struct Cnf_* cnf, long litNum);
void UnimplyD2Var(struct Cnf_* cnf, long litNum);
void UnimplyD2VarDelRemD1Confs(struct Cnf_* cnf, long litNum);
void UnimplyAisVar(struct Cnf_* cnf, long litNum);
void UnimplyConfVar(struct Cnf_* cnf, long litNum);
void UnimplyOtherVars(struct Cnf_* cnf, long litNum);

void DelUsedConfsFromList(Cnf* cnf, long litNum);
void DelConfFromAddedCCList(Cnf* cnf, CCBox* lastCCBOfCC, long implLvlMmTill);
void DelClssFromList(Cnf* cnf, long litNum);
void InsToEndOfConfLst(Cnf* cnf, CCBox* lastWCCB);
void RemFromConfLst(Cnf* cnf, CCBox* lastWCCB);

void RecreateConfOfAISVarIfNeeded(Cnf* cnf, long varNum);
void DelCCsNotIncAISLeave(Cnf* cnf, CCBox* ccbFrom, CCBox* ccbTill);
Boolean IfAddToAis(Cnf* cnf, long varNumToChk, long varNumInImplStTill);
void OutputTrace(Cnf* cnf, char* proc, int line, char* module, char* buff);
void PrintCCGivenCCBox(Cnf* cnf, CCBox* cc, Boolean contPrint, Boolean noWchChk);
char GetAisLetter(Cnf* cnf, long litNum, int aisStat);

void GoThrowList(CCBox** lstBeg, CCArrowsBox*(*GetArrFromCC)(CCBox* ccb));
void GoThrowAllLists(Cnf* cnf);
void InsIntoWListsOnBot2Unimply(Cnf* cnf, long litUnimpl);
void GoThrowWathedList(Cnf* cnf, long litNum, CCBox* frstBox);
void GoThrowD1ConfsList(Cnf* cnf, long varNum, CCBox* d1ConfsList);
long* D1EmergOrd(CCBox* ccb);
struct CnfInfo_* ReadFromFile();
//void ChkAndTreat1ConfEachSideForD2(Cnf* cnf, long litNum);
Boolean TreatSimilarityBefConfChk(Cnf* cnf, long litNum);
void IncrHeur(Cnf* cnf, long litNum, int value);
void ReduceInitHeurToMax(Cnf* cnf, long litNum, double maxVal);
Boolean CheckModel(Cnf* cnf, long* model);
void RecClsFromConfIVSVars(Cnf* cnf);
Cnf* theCnf;

CCBox* Add1UIPClause(Cnf* cnf, CCBox* clsWithContradiction, long* litNum);
void PrintLayers(Cnf* cnf);
void UpdAISInfoFor1UIP(Cnf* cnf, CCBox* lastCcb, long litNum);
CnfInfo* ReadFromFile();
Cnf* BuildClauses(CnfInfo* cnfInfo, unsigned long cutOff, double maxClsLenToRec, double maxCCBoxesInCC, unsigned long cbNumToRecAnyway, long heur, unsigned short qsNum, double maxInterval, double notToChkConfsFrom, double heurPartOfS1, unsigned long restartAft, unsigned long restartAdd, long clsDensFromNotInc, Boolean ifAdd1UipClause, long max1UIPLenToRec, int incrHeur1UIP, int restartsStrategy, int restartDelayTimes, int restartsMaxClssToRec, char ifGetFirstBackAftRestart);
Boolean TC(Cnf* cnf, char* pc);

CnfInfo* ReadFromFileNoTcl();
Cnf* BuildClausesNoTcl(CnfInfo* cnfInfo, unsigned long cutOff, double maxClsLenToRec, double maxCCBoxesInCC, unsigned long cbNumToRecAnyway, long heur, unsigned short qsNum, double maxInterval, double notToChkConfsFrom, double heurPartOfS1, unsigned long restartAft, unsigned long restartAdd, long clsDensFromNotInc, Boolean ifAdd1UipClause, long max1UIPLenToRec, int incrHeur1UIP, int restartsStrategy, int restartDelayTimes, int restartsMaxClssToRec, char ifGetFirstBackAftRestart);
#endif
