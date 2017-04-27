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

#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#ifdef _DEBUG
#include <windows.h>
#endif

#include "C4Globals.h"
#include "C4PQ.h"

#ifdef WIN32
#include <crtdbg.h>
#endif
#ifdef FBG1UIP
#include "C4FBG.h"
#endif
#ifdef FBGNEWRESTART
#include "C4FBG.h"
#endif

#ifdef WIN32
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
void AnalyzeNewConflict(Cnf* cnf, CCBox* clsWithContradiction, ConfCircum cc)
{
	CCBox* ccb;
	Var *var, *headD2Var;
	long litNum;
	Boolean ifUpdAisAft;
	long skipTill;
	
#ifdef FBG
	long i;
	Boolean ifModel;
	for (ifModel = TRUE, i = cnf->ivs->cpHead; i>=0; i = cnf->ivs->bs[i].prevCPInd)
	{
		if (seqFBG[labs(cnf->ivs->bs[i].litNum)] != cnf->ivs->bs[i].litNum)
		{
			ifModel = FALSE;
			break;
		}
	}
	if (ifModel)
		DebugBreak();
#endif
	skipTill = cnf->implLvlToSkipTill = LONG_MAX;
	cnf->lastCCBoxOfCCNotToDel = 0;
	cnf->actualCCNum++;
	
	switch (cc) {
	case aiDummyAftConf:
	case aiDummyAftD2:
		//this is the AIS var inconsistently implied as REG_RES
		var = &cnf->vars[cnf->varNumWithContr];
		if (var->aisLastCCBox)
			GetVarsInConf(cnf, var->aisLastCCBox);
		else
			GetVarsInConf(cnf, var->implCls);
		RecConf(cnf, -var->ivsBox->litNum);
		break;
	case confDummyRegRes:
		var = &cnf->vars[cnf->varNumWithContr];
		GetVarsInConf(cnf, var->implCls);
		RecConf(cnf, -var->ivsBox->litNum);
		break;
	case confDummyAddImpl:
		var = &cnf->vars[cnf->varNumWithContr];
		var->thisImplApp++;
		if (var->aisLastCCBox)
		{
			headD2Var = &cnf->vars[labs(cnf->ivs->bs[cnf->ivs->d2Head].litNum)];

			if (labs(((var->aisLastCCBox)-1)->litNum) == headD2Var->varNum)
				headD2Var->thisImplApp++;
				
		}
		cnf->currCCNum++;
		break;
	default:
		if (cnf->ifAdd1UipClause)
		{
			ccb = Add1UIPClause(cnf, clsWithContradiction, &litNum);
			/*
			UpdAISInfoFor1UIP(cnf, ccb, litNum);
			skipTill = cnf->implLvlToSkipTill;
			cnf->implLvlToSkipTill = LONG_MAX;*/
			
#ifdef PRINT_IVS
			if (TC(cnf, "PRINT_IVS"))
			{
				GoThrowAllLists(cnf);
			}
#endif
		}
		GetVarsInConf(cnf, clsWithContradiction);
		ccb = RecClsAndConf(cnf, &ifUpdAisAft);
		if (ifUpdAisAft)
		{
			if (ccb)
				UpdateAddImplStInfo(cnf, ccb, ccb->litNum);
			else
				UpdateAddImplStInfo(cnf, 0, cnf->ivs->bs[cnf->ivs->cpHead].litNum);
		}
		cnf->implLvlToSkipTill = min(cnf->implLvlToSkipTill, skipTill);
		break;
	}
}

void GetVarsInConf_(Cnf* cnf, CCBox* clsWithContradiction)
{
	Var *currVar;
	CCBox *vertBox;
	//char leaveOneVar; 
#ifdef PRINT_IVS
	unsigned long* currImplStat;
	long litNumAsImplied;
	/*	PrintCCGivenCCBox(cnf, clsWithContradiction, TRUE, FALSE);
	OutputTrace(cnf, "\nGetVarsInConf", 63, "", buff);*/
	
#endif
	//leaveOneVar = 0;
	if ((cnf->heur&128)&&(CCB_DOWN_FROM_CC(clsWithContradiction)->clsFromConfStat))
	{
		currVar = &cnf->vars[labs(*(GetDelClsLit(clsWithContradiction)))];
		if ((!cnf->varToDelCls)||(currVar->implLevel > cnf->vars[labs(cnf->varToDelCls)].implLevel))
			cnf->varToDelCls = currVar->varNum;
			/*		if (CCB_DOWN_FROM_CC(clsWithContradiction)->clsFromConfStat == 2)
		leaveOneVar = 1;*/
	}
	for (vertBox = CLS_CCBOXES_BEG(clsWithContradiction)/*+leaveOneVar*/; (vertBox != FRST_ARROW_FROM_CC(clsWithContradiction)); vertBox++)
	{
		if (cnf->lastConfNum[labs(vertBox->litNum)] != (cnf->currCCNum+1))
		{
			currVar = &cnf->vars[labs(vertBox->litNum)];
			if (currVar->implLevel > cnf->initAisVarsImplLvlIncl)
			{
				//if the variable is implied independently of others
				//or is implied in this clause
				if ( (!currVar->implCls) || (CLS_CCBOXES_BEG(currVar->implCls) == CLS_CCBOXES_BEG(clsWithContradiction)) )
				{
					currVar->thisImplApp++;
					cnf->lastConfNum[labs(vertBox->litNum)] = cnf->currCCNum+1;
					
					IncrHeur(cnf, (currVar->implStat&IMPL_POS)?(currVar->varNum):(-(long)(currVar->varNum)), 1);
					
					if (currVar->implLevel < cnf->implLvlMinInCC)
						cnf->implLvlMinInCC = currVar->implLevel;
					
#ifdef PRINT_IVS
					if (TC(cnf, "PRINT_IVS"))
					{
						currImplStat = &cnf->vars[labs(vertBox->litNum)].implStat;
						
						if (ON(*currImplStat,IMPL_POS))
							litNumAsImplied = labs(vertBox->litNum);
						else
							litNumAsImplied = -labs(vertBox->litNum);
						
						buffInd+=sprintf(buff+buffInd, "%d%c,", litNumAsImplied, GetStatLetter(cnf, litNumAsImplied, *currImplStat));fflush(outFile);
					}
#endif
					
				}
				else
					GetVarsInConf_(cnf, currVar->implCls);
			} else
				cnf->lastConfNum[labs(vertBox->litNum)] = cnf->currCCNum+1;
		}
	}
}

void GetVarsInConf(Cnf* cnf, CCBox* clsWithContradiction)
{
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd = 0;
		buffInd = sprintf(buff, "\nCONFLICT: {");
	}
#endif
	cnf->implLvlMinInCC = LONG_MAX;
	cnf->varToDelCls = 0;
	GetVarsInConf_(cnf, clsWithContradiction);
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd+=sprintf(buff+buffInd, "}");fflush(outFile);
		OutputTrace(cnf, "\nGetVarsInConf", 22, "", buff);
	}
#endif
}

void RecClsFromConfIVSVars(Cnf* cnf)
{
	Var *d2Var, *var, *aisVar, *ccTill;
	char* cc;
	CCBox* litsArr;
	unsigned long litsNum;
	long litsInd, ivsInd;
	OneIVSBox* ib;
#ifdef EXT2_PRINT
	if (TC(cnf, "EXT2_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nRecClsFromConfIVSVars : ");
	}
#endif
	d2Var = &cnf->vars[labs(cnf->ivs->bs[cnf->ivs->d2Head].litNum)];
	
	aisVar = ((ElemNum(cnf->confVarsPart) == 1) ? (&cnf->vars[labs(GetHead(cnf->confVarsPart))]) : (0));
	/*if ((d2Var->d2ImplStatBefCheckConf < d2Var->thisImplApp)||aisVar)
	Push(cnf->confVarsPart, cnf->ivs->bs[cnf->ivs->d2Head].litNum);*/
	
	if (d2Var->d2ImplStatBefCheckConf < d2Var->thisImplApp)
	{
		Push(cnf->confVarsPart, cnf->ivs->bs[cnf->ivs->d2Head].litNum);	
		ccTill = d2Var;
	}
	else
	{
		for (ccTill = 0, ivsInd  = d2Var->ivsBox->prevCPInd; ( (ivsInd != -1) && (cnf->vars[labs(cnf->ivs->bs[ivsInd].litNum)].implLevel > cnf->initAisVarsImplLvlIncl)); ivsInd = ib->prevCPInd, ccTill = 0)
		{
			ib = &cnf->ivs->bs[ivsInd];
			ccTill = &cnf->vars[labs(ib->litNum)];
			if (ccTill->thisImplApp) break;
		}
		if (ccTill)
			Push(cnf->confVarsPart, ib->litNum);	
	}
	
	litsNum = ElemNum(cnf->confVarsPart);
	
	if (litsNum > 1)
	{
		//allocating memory for the clause
		cc = malloc(
			sizeof(unsigned long) + //number of literals
			sizeof(unsigned long) + //sorted from
			litsNum*sizeof(CCBox) + //literals' CC boxes
			2*sizeof(CCArrowsBox) + //watched(or AIS del. info.) arrow boxes
			sizeof(CCArrowsBox) + //delete cls arrows, if needed
			sizeof(CCArrowsBox) + //delete conflixt arrows
			sizeof(CCArrowsBox) + //CC list arrows
			//d1 order for var being checked currently, valid ONLY
			//if ifNextClsBoxInd becomes TRUE
			sizeof(long) +
			//delClsLit
			sizeof(long)
			);
		
		litsArr = (CCBox*)(&(cc[2*sizeof(unsigned long)])); 
		//filling cc with information
		
		//number of literals
		(*((unsigned long*)(&cc[0]))) = litsNum;
		
		//setting watch CC boxes info
		
		litsArr[litsNum-1].ccType = CC_TYPE_CLS_ONLY;
		litsArr[litsNum-1].ifSorted = FALSE;
		litsArr[litsNum-1].ifConfSimTreated = FALSE;
		litsArr[litsNum-1].ifDelCls = ((ccTill)?(1):(0));
		litsArr[litsNum-1].ifRecClsFromConfCreated = 1;
		litsArr[litsNum-1].ifAdd1UIP = 0;
		
		//literals' CC boxes
		for (litsInd = 0; (!(IsEmpty(cnf->confVarsPart))); litsInd++)
		{
			litsArr[litsInd].litNum = -Pop(cnf->confVarsPart);
			litsArr[litsInd].ordFromUp0Ind = litsInd;
			
			var = &cnf->vars[labs(litsArr[litsInd].litNum)];
#ifdef EXT2_PRINT
			if (TC(cnf, "EXT2_PRINT"))
			{
				//	buffInd += sprintf(buff+buffInd, "%ld, ", litsArr[litsInd].litNum);
			}
#endif
			litsArr[litsInd].ifNextClsBoxInd = 1;
			litsArr[litsInd].ifUsedConfsDelHead = 0;
			litsArr[litsInd].nextClsIndOrD1ArrOrd = litsInd-1;		
		}
		
		if (aisVar)
		{
			aisVar->aisLastCCBox = &litsArr[litsNum-1];		
			PushIntoAddImplSt(cnf, -cnf->confVarsPart->elemArr[0]);
			litsArr[litsNum-1].ifBot2 = 0;
			//InsCCIntoLstBeg(cnf, &cnf->vars[labs(litsArr[litsNum-2].litNum)].bot2CC, &litsArr[litsNum-2], Bot2FromCcBox);
		} else
		{
			litsArr[litsNum-1].ifBot2 = 1;
			InsWCCBoxIntoVarsWList(cnf, &litsArr[litsNum-1], &litsArr[litsNum-1]);
			InsWCCBoxIntoVarsWList(cnf, &litsArr[litsNum-2], &litsArr[litsNum-1]);
		}
		
		
		//set "delete cls arrows"
		if (litsArr[litsNum-1].ifDelCls)
		{
			litsArr[litsNum-1].clsFromConfStat = 1;
			InsCCIntoLstBeg(cnf, &ccTill->clssToDel, &litsArr[litsNum-1], GetDelCls);
			*GetDelClsLit(litsArr) = ccTill->varNum;
		} else
		{
			litsArr[litsNum-1].clsFromConfStat = 0;
		}
		*(D1EmergOrd(litsArr)) = -1;
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
		{
			PrintCCGivenCCBox(cnf, litsArr, TRUE, TRUE);
			OutputTrace(cnf, "\nRecClsFromConfIVSVars", 39, "", buff);
		}
#endif
	} else
	{
		aisVar->aisLastCCBox = 0;
		PushIntoAddImplSt(cnf, -cnf->confVarsPart->elemArr[0]);
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nRecClsFromConfIVSVars : no CC - ais %ld added", -cnf->confVarsPart->elemArr[0]);
			OutputTrace(cnf, "\nRecClsFromConfIVSVars", 39, "", buff);
		}
#endif
	}
}

CCBox* RecClsAndConf_(Cnf* cnf, Boolean ifConfOnly, long addLitNum, Boolean* ifUpdAisAft)
{
	IVS* ivs;
	OneIVSBox *ib;
	unsigned long d1LiveNum, obsolRccStLitsNum, litsNum;
	long litsInd, d1ArrInd, ivsInd;
	Var  *var, *varToDelCls;
	Boolean ifDelCls;
	char* cc;
	CCBox* litsArr;
	long clsDensityVal;
	//CCArrowsBox *d1ArrowsArr, *currArrowsBox;
#ifdef FBGNEWRESTART
	Boolean fbgOneSat;
//	if (cnf->actualCCNum == 170)
//		DebugBreak();
#endif
	
	*ifUpdAisAft = TRUE;
	ivs = cnf->ivs;
	d1LiveNum = 0;
	EmptyStack(cnf->rccCPLits);
	
	if (addLitNum)
	{
		var = &cnf->vars[labs(addLitNum)];
		
		Push(cnf->rccCPLits, -addLitNum);
		
		var->ifD1LiveInCurrCC = FALSE;
		//var->thisImplApp++;
		
	}
	//recording lits till cnf->maxCCBoxesInCC 
	for (clsDensityVal = 0,obsolRccStLitsNum = 0, ivsInd = ivs->cpHead; ( (ivsInd != -1) && (cnf->vars[labs(ivs->bs[ivsInd].litNum)].implLevel >= cnf->implLvlMinInCC)); ivsInd = ib->prevCPInd)
		//for (obsolRccStLitsNum = 0, ivsInd = ivs->cpHead; ( (ivsInd != -1) && (cnf->vars[labs(ivs->bs[ivsInd].litNum)].implLevel >= cnf->implLvlMinInCC)); ivsInd = ib->prevCPInd)
	{
		ib = &ivs->bs[ivsInd];
#ifdef WIN32
		_ASSERT((cnf->heur&128)||(ib->prevCPInd == ib->prevCPInd));
#endif
		if (cnf->lastConfNum[labs(ib->litNum)] == (cnf->currCCNum+1))
		{
			var = &cnf->vars[labs(ib->litNum)];
			//var->thisImplApp++;	
			Push(cnf->rccCPLits, -ib->litNum);
			
			var->ifD1LiveInCurrCC = FALSE;
			
			if (ElemNum(cnf->rccCPLits) > cnf->maxCCBoxesInCC)
			{
				if ( ((var->thisImplApp - 1) == var->thisImplCCBox)
					&& 
					(var->thisImplApp <= var->cbNumToRecAnyway)
					)
				{
					obsolRccStLitsNum = 0;
					if ( (ON(var->implStat, IMPL_D1)) && (var->cpImplLevel != ivs->cpNum) )
					{
						d1LiveNum++;
						var->ifD1LiveInCurrCC = TRUE;
					}
				} else
				{
					obsolRccStLitsNum++;
				}
			} else
			{
				if ((var->thisImplApp - 1) == var->thisImplCCBox)// || (var->thisImplApp <= var->cbNumToRecAnyway) )
				{
					obsolRccStLitsNum = 0;
					if ( (ON(var->implStat, IMPL_D1)) && (var->cpImplLevel != ivs->cpNum) )
					{
						d1LiveNum++;
						var->ifD1LiveInCurrCC = TRUE;
					}
				}
				else
				{
					obsolRccStLitsNum++;
				}
			}		
			
			if (ElemNum(cnf->rccCPLits) == cnf->maxClsLenToRec)
			{
				if (clsDensityVal < cnf->clsDensFromNotInc)
				{
					ifConfOnly = 1;					
				}
			}
			
			if ((ifConfOnly)&&(ElemNum(cnf->rccCPLits) >= cnf->maxCCBoxesInCC))
				break;
			
		} else
		{
			clsDensityVal+=(cnf->maxClsLenToRec > ElemNum(cnf->rccCPLits))*(cnf->maxClsLenToRec-ElemNum(cnf->rccCPLits));
		}
	}
	
	if ((!ifConfOnly)&&(clsDensityVal < cnf->clsDensFromNotInc))
	{
		ifConfOnly = 1;
	}
	if (ifConfOnly)
		*ifUpdAisAft = FALSE;
	
	if (ElemNum(cnf->rccCPLits) == 1)
	{	
		cnf->currCCNum++;
		return 0;
	}
	
	if ( (ElemNum(cnf->rccCPLits) - obsolRccStLitsNum) == 1)
		obsolRccStLitsNum--;
	
	if (!ifConfOnly)
		ifDelCls = (((ElemNum(cnf->rccCPLits) > cnf->maxClsLenToRec)||(cnf->varToDelCls)) ? (1) : (0));
	else
		ifDelCls = 0;
#ifdef FBGNEWRESTART
	fbgOneSat = FALSE;
#endif
	
	cc = malloc(
		sizeof(unsigned long) + //number of literals
		sizeof(unsigned long) + //sorted from
		ElemNum(cnf->rccCPLits)*sizeof(CCBox) + //literals' CC boxes
		2*sizeof(CCArrowsBox) + //watched(or AIS del. info.) arrow boxes
		sizeof(CCArrowsBox) + //delete cls arrows, if needed
		sizeof(CCArrowsBox) + //delete conflict arrows
		sizeof(CCArrowsBox) + //CC list arrows
		//d1 order for var being checked currently, valid ONLY
		//if ifNextClsBoxInd becomes TRUE
		sizeof(long) +
		//delClsLit
		sizeof(long) +
		d1LiveNum*sizeof(CCArrowsBox) //D1 vars arrows
		);
	
	litsArr = (CCBox*)(&(cc[2*sizeof(unsigned long)])); 
	//filling cc with information
	
	//number of literals
	(*((unsigned long*)(&cc[0]))) = ElemNum(cnf->rccCPLits);
	//sorted from
	(*((unsigned long*)(&cc[sizeof(unsigned long)]))) = cnf->currCCNum;
	
	litsNum = ElemNum(cnf->rccCPLits);
	//setting watch CC boxes info
	//Note : ifDelCls has to be set before the loop, for GetD1ArrFromCC to work properly
	
	litsArr[litsNum-1].ifDelCls = ifDelCls;
	litsArr[litsNum-1].ccType = ((ifConfOnly) ? (CC_TYPE_CONF_ONLY) : (CC_TYPE_CONF_CLS));
	litsArr[litsNum-1].ifSorted = TRUE;
	litsArr[litsNum-1].ifConfSimTreated = FALSE;
	litsArr[litsNum-1].clsFromConfStat = ((cnf->varToDelCls&&ifDelCls) ? (1) : (0));
	litsArr[litsNum-1].ifBot2 = 1;
	litsArr[litsNum-1].ifRecClsFromConfCreated = 0;
	litsArr[litsNum-1].ifAdd1UIP = 0;
	//d1ArrowsArr = (CCArrowsBox*)(&(cc[2*sizeof(unsigned long)+ElemNum(cnf->rccCPLits)*sizeof(CCBox)+(2*(litsArr[litsNum-1].ccType == CC_TYPE_CONF_CLS)+2+ifDelCls)*sizeof(CCArrowsBox)]));
	
	//literals' CC boxes and D1 arrow boxes
	for (litsInd = d1ArrInd = 0; (!(IsEmpty(cnf->rccCPLits))); litsInd++)
	{
		litsArr[litsInd].litNum = Pop(cnf->rccCPLits);
#ifdef FBGNEWRESTART
		if (litsArr[litsInd].litNum == fbgNewRest[cnf->vars[labs(litsArr[litsInd].litNum)].varNum])
			fbgOneSat = TRUE;
#endif
		
		litsArr[litsInd].ordFromUp0Ind = litsInd;
		//		litsArr[litsInd].ifOnWchList = FALSE;
		
		var = &cnf->vars[labs(litsArr[litsInd].litNum)];
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd += sprintf(buff+buffInd, "%ld%c", litsArr[litsInd].litNum, GetStatLetter(cnf, litsArr[litsInd].litNum, var->implStat));
			
			if (var->ifD1LiveInCurrCC)
				buffInd += sprintf(buff+buffInd, "]");
			if (litsInd == obsolRccStLitsNum)
				buffInd += sprintf(buff+buffInd, "*");
			buffInd += sprintf(buff+buffInd, ", ");
			//	OutputTrace(cnf, "\nRecClsAndConf_", 287, "", buff);
		}
#endif
		
		if (litsInd >= obsolRccStLitsNum)
			var->thisImplCCBox++;
		
		litsArr[litsInd].ifNextClsBoxInd = 0;
		if (litsInd == obsolRccStLitsNum)
			litsArr[litsInd].ifUsedConfsDelHead = 1;
		else
			litsArr[litsInd].ifUsedConfsDelHead = 0;
		
		if (var->ifD1LiveInCurrCC)
		{
			litsArr[litsInd].nextClsIndOrD1ArrOrd = d1ArrInd;
			
			InsCCIntoLstBeg(cnf, &var->d1ConfsList, &litsArr[litsInd], GetD1ArrFromCC);
			
			var->thisImplD1Box++;
			
			d1ArrInd++;
			
		} else
		{
			litsArr[litsInd].nextClsIndOrD1ArrOrd = -1;
		}
	}
#ifdef FBGNEWRESTART
	if ((!ifConfOnly)&&(!fbgOneSat))
		DebugBreak();
#endif
	
	//set "delete cls arrows", if needed
	if (ifDelCls)
	{
		if ((litsNum > cnf->maxClsLenToRec)&&(cnf->vars[labs(litsArr[litsNum-cnf->maxClsLenToRec-1].litNum)].implLevel > cnf->vars[labs(cnf->varToDelCls)].implLevel))
			varToDelCls = &cnf->vars[labs(litsArr[litsNum-cnf->maxClsLenToRec-1].litNum)];
		else
			varToDelCls = &cnf->vars[labs(cnf->varToDelCls)];
		
		InsCCIntoLstBeg(cnf, &varToDelCls->clssToDel, &litsArr[litsNum-1], GetDelCls);
		*GetDelClsLit(litsArr) = varToDelCls->varNum;
	}
	//InsCCIntoLstBeg(cnf, &cnf->vars[labs(litsArr[litsNum-cnf->maxClsLenToRec-1].litNum)].clssToDel, &litsArr[litsNum-cnf->maxClsLenToRec-1], GetDelCls);
	//set "delete used conf. arrows" into appropriate list
	InsCCIntoLstBeg(cnf, &cnf->vars[labs(litsArr[obsolRccStLitsNum].litNum)].usedConfToDel, &litsArr[obsolRccStLitsNum], GetDelUsedConf);
	//set "added confs list arrows"
	InsToEndOfConfLst(cnf, &litsArr[litsNum-1]);
	//update 2nd from bottom var list to ensure that 2 bottommost CCBox's
	//will be added appropriate watched box lists
	if (!ifConfOnly)
	{
		if ((!ifDelCls)||(varToDelCls->varNum != labs(litsArr[litsNum-2].litNum)))
			InsCCIntoLstBeg(cnf, &cnf->vars[labs(litsArr[litsNum-2].litNum)].bot2CC, &litsArr[litsNum-2], Bot2FromCcBox);
		else
			litsArr[litsNum-1].ifBot2 = 0;
	}
	*(D1EmergOrd(litsArr)) = -1;
	
	cnf->currCCNum++;
	
#ifdef FBG1UIP
	for (fbgCcb = litsArr; fbgCcb != FRST_ARROW_FROM_CC(litsArr); fbgCcb++)
	{
		if (fbgCcb->litNum*seqFBG[labs(fbgCcb->litNum)] > 0)
			goto AftDebugBreak;
	}
	DebugBreak();
AftDebugBreak:
#endif
	
	return (cnf->lastCCBOfLastCCAdded = (&litsArr[litsNum-1]));
}

CCBox* RecClsAndConf(Cnf* cnf, Boolean* ifUpdAisAft)
{
	CCBox* ccb;
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd = 0;
		buffInd = sprintf(buff, "\nCLS AND CONF #%ld:{", cnf->currCCNum+1);
	}
#endif
	ccb = RecClsAndConf_(cnf, FALSE, 0, ifUpdAisAft);
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd += sprintf(buff+buffInd, "}");
		if (ccb)
			PrintCCGivenCCBox(cnf, cnf->lastCCBOfLastCCAdded, TRUE, TRUE);
		else
			buffInd+=sprintf(buff+buffInd, "\nNO CC TO ADD : CCB == 0");
		OutputTrace(cnf, "\nRecClsAndConf", 287, "", buff);
	}
#endif
	
	return ccb;
}

CCBox* RecConf(Cnf* cnf, long addLitNum)
{
	CCBox* ccb;
	Boolean ifUpdAisAft;
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd = 0;	
		buffInd = sprintf(buff, "\nCONF ONLY #%ld", cnf->currCCNum+1);
	}
#endif
	ccb = RecClsAndConf_(cnf, TRUE, addLitNum, &ifUpdAisAft);
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd += sprintf(buff+buffInd, "}");
		PrintCCGivenCCBox(cnf, cnf->lastCCBOfLastCCAdded, TRUE, FALSE);
		OutputTrace(cnf, "\nRecConf", 303, "", buff);
	}
#endif
	
	return ccb;
}

void UpdateAddImplStInfo(Cnf* cnf, CCBox* lastCCBox, long varNumToChk)
{
	Var *varToChk, *vrInImplStTill;
	long existImplLvlTill;
	AddImplStat oldAisStat;
	
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd = sprintf(buff, "\nUpdateAddImplStInfo:");
	}
#endif
	varToChk = &cnf->vars[labs(varNumToChk)];
	if (lastCCBox)
	{
		vrInImplStTill = &cnf->vars[labs((lastCCBox-1)->litNum)];
		/*		if ((lastCCBox->clsFromConfStat)&&(vrInImplStTill->implLevel < cnf->vars[labs(*GetDelClsLit(vrInImplStTill))].implLevel))
		{
		#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
		buffInd+=sprintf(buff+buffInd, "NO AIS ADD - CLS_FROM_CONF");
		OutputTrace(cnf, "\nUpdateAddImplStInfo", 345, "", buff);
		}
		#endif
		return;
	}*/
	}
	else
		vrInImplStTill = 0;
	
	if (!lastCCBox)
	{
		oldAisStat = varToChk->aisStatus;
		if (ON(varToChk->implStat,IMPL_ADD_IMPL_ST))
			varToChk->aisStatus = addImplChngSide;
		else
			varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
		
		if ( (ON(varToChk->implStat,IMPL_ADD_IMPL_ST)) 
			||
			( (ON(varToChk->implStat,IMPL_D2)) && (oldAisStat != confPartNotAdd) )
			)
		{
			cnf->lastCCBoxOfCCNotToDel = varToChk->aisCCOfVarToSkipTill = varToChk->aisLastCCBox;
			
			if (varToChk->aisLastCCBox)
				cnf->implLvlToSkipTill = cnf->vars[labs((varToChk->aisLastCCBox-1)->litNum)].implLevel;
			else
				cnf->implLvlToSkipTill = -1;
		}
		
		varToChk->aisLastCCBox = 0;
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "No last box");
			OutputTrace(cnf, "\nUpdateAddImplStInfo", 333, "", buff);
		}
#endif
		return;
	}
	
	/*InsWCCBoxIntoVarsWList(cnf, lastCCBox-1);
	InsWCCBoxIntoVarsWList(cnf, lastCCBox);*/
	
	if (!((IfAddToAis(cnf, varToChk->varNum, vrInImplStTill->varNum))))
	{
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "NO AIS ADD");
			OutputTrace(cnf, "\nUpdateAddImplStInfo", 345, "", buff);
		}
#endif
		return;
	}
	
	
	if (ON(varToChk->implStat,IMPL_D1))
	{
		varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
		varToChk->aisLastCCBox = lastCCBox;
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "D1 : %c; Skip:%ld", GetAisLetter(cnf, varToChk->varNum, varToChk->aisStatus), ((cnf->implLvlToSkipTill!=LONG_MAX)?(cnf->ivs->bs[cnf->implLvlToSkipTill-1].litNum):(LONG_MAX)));
		}
#endif
	}
	
	if (ON(varToChk->implStat,IMPL_D2))
	{
		if (varToChk->aisStatus == confPartNotAdd)
		{
			varToChk->aisLastCCBox = lastCCBox;
			varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
		} else
		{
			cnf->lastCCBoxOfCCNotToDel = varToChk->aisLastCCBox;
			existImplLvlTill = ((varToChk->aisLastCCBox) ? (cnf->vars[labs((varToChk->aisLastCCBox-1)->litNum)].implLevel) : (-1));
			//if 2-nd side implLevel > 1-side implLevel
			if (vrInImplStTill->implLevel > existImplLvlTill)
			{
				//leaving all 1-side information
				varToChk->aisCCOfVarToSkipTill = lastCCBox;
				cnf->implLvlToSkipTill = vrInImplStTill->implLevel;
			}
			
			//if 2-nd side implLevel < 1-side implLevel
			if (vrInImplStTill->implLevel < existImplLvlTill)
			{
				cnf->implLvlToSkipTill = existImplLvlTill;
				varToChk->aisCCOfVarToSkipTill = varToChk->aisLastCCBox;
				varToChk->aisLastCCBox = lastCCBox;
				varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
			}
			
			//if 2-nd side implLevel == 1-side implLevel
			if (vrInImplStTill->implLevel == existImplLvlTill)
			{
				varToChk->aisStatus = confPartNotAdd;
				cnf->implLvlToSkipTill = existImplLvlTill;
				varToChk->aisCCOfVarToSkipTill = lastCCBox;
			}
		}
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "D2 : %c; Skip:%ld", GetAisLetter(cnf, varToChk->varNum, varToChk->aisStatus), ((cnf->implLvlToSkipTill!=LONG_MAX)?(cnf->ivs->bs[cnf->implLvlToSkipTill-1].litNum):(LONG_MAX)));
		}
#endif
		
	}
	
	if (ON(varToChk->implStat,IMPL_ADD_IMPL_ST))
	{
		cnf->lastCCBoxOfCCNotToDel = varToChk->aisLastCCBox;
		existImplLvlTill = ((varToChk->aisLastCCBox) ? (cnf->vars[labs((varToChk->aisLastCCBox-1)->litNum)].implLevel) : (-1));
		if (vrInImplStTill->implLevel > existImplLvlTill)
		{
			varToChk->aisCCOfVarToSkipTill = lastCCBox;
			cnf->implLvlToSkipTill = vrInImplStTill->implLevel;
		}
		if (vrInImplStTill->implLevel < existImplLvlTill)
		{
			cnf->implLvlToSkipTill = existImplLvlTill;
			varToChk->aisCCOfVarToSkipTill = varToChk->aisLastCCBox;
			varToChk->aisLastCCBox = lastCCBox;
			varToChk->aisStatus = addImplChngSide;
		}
		if (vrInImplStTill->implLevel == existImplLvlTill)
		{
			cnf->implLvlToSkipTill = existImplLvlTill;
			varToChk->aisCCOfVarToSkipTill = lastCCBox;
		}
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "AIS : %c; Skip:%ld", GetAisLetter(cnf, varToChk->varNum, varToChk->aisStatus), ((cnf->implLvlToSkipTill!=LONG_MAX)?(cnf->ivs->bs[cnf->implLvlToSkipTill-1].litNum):(LONG_MAX)));
		}
#endif
	}
	
	if (ON(varToChk->implStat,IMPL_CONF))
	{
		varToChk->aisLastCCBox = lastCCBox;
		varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "CONF : %c; Skip:%ld", GetAisLetter(cnf, varToChk->varNum, varToChk->aisStatus), ((cnf->implLvlToSkipTill!=LONG_MAX)?(cnf->ivs->bs[cnf->implLvlToSkipTill-1].litNum):(LONG_MAX)));
		}
#endif
	}
	
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		OutputTrace(cnf, "\nUpdateAddImplStInfo:", 435, "", buff);
	}
#endif
}
