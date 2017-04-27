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

#ifdef _DEBUG
#include <windows.h>
#endif
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "C4Globals.h"
#include "C4IVSStacks.h"
#include "C4PQ.h"
#ifdef WIN32
#include <crtdbg.h>
#endif
#ifdef FBG
#include "C4FBG.h"
#endif
#ifdef FBGNEWRESTART
#include "C4FBG.h"
#endif
#ifdef WIN32
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
IVS* IVSCreate(unsigned long litsNum)
{
	IVS* ivs;
	
	ivs = calloc(1, sizeof(struct ImplVarsStacks_));
	ivs->totalCapacity = litsNum;
	ivs->allHead = ivs->decHead = ivs->d1Head = ivs->d2Head = ivs->cpHead = -1;
	ivs->bs = calloc(litsNum, sizeof(OneIVSBox));
	
	return ivs;
}

void IVSDestroy(IVS* ivs)
{
	free(ivs->bs);
	free(ivs);
}

long IVSPush(struct Cnf_* cnf, IVS* ivs, long litNum)
{
	Var* var;
	unsigned long* implStat;
	OneIVSBox* currIB;
#ifdef FBG
	CCBox *ccb, *ccb2;
	Boolean ifOneModel;
#endif
	if (litNum == MAX_PQ_LIT)
	{
#ifndef FND_BG
		litNum = PQRemMaxVar(cnf, cnf->pq, GetLitsVal);
		var = &cnf->vars[labs(litNum)];
		var->implStat = NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(litNum);
		var->implCls = 0;
		var->implLayer = cnf->lastLayerPerCpNum[ivs->cpNum]+1;
#endif
#ifdef FND_BG
		if (seqFndBg[seqFndBgInd] == 0)
		{
			litNum = PQRemMaxVar(cnf, cnf->pq, GetLitsVal);
			var = &cnf->vars[labs(litNum)];
			var->implStat = NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(litNum);
			var->implCls = 0;
		} else
		{
			while (cnf->vars[labs(seqFndBg[seqFndBgInd])].implStat != NOT_IMPLIED)
			{
				seqFndBgInd++;
				if (seqFndBg[seqFndBgInd] == 0)
				{
					litNum = PQRemMaxVar(cnf, cnf->pq, GetLitsVal);
					var = &cnf->vars[labs(litNum)];
					var->implStat = NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(litNum);
					var->implCls = 0;
					break;
				}
			}
			if (seqFndBg[seqFndBgInd] != 0)
			{
				litNum = seqFndBg[seqFndBgInd++];
				PQRemVar(cnf, cnf->pq, litNum);
				
				var = &cnf->vars[labs(litNum)];
				var->implStat = NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(litNum);
				var->implCls = 0;
			}
		}
#endif
	}
	else
	{
		var = &cnf->vars[labs(litNum)];
		PQRemVar(cnf, cnf->pq, litNum);
	}
	
	var->prevImplD2Num = ((ivs->d2Head != -1) ? (ivs->bs[ivs->d2Head].litNum) : (0));
	var->lastCCBefImplNum = cnf->currCCNum;
	implStat = &var->implStat;
	var->implLevel = ++ivs->allNum;
	TURN_OFF(*implStat,NOT_IMPLIED);
	TURN_ON(*implStat,IMPLIED);
	
	currIB = var->ivsBox = &ivs->bs[++ivs->allHead];
	currIB->litNum = litNum;
	
	if (ONE_ON_FOR_DISJ(*implStat,IMPL_D1|IMPL_D2|IMPL_ADD_IMPL_ST|IMPL_CONF))
	{
		currIB->prevCPInd = ivs->cpHead;
		ivs->cpHead = ivs->allHead;
		var->cpImplLevel = ++ivs->cpNum;
		cnf->lastLayerPerCpNum[ivs->cpNum] = var->implLayer;
		cnf->layersArr[var->implLayer] = 0;
		
		if (ON(*implStat,IMPL_D1))
		{
			currIB->prevDecInd = ivs->decHead;
			currIB->prevD1Ind = ivs->d1Head;
			ivs->decHead = ivs->d1Head = ivs->allHead;
			ivs->decNum++;
			ivs->d1Num++;
			var->lastCCBBefD1 = cnf->lastCCBOfLastCCAdded;
			var->d1ConfsList = 0;
			var->actualCCNumBefD1Impl = cnf->actualCCNum;
			if (ivs->d2Num == 0)
				ivs->d1Depth++;
		}
		
		//if the var is a decision var implied to the left and then to the right
		//adding it to appropriate list
		if (ON(*implStat,IMPL_D2))
		{
			currIB->prevDecInd = ivs->decHead;
			currIB->prevD2Ind = ivs->d2Head;
			ivs->decHead = ivs->d2Head = ivs->allHead;
			ivs->decNum++;
			ivs->d2Num++;	
			var->lastCCBBefD2 = cnf->lastCCBOfLastCCAdded;
			var->d2NumOfCurrChkConf = 0;
			var->d2ConfToBeChecked = 0;
			var->d2CbToBeImplied = 0;
			
			if ( 
				(var->thisImplD1Box == var->thisImplApp) 
				&&
				( (((double)(var->thisImplD1Box)) / ((double)(cnf->actualCCNum-var->actualCCNumBefD1Impl))) < cnf->notToChkConfsFrom)
				)
			{
#ifdef EXT2_PRINT
				if (TC(cnf, "EXT2_PRINT"))
					{
				PrintRecConfsOfVar(cnf, var);			
				}
#endif
#ifdef FBGNEWRESTART
//				if ((cnf->currCCNum >= 49228)&&(var->varNum == 2374))
//					DebugBreak();
#endif
#ifdef FBG
				/*if (seqFBG[labs(litNum)] == litNum)
				{
				for (ifOneModel = FALSE,ccb = var->d1ConfsList; ccb; ccb = GetD1ArrFromCC(ccb)->next)
				{
				for (ifOneModel = TRUE,ccb2 = ccb+1; (ccb2-1)->ordFromUp0Ind != ((*CC_LITS_NUM(ccb))-1); ccb2++)
				{
				if (-ccb2->litNum != seqFBG[labs(ccb2->litNum)])
				{
				ifOneModel = FALSE;
				break;
				}
				}
				if (ifOneModel)
				break;
				}
				if (!ifOneModel)
				DebugBreak();
			}*/
#endif
				var->lastCCBBefConfChk = cnf->lastCCBOfLastCCAdded;
				var->d2IfConfChck = TRUE;
				cnf->d2VarsConfsCheck++;
				if (var->implLevel < cnf->d2VarsConfsCheckMinImplLvl)
					cnf->d2VarsConfsCheckMinImplLvl = var->implLevel;
			} else
			{
				var->d2IfConfChck = FALSE;
				cnf->d2VarsConfsNotCheck++;
			}
			var->d1ImplApp = var->thisImplApp;
			var->thisImplApp = var->thisImplCCBox = 0;
		} else
		{
			var->d1ConfsList = 0;
			var->aisStatus = confPartNotAdd;			
			var->thisImplApp = var->thisImplCCBox = var->thisImplD1Box = 0;
		}
		
		if (ON(*implStat,IMPL_ADD_IMPL_ST))
			var->aisStatus = addImplImpl;
	} else
	{
		var->thisImplApp = var->thisImplCCBox = var->thisImplD1Box = 0;
		var->cpImplLevel = ivs->cpNum;
		
		if (var->implLayer > cnf->lastLayerPerCpNum[ivs->cpNum])
			cnf->lastLayerPerCpNum[ivs->cpNum] = var->implLayer;
		
			/*		if (CCB_DOWN_FROM_CC(var->implCls)->clsFromConfStat)
			{
			currIB->prevInCCInd = ivs->inCCHead;
			ivs->inCCHead = ivs->allHead;
			ivs->inCCNum++;
	}*/
		if (var->aisStatus != notImplOut)			
		{
			//if it's to be implied to the same direction
			if (var->aisBox->litNum == litNum)
				var->aisStatus = implCons;
			else
				var->aisStatus = implIncons;
			
			if (cnf->aisPtr == var->aisBox)
				cnf->aisPtr = cnf->aisPtr->prev;
			
			if (cnf->ais == var->aisBox)
				cnf->ais = var->aisBox->next;
			if (var->aisBox->prev)
				var->aisBox->prev->next = var->aisBox->next;
			if (var->aisBox->next)
				var->aisBox->next->prev = var->aisBox->prev;
			var->aisBox->prev = var->aisBox->next = 0;
#ifdef PRINT_AIS
			if (TC(cnf, "PRINT_AIS"))
				{
			PrintAis(cnf);
			}
#endif
		}
	}
	
	if (ivs->decNum == 0)
		cnf->initAisVarsImplLvlIncl = cnf->ivs->allNum;
	
	cnf->currNodeNum++;
	
	return litNum;
	
}

void UnimplyChangeSide(struct Cnf_* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];
	cnf->layersArr[var->implLayer] = 0;
	
	RemFromAisOnUnimply(cnf, litNum);
	DelClssFromList(cnf, litNum);
	/*
	#ifdef EXT2_PRINT
	if (TC(cnf, "ifdef "))
	{
	PrintRecConfsOfVar(cnf, var);			
	}
	#endif
	*/
	
	/*	if (cnf->heur&8192)
	{
	TreatSimilarityBefConfChk(cnf, litNum);
}*/
	
	var->implStat = NOT_IMPLIED;
	var->ivsBox = 0;
}

void UnimplyD1Var(struct Cnf_* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];
	cnf->layersArr[var->implLayer] = 0;
	
	var->thisImplApp = 0;
	var->implStat = NOT_IMPLIED;
	var->ivsBox = 0;
	var->aisStatus = notImplOut;
	
	//4 next lines aren't necessary???
	RemFromAisOnUnimply(cnf, litNum);
	DelUsedConfsFromList(cnf, litNum);
	InsIntoWListsOnBot2Unimply(cnf, litNum);
	DelClssFromList(cnf, litNum);
}

void UnimplyD2VarDelRemD1Confs(struct Cnf_* cnf, long litNum)
{
	Var* var;
	CCBox *ccb, *ccbNext;
	long implLvlOfTopDecVar;
	
	var = &cnf->vars[labs(litNum)];
	
	implLvlOfTopDecVar = cnf->vars[labs(cnf->ivs->bs[cnf->ivs->decHead].litNum)].implLevel;
	
	for (ccb = var->d1ConfsList; ccb; ccb = ccbNext)
	{
		ccbNext = GetD1ArrFromCC(ccb)->next;
		DelConfFromAddedCCList(cnf, CCB_DOWN_FROM_CC(ccb), implLvlOfTopDecVar);
	}
	
	UnimplyD2Var(cnf, litNum);
}

void UnimplyD2Var(struct Cnf_* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
		{
	sprintf(buff, "\nUnimplyD2Var");
	OutputTrace(cnf, "\nUnimplyD2", 257, "", buff);
	//GoThrowAllLists(cnf);
	}
#endif
	/*
	if (cnf->heur&256)
	ChkAndTreat1ConfEachSideForD2(cnf, litNum);	
	*/
	cnf->layersArr[var->implLayer] = 0;
	var->implStat = NOT_IMPLIED;
	var->ivsBox = 0;
	var->thisImplApp = 0;
	RemFromAisOnUnimply(cnf, litNum);
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
		{
	sprintf(buff, "\nBefore PushIntoAddImplSt");
	OutputTrace(cnf, "\nUnimplyD2", 257, "", buff);
	}
#endif
	switch (var->aisStatus) {
	case confPartAddPos:
		PushIntoAddImplSt(cnf, litNum);
		break;
	case confPartAddNeg:
		PushIntoAddImplSt(cnf, -litNum);
		break;
		//???
	case notImplIn:
		break;
	default:
		var->aisStatus = notImplOut;
	}
	
	DelUsedConfsFromList(cnf, litNum);
	InsIntoWListsOnBot2Unimply(cnf, litNum);
	DelClssFromList(cnf, litNum);
	var->d1ConfsList = 0;
}

void UnimplyAisVar(struct Cnf_* cnf, long litNum)
{
	Var* var;
#ifdef DEL_PRINT
	long i;
	
#endif
	var = &cnf->vars[labs(litNum)];
	
	cnf->layersArr[var->implLayer] = 0;
	
	var->implStat = NOT_IMPLIED;
	var->ivsBox = 0;
	var->thisImplApp = 0;
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
		{
/*	for (i = 1; i <= cnf->initVarsNum; i++)
	{
		GoThrowWathedList(cnf, -(long)i, cnf->vars[i].watchedNeg);
		GoThrowWathedList(cnf, i, cnf->vars[i].watchedPos);
	}*/
	}
#endif
	
	RemFromAisOnUnimply(cnf, litNum);
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
		{
	/*for (i = 1; i <= cnf->initVarsNum; i++)
	{
		GoThrowWathedList(cnf, -(long)i, cnf->vars[i].watchedNeg);
		GoThrowWathedList(cnf, i, cnf->vars[i].watchedPos);
	}*/
	}
#endif
	
	DelUsedConfsFromList(cnf, litNum);
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
		{
	/*for (i = 1; i <= cnf->initVarsNum; i++)
	{
		GoThrowWathedList(cnf, -(long)i, cnf->vars[i].watchedNeg);
		GoThrowWathedList(cnf, i, cnf->vars[i].watchedPos);
	}*/
	}
#endif
	
	InsIntoWListsOnBot2Unimply(cnf, litNum);
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
		{
	/*for (i = 1; i <= cnf->initVarsNum; i++)
	{
		GoThrowWathedList(cnf, -(long)i, cnf->vars[i].watchedNeg);
		GoThrowWathedList(cnf, i, cnf->vars[i].watchedPos);
	}*/
	}
#endif
	
	DelClssFromList(cnf, litNum);
	
	if (var->aisStatus == addImplChngSide)
	{
		if (var->aisCCOfVarToSkipTill)
		{
			RemCCFromLst(cnf, &(cnf->vars[labs((var->aisCCOfVarToSkipTill-1)->litNum)].remFromAIS), (var->aisCCOfVarToSkipTill-1), RfaisFromCcBox);
			/*			InsWCCBoxIntoVarsWList(cnf, var->aisCCOfVarToSkipTill);
			InsWCCBoxIntoVarsWList(cnf, var->aisCCOfVarToSkipTill-1);*/
		}
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
			{
		//				GoThrowAllLists(cnf);
			}
#endif
		if (var->aisLastCCBox)
		{
		/*			RemWCCBoxFromVarsWList(cnf, var->aisLastCCBox);
			RemWCCBoxFromVarsWList(cnf, var->aisLastCCBox-1);*/
			InsCCIntoLstBeg(cnf, &(cnf->vars[labs((var->aisLastCCBox-1)->litNum)].remFromAIS), (var->aisLastCCBox-1), RfaisFromCcBox);
		}
	}
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	//				GoThrowAllLists(cnf);
		}
#endif	
	cnf->aisPtr = var->aisBox;
	var->aisStatus = notImplIn;
}

void UnimplyConfVar(struct Cnf_* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];
	cnf->layersArr[var->implLayer] = 0;
	
	var->implStat = NOT_IMPLIED;
	var->ivsBox = 0;
	var->thisImplApp = 0;
	
	RemFromAisOnUnimply(cnf, litNum);
	
	switch (var->aisStatus) {
	case confPartAddPos:
		PushIntoAddImplSt(cnf, litNum);
		break;
	case confPartAddNeg:
		PushIntoAddImplSt(cnf, -litNum);
		break;
		//???
	case notImplIn:
		break;
	default:
		var->aisStatus = notImplOut;
	}
	
	DelUsedConfsFromList(cnf, litNum);
	InsIntoWListsOnBot2Unimply(cnf, litNum);
	DelClssFromList(cnf, litNum);
}

void UnimplyOtherVars(struct Cnf_* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];
	cnf->layersArr[var->implLayer] = 0;
	
	var->implStat = NOT_IMPLIED;
	var->ivsBox = 0;
	var->thisImplApp = 0;
	
	RemFromAisOnUnimply(cnf, litNum);
	
	switch (var->aisStatus) {
	case addImplChngSide:
		if (var->aisCCOfVarToSkipTill)
			RemCCFromLst(cnf, &(cnf->vars[labs((var->aisCCOfVarToSkipTill-1)->litNum)].remFromAIS), (var->aisCCOfVarToSkipTill-1), RfaisFromCcBox);
		if (var->aisLastCCBox)
			InsCCIntoLstBeg(cnf, &(cnf->vars[labs((var->aisLastCCBox-1)->litNum)].remFromAIS), (var->aisLastCCBox-1), RfaisFromCcBox);
		//before implication the var was in the ais, 
		//was taken out from there in implication, now adding it back
	case implCons:
	case implIncons:
		{
			var->aisStatus = notImplIn;
			var->aisBox->prev = 0;
			var->aisBox->next = cnf->ais;
			if (cnf->ais)
				cnf->ais->prev = var->aisBox;
			cnf->ais = var->aisBox;
			if (!cnf->aisPtr)
				cnf->aisPtr = cnf->ais;
#ifdef PRINT_AIS
			if (TC(cnf, "PRINT_AIS"))
				{
			PrintAis(cnf);
			}
#endif
		}
		break;
	case confPartAddPos:
		PushIntoAddImplSt(cnf, litNum);
		break;
	case confPartAddNeg:
		PushIntoAddImplSt(cnf, -litNum);
		break;
	case notImplIn:
		break;
	default:
		var->aisStatus = notImplOut;
	}
	
	DelUsedConfsFromList(cnf, litNum);
	InsIntoWListsOnBot2Unimply(cnf, litNum);
	DelClssFromList(cnf, litNum);
	
}

//returns recently deleted literal number
long IVSDelTillLastDecVar(struct Cnf_* cnf, IVS* ivs, void (*UnimplyD1Var)(struct Cnf_* cnf, long varNum), void (*UnimplyD2Var)(struct Cnf_* cnf, long varNum), void (*UnimplyAisVar)(struct Cnf_* cnf, long varNum), void (*UnimplyConfVar)(struct Cnf_* cnf, long varNum), void (*UnimplyOtherVars)(struct Cnf_* cnf, long varNum))
{
	long currInd, lastDelLit;
	Var* var;
	
	if (ivs->decHead == -1) return 0;
	
	for (currInd = ivs->allHead; currInd >= ivs->decHead; currInd--)
	{
		ivs->allNum--;
		lastDelLit = ivs->bs[currInd].litNum;
		var = &cnf->vars[labs(lastDelLit)];
		PQAddVar(cnf, cnf->pq, var->varNum, GetLitsVal, GetVarsSign);
		if (ONE_ON_FOR_DISJ(var->implStat,IMPL_D1|IMPL_D2|IMPL_ADD_IMPL_ST|IMPL_CONF))
		{
			ivs->cpNum--;
			if (ONE_ON_FOR_DISJ(var->implStat,IMPL_D1|IMPL_D2))
			{
				ivs->decNum--;
				if (ON(var->implStat,IMPL_D1))
				{
					ivs->d1Num--;
					if (ivs->d2Num == 0)
						ivs->d1Depth--;
					UnimplyD1Var(cnf, var->varNum);
				}
				if (ON(var->implStat,IMPL_D2))
				{
					ivs->d2Num--;
					UnimplyD2Var(cnf, var->varNum);
				}
			} else
			{
				if (ON(var->implStat,IMPL_ADD_IMPL_ST))
					UnimplyAisVar(cnf, var->varNum);
				else
					UnimplyConfVar(cnf, var->varNum);
			}
		} else
		{
		/*			if (CCB_DOWN_FROM_CC(var->implCls)->clsFromConfStat)
			ivs->inCCNum--;*/
			UnimplyOtherVars(cnf, var->varNum);
		}
	}
	
	ivs->allHead = ivs->allNum-1;
	ivs->cpHead = ivs->bs[ivs->decHead].prevCPInd;
	
	if (ivs->d1Head == ivs->decHead)
		ivs->d1Head = ivs->bs[ivs->decHead].prevD1Ind;
	else
		ivs->d2Head = ivs->bs[ivs->decHead].prevD2Ind;
	
	ivs->decHead = ivs->bs[ivs->decHead].prevDecInd;
	
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	//IVSPrint(cnf, cnf->ivs);
		}
#endif
	
	return lastDelLit;
}

//returns recently deleted literal number
long IVSDelTillLastConfPartVar(struct Cnf_* cnf, IVS* ivs, void (*UnimplyD1Var)(struct Cnf_* cnf, long varNum), void (*UnimplyD2Var)(struct Cnf_* cnf, long varNum), void (*UnimplyAisVar)(struct Cnf_* cnf, long varNum), void (*UnimplyConfVar)(struct Cnf_* cnf, long varNum), void (*UnimplyOtherVars)(struct Cnf_* cnf, long varNum))
{
	long currInd, lastDelLit;
	Var* var;
	
	if (ivs->cpHead == -1) return 0;
	
	if (ivs->cpHead == ivs->decHead)
		return IVSDelTillLastDecVar(cnf, ivs, UnimplyD1Var, UnimplyD2Var, UnimplyAisVar, UnimplyConfVar, UnimplyOtherVars);
	
	for (currInd = ivs->allHead; currInd >= ivs->cpHead; currInd--)
	{
		ivs->allNum--;
		lastDelLit = ivs->bs[currInd].litNum;
		var = &cnf->vars[labs(lastDelLit)];
		PQAddVar(cnf, cnf->pq, var->varNum, GetLitsVal, GetVarsSign);
		if (ONE_ON_FOR_DISJ(var->implStat,IMPL_ADD_IMPL_ST|IMPL_CONF))
		{
			ivs->cpNum--;
			if (ON(var->implStat,IMPL_ADD_IMPL_ST))
				UnimplyAisVar(cnf, var->varNum);
			else
				UnimplyConfVar(cnf, var->varNum);
		} else
		{
		/*			if (CCB_DOWN_FROM_CC(var->implCls)->clsFromConfStat)
			ivs->inCCNum--;*/
			UnimplyOtherVars(cnf, var->varNum);
		}
	}
	
	ivs->allHead = ivs->allNum-1;
	ivs->cpHead = ivs->bs[ivs->cpHead].prevCPInd;
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	//IVSPrint(cnf, cnf->ivs);
		}
#endif
	return lastDelLit;
}

void IVSPrint(struct Cnf_* cnf, IVS* ivs)
{
	long currInd, i;
	char stat, lastChar;
	Boolean cp1, d11, d21, dec1;
#ifndef SAT2003	

//	if (cnf->actualCCNum >= 35610)
	{
		buffInd = 0;
		buffInd += sprintf(buff+buffInd, "\nIVS(%ld,%ld):{", ivs->allNum, ivs->cpNum);
		
		if (cnf->heur&512)
		{
			if (!(IsEmpty(cnf->prePushSt)))
			{
				buffInd+=sprintf(buff+buffInd, "#");
				for (i = cnf->prePushSt->headInd; i != -1; i--)
					//for (i = cnf->prePushSt->headInd; i != cnf->prePushSt->tailInd; i = Modulo(i+1, cnf->prePushSt->maxNumOfElem))
				{
					lastChar = ((i != 0) ? (',') : ('}'));
					stat = GetStatLetter(cnf, cnf->prePushSt->elemArr[i], cnf->vars[labs(cnf->prePushSt->elemArr[i])].implStat);
					
					buffInd+=sprintf(buff+buffInd, "%d%c%c", cnf->prePushSt->elemArr[i], stat, lastChar);
				}
				buffInd+=sprintf(buff+buffInd, ",#");
			}
			
		}	
		
		for (cp1 = d11 = d21 = dec1 = FALSE, currInd = ivs->allHead; currInd != -1; currInd--)
		{
			stat = GetStatLetter(cnf, ivs->bs[currInd].litNum, cnf->vars[labs(ivs->bs[currInd].litNum)].implStat);
			
			if (stat != 'R')
			{
				if ((!cp1)&&((stat == 'F')||(stat == 'S')||(stat == 'A')||(stat == 'C')))
				{
					if (currInd != ivs->cpHead)
						buffInd += sprintf(buff+buffInd, "\nINCONSISTENCY IN IVS : ivs->cpHead==%ld is wrong", ivs->cpHead);
					_ASSERT(currInd == ivs->cpHead);
					cp1 = TRUE;
				}
				
				if ((!dec1)&&((stat == 'F')||(stat == 'S')))
				{
					if (currInd != ivs->decHead)
						buffInd += sprintf(buff+buffInd, "\nINCONSISTENCY IN IVS : ivs->decHead==%ld is wrong", ivs->decHead);
					_ASSERT(currInd == ivs->decHead);
					dec1 = TRUE;
				}
				
				if ((!d11)&&(stat == 'F'))
				{
					if (currInd != ivs->d1Head)
						buffInd += sprintf(buff+buffInd, "\nINCONSISTENCY IN IVS : ivs->d1Head==%ld is wrong", ivs->d1Head);
					_ASSERT(currInd == ivs->d1Head);
					d11 = TRUE;
				}
				
				if ((!d21)&&(stat == 'S'))
				{
					if (currInd != ivs->d2Head)
						buffInd += sprintf(buff+buffInd, "\nINCONSISTENCY IN IVS : ivs->d2Head==%ld is wrong", ivs->d2Head);
					_ASSERT(currInd == ivs->d2Head);
					d21 = TRUE;
				}
				
				lastChar = ((currInd) ? (',') : ('}'));
				
				//buffInd += sprintf(buff+buffInd, "%ld%c(%lu,%lu)%c", ivs->bs[currInd].litNum, stat, cnf->vars[labs(ivs->bs[currInd].litNum)].thisImplCCBox, cnf->vars[labs(ivs->bs[currInd].litNum)].thisImplApp, lastChar);
#ifndef FBGNEWRESTART
				buffInd += sprintf(buff+buffInd, "%ld%c(%ld)%c", ivs->bs[currInd].litNum, stat, cnf->vars[labs(ivs->bs[currInd].litNum)].implLayer, lastChar);
#else
				buffInd += sprintf(buff+buffInd, "%ld%c(%c,%ld)%c", ivs->bs[currInd].litNum, stat, ((ivs->bs[currInd].litNum==fbgNewRest[labs(ivs->bs[currInd].litNum)]) ? ('!') : ('~')), cnf->vars[labs(ivs->bs[currInd].litNum)].thisImplApp, lastChar);
#endif

			}
		}
		
		OutputTrace(cnf, "\nIVSPrint", 483, "", buff);
	}
	
#endif
}

void IVSPrintSorted(struct Cnf_* cnf, IVS* ivs)
{
	long i;
	OneIVSBox* ivsBox;
	
	fprintf(outFile, "\nThe model is :\n");
	for (i=1; i<=cnf->initVarsNum; i++)
	{
		ivsBox = cnf->vars[i].ivsBox;
		if (ivsBox == 0)
			fprintf(outFile, "*");
		else
			if (ivsBox->litNum > 0)
				fprintf(outFile, "+");
			else
				fprintf(outFile, "-");
			fprintf(outFile, "%ld,", i);
			if (!(i%20))
			{
				fprintf(outFile,"\n");
				fflush(outFile);
			}
	}
}
