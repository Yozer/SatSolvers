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

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifndef NO_TCL
#include <tcl.h>
#endif
#include <limits.h>

#include "C4Globals.h"
#include "C4PQ.h"
#ifdef WIN32
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
//Build Cnf constants
#define MAXLINE 4096
#define MAXCHARINCLS (4*4096)
#define MAXLITNUMINCLS 1000

Boolean IfValid(char chr)
{
#ifdef SOLARIS_BUG
//	printf("*%c-%d*", chr, (chr!='\n')&&(chr!='\t')&&(chr!=' '));
#endif
	return ((chr != '\n') && (chr != '\t') &&(chr != ' '));
}


void FillCCBoxInBldCnf(CCBox* ccb, long litNum, unsigned long ordFromUp0Ind, char wchStat)
{
	ccb->litNum = litNum;
	ccb->ordFromUp0Ind = ordFromUp0Ind;
	
	ccb->ccType = CC_TYPE_INIT_CLS;
	ccb->ifSorted = 0;
	ccb->nextClsIndOrD1ArrOrd = ordFromUp0Ind - 1;
	ccb->ifNextClsBoxInd = 1;
	ccb->ifUsedConfsDelHead = 0;
	ccb->ifDelCls = 0;
	ccb->clsFromConfStat = 0;
	ccb->ifBot2 = 0;
	ccb->ifAdd1UIP = 0;

//	ccb->ifOnWchList = wchStat;
}
#ifndef NO_TCL
CnfInfo* ReadFromFile()
{
	Tcl_Interp* interp;	
	char line[MAXLINE];	
	char cls[MAXLINE];
	long lits[MAXLITNUMINCLS];	
	unsigned long lineNum, currLitsNum, currOvBuffInd;
	long numOfEl, i, j, currLit, currClsNum;	
	char** lineSplitted;	
	Boolean errInCls, litAlreadyApp, tau;
	CnfInfo* cnfInfo;

	cnfInfo = malloc(sizeof(CnfInfo));
	
	if (!(interp = Tcl_CreateInterp()))
	{
		fprintf(outFile, "\nNumOfVarsInFile: Couldn't create Tcl interpreter");		
		return 0;
	}
	
	currClsNum = -2;
	lineNum = 0;
	
	while (fgets(line, MAXLINE, inFile)) {
		lineNum++;
		switch (line[0]) {
		case 'c' : 
			break;
		case 'p' : 
			//SplitList(line, &numOfEl, &lineSplitted);
			//FreeList(&lineSplitted);
			Tcl_SplitList(interp, line, &numOfEl, &lineSplitted); 			
			//getting the number of variables 
			if (!(cnfInfo->initVarsNum = atol(lineSplitted[2])))
			{
				Tcl_Free((char*) lineSplitted);
				Tcl_DeleteInterp(interp);
				return 0;
			}
			//getting the number of clauses
			if (!(cnfInfo->initClssNum = atol(lineSplitted[3])))
			{
				Tcl_Free((char*) lineSplitted);
				Tcl_DeleteInterp(interp);
				return 0;
			}
		
			cnfInfo->clssBuff = calloc(MAXLINE*4, sizeof(char));
			cnfInfo->currOvBuffSize = MAXLINE*4;
			cnfInfo->sizeOfBuffForCnf = 0;
			currOvBuffInd = 0;
			currClsNum = -1;

			Tcl_Free((char*) lineSplitted);					
			break;
		default:
			Tcl_SplitList(interp, line, &numOfEl, &lineSplitted); 
			if (currClsNum == -2)
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Clauses come before the number of variables os stated", lineNum);
				break;
			} 
			
			if (numOfEl == 0)
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Empty line", lineNum);
				Tcl_Free((char*) lineSplitted);
				break;
			}

			//getting the rest of the clause if it consists of a few lines
			strcpy(cls, line);

			while ( (strcmp(lineSplitted[numOfEl-1],"0"))&&(strlen(cls)<(MAXCHARINCLS-MAXLINE)) ) 
			{
				Tcl_Free((char*) lineSplitted);
				if (fgets(line, MAXLINE, inFile)) 
				{
					lineNum++;
					strcat(cls, line);
					Tcl_SplitList(interp, cls, &numOfEl, &lineSplitted); 
				} else {
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Unexpected end of clause", lineNum);
					Tcl_Free((char*) lineSplitted);
					break;
				}
			}

			//There are 3 reasons for previous while loop
			//ending : 
			//1)end of file 
			//2)no zero despite reading MAXCHARINCLS-MAXLINE characters
			//3)clause has been read
			//Next lines react appropriately to the reason
			
			//end of file
			if ((strcmp(lineSplitted[numOfEl-1],"0"))&&(strlen(cls)<MAXCHARINCLS-MAXLINE))
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Unexpected end of file", lineNum);
				break;
			} 
			
			//no zero despite reading MAXCHARINCLS-MAXLINE characters
			if (strlen(cls)>=MAXCHARINCLS-MAXLINE)
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Clause is too long or 0 not appears in the end of some clause/clauses", lineNum);
				Tcl_Free((char*) lineSplitted);
				break;
			}
			
			//clause has been read
			//checking the validity of the clause and if everything is OK 
			//inserting the literals into lits array.
			////fprintf(outFile, "\nCurrClsNum is %d", currClsNum);
			currLitsNum = 0;
			errInCls = FALSE;
			for (i=0; i<numOfEl-1; i++)
			{
				currLit = atol(lineSplitted[i]);
				if ((labs(currLit) < 1) || (labs(currLit) > cnfInfo->initVarsNum))
				{
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Literal in clause is wrong. Clause ignored", lineNum);
					errInCls = TRUE;
					break;
				}
				
				//checking whether the variable already appears
				//in the clause
				litAlreadyApp = FALSE;
				tau = FALSE;
				for (j=0; j<currLitsNum; j++)
				{
					if (currLit == lits[j])
					{
						litAlreadyApp = TRUE;
						break;
					}
					
					if (currLit == -lits[j])
					{
						tau = TRUE;
						break;
					}
				}
				
				if (tau) 
				{
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Clause is a tautology. Clause ignored.", lineNum);
					errInCls = TRUE;
					break;
				}
				
				//if the literal is new to the clause inserting it
				if (!litAlreadyApp)
				{
					lits[currLitsNum++] = currLit;
				}
			}

			if (!errInCls)
			{
				if (!currLitsNum)
				{
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : No literals in clause. Clause ignored.", lineNum);
				} else
				//now building the clause itself
				{
					++currClsNum;
					
					if (currLitsNum != 1)
						cnfInfo->sizeOfBuffForCnf+=(2*sizeof(unsigned long) + 2*sizeof(CCArrowsBox) + currLitsNum*sizeof(CCBox));					
					if (currOvBuffInd+currLitsNum*8+1 > cnfInfo->currOvBuffSize)
					{
						cnfInfo->clssBuff = realloc(cnfInfo->clssBuff, cnfInfo->currOvBuffSize+4*MAXLINE);
						cnfInfo->currOvBuffSize+=4*MAXLINE;
					}
					for (i = 0; i < currLitsNum; i++)
					{
						if (i != (currLitsNum-1))
							currOvBuffInd+=sprintf(cnfInfo->clssBuff+currOvBuffInd, "%ld ", lits[i]);
						else
							currOvBuffInd+=sprintf(cnfInfo->clssBuff+currOvBuffInd, "%ld\0", lits[i]);
					}
					currOvBuffInd++;
				}
			}

			Tcl_Free((char*) lineSplitted);
			break;			
		}
	}

	cnfInfo->clssBuff = realloc(cnfInfo->clssBuff, currOvBuffInd);
	cnfInfo->currOvBuffSize = currOvBuffInd;
	cnfInfo->initClssNum = currClsNum+1;
	
	Tcl_DeleteInterp(interp);

	return cnfInfo;
}

Cnf* BuildClauses(CnfInfo* cnfInfo, unsigned long cutOff, double maxClsLenToRec, double maxCCBoxesInCC, unsigned long cbNumToRecAnyway, long heur, unsigned short qsNum, double maxInterval, double notToChkConfsFrom, double heurPartOfS1, unsigned long restartAft, unsigned long restartAdd, long clsDensFromNotInc, Boolean ifAdd1UipClause, long max1UIPLenToRec, int incrHeur1UIP, int restartsStrategy, int restartDelayTimes, int restartsMaxClssToRec, char ifGetFirstBackAftRestart)
{
	Cnf* cnf;
	Tcl_Interp* interp;	
	unsigned long bytesNeededForCls, currLitsNum;
	long toInsFromBuffInd, i, currLit;	
	char** lineSplitted;	
	CCBox *ccBox;
	CCArrowsBox *ccArrowBox;
	char wchStat;
	Var* currVar;
	double maxVal;
	unsigned long currBuffInd;
	Boolean ifGetFirst;

	srand( (unsigned)time( NULL ) );

	if (!(interp = Tcl_CreateInterp()))
	{
		fprintf(outFile, "\nBUILD_CNF_FROM_FILE: Couldn't create Tcl interpreter");		
		return 0;
	}
	
	// creating the cnf 
	if (!(cnf = calloc(1, sizeof(Cnf))))
	{
		fprintf(outFile, "\nBUILD_CNF_FROM_FILE: Couldn't allocate memory for CNF");		
		return 0;
	}

	cnf->d2VarsConfsCheckMinImplLvl = ULONG_MAX;
	cnf->notToChkConfsFrom = notToChkConfsFrom;
	cnf->heur = heur;
	cnf->heurPartOfS1 = heurPartOfS1;
	cnf->cutOff = cutOff;
	cnf->initClssNum = cnfInfo->initClssNum;
	cnf->initVarsNum = cnfInfo->initVarsNum;
	cnf->maxClsLenToRec = max(maxClsLenToRec,3);
	cnf->maxCCBoxesInCC = max(maxCCBoxesInCC,3);
	cnf->restartAdd = cnf->restartAddInit = restartAdd;
	cnf->restartAft = cnf->restartAftInit = restartAft;
	cnf->varsNumForRestart = malloc(cnf->initVarsNum*sizeof(long));
	cnf->clsDensFromNotInc = clsDensFromNotInc;
	cnf->restartsStrategy = restartsStrategy;
	cnf->restartDelayTimes = restartDelayTimes;
	toInsFromBuffInd = 0;
				
	//			cnf->implLvlNotCPFromDown = -1;
	cnf->vars = calloc(cnf->initVarsNum+1, sizeof(Var));
	cnf->pushD1BlockSt = CreateStack(cnf->initVarsNum);
	cnf->toBeImplied = CreateQueue(cnf->initVarsNum);
	cnf->ivs = IVSCreate(cnf->initVarsNum);
	cnf->prePushSt = CreateStack(cnf->initVarsNum);
	cnf->lastConfNum = calloc(cnf->initVarsNum+1, sizeof(unsigned long));
	cnf->lastPrepushConfNum = calloc(cnf->initVarsNum+1, sizeof(unsigned long));
	cnf->rccCPLits = CreateStack(cnf->initVarsNum);
	cnf->confVarsPart = CreateStack(cnf->initVarsNum);
	cnf->initCCBuff = malloc(cnfInfo->sizeOfBuffForCnf);
	cnf->sizeOfBuffForCnf = cnfInfo->sizeOfBuffForCnf;
	cnf->lastLayerPerCpNum = calloc(cnf->initVarsNum+1, sizeof(long));
	cnf->layersArr = calloc(cnf->initVarsNum+1, sizeof(LLBox*));
	cnf->lastLayerCoPerCpLvl = calloc(cnf->initVarsNum+1, sizeof(LLBox*));
	cnf->ifAdd1UipClause = ifAdd1UipClause;
	cnf->max1UIPLenToRec = max1UIPLenToRec;
	cnf->incrHeur1UIP = incrHeur1UIP;
	ifGetFirst = heur&1;
	cnf->restartsMaxClssToRec = restartsMaxClssToRec;
	cnf->minD1Depth = ULONG_MAX;
	cnf->ifGetFirstBackAftRestart = ifGetFirstBackAftRestart;
	for (currBuffInd = 0; currBuffInd < cnfInfo->currOvBuffSize; currBuffInd+=(strlen(cnfInfo->clssBuff+currBuffInd)+1))
	{
	//while (fgets(line, MAXLINE, inFile)) {
		Tcl_SplitList(interp, cnfInfo->clssBuff+currBuffInd, &currLitsNum, &lineSplitted); 
		
		//now building the clause itself		
		if (currLitsNum != 1)
		{
			bytesNeededForCls = 2*sizeof(unsigned long) + 2*sizeof(CCArrowsBox) + currLitsNum*sizeof(CCBox);
						
			//setting the number of lits
			*((unsigned long*)(&cnf->initCCBuff[toInsFromBuffInd])) = currLitsNum;
			toInsFromBuffInd+=sizeof(unsigned long);
			//setting "sorted from"
			*((unsigned long*)(&cnf->initCCBuff[toInsFromBuffInd])) = 0;
			toInsFromBuffInd+=sizeof(unsigned long);
			for (i = 0; i < currLitsNum; i++)
			{							
				ccBox = (CCBox*)(&cnf->initCCBuff[toInsFromBuffInd]);					
				toInsFromBuffInd+=sizeof(CCBox);
				
				if (i == currLitsNum - 2)
				{
					ccArrowBox = (CCArrowsBox*)(&cnf->initCCBuff[toInsFromBuffInd+sizeof(CCBox)]);
					wchStat = FALSE;//WCH_STAT_UP;
				}
				else
				{
					if (i == currLitsNum - 1)
					{
						ccArrowBox = (CCArrowsBox*)(&cnf->initCCBuff[toInsFromBuffInd+sizeof(CCArrowsBox)]);									
						wchStat = TRUE;//WCH_STAT_DOWN;
					}
					else
						wchStat = FALSE;
				}
				//ifSorted == FALSE is relevant only for the last cb
				FillCCBoxInBldCnf(ccBox, atol(lineSplitted[i]), i, wchStat);
				
				currVar = &cnf->vars[labs(atol(lineSplitted[i]))];
				
				((atol(lineSplitted[i]) > 0) ? (currVar->overallApp[POS_APP]++) : (currVar->overallApp[NEG_APP]++));
			}
			
			//arranging watched boxes pointers
			InsWCCBoxIntoVarsWList(cnf, &cnf->initCCBuff[toInsFromBuffInd-2*sizeof(CCBox)], &cnf->initCCBuff[toInsFromBuffInd-sizeof(CCBox)]);
			InsWCCBoxIntoVarsWList(cnf, &cnf->initCCBuff[toInsFromBuffInd-sizeof(CCBox)], &cnf->initCCBuff[toInsFromBuffInd-sizeof(CCBox)]);						
			
			toInsFromBuffInd+=2*sizeof(CCArrowsBox);
			
		} else
		{
			currVar = &cnf->vars[labs(atol(lineSplitted[0]))];						
			
			if (!currVar->aisBox)
			{
				currVar->aisBox = calloc(1, sizeof(LLBox));
				currVar->aisBox->litNum = currVar->varNum;
			}
			if (currVar->aisStatus == notImplOut)
				PushIntoAddImplSt(cnf, atol(lineSplitted[0]));
			else
			{
				if (currVar->aisBox->litNum == -atol(lineSplitted[0]))
				{
					//UNSAT!!!
					ifUnsAftBuildCnf = TRUE;
					//return 0;
				}
			}
		}
				
		Tcl_Free((char*) lineSplitted);
	}

	for (maxVal = 0.0, i = 1; i <= cnf->initVarsNum; i++)
	{
		currVar = &cnf->vars[i];
		currVar->varNum = i;
		currVar->pqBox = calloc(1, sizeof(OneQBox));
		currVar->pqBox->litNum = i;
		currVar->implStat = NOT_IMPLIED;
		if (!currVar->aisBox)
		{
			currVar->aisBox = calloc(1, sizeof(LLBox));
			currVar->aisBox->litNum = i;
		}
		if (i != 1)
		{
			currVar->pqBox->prev = cnf->vars[i-1].pqBox;
			cnf->vars[i-1].pqBox->next = currVar->pqBox;
		}
		
		if (maxVal < GetLitsVal(cnf, i))
			maxVal = GetLitsVal(cnf, i);
		currVar->cbNumToRecAnyway = cbNumToRecAnyway;	
		cnf->varsNumForRestart[i-1] = i;
		currVar->layerBox.litNum = i;
	}

	cnf->initClssNum = cnf->currCCNum = cnfInfo->initClssNum;

	cnf->pq = PQCreate(cnf, qsNum, maxInterval, ifGetFirst, GetLitsVal, GetVarsSign, ReduceInitHeurToMax);

	theCnf = cnf;
	free(cnfInfo->clssBuff);

	return cnf;
}

#endif
void SplitList(char* inBuff, long* numOfEl, char*** lineSplitted)
{
	long curr, prev, outInd, lsInd;
	char* outBuf;
#ifdef SOLARIS_BUG
	long j;
#endif

	outBuf = malloc(strlen(inBuff)+2);
	*lineSplitted = malloc((strlen(inBuff)+2)*sizeof(char*));
	outInd = lsInd = 0;
	
	*numOfEl = 0;
	
	for (prev = 0; prev<strlen(inBuff); prev++)
	{
		if (IfValid(inBuff[prev]))
			break;
	}
	
	if (prev != strlen(inBuff))
	{	
#ifdef SOLARIS_BUG
		printf("\ninBuff-\n");
#endif
		for (curr = prev; curr < strlen(inBuff); curr++)
		{
#ifdef SOLARIS_BUG
			printf("%c,", inBuff[curr]);
#endif
			if (!IfValid(inBuff[curr]))
			{
				(*numOfEl)++;
				strncpy(outBuf+outInd, inBuff+prev, curr-prev);
				(*lineSplitted)[lsInd++] = outBuf+outInd;
				outInd+=(curr-prev);
				outBuf[outInd++] = '\0';
				for (prev = curr; prev<strlen(inBuff); prev++)
				{
					if (IfValid(inBuff[prev]))
						break;
				}
				curr = prev-1;
			}
		}
		if (prev != strlen(inBuff))
		{
			(*numOfEl)++;
			strncpy(outBuf+outInd, inBuff+prev, curr-prev);
			(*lineSplitted)[lsInd++] = outBuf+outInd;
			outInd+=(curr-prev);
			outBuf[outInd++] = '\0';
		}
	} else
	{
		outBuf[outInd++] = '\0';
	}
	
	outBuf[outInd++] = '\0';
	
	outBuf = realloc(outBuf, outInd);
#ifdef SOLARIS_BUG
	printf("\nnumOfEl-%ld", *numOfEl);
	printf("\nlineSplitted-%ld; *lineSplitted-%ld", lineSplitted, *lineSplitted);
	for (j=0; j<*numOfEl; j++)
	{		
		printf("\nLS[%ld]=%s", j, (*lineSplitted)[j]);		
	}
#endif
	*lineSplitted = realloc(*lineSplitted, (*numOfEl)*sizeof(char*));
#ifdef SOLARIS_BUG
	printf("\nnumOfEl-%ld", *numOfEl);
	printf("\nlineSplitted-%ld; *lineSplitted-%ld", lineSplitted, *lineSplitted);
	for (j=0; j<*numOfEl; j++)
	{		
		printf("\nLS[%ld]=%s", j, (*lineSplitted)[j]);
		printf("&&((*lineSplitted)[j])-%ld", &(&((*lineSplitted)[j])));
	}
#endif

}

void FreeList(char*** lineSplitted)
{	
	if (*lineSplitted)
		free((**lineSplitted));
	if (lineSplitted)
		free(*lineSplitted);
}

CnfInfo* ReadFromFileNoTcl()
{
	char line[MAXLINE];	
	char cls[MAXLINE];
	long lits[MAXLITNUMINCLS];	
	unsigned long lineNum, currLitsNum, currOvBuffInd;
	long numOfEl, i, j, currLit, currClsNum;	
	char** lineSplitted;	
	Boolean errInCls, litAlreadyApp, tau;
	CnfInfo* cnfInfo;

	cnfInfo = malloc(sizeof(CnfInfo));
	
	currClsNum = -2;
	lineNum = 0;
	
	while (fgets(line, MAXLINE, inFile)) {
		lineNum++;
		switch (line[0]) {
		case 'c' : 
			break;
		case 'p' : 			
			SplitList(line, &numOfEl, &lineSplitted);
			//getting the number of variables 
			if (!(cnfInfo->initVarsNum = atol(lineSplitted[2])))
			{
				FreeList(&lineSplitted);
				return 0;
			}
			//getting the number of clauses
			if (!(cnfInfo->initClssNum = atol(lineSplitted[3])))
			{
				FreeList(&lineSplitted);
				return 0;
			}
		
			cnfInfo->clssBuff = calloc(MAXLINE*4, sizeof(char));
			cnfInfo->currOvBuffSize = MAXLINE*4;
			cnfInfo->sizeOfBuffForCnf = 0;
			currOvBuffInd = 0;
			currClsNum = -1;

			FreeList(&lineSplitted);					
			break;
		default:
			SplitList(line, &numOfEl, &lineSplitted); 
#ifdef SOLARIS_BUG
			printf("\nFIRST-%s", line);
#endif
			if (currClsNum == -2)
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Clauses come before the number of variables os stated", lineNum);
				break;
			} 
			
			if (numOfEl == 0)
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Empty line", lineNum);
				FreeList(&lineSplitted);
				break;
			}

			//getting the rest of the clause if it consists of a few lines
			strcpy(cls, line);

			while ( (strcmp(lineSplitted[numOfEl-1],"0"))&&(strlen(cls)<(MAXCHARINCLS-MAXLINE)) ) 
			{
				FreeList(&lineSplitted);
				if (fgets(line, MAXLINE, inFile)) 
				{
					lineNum++;
					strcat(cls, line);
					SplitList(cls, &numOfEl, &lineSplitted); 
				} else {
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Unexpected end of clause", lineNum);
					FreeList(&lineSplitted);
					break;
				}
			}

			//There are 3 reasons for previous while loop
			//ending : 
			//1)end of file 
			//2)no zero despite reading MAXCHARINCLS-MAXLINE characters
			//3)clause has been read
			//Next lines react appropriately to the reason
			
			//end of file
			if ((strcmp(lineSplitted[numOfEl-1],"0"))&&(strlen(cls)<MAXCHARINCLS-MAXLINE))
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Unexpected end of file", lineNum);
				break;
			} 
			
			//no zero despite reading MAXCHARINCLS-MAXLINE characters
			if (strlen(cls)>=MAXCHARINCLS-MAXLINE)
			{
				//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Clause is too long or 0 not appears in the end of some clause/clauses", lineNum);
				FreeList(&lineSplitted);
				break;
			}
			
			//clause has been read
			//checking the validity of the clause and if everything is OK 
			//inserting the literals into lits array.
			////fprintf(outFile, "\nCurrClsNum is %d", currClsNum);
			currLitsNum = 0;
			errInCls = FALSE;
			for (i=0; i<numOfEl-1; i++)
			{
				currLit = atol(lineSplitted[i]);
				if ((labs(currLit) < 1) || (labs(currLit) > cnfInfo->initVarsNum))
				{
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Literal in clause is wrong. Clause ignored", lineNum);
					errInCls = TRUE;
					break;
				}
				
				//checking whether the variable already appears
				//in the clause
				litAlreadyApp = FALSE;
				tau = FALSE;
				for (j=0; j<currLitsNum; j++)
				{
					if (currLit == lits[j])
					{
						litAlreadyApp = TRUE;
						break;
					}
					
					if (currLit == -lits[j])
					{
						tau = TRUE;
						break;
					}
				}
				
				if (tau) 
				{
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : Clause is a tautology. Clause ignored.", lineNum);
					errInCls = TRUE;
					break;
				}
				
				//if the literal is new to the clause inserting it
				if (!litAlreadyApp)
				{
					lits[currLitsNum++] = currLit;
				}
			}

			if (!errInCls)
			{
				if (!currLitsNum)
				{
					//fprintf(outFile, "\nREAD_FROM_FILE(line #%d) : No literals in clause. Clause ignored.", lineNum);
				} else
				//now building the clause itself
				{
					++currClsNum;
					
					if (currLitsNum != 1)
						cnfInfo->sizeOfBuffForCnf+=(2*sizeof(unsigned long) + 2*sizeof(CCArrowsBox) + currLitsNum*sizeof(CCBox));					
					if (currOvBuffInd+currLitsNum*8+1 > cnfInfo->currOvBuffSize)
					{
						cnfInfo->clssBuff = realloc(cnfInfo->clssBuff, cnfInfo->currOvBuffSize+4*MAXLINE);
						cnfInfo->currOvBuffSize+=4*MAXLINE;
					}
					for (i = 0; i < currLitsNum; i++)
					{
						if (i != (currLitsNum-1))
							currOvBuffInd+=sprintf(cnfInfo->clssBuff+currOvBuffInd, "%ld ", lits[i]);
						else
							currOvBuffInd+=sprintf(cnfInfo->clssBuff+currOvBuffInd, "%ld\0", lits[i]);
					}
					currOvBuffInd++;
				}
			}

			FreeList(&lineSplitted);
			break;			
		}
	}

	cnfInfo->clssBuff = realloc(cnfInfo->clssBuff, currOvBuffInd);
	cnfInfo->currOvBuffSize = currOvBuffInd;
	cnfInfo->initClssNum = currClsNum+1;
	
	return cnfInfo;
}

Cnf* BuildClausesNoTcl(CnfInfo* cnfInfo, unsigned long cutOff, double maxClsLenToRec, double maxCCBoxesInCC, unsigned long cbNumToRecAnyway, long heur, unsigned short qsNum, double maxInterval, double notToChkConfsFrom, double heurPartOfS1, unsigned long restartAft, unsigned long restartAdd, long clsDensFromNotInc, Boolean ifAdd1UipClause, long max1UIPLenToRec, int incrHeur1UIP, int restartsStrategy, int restartDelayTimes, int restartsMaxClssToRec, char ifGetFirstBackAftRestart)
{
	Cnf* cnf;
	unsigned long bytesNeededForCls, currLitsNum;
	long toInsFromBuffInd, i, currLit;	
	char** lineSplitted;	
	CCBox *ccBox;
	CCArrowsBox *ccArrowBox;
	char wchStat;
	Var* currVar;
	double maxVal;
	unsigned long currBuffInd;
	Boolean ifGetFirst;

	srand( (unsigned)time( NULL ) );
	
	// creating the cnf 
	if (!(cnf = calloc(1, sizeof(Cnf))))
	{
		fprintf(outFile, "\nBUILD_CNF_FROM_FILE: Couldn't allocate memory for CNF");		
		return 0;
	}

	cnf->d2VarsConfsCheckMinImplLvl = ULONG_MAX;
	cnf->notToChkConfsFrom = notToChkConfsFrom;
	cnf->heur = heur;
	cnf->heurPartOfS1 = heurPartOfS1;
	cnf->cutOff = cutOff;
	cnf->initClssNum = cnfInfo->initClssNum;
	cnf->initVarsNum = cnfInfo->initVarsNum;
	cnf->maxClsLenToRec = max(maxClsLenToRec,3);
	cnf->maxCCBoxesInCC = max(maxCCBoxesInCC,3);
	cnf->restartAdd = cnf->restartAddInit = restartAdd;
	cnf->restartAft = cnf->restartAftInit = restartAft;
	cnf->varsNumForRestart = malloc(cnf->initVarsNum*sizeof(long));
	cnf->clsDensFromNotInc = clsDensFromNotInc;
	cnf->restartsStrategy = restartsStrategy;	
	cnf->restartDelayTimes = restartDelayTimes;
	toInsFromBuffInd = 0;
				
	//			cnf->implLvlNotCPFromDown = -1;
	cnf->vars = calloc(cnf->initVarsNum+1, sizeof(Var));
	cnf->pushD1BlockSt = CreateStack(cnf->initVarsNum);
	cnf->toBeImplied = CreateQueue(cnf->initVarsNum);
	cnf->ivs = IVSCreate(cnf->initVarsNum);
	cnf->prePushSt = CreateStack(cnf->initVarsNum);
	cnf->lastConfNum = calloc(cnf->initVarsNum+1, sizeof(unsigned long));
	cnf->lastPrepushConfNum = calloc(cnf->initVarsNum+1, sizeof(unsigned long));
	cnf->rccCPLits = CreateStack(cnf->initVarsNum);
	cnf->confVarsPart = CreateStack(cnf->initVarsNum);
	cnf->initCCBuff = malloc(cnfInfo->sizeOfBuffForCnf);
	cnf->sizeOfBuffForCnf = cnfInfo->sizeOfBuffForCnf;
	cnf->lastLayerPerCpNum = calloc(cnf->initVarsNum+1, sizeof(long));
	cnf->layersArr = calloc(cnf->initVarsNum+1, sizeof(LLBox*));
	cnf->lastLayerCoPerCpLvl = calloc(cnf->initVarsNum+1, sizeof(LLBox*));
	cnf->ifAdd1UipClause = ifAdd1UipClause;
	cnf->max1UIPLenToRec = max1UIPLenToRec;
	cnf->incrHeur1UIP = incrHeur1UIP;
	ifGetFirst = heur&1;
	cnf->restartsMaxClssToRec = restartsMaxClssToRec;
	cnf->minD1Depth = ULONG_MAX;
	cnf->ifGetFirstBackAftRestart = ifGetFirstBackAftRestart;
	
	for (currBuffInd = 0; currBuffInd < cnfInfo->currOvBuffSize; currBuffInd+=(strlen(cnfInfo->clssBuff+currBuffInd)+1))
	{
	//while (fgets(line, MAXLINE, inFile)) {
		SplitList(cnfInfo->clssBuff+currBuffInd, &currLitsNum, &lineSplitted); 
		
		//now building the clause itself		
		if (currLitsNum != 1)
		{
			bytesNeededForCls = 2*sizeof(unsigned long) + 2*sizeof(CCArrowsBox) + currLitsNum*sizeof(CCBox);
						
			//setting the number of lits
			*((unsigned long*)(&cnf->initCCBuff[toInsFromBuffInd])) = currLitsNum;
			toInsFromBuffInd+=sizeof(unsigned long);
			//setting "sorted from"
			*((unsigned long*)(&cnf->initCCBuff[toInsFromBuffInd])) = 0;
			toInsFromBuffInd+=sizeof(unsigned long);
			for (i = 0; i < currLitsNum; i++)
			{							
				ccBox = (CCBox*)(&cnf->initCCBuff[toInsFromBuffInd]);					
				toInsFromBuffInd+=sizeof(CCBox);
				
				if (i == currLitsNum - 2)
				{
					ccArrowBox = (CCArrowsBox*)(&cnf->initCCBuff[toInsFromBuffInd+sizeof(CCBox)]);
					wchStat = FALSE;//WCH_STAT_UP;
				}
				else
				{
					if (i == currLitsNum - 1)
					{
						ccArrowBox = (CCArrowsBox*)(&cnf->initCCBuff[toInsFromBuffInd+sizeof(CCArrowsBox)]);									
						wchStat = TRUE;//WCH_STAT_DOWN;
					}
					else
						wchStat = FALSE;
				}
				//ifSorted == FALSE is relevant only for the last cb
				FillCCBoxInBldCnf(ccBox, atol(lineSplitted[i]), i, wchStat);
				
				currVar = &cnf->vars[labs(atol(lineSplitted[i]))];
				
				((atol(lineSplitted[i]) > 0) ? (currVar->overallApp[POS_APP]++) : (currVar->overallApp[NEG_APP]++));
			}
			
			//arranging watched boxes pointers
			InsWCCBoxIntoVarsWList(cnf, &cnf->initCCBuff[toInsFromBuffInd-2*sizeof(CCBox)], &cnf->initCCBuff[toInsFromBuffInd-sizeof(CCBox)]);
			InsWCCBoxIntoVarsWList(cnf, &cnf->initCCBuff[toInsFromBuffInd-sizeof(CCBox)], &cnf->initCCBuff[toInsFromBuffInd-sizeof(CCBox)]);						
			
			toInsFromBuffInd+=2*sizeof(CCArrowsBox);
			
		} else
		{
			currVar = &cnf->vars[labs(atol(lineSplitted[0]))];						
			
			if (!currVar->aisBox)
			{
				currVar->aisBox = calloc(1, sizeof(LLBox));
				currVar->aisBox->litNum = currVar->varNum;
			}
			if (currVar->aisStatus == notImplOut)
				PushIntoAddImplSt(cnf, atol(lineSplitted[0]));
			else
			{
				if (currVar->aisBox->litNum == -atol(lineSplitted[0]))
				{
					//UNSAT!!!
					ifUnsAftBuildCnf = TRUE;
					//return 0;
				}
			}
		}
				
		FreeList(&lineSplitted);
	}

	for (maxVal = 0.0, i = 1; i <= cnf->initVarsNum; i++)
	{
		currVar = &cnf->vars[i];
		currVar->varNum = i;
		currVar->pqBox = calloc(1, sizeof(OneQBox));
		currVar->pqBox->litNum = i;
		currVar->implStat = NOT_IMPLIED;
		if (!currVar->aisBox)
		{
			currVar->aisBox = calloc(1, sizeof(LLBox));
			currVar->aisBox->litNum = i;
		}
		if (i != 1)
		{
			currVar->pqBox->prev = cnf->vars[i-1].pqBox;
			cnf->vars[i-1].pqBox->next = currVar->pqBox;
		}
		
		if (maxVal < GetLitsVal(cnf, i))
			maxVal = GetLitsVal(cnf, i);
		currVar->cbNumToRecAnyway = cbNumToRecAnyway;	
		cnf->varsNumForRestart[i-1] = i;
		currVar->layerBox.litNum = i;
	}

	cnf->initClssNum = cnf->currCCNum = cnfInfo->initClssNum;

	cnf->pq = PQCreate(cnf, qsNum, maxInterval, ifGetFirst, GetLitsVal, GetVarsSign, ReduceInitHeurToMax);

#ifdef CHECK_PQ
	PQPrint(cnf,	 cnf->pq, GetLitsVal);
	PQRemVar(cnf, cnf->pq, 1);
	PQRemVar(cnf, cnf->pq, 50);
	PQRemVar(cnf, cnf->pq, 2);
	PQRemVar(cnf, cnf->pq, 18);
	PQPrint(cnf, cnf->pq, GetLitsVal);	
	for (i=0; i<10; i++)
	{
		j = PQRemMaxVar(cnf, cnf->pq, GetLitsVal);
		PQPrint(cnf, cnf->pq, GetLitsVal);
		//cnf->vars[labs(j)].posInitClssApp+=(i+1)*10;
		PQAddVar(cnf, cnf->pq, j, GetLitsVal, GetVarsSign);
		PQPrint(cnf, cnf->pq, GetLitsVal);
	}
#endif

	theCnf = cnf;
	free(cnfInfo->clssBuff);

	return cnf;
}
