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
#ifdef FND_BG4
#include "C4FBG.h"
#endif
#ifdef WIN32
#define COMPARESTRINGS stricmp
#endif
#ifndef WIN32
#define COMPARESTRINGS strcmp
#endif

#define MAX_OUT_FILE_NAME_LEN 4096
void main(int argc, char *argv[])
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
//	HANDLE outFileHndl;
	ifUnsAftBuildCnf = 0;
	buffInd = 0;
	
#ifdef CHECK_HEAP
	tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag|=_CRTDBG_CHECK_ALWAYS_DF;
	tmpFlag|=_CRTDBG_DELAY_FREE_MEM_DF;
	tmpFlag|=_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag( tmpFlag );	
//    _CrtSetAllocHook( MyAllocHook );

#endif
	
  	if (!(inFile = fopen(argv[1], "r"))) 
	{
		fprintf(stderr, "CANNOT OPEN INPUT FILE");
		return;
	}
	
	/*
	if (!(outFile = fopen(argv[2], "a"))) 
		{
			fprintf(stderr, "CANNOT OPEN OUTPUT FILE");
			return;
		}*/
	
#ifdef WIN32
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW|_CRTDBG_MODE_DEBUG);
#endif	

	maxClsLen = 6; clsDensity = 5;
	qsNum = 20; maxInterval = 20;
	ifGetFirst = 1; ifLittleRand = 0;
	ifSortInImply = 0; 
	ifPrepush = 0;
	restartAft = 1000; restartAdd = 0; ifClsOnRestart = 1;
	maxMinHeur = 0.7;
	ifLessSign = 0;
	notCheckConfsFrom = 0.5; ifAddClsAftConfChk = 0; maxConfLen = 20;
	ifAdd1UipClause = 1;
	max1UIPLenToRec = 20;
	incrHeur1UIP = 1;
	cutOff = LONG_MAX;
	restartsStrategy = 1; restartDelayTimes = 0;
	strcpy(outFileName, "stdout"); outFile = stdout;
	restartsMaxClssToRec = 20;
	ifGetFirstBackAftRestart = 1;

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
	if (!outFile)
		outFile = stdout;
	heur = ifGetFirst|(int)ifLittleRand<<1|(int)4|(int)ifSortInImply<<3|(int)0|(int)ifClsOnRestart<<5|(int)ifLessSign<<6|(int)ifAddClsAftConfChk<<7|(int)0|(int)ifPrepush<<9|(int)0;

#ifdef NO_TCL
	cnfInfo = ReadFromFileNoTcl();
	cnf = BuildClausesNoTcl(cnfInfo, cutOff, maxClsLen, maxConfLen, 0, heur, qsNum, maxInterval, notCheckConfsFrom, maxMinHeur, restartAft, restartAdd, clsDensity, ifAdd1UipClause, max1UIPLenToRec, incrHeur1UIP, restartsStrategy, restartDelayTimes, restartsMaxClssToRec, ifGetFirstBackAftRestart);
#endif

#ifndef NO_TCL
	cnfInfo = ReadFromFile();
	cnf = BuildClauses(cnfInfo, cutOff, maxClsLen, maxConfLen, 0, heur, qsNum, maxInterval, notCheckConfsFrom, maxMinHeur, restartAft, restartAdd, clsDensity, ifAdd1UipClause, max1UIPLenToRec, incrHeur1UIP, restartsStrategy, restartDelayTimes, restartsMaxClssToRec, ifGetFirstBackAftRestart);
#endif
	buffInd += sprintf(buff+buffInd, "\n\n%s\t", argv[1]);

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
#ifndef PRINT_MINIMAL_INFO
	OutputTrace(cnf, "\nMain", 111, "", buff);	
#endif
#ifdef PRINT_MINIMAL_INFO
	buffInd = 0;
#endif

	free(cnfInfo);

	fclose(inFile);

#ifdef FND_BG4
	if (!(CheckModel(cnf, seqFndBg))) 
		printf("BAD MODEL");
	else
		printf("GOOD MODEL");
#endif
	clk1 = clock();
	switch (AlgMain(cnf, clk1, cutOff)) {
	case unknown : buffInd+=sprintf(buff+buffInd, "UNK\t"); satRes = 2; break;//buffInd+=sprintf(buff+buffInd, "UNKNOWN"); break;
	    case sat : buffInd+=sprintf(buff+buffInd, "SAT\t"); satRes = 1; /*printf("\nSATISFIABLE")*/; 
		break;//buffInd+=sprintf(buff+buffInd, "SATISFIABLE. THE MODEL IS:"); IVSPrint(cnf, cnf->ivs); break;
	case unsat : buffInd+=sprintf(buff+buffInd, "UNS\t"); /*printf("\nUNSATISFIABLE")*/;satRes = 0; break;//buffInd+=sprintf(buff+buffInd, "NOT SATISFIABLE"); break;
	case cutoff : buffInd+=sprintf(buff+buffInd, "CUT\t"); satRes = 3; break;//buffInd+=sprintf(buff+buffInd, "\nCUT OFF"); break;
	}
	clk2 = clock();
	if ((satRes != 1)&&(cnf)&&(cnf->ivs->d1Depth < cnf->minD1Depth))
		cnf->minD1Depth = cnf->ivs->d1Depth;
	if (satRes == 1)
		cnf->minD1Depth = 0;

	//printf("\n%5.40f seconds", (double)(clk2-clk1)/(double)CLOCKS_PER_SEC);
	if (cnf) {
		buffInd += sprintf(buff+buffInd, "%5.40f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu", (double)(clk2-clk1)/(double)CLOCKS_PER_SEC, cnf->currNodeNum, cnf->d2VarsConfsCheck, cnf->d2VarsConfsNotCheck, cnf->d2VarsConfsCheckMinImplLvl, cnf->restartsNum, cnf->restartsClsAddNum, cnf->restartsClsNotAddNum, cnf->minD1Depth);
		OutputTrace(cnf, "\nMain", 132, "", buff);
	}
	
#ifdef PRINT_MODEL
	if (satRes == 1)
		IVSPrintSorted(cnf, cnf->ivs);
#endif

#ifdef WIN32	
	if (cnf) {
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
	}
	RegCloseKey(phkResult);
#endif

	FreeCnf(cnf);

	fclose(outFile);
//	CloseHandle(outFileHndl);
}

/*
	if (argc > 3)
	{
		if (!(cutOff = atol(argv[3])))
			cutOff = 1800;
	} else
		cutOff = 1800;

	if (argc > 4)
	{
		maxClsLen = atof(argv[4]);
	} else 
		maxClsLen = 10;

	if (argc > 5)
	{
		maxConfLen = atof(argv[5]);
	} else 
		maxConfLen = 20;

	if (argc > 6)
	{
		cbNumToRecAnyway = atoi(argv[6]);
	} else 
		cbNumToRecAnyway = 10;
	

	if (argc > 7)
	{
		if (argv[7][1] == 'x')
		{
			heur = strtol(argv[7], &stopCh, 16);
		} else
		{
			if (argv[7][1] == 'b')
			{
				heur = strtol(argv[7], &stopCh, 2);
			} else
			{
				heur = atoi(argv[7]);
			}
		}		
	} else 
		heur = 0x1005;

	if (argc > 8)
	{
		qsNum = atoi(argv[8]);
	} else 
		qsNum = 10;

	if (argc > 9)
	{
		maxInterval = (double)atof(argv[9]);
	} else 
		maxInterval = 20.0;

	if (argc > 10)
	{
		notCheckConfsFrom = (double)atof(argv[10]);
	} else 
		notCheckConfsFrom = 0.9;
	
	if (argc > 11)
		restartAft = atol(argv[11]);
	else
		restartAft = ULONG_MAX;
	
	if (argc > 12)
		restartAdd = atol(argv[12]);
	else
		restartAdd = 100;

	if (argc > 13)
	{
		maxMinHeur = (double)atof(argv[13]);
	} else 
		maxMinHeur = 0.5;

	if (argc > 14)
		clsDensity = atol(argv[14]);
	else
		clsDensity = (long)maxClsLen-1;
*/
