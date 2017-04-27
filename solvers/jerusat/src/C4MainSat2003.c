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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "C4Globals.h"
#include "C4PQ.h"
#ifdef WIN32
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif

#ifdef WIN32
#define COMPARESTRINGS stricmp
#endif
#ifndef WIN32
#define COMPARESTRINGS strcmp
#endif

void ModelPrint(struct Cnf_* cnf, IVS* ivs)
{
	long i;
	OneIVSBox* ivsBox;
	
	printf("v ");
	for (i=1; i<=cnf->initVarsNum; i++)
	{
		ivsBox = cnf->vars[labs(i)].ivsBox;
		if ((ivsBox)&&(ivsBox->litNum < 0))
			printf("-");
		if (ivsBox)
			printf("%ld ", i);
		if (!(i%20))
			printf("\nv ");
	}
	printf("0\n");
	fflush(stdout);
}
#define MAX_OUT_FILE_NAME_LEN 4096

int main(int argc, char *argv[])
{
	Cnf* cnf;
	clock_t clk1, clk2;
	unsigned long cutOff;
//	unsigned int cbNumToRecAnyway;
	double maxClsLen, maxConfLen;
	int tmpFlag;
	int heur, st, index;
	unsigned short qsNum;
	double maxInterval, notCheckConfsFrom, maxMinHeur;
	CnfInfo* cnfInfo;
	unsigned long restartAft, restartAdd;
	long clsDensity;
	Boolean ifGetFirst, ifLittleRand, ifSortInImply, ifPrepush, ifClsOnRestart, ifLessSign, ifAddClsAftConfChk, ifAdd1UipClause;
	long max1UIPLenToRec;
	int incrHeur1UIP;
	int satRes;
	char outFileName[MAX_OUT_FILE_NAME_LEN];
	int restartsStrategy, restartDelayTimes, restartsMaxClssToRec;
	char ifGetFirstBackAftRestart;	
#ifdef WIN32
	HKEY handle;
	LONG err;
	char regStr[MAX_OUT_FILE_NAME_LEN];
	char keyName[512];
	LPDWORD lpdwDisposition;
	PHKEY phkResult;
	char* stopCh;
	long aux;
#endif

	ifUnsAftBuildCnf = 0;
  	if (!(inFile = fopen(argv[1], "r"))) 
	{
		printf("c Cannot open input File\n");
		return;
	}
	
	printf("c Input File : %s\n", argv[1]);

	maxClsLen = 7; clsDensity = 6;
	qsNum = 20; maxInterval = 20;
	ifGetFirst = 1; ifLittleRand = 0;
	ifSortInImply = 0; 
	ifPrepush = 0;
	restartAft = 1000; restartAdd = 0; ifClsOnRestart = 1;
	maxMinHeur = 0.5;
	ifLessSign = 0;
	notCheckConfsFrom = 0.5; ifAddClsAftConfChk = 0; maxConfLen = 9;
	ifAdd1UipClause = 1;
	max1UIPLenToRec = 10;
	incrHeur1UIP = 1;
	cutOff = LONG_MAX;
	restartsStrategy = 1; restartDelayTimes = 0;
	strcpy(outFileName, "stdout"); outFile = stdout;
	restartsMaxClssToRec = 40;
	ifGetFirstBackAftRestart = 1;
#ifdef DEFAULT_1
	printf("c Jerusat 1.2.a; Nadel Alexander (ale1@post.tau.ac.il) 2001-2003\n");	
#endif
#ifdef DEFAULT_2
	notCheckConfsFrom = 0.0;
	printf("c Jerusat 1.2.b; Nadel Alexander (ale1@post.tau.ac.il) 2001-2003\n");
#endif
#ifdef DEFAULT_3
	restartAft = 100; restartAdd = 10; ifClsOnRestart = 1;
	restartsStrategy = 5;
	maxConfLen = 25;
	max1UIPLenToRec = 15;
	maxClsLen = 15; clsDensity = 14;
	printf("c Jerusat 1.2.c; Nadel Alexander (ale1@post.tau.ac.il) 2001-2003\n");
#endif
#ifdef DEFAULT_4
	notCheckConfsFrom = 0.1;
	maxConfLen = 16;
	printf("c Jerusat 1.3; Nadel Alexander (ale1@post.tau.ac.il) 2001-2003\n");
#endif
	
	for (outFile = 0, index = 2 ; index < argc ;  index++) 
	{
		if (!COMPARESTRINGS(argv[index],"-outFile")) {
			if (!(outFile = fopen(argv[++index], "a")))
			{
				outFile = stdout;
				strcpy(outFileName, "stdout");
			} else
			{
				strcpy(outFileName, argv[index]);
			}
		}
		if (!COMPARESTRINGS(argv[index],"-max1UIPLenToRec")) {
			max1UIPLenToRec = atol(argv[++index]);
			continue;
		}

		if (!COMPARESTRINGS(argv[index],"-incrHeur1UIP")) {
			incrHeur1UIP = atoi(argv[++index]);
			continue;
		}

		if (!COMPARESTRINGS(argv[index],"-maxClsLen")) {
			maxClsLen = atof(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-clsDensity")) {
			clsDensity = atol(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-qsNum")) {
			qsNum = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-maxInterval")) {
			maxInterval = atof(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifGetFirst")) {
			ifGetFirst = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifLittleRand")) {
			ifLittleRand = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifSortInImply")) {
			ifSortInImply = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifPrepush")) {
			ifPrepush = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-restartAft")) {
			restartAft = atol(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-restartAdd")) {
			restartAdd = atol(argv[++index]);
			continue;
		}
/*
		if (!COMPARESTRINGS(argv[index],"-ifClsOnRestart")) {
			ifClsOnRestart = atoi(argv[++index]);
			continue;
		}
*/
		if (!COMPARESTRINGS(argv[index],"-maxMinHeur")) {
			maxMinHeur = atof(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifLessSign")) {
			ifLessSign = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-notCheckConfsFrom")) {
			notCheckConfsFrom = atof(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifAddClsAftConfChk")) {
			ifAddClsAftConfChk = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-maxConfLen")) {
			maxConfLen = atol(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-cutOff")) {
			cutOff = atol(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifAdd1UipClause")) {
			ifAdd1UipClause = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-restartsStrategy")) {
			restartsStrategy = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-restartDelayTimes")) {
			restartDelayTimes = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-restartsMaxClssToRec")) {
			restartsMaxClssToRec = atoi(argv[++index]);
			continue;
		}
		if (!COMPARESTRINGS(argv[index],"-ifGetFirstBackAftRestart")) {
			ifGetFirstBackAftRestart = atoi(argv[++index]);
			continue;
		}		
	}

	switch(restartsStrategy) {
	case NoClsLinear :
	case NoClsExp :
	case NoClsLikeVer1 :
		ifClsOnRestart = 0;
		break;
	case ClsLinear :
	case ClsExp1 :
	case ClsExp2 :
	case ClsLikeVer1 :
	default:
		ifClsOnRestart = 1;
	}
	
	heur = ifGetFirst|(int)ifLittleRand<<1|(int)4|(int)ifSortInImply<<3|(int)0|(int)ifClsOnRestart<<5|(int)ifLessSign<<6|(int)ifAddClsAftConfChk<<7|(int)0|(int)ifPrepush<<9|(int)0;
	
	printf("c maxClsLen=%ld; clsDensity=%ld; qsNum=%hu; maxInterval=%2.2f\n", (long)maxClsLen, clsDensity, qsNum, maxInterval);
	printf("c ifGetFirst=%d; ifLittleRand=%d; ifSortInImply=%d; ifPrepush=%d\n", ifGetFirst, ifLittleRand, ifSortInImply, ifPrepush);
	printf("c restartAft=%lu; restartAdd=%lu; ifClsOnRestart=%d; maxMinHeur=%2.2f\n", restartAft, restartAdd, ifClsOnRestart, maxMinHeur);
	printf("c ifLessSign=%d; notCheckConfsFrom=%2.2f; ifAddClsAftConfChk=%d; maxConfLen=%ld\n", ifLessSign, notCheckConfsFrom, ifAddClsAftConfChk, (long)maxConfLen);
	printf("c ifAdd1UipClause=%d; max1UIPLenToRec=%ld; incrHeur1UIP=%d; cutOff=%ld\n", ifAdd1UipClause, max1UIPLenToRec, incrHeur1UIP, cutOff);
	printf("c restartsStrategy=%d; restartDelayTimes=%d; restartsMaxClssToRec=%ld; ifGetFirstBackAftRestart=%d\n", restartsStrategy, restartDelayTimes, restartsMaxClssToRec, ifGetFirstBackAftRestart);

	cnfInfo = ReadFromFileNoTcl();
	cnf = BuildClausesNoTcl(cnfInfo, cutOff, maxClsLen, maxConfLen, 0, heur, qsNum, maxInterval, notCheckConfsFrom, maxMinHeur, restartAft, restartAdd, clsDensity, ifAdd1UipClause, max1UIPLenToRec, incrHeur1UIP, restartsStrategy, restartDelayTimes, restartsMaxClssToRec, ifGetFirstBackAftRestart);
	free(cnfInfo);

	fclose(inFile);
#ifdef WIN32	
	strcpy(keyName, "SOFTWARE\\C4\\");
	lpdwDisposition = malloc(sizeof(DWORD));
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, KEY_ALL_ACCESS, &phkResult))
	{
		buffInd+= sprintf(buff+buffInd, "%lu\t", cnf->cutOff);
		RegSetValueEx(phkResult, "cutOff", NULL, REG_DWORD, &cnf->cutOff, 4);

		buffInd+= sprintf(buff+buffInd, "%lu\t", cnf->maxClsLenToRec);
		RegSetValueEx(phkResult, "maxClsLen", NULL, REG_DWORD, &cnf->maxClsLenToRec, 4);		
		buffInd+= sprintf(buff+buffInd, "%ld\t", cnf->clsDensFromNotInc);
		RegSetValueEx(phkResult, "clsDensity", NULL, REG_DWORD, &cnf->clsDensFromNotInc, 4);
		
		buffInd+= sprintf(buff+buffInd, "%hu\t", cnf->pq->qsNum);
		aux = cnf->pq->qsNum;
		RegSetValueEx(phkResult, "qsNum", NULL, REG_DWORD, &aux, 4);		
		
		buffInd+= sprintf(buff+buffInd, "%5.10f\t", cnf->pq->maxInterval);
		sprintf(regStr, "%5.10f", cnf->pq->maxInterval);
		RegSetValueEx(phkResult, "maxInterval", NULL, REG_SZ, &regStr, strlen(regStr));		

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->pq->ifGetFirst));
		aux = cnf->pq->ifGetFirst;
		RegSetValueEx(phkResult, "ifGetFirst", NULL, REG_DWORD, &aux, 4);		
		
		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->heur&2));
		aux = ((cnf->heur&2) ? (1) : (0));
		RegSetValueEx(phkResult, "ifLittleRand", NULL, REG_DWORD, &aux, 4);		

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->heur&8));		
		aux = ((cnf->heur&8) ? (1) : (0));
		RegSetValueEx(phkResult, "ifSortInImply", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->heur&512));
		aux = ((cnf->heur&512) ? (1) : (0));
		RegSetValueEx(phkResult, "ifPrepush", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%lu\t", cnf->restartAft);
		RegSetValueEx(phkResult, "restartAft", NULL, REG_DWORD, &cnf->restartAft, 4);
		buffInd+= sprintf(buff+buffInd, "%lu\t", cnf->restartAdd);
		RegSetValueEx(phkResult, "restartAdd", NULL, REG_DWORD, &cnf->restartAdd, 4);

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->heur&32));
		aux = ((cnf->heur&32) ? (1) : (0));
		RegSetValueEx(phkResult, "ifClsOnRestart", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%5.10f\t", cnf->heurPartOfS1);
		sprintf(regStr, "%5.10f", cnf->heurPartOfS1);
		RegSetValueEx(phkResult, "maxMinHeur", NULL, REG_SZ, &regStr, strlen(regStr));

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->heur&64));
		aux = ((cnf->heur&64) ? (1) : (0));
		RegSetValueEx(phkResult, "ifLessSign", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%lu\t", cnf->maxCCBoxesInCC);
		RegSetValueEx(phkResult, "maxConfLen", NULL, REG_DWORD, &cnf->maxCCBoxesInCC, 4);

		buffInd+= sprintf(buff+buffInd, "%5.10f\t", cnf->notToChkConfsFrom);
		sprintf(regStr, "%5.10f", cnf->notToChkConfsFrom);
		RegSetValueEx(phkResult, "notCheckConfsFrom", NULL, REG_SZ, &regStr, strlen(regStr));

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)(cnf->heur&128));
		aux = ((cnf->heur&128) ? (1) : (0));
		RegSetValueEx(phkResult, "ifAddClsAftConfChk", NULL, REG_DWORD, &aux, 4);

		strcpy(regStr, argv[1]);
		RegSetValueEx(phkResult, "InFile", NULL, REG_SZ, &regStr, strlen(regStr));
		
		strcpy(regStr, outFileName);
		RegSetValueEx(phkResult, "OutFile", NULL, REG_SZ, &regStr, strlen(regStr));

		buffInd+= sprintf(buff+buffInd, "%d\t", (int)ifAdd1UipClause);
		aux = ifAdd1UipClause;
		RegSetValueEx(phkResult, "IfAdd1UIPClause", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%ld\t", max1UIPLenToRec);
		aux = max1UIPLenToRec;
		RegSetValueEx(phkResult, "max1UIPLenToRec", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%d\t", incrHeur1UIP);
		aux = incrHeur1UIP;
		RegSetValueEx(phkResult, "incrHeur1UIP", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%d\t", restartsStrategy);
		aux = restartsStrategy;
		RegSetValueEx(phkResult, "restartsStrategy", NULL, REG_DWORD, &aux, 4);
		
		buffInd+= sprintf(buff+buffInd, "%d\t", restartDelayTimes);
		aux = restartDelayTimes;
		RegSetValueEx(phkResult, "restartDelayTimes", NULL, REG_DWORD, &aux, 4);
		
		buffInd+= sprintf(buff+buffInd, "%d\t", restartsMaxClssToRec);
		aux = restartsMaxClssToRec;
		RegSetValueEx(phkResult, "restartsMaxClssToRec", NULL, REG_DWORD, &aux, 4);

		buffInd+= sprintf(buff+buffInd, "%d\t", ifGetFirstBackAftRestart);
		aux = ifGetFirstBackAftRestart;
		RegSetValueEx(phkResult, "ifGetFirstBackAftRestart", NULL, REG_DWORD, &aux, 4);		
}
	free(lpdwDisposition);
	RegCloseKey(phkResult);
#endif
//	CheckModel(cnf, fbgNewRest);
//	printf("c %s", buff);
	clk1 = clock();
	switch (AlgMain(cnf, clk1, cutOff)) {
	case unknown : printf("s UNKNOWN\n"); satRes = 2; break;
	case sat : printf("s SATISFIABLE\n"); satRes = 1; break;
	case unsat : printf("s UNSATISFIABLE\n"); satRes = 0; break;
	case cutoff : printf("s UNKNOWN\n"); satRes = 3; break;
	}
	clk2 = clock();
	
	if (satRes == 1)
		ModelPrint(cnf, cnf->ivs);
	
#ifdef WIN32	
	lpdwDisposition = malloc(sizeof(DWORD));
	//if (!(err = RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, NULL, 0, 0, NULL, &phkResult, lpdwDisposition)))
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, KEY_ALL_ACCESS, &phkResult))
	{
		sprintf(regStr, "%5.40f", (double)(clk2-clk1)/(double)CLOCKS_PER_SEC);
		RegSetValueEx(phkResult, "OUT_Time", NULL, REG_SZ, regStr, strlen(regStr));
		RegSetValueEx(phkResult, "OUT_Nodes", NULL, REG_DWORD, &cnf->currNodeNum, 4);
		RegSetValueEx(phkResult, "OUT_D2ConfsChk", NULL, REG_DWORD, &cnf->d2VarsConfsCheck, 4);
		RegSetValueEx(phkResult, "OUT_D2ConfsNotChk", NULL, REG_DWORD, &cnf->d2VarsConfsNotCheck, 4);
		RegSetValueEx(phkResult, "OUT_D2ConfsChkMinImplLvl", NULL, REG_DWORD, &cnf->d2VarsConfsCheckMinImplLvl, 4);
		RegSetValueEx(phkResult, "OUT_Res", NULL, REG_DWORD, &satRes, 4);
		RegSetValueEx(phkResult, "OUT_RestartsNum", NULL, REG_DWORD, &cnf->restartsNum, 4);
		RegSetValueEx(phkResult, "OUT_RestartsClsAddNum", NULL, REG_DWORD, &cnf->restartsClsAddNum, 4);
		RegSetValueEx(phkResult, "OUT_RestartsClsNotAddNum", NULL, REG_DWORD, &cnf->restartsClsNotAddNum, 4);
		RegSetValueEx(phkResult, "OUT_D1MinDepth", NULL, REG_DWORD, &cnf->minD1Depth, 4);
	}
	free(lpdwDisposition);
	RegCloseKey(phkResult);
#endif
	if (cnf) {
		printf("c Time=%5.5f; Nodes=%lu; D2ConfsChk=%lu; D2ConfsNotChk=%lu\n", (double)(clk2-clk1)/(double)CLOCKS_PER_SEC, cnf->currNodeNum, cnf->d2VarsConfsCheck, cnf->d2VarsConfsNotCheck);
		printf("c D2ConfsChkMinImplLvl=%lu; RestartsNum=%lu; RestartsClsAddNum=%lu; D1MinDept=%lu\n", cnf->d2VarsConfsCheckMinImplLvl, cnf->restartsNum, cnf->restartsClsAddNum, cnf->minD1Depth);
		FreeCnf(cnf);
	}

	switch (satRes) {
	case 0 : return 20;
	case 1 : return 10;
	default : return 0;
	}
}