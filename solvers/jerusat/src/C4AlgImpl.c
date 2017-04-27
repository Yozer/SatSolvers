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
long* D1EmergOrd(CCBox* ccb)
{
	unsigned long* litsNum;
	
	litsNum = (unsigned long*)(ccb - ccb->ordFromUp0Ind) - 2;		
	return ((long*)((CCArrowsBox*)(ccb + *litsNum - ccb->ordFromUp0Ind)+5));
}

long* GetDelClsLit(CCBox* ccb)
{
	unsigned long* litsNum;
	
	litsNum = (unsigned long*)(ccb - ccb->ordFromUp0Ind) - 2;		
	return ((long*)((CCArrowsBox*)(ccb + *litsNum - ccb->ordFromUp0Ind)+5)+1);
}

CCArrowsBox* GetD1ArrFromCC(CCBox* ccb)
{
	unsigned long* litsNum;
	CCBox* wDown;
	long d1Ord;
	
	litsNum = (unsigned long*)(ccb - ccb->ordFromUp0Ind) - 2;
	wDown = ccb + (*litsNum - ccb->ordFromUp0Ind - 1);
	
	if (ccb->ifNextClsBoxInd)
		d1Ord = *((long*)((CCArrowsBox*)(wDown+1)+5));
	else
		d1Ord = ccb->nextClsIndOrD1ArrOrd;
	
	return (CCArrowsBox*)((char*)(wDown+1) + 2*sizeof(unsigned long) + (sizeof(CCArrowsBox)*(5+d1Ord)) );
}

CCArrowsBox* GetDelUsedConf(CCBox* ccb)
{
	unsigned long* litsNum;
	
	litsNum = (unsigned long*)(ccb - ccb->ordFromUp0Ind) - 2;		
	return (CCArrowsBox*)(ccb + *litsNum - ccb->ordFromUp0Ind)+3;
}

CCArrowsBox* GetAddedCCList(CCBox* ccb)
{
	unsigned long* litsNum;
	
	litsNum = (unsigned long*)(ccb - ccb->ordFromUp0Ind) - 2;		
	return (CCArrowsBox*)(ccb + *litsNum - ccb->ordFromUp0Ind)+4;
}

CCArrowsBox* GetDelCls(CCBox* ccb)
{ 
	unsigned long* litsNum;
	
	litsNum = (unsigned long*)(ccb - ccb->ordFromUp0Ind) - 2;		
	return (CCArrowsBox*)(ccb + *litsNum - ccb->ordFromUp0Ind)+2;;
}


CCArrowsBox* WchFromCcBox(CCBox* ccb, CCBox* wDownCcb)
{
	unsigned long* litsNum;
	CCBox* frstCCB;
	CCArrowsBox* frstArBox;
	
	frstCCB =  ((ccb) - (ccb)->ordFromUp0Ind);
	litsNum = ((unsigned long*)frstCCB) - 2;
	//litsNum-=2;
	frstArBox = (CCArrowsBox*)(frstCCB + (*litsNum));
	
	if (ccb == wDownCcb)
		frstArBox++;
	
	return frstArBox;
	//return ( (ccb == wDownCcb) ? (((CCArrowsBox*)(FRST_ARROW_FROM_CC(ccb)))+1) : ((CCArrowsBox*)(FRST_ARROW_FROM_CC(ccb))) );
}


void InsCCIntoLstBeg(Cnf* cnf, CCBox** lstBeg, CCBox* ccb, CCArrowsBox*(*GetArrFromCC)(CCBox* ccb))
{
	CCArrowsBox *newAB, *oldAB;
	
	newAB = GetArrFromCC(ccb);
	
	if ((*lstBeg) == 0)
	{
		*lstBeg = ccb;
		newAB->prev = newAB->next = 0;
	} else
	{
		oldAB = GetArrFromCC(*lstBeg);
		newAB->prev = 0;
		newAB->next = *lstBeg;
		*lstBeg = ccb;
		oldAB->prev = ccb;
	}
}

void InsWCCBoxIntoVarsWList(Cnf* cnf, CCBox* ccbToIns, CCBox* wDownCcb)
{
	CCBox** lstBeg;
	CCArrowsBox *currArrowBox, *oldArrowBox;
	
	lstBeg = ((ccbToIns->litNum>0)?(&cnf->vars[labs(ccbToIns->litNum)].watchedPos):(&cnf->vars[labs(ccbToIns->litNum)].watchedNeg));
	currArrowBox = WchFromCcBox(ccbToIns, wDownCcb);
	
	if ((*lstBeg) == 0)
	{
		*lstBeg = wDownCcb;
		currArrowBox->prev = currArrowBox->next = 0;
	} else
	{
		oldArrowBox = WchFromCcBox(WCCB_BY_WDOWN_AND_LIT(*lstBeg,ccbToIns->litNum), *lstBeg); 
		currArrowBox->prev = 0;
		currArrowBox->next = *lstBeg;
		*lstBeg = wDownCcb;
		oldArrowBox->prev = wDownCcb;
	}
}

void RemWCCBoxFromVarsWList(Cnf* cnf, CCBox* ccbToRem, CCBox* wDownCcb)
{
	CCBox** lstBeg;
	CCArrowsBox *currArrowBox;
	
	lstBeg = ((ccbToRem->litNum>0)?(&cnf->vars[labs(ccbToRem->litNum)].watchedPos):(&cnf->vars[labs(ccbToRem->litNum)].watchedNeg));
	currArrowBox = WchFromCcBox(ccbToRem, wDownCcb);
	
	if (!(currArrowBox->prev))
	{
		*lstBeg = currArrowBox->next;
		if (*lstBeg)
			WchFromCcBox(WCCB_BY_WDOWN_AND_LIT(*lstBeg,ccbToRem->litNum), *lstBeg)->prev = 0;
		
	} else
	{
		WchFromCcBox(WCCB_BY_WDOWN_AND_LIT(currArrowBox->prev,ccbToRem->litNum), currArrowBox->prev)->next = currArrowBox->next;
		if (currArrowBox->next)
			WchFromCcBox(WCCB_BY_WDOWN_AND_LIT(currArrowBox->next,ccbToRem->litNum), currArrowBox->next)->prev = currArrowBox->prev;
	}
}

void RemCCFromLst(Cnf* cnf, CCBox** lstBeg, CCBox* ccb, CCArrowsBox*(*GetArrFromCC)(CCBox* ccb))
{
	CCArrowsBox* arrBox;
#ifdef EXT3_PRINT
	CCBox* cccb;
	CCArrowsBox* ccArr;
	if (TC(cnf, "EXT3_PRINT"))
	{
		
		buffInd+=sprintf(buff+buffInd, "\nIn RemCCFromLst");
		OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
	}
#endif
	
	arrBox = GetArrFromCC(ccb);
	
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\narrBox = GetArrFromCC(ccb);");
		OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
	}
#endif
	
	if (!(arrBox->prev))
	{
#ifdef EXT3_PRINT
		if (TC(cnf, "EXT3_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nif (!(arrBox->prev))");
			OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
		}
#endif
		*lstBeg = arrBox->next;
		if (*lstBeg)
		{
#ifdef EXT3_PRINT
			if (TC(cnf, "EXT3_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\nif (*lstBeg)");
				OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
			}
#endif
			GetArrFromCC(*lstBeg)->prev = 0;
#ifdef EXT3_PRINT
			if (TC(cnf, "EXT3_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\nGetArrFromCC(*lstBeg)->prev = 0;");
				OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
			}
#endif
			
		}
	} else
	{
#ifdef EXT3_PRINT
		if (TC(cnf, "EXT3_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nelse");
			OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
		}
#endif
		(GetArrFromCC(arrBox->prev))->next = arrBox->next;
#ifdef EXT3_PRINT
		if (TC(cnf, "EXT3_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\n(GetArrFromCC(arrBox(%#x)->prev))->next = arrBox->next;", arrBox);
			OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
		}
#endif
		if (arrBox->next)
		{
#ifdef EXT3_PRINT
			if (TC(cnf, "EXT3_PRINT"))
			{
				cccb = arrBox->next;//PrintCCGivenCCBox(cnf, cccb, FALSE, TRUE);
				buffInd+=sprintf(buff+buffInd, "\n%#x:%ld,%ld,%ld", cccb, cccb->litNum, cccb->nextClsIndOrD1ArrOrd, cccb->ordFromUp0Ind);OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
				ccArr = GetArrFromCC(arrBox->next);
				buffInd+=sprintf(buff+buffInd, "\n%#x", ccArr);OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
				cccb = ccArr->prev;
				buffInd+=sprintf(buff+buffInd, "\n%#x", cccb);OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
				cccb = arrBox->prev;
				buffInd+=sprintf(buff+buffInd, "\n%#x", cccb);OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
				//OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
			}
#endif
			(GetArrFromCC(arrBox->next))->prev = arrBox->prev;		
#ifdef EXT3_PRINT
			if (TC(cnf, "EXT3_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\n(GetArrFromCC(arrBox->next))->prev = arrBox->prev;");
				OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
			}
#endif
		}
	}
}

void InsToEndOfConfLst(Cnf* cnf, CCBox* lastWCCB)
{
	CCArrowsBox *newAB, *oldAB;
	
	newAB = GetAddedCCList(lastWCCB);
	
	if (cnf->lastCCBOfLastCCAdded == 0)
	{
		cnf->lastCCBOfFirstCCAdded = cnf->lastCCBOfLastCCAdded = lastWCCB;
		newAB->prev = newAB->next = 0;
	} else
	{
		oldAB = GetAddedCCList(cnf->lastCCBOfLastCCAdded);
		newAB->prev = cnf->lastCCBOfLastCCAdded;
		newAB->next = 0;
		cnf->lastCCBOfLastCCAdded = lastWCCB;
		oldAB->next = lastWCCB;
	}
}

void RemFromConfLst(Cnf* cnf, CCBox* lastWCCB)
{	
	if (cnf->lastCCBOfLastCCAdded == lastWCCB)
		cnf->lastCCBOfLastCCAdded = GetAddedCCList(lastWCCB)->prev;
	RemCCFromLst(cnf, &cnf->lastCCBOfFirstCCAdded, lastWCCB, GetAddedCCList);
}


int compareRandLits(void* l1, void* l2)
{
	Var *v1, *v2;
	
	v1 = &theCnf->vars[*((long*)l1)];
	v2 = &theCnf->vars[*((long*)l2)];
	
	if (v1->randNumPerVar == v2->randNumPerVar)
		return 0;	
	if (v1->randNumPerVar > v2->randNumPerVar) 
		return -1;
	else
		return 1;
}


void GetFreeLit(Cnf* cnf)
{
	long k;
	long sign;
	Var* var;
	
#ifdef FIND_BUG
	/*	static int seq[] = {-40,26,-3,1,24,5,15,7,27,-10,-34,46,-13,36,-14,-6,45,-41,22,2,-48,50,41,-20,-19,-22,45,22,35,-6,27,34,-16,38,31,46,36,-14,33,-18,2,-48,42,4,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};
	static int seqInd = 0;
	litNum = (seq[seqInd++]);
	PrepVarToImpl(cnf, litNum, 0, NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(litNum));
	return;*/
#endif
	
	if (!cnf->ifRestarting)
		Enqueue(cnf->toBeImplied, MAX_PQ_LIT);
	else
	{
		if (cnf->heur&16)
		{
			if (cnf->ifRestarting == 1)
			{
				for (k=1; k<=cnf->initVarsNum; k++)
					cnf->vars[k].randNumPerVar = rand();
				qsort(cnf->varsNumForRestart, cnf->initVarsNum, sizeof(long), compareRandLits);
				cnf->ifRestarting = 2;
				cnf->varsNumForRestartInd = 0;
				cnf->restartAft+=cnf->restartAdd;
#ifdef PRINT_IVS
				if (TC(cnf, "PRINT_IVS"))
				{
					buffInd+=sprintf(buff+buffInd, "\nRestarting");
					OutputTrace(cnf, "GetFreeLit", 346, "", buff);
				}
#endif
			}
			
			do {
				var = &cnf->vars[cnf->varsNumForRestart[cnf->varsNumForRestartInd++]];
			} while (ON(var->implStat,IMPLIED));
			
			sign = ((RandFrom0ToNMin1(2)) ? (1) : (-1));
			PrepVarToImpl(cnf, sign*(long)var->varNum, 0, NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(sign*(long)var->varNum), cnf->lastLayerPerCpNum[cnf->ivs->cpNum]+1);
		} else
		{
			if (cnf->ifRestarting == 1)
			{
				if (cnf->pq->ifGetFirst)
					cnf->ifRestarting = 3;
				else
					cnf->ifRestarting = 2;
				cnf->pq->ifGetFirst = FALSE;
#ifdef PRINT_IVS
				if (TC(cnf, "PRINT_IVS"))
				{
					buffInd+=sprintf(buff+buffInd, "\nRestarting");
					OutputTrace(cnf, "GetFreeLit", 346, "", buff);
				}
#endif
			} 
			
			Enqueue(cnf->toBeImplied, MAX_PQ_LIT);
			
		}
	}
	
	if (cnf->heur&512)
		cnf->ifPrePushD1Block = TRUE;	
}

void AddClsOnRestart(Cnf* cnf)
{
	CCBox* litsArr;
	long litsNum, litsInd;
	char* cc;
	Var* var;	
#ifdef FBGNEWRESTART
	Boolean fbgOneSat;
#endif
	litsNum = ElemNum(cnf->rccCPLits);
	
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
		litsArr[litsNum-1].ifDelCls = 0;
		litsArr[litsNum-1].ifRecClsFromConfCreated = 0;
		litsArr[litsNum-1].ifBot2 = 1;
		litsArr[litsNum-1].clsFromConfStat = 0;
		litsArr[litsNum-1].ifAdd1UIP = 0;
#ifdef FBGNEWRESTART
		fbgOneSat = FALSE;
#endif
		
		//literals' CC boxes
		for (litsInd = 0; (!(IsEmpty(cnf->rccCPLits))); litsInd++)
		{
			litsArr[litsInd].litNum = -Pop(cnf->rccCPLits);
#ifdef FBGNEWRESTART
			if (litsArr[litsInd].litNum == fbgNewRest[cnf->vars[labs(litsArr[litsInd].litNum)].varNum])
				fbgOneSat = TRUE;
#endif
			
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
#ifdef FBGNEWRESTART
		if (!fbgOneSat)
			DebugBreak();
#endif
		InsWCCBoxIntoVarsWList(cnf, &litsArr[litsNum-1], &litsArr[litsNum-1]);
		InsWCCBoxIntoVarsWList(cnf, &litsArr[litsNum-2], &litsArr[litsNum-1]);
		
		*(D1EmergOrd(litsArr)) = -1;
#ifdef RESTART_CLS_PRINT
		if (TC(cnf, "RESTART_CLS_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nRESTART_CLS_PRINT");
			PrintCCGivenCCBox(cnf, litsArr, TRUE, TRUE);
			OutputTrace(cnf, "\nRestart", 39, "", buff);
		}
#endif
	} else
	{
		cnf->vars[labs(-cnf->rccCPLits->elemArr[0])].aisLastCCBox = 0;
		PushIntoAddImplSt(cnf, -cnf->rccCPLits->elemArr[0]);
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nRestart : no CC - ais %ld added", -cnf->rccCPLits->elemArr[0]);
			OutputTrace(cnf, "\nRestart", 39, "", buff);
		}
#endif
	}
}

void RestartsClsManage(Cnf* cnf)
{
	int clssRecorded;
	long ivsInd, headInd, decHead;
	IVS* ivs;
	Var* var;
	Boolean ifAftFrstD2;
	
	ivs  = cnf->ivs;
	decHead = ivs->decHead;
	ifAftFrstD2 = FALSE;
	
	EmptyStack(cnf->rccCPLits);	
	
	for (clssRecorded = 0, ivsInd = 0; ((clssRecorded < cnf->restartsMaxClssToRec)&&(ivsInd <= decHead)); ivsInd++)
	{
		var = &cnf->vars[labs(ivs->bs[ivsInd].litNum)];
		
		if ((ON(var->implStat,IMPL_D1))&&(var->thisImplApp))
		{
			Push(cnf->rccCPLits, ivs->bs[ivsInd].litNum);
			
#ifdef FBGNEWRESTART
			//			if (-ivs->bs[ivsInd].litNum != fbgNewRest[var->varNum])
			//				DebugBreak();
#endif
			
		}
		else
		{
			if (ON(var->implStat,IMPL_D2))
			{
				ifAftFrstD2 = TRUE;
				Push(cnf->rccCPLits, -ivs->bs[ivsInd].litNum);
				
#ifdef FBGNEWRESTART
				//				if (ivs->bs[ivsInd].litNum != fbgNewRest[var->varNum])
				//					DebugBreak();
#endif
				
				headInd = cnf->rccCPLits->headInd;
				AddClsOnRestart(cnf);
				cnf->rccCPLits->headInd = headInd-1;
#ifdef PRINT_IVS
				if (TC(cnf, "PRINT_IVS"))
				{
//					GoThrowAllLists(cnf);
				}
#endif
				
				clssRecorded++;
				cnf->restartsClsAddNum++;
				
				if (var->thisImplApp)
				{
					
#ifdef FBGNEWRESTART
					//					if (-ivs->bs[ivsInd].litNum != fbgNewRest[var->varNum])
					//						DebugBreak();
#endif
					
					Push(cnf->rccCPLits, ivs->bs[ivsInd].litNum);
				}
			} else 
			{
				if ((ifAftFrstD2)&&(var->thisImplApp)&&((ON(var->implStat,IMPL_CONF))||(ON(var->implStat,IMPL_ADD_IMPL_ST))))
				{
					Push(cnf->rccCPLits, -ivs->bs[ivsInd].litNum);
				}
			}
		}
	}
	
	while (cnf->ivs->allNum)
		IVSDelTillLastConfPartVar(cnf, cnf->ivs, UnimplyD1Var, UnimplyD2Var, UnimplyAisVar, UnimplyConfVar, UnimplyOtherVars);
	
	
}

void RestartAddCls(Cnf* cnf)
{
	IVS* ivs;
	Var* var;	
	
	cnf->restartsClsAddNum++;
	ivs = cnf->ivs;
	EmptyStack(cnf->rccCPLits);	
				
	var = &cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)];
	Push(cnf->rccCPLits, -ivs->bs[ivs->d2Head].litNum);
	IVSDelTillLastConfPartVar(cnf, ivs, UnimplyD1Var, UnimplyD2Var, UnimplyAisVar, UnimplyConfVar, UnimplyOtherVars);
				
	while (ivs->allNum > cnf->initAisVarsImplLvlIncl)
	{
		var = &cnf->vars[labs(ivs->bs[ivs->cpHead].litNum)];
		if (var->thisImplApp)
			Push(cnf->rccCPLits, ivs->bs[ivs->cpHead].litNum);
		IVSDelTillLastConfPartVar(cnf, ivs, UnimplyD1Var, UnimplyD2Var, UnimplyAisVar, UnimplyConfVar, UnimplyOtherVars);
	}
	
	AddClsOnRestart(cnf);
}

void RestartNotAddCls(Cnf* cnf)
{
	cnf->restartsClsNotAddNum++;
	while (cnf->ivs->allNum)
		IVSDelTillLastConfPartVar(cnf, cnf->ivs, UnimplyD1Var, UnimplyD2Var, UnimplyAisVar, UnimplyConfVar, UnimplyOtherVars);
	
}

void Restart(Cnf* cnf)
{
	IVS* ivs;
	Boolean ifInternalRestart, ifDelayNow, ifNotClsRestForClsStrat;
	double one_percent;

#ifdef EXT2_PRINT
	if (TC(cnf, "EXT2_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nRestart : ");
	}
#endif
	
	one_percent = ((double)cnf->small_confs + (double)cnf->large_confs) / 100.0;
	if ( (double)cnf->small_confs < one_percent*5.0 )
		cnf->max1UIPLenToRec++;

	if ( (double)cnf->small_confs > one_percent*15.0 )
	{
		if (cnf->max1UIPLenToRec > 3)
			cnf->max1UIPLenToRec--;
	}
	cnf->small_confs = cnf->large_confs = 0;
	

	ivs = cnf->ivs;
	
	ifInternalRestart = ifDelayNow = ifNotClsRestForClsStrat = FALSE;
	
	if (cnf->restartDelayTimesNow == 0)
	{
		if (cnf->heur&32)
		{
			if ((ivs->d2Num == 1) && (ivs->d2Head == ivs->cpHead) && (ivs->d1Num > 0) && (!cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)].d2ConfToBeChecked))
			{
				RestartAddCls(cnf);
				ifInternalRestart = TRUE;
			} else
			{
#ifdef EXT2_PRINT
				if (TC(cnf, "EXT2_PRINT"))
				{
					buffInd+=sprintf(buff+buffInd, "\nRestart delay");
					OutputTrace(cnf, "\nRestart", 39, "", buff);
				}
#endif
				cnf->restartDelayTimesNow = 1;
				ifDelayNow = TRUE;
				cnf->restartAddWhenDelay = cnf->restartAdd;
				cnf->restartAftWhenDelay = cnf->restartAft;
			}
		} else
		{	
			RestartsClsManage(cnf);
			ifInternalRestart = TRUE;
		}
	}
	
	if (cnf->restartDelayTimesNow > 0)
	{
		if ((ivs->d2Num == 1) && (ivs->d2Head == ivs->cpHead) && (ivs->d1Num > 0) && (!cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)].d2ConfToBeChecked))
		{
			RestartAddCls(cnf);
			ifInternalRestart = TRUE;
			ifNotClsRestForClsStrat = TRUE;
		} else 
		{
			if (!ifDelayNow)
				cnf->restartDelayTimesNow++;
			
			if (cnf->restartDelayTimes == (cnf->restartDelayTimesNow - 1))
			{
				RestartsClsManage(cnf);
				ifInternalRestart = TRUE;
			}
		}
	}
	
	switch(cnf->restartsStrategy) {
	case NoClsLinear :
	case ClsLinear :
		if (!ifNotClsRestForClsStrat)
		{
			cnf->restartAft = cnf->actualCCNum + cnf->restartAftInit + cnf->restartAdd;
			cnf->restartAdd+=cnf->restartAddInit;
		} else
		{
			cnf->restartAft = cnf->actualCCNum + cnf->restartAftInit + cnf->restartAddWhenDelay;
		}
		break;
	case NoClsExp :
		cnf->restartAft*=2;
		break;
	case ClsExp1:
		if (!ifNotClsRestForClsStrat)
			cnf->restartAft+=cnf->actualCCNum;
		else
			cnf->restartAft = 2*cnf->actualCCNum;
		break;
	case ClsExp2 :
		cnf->restartAft = 2*cnf->actualCCNum;
		break;
	case ClsLikeVer1 :
		if (!ifNotClsRestForClsStrat)
			cnf->restartAft+=(cnf->actualCCNum + cnf->restartAft + cnf->restartAdd);
		else
			cnf->restartAft = cnf->restartAftWhenDelay + cnf->actualCCNum + cnf->restartAftWhenDelay + cnf->restartAddWhenDelay;
		break;
	case NoClsLikeVer1 :
		cnf->restartAft+=(cnf->restartAft+cnf->restartAdd);
		break;
	}
	
	
	
	if (ifInternalRestart)
	{
		cnf->restartsNum++;
		cnf->ifRestarting = TRUE;
		cnf->currCCNum++;
		cnf->restartDelayTimesNow = 0;
	}
}

void GetLitToJump(Cnf* cnf)
{
	Var *headD2Var, *addedVar;
	IVS* ivs;
	
	ivs = cnf->ivs;
	
	if ((cnf->actualCCNum > cnf->restartAft)||((cnf->restartDelayTimesNow > 0)&&((ivs->d2Num == 1) && (ivs->d2Head == ivs->cpHead) && (ivs->d1Num > 0) && (!cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)].d2ConfToBeChecked))))
	{
		Restart(cnf);
	} 
	
	if (cnf->aisPtr)
	{
		addedVar = &cnf->vars[labs(cnf->aisPtr->litNum)];
		PrepVarToImpl(cnf, cnf->aisPtr->litNum, addedVar->aisLastCCBox, NOT_IMPLIED | IMPL_ADD_IMPL_ST | IMPL_SIGN(cnf->aisPtr->litNum), cnf->lastLayerPerCpNum[cnf->ivs->cpNum]+1);
		cnf->aisPtr = cnf->aisPtr->prev;
		return;
	}
	
	if ( (ivs->d2Num == 0) || (ivs->decHead == ivs->d1Head) )
	{
		GetFreeLit(cnf);
		return;
	}
	
	headD2Var = &cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)];
	
	if (headD2Var->d2IfConfChck)
	{
		if (!headD2Var->d2ConfToBeChecked)
			PrepFirstConfForChk(cnf, headD2Var->varNum);
		if (headD2Var->d2CbToBeImplied != FRST_ARROW_FROM_CC(headD2Var->d2ConfToBeChecked))
		{			
			if (cnf->ifNextConfAlsoChked)
			{
				if (!cnf->nextChCbEqTill)
					cnf->ifNextConfAlsoChked = FALSE;
				else
				{
					if (cnf->nextChCbEqTill->litNum == headD2Var->d2CbToBeImplied->litNum)
						cnf->nextChCbEqTill++;
					//cnf->nextChCbEqTill = cnf->nextChCbEqTill+1;
					else
						cnf->ifNextConfAlsoChked = FALSE;
				}
			}
			
			PrepVarToImpl(cnf, -headD2Var->d2CbToBeImplied->litNum, 0, NOT_IMPLIED | IMPL_CONF | IMPL_SIGN(-headD2Var->d2CbToBeImplied->litNum), cnf->lastLayerPerCpNum[cnf->ivs->cpNum]+1);					
			headD2Var->d2CbToBeImplied++;
			return;
		}
	}
	
	GetFreeLit(cnf);	
}

ConfCircum DetermineConfCirc(Cnf* cnf, CCBox* clsWithContr)
{
	ConfCircum cc;
	Var *headConfPartVar, *headDec2Var;
	IVS* ivs;
	
	ivs = cnf->ivs;
	headConfPartVar = &cnf->vars[labs(ivs->bs[ivs->cpHead].litNum)];
	headDec2Var = ( (ivs->d2Head != -1) ? (&cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)]) : (0) );
	
	if (clsWithContr == &cnf->dummyConfBox)
	{
		if (ON(cnf->vars[labs(cnf->varNumWithContr)].implStat,IMPL_REG_RES))
		{
			cc = confDummyRegRes;
		}
		else
			cc = confDummyAddImpl;
	}
	else
	{
		if (clsWithContr == &cnf->dummyAISBox)
		{
			if (headDec2Var->d2ConfToBeChecked)
				cc = aiDummyAftConf;
			else
				cc = aiDummyAftD2;
		}
		else
		{		
			if (ON(headConfPartVar->implStat,IMPL_D1))
				cc = d1;				
			if (ON(headConfPartVar->implStat,IMPL_D2))
				cc = d2NoOrBefConfChk;
			if (ON(headConfPartVar->implStat,IMPL_ADD_IMPL_ST))
			{
				if (headDec2Var->d2ConfToBeChecked)
					cc = addAftConf;
				else
					cc = addAftD2;
			}
			if (ON(headConfPartVar->implStat,IMPL_CONF))
			{
				cc = confReg;
			}
		}
	}
	
#ifdef PRINT_IVS	
	if (TC(cnf, "PRINT_IVS"))
	{
		buffInd = sprintf(buff, "\nConf. circ. is : ");
		switch (cc) {
		case d1:
			buffInd += sprintf(buff+buffInd,"d1");
			break;
		case d2NoOrBefConfChk:
			buffInd += sprintf(buff+buffInd,"d2NoOrBefConfChk");
			break;
		case addAftD2:
			buffInd += sprintf(buff+buffInd,"addAftD2");
			break;
		case addAftConf:
			buffInd += sprintf(buff+buffInd,"addAftConf");
			break;
		case confReg:
			buffInd += sprintf(buff+buffInd,"confReg");
			break;
		case confDummyRegRes:
			buffInd += sprintf(buff+buffInd,"confDummyRegRes");
			break;
		case confDummyAddImpl:
			buffInd += sprintf(buff+buffInd,"confDummyAddImpl");
			break;
		case aiDummyAftConf:
			buffInd += sprintf(buff+buffInd,"aiDummyAftConf");
			break;
		case aiDummyAftD2:
			buffInd += sprintf(buff+buffInd,"aiDummyAftD2");
			break;
		}
		OutputTrace(cnf, "\nDetermineConfCirc", 307, "", buff);
	}
#endif
	return cc;
}

Sat AlgMain(Cnf* cnf, double initTime, long cutOffTime)
{
	CCBox* clsWithContradiction;
	IVS* ivs;
	Var *vars, *headConfPartVar, *headDecVar, *headDec2Var;
	long litNum;
	ConfCircum cc;
	Boolean confVarsBefDecVar;
	//	double prevTime, currTime, runTime;
#ifdef PQ_PRINT
	long prPr;
	if (TC(cnf, "PQ_PRINT"))
	{
		
		prPr = 0;
	}
#endif
#ifdef FBG1UIP
	long i;
	Boolean ifModel;
#endif
#ifdef FBGNEWRESTART
	long i;
#endif
	
	/*
	prevTime = initTime;
	runTime = 0;
	*/
	if (ifUnsAftBuildCnf)
		return unsat;
	
	cnf->small_confs = cnf->large_confs = 0;

	while (TRUE)
	{	
		if (cnf->currCCNum > ULONG_MAX-2)
			return unknown;
#ifndef _DEBUG
		if (((double)(clock() - initTime) / CLOCKS_PER_SEC) > cutOffTime)
			return cutoff;
#endif
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			//PQPrint(cnf, cnf->pq, GetLitsVal);
		}
#endif
		
		clsWithContradiction = Imply(cnf);
		
#ifdef PRINT_MIDDLE_IVS
		if (TC(cnf, "PRINT_MIDDLE_IVS"))
		{
			IVSPrint(cnf, cnf->ivs);
		}
#endif
		#ifdef PQ_PRINT/*
		if (TC(cnf, "PQ_PRINT"))
		{
		if (div(prPr,1).rem == 0)
		PQPrint(cnf, cnf->pq, GetLitsVal);
		prPr++;*/
	}
#endif
#ifdef FBGNEWRESTART
//	if (cnf->currCCNum >= 49230)
//		PrintRecConfsOfVar(cnf, &cnf->vars[3241]);
	//		if (cnf->actualCCNum >= 35618)
	//			IVSPrint(cnf, cnf->ivs);
/*	if (cnf->currNodeNum >= 4805448)
	{
		for (i = cnf->ivs->allHead; i>=0; i--)
		{
			if (fbgNewRest[labs(cnf->ivs->bs[i].litNum)] != cnf->ivs->bs[i].litNum)
			{
				goto AftFbg1UIP2;
			}
		}
		goto AftFbg1UIP3;
AftFbg1UIP2:
		DebugBreak();
		
	}
	
AftFbg1UIP3:*/
#endif
	if (((cnf->ivs->allNum+cnf->ifPrePushD1Block*ElemNum(cnf->prePushSt)) == cnf->initVarsNum) && (!clsWithContradiction))
		return sat;
	
	if (clsWithContradiction != 0)
	{
#ifdef FBGNEWRESTART
//		if (cnf->currCCNum >= 49230)
//			PrintRecConfsOfVar(cnf, &cnf->vars[3241]);
#endif

		if (TC(cnf, "PRINT_MIDDLE_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "\n");
			OutputTrace(cnf, "\nAlgMain", 668, "", buff);
		}
#
	/*
	#ifndef _DEBUG
	currTime = clock();
	if (currTime > prevTime)
	{
				runTime+= ((double)(currTime-prevTime) / CLOCKS_PER_SEC)
				} else
				{
				
				  }
				  #endif
		*/
		
		if (cnf->ifRestarting == 3)
			//there was a bug here, ifGetFirst should have been set to TRUE
			//it seems though that not setting ifGetFirst improves the 
			//performance, so we leave the possibility of setting
			//ifGetFirst to FALSE after the first restart (if cnf->ifGetFirstBackAftRestart==FALSE)
			cnf->pq->ifGetFirst = cnf->ifGetFirstBackAftRestart;
		//#endif
		cnf->ifRestarting = 0;
		
		ivs = cnf->ivs;
		vars = cnf->vars;
		
		if (cnf->ifPrePushD1Block)
		{
			clsWithContradiction = PushD1Block(cnf, clsWithContradiction);
			if (!clsWithContradiction)
				return sat;
			cnf->ifPrePushD1Block = FALSE;
#ifdef PRINT_IVS
			if (TC(cnf, "PRINT_IVS"))
			{
				IVSPrint(cnf, cnf->ivs);
			}
#endif
		}
#ifdef FBGNEWRESTART
		for (i = cnf->ivs->bs[cnf->ivs->cpHead].prevCPInd; i>=0; i = cnf->ivs->bs[i].prevCPInd)
		{
			if (fbgNewRest[labs(cnf->ivs->bs[i].litNum)] != cnf->ivs->bs[i].litNum)
			{
				goto AftFbg1UIP;
			}
		}
		buffInd+=sprintf(buff+buffInd, "\nIVS equal to model, but the last D2 variable");
		OutputTrace(cnf, "\nAlgMain", 1008, "", buff);

		//DebugBreak();
AftFbg1UIP:
#endif
		
		if (ivs->decNum == 0)
			return unsat;
		
		headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
		headDecVar = &cnf->vars[labs(ivs->bs[ivs->d1Head].litNum)];
		headDec2Var = ((ivs->d2Num) ? (&cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)]) : (0));
#ifdef PRINT_IVS
		if (TC(cnf, "PRINT_IVS"))
		{
			IVSPrint(cnf, cnf->ivs);
			//		PQPrint(cnf, cnf->pq, GetLitsVal);
		}
#endif
		
		cc = DetermineConfCirc(cnf, clsWithContradiction);
		AnalyzeNewConflict(cnf, clsWithContradiction, cc);
		//typedef enum{d1,d2NoOrBefConfChk,addAftD2,addAftConf,confReg,confDummyRegRes,aiDummyAftD2,aiDummyAftConf,confDummyAddImpl} ConfCircum;
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
		{
			sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
			OutputTrace(cnf, "\nAlgMain", 357, "", buff);
		}
#endif
		switch (cc) 
		{
		case d1:
			litNum = IVSDelTillLastDecVar(cnf, cnf->ivs, UnimplyChangeSide, 0,0,0, UnimplyOtherVars);
			PrepVarToImpl(cnf, -litNum, 0, NOT_IMPLIED | IMPL_D2 | IMPL_SIGN(-litNum), cnf->lastLayerPerCpNum[cnf->ivs->cpNum]+1);
			goto ImplyNext;
			break;
		case d2NoOrBefConfChk:
			IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
			goto FurtherExamineStack;
			break;
		case addAftD2:
			//added in CCC version
			if ( (headDec2Var) && (headDec2Var->implLevel > cnf->implLvlToSkipTill) )
				headDec2Var->thisImplApp = 0;
			
			if (headDec2Var->thisImplApp == 0)
				DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, cnf->lastCCBOfLastCCAdded);
			while (OFF(headConfPartVar->implStat,IMPL_D2))
			{
				//added in CCC version
				if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
					headConfPartVar->thisImplApp = 0;
				RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
				IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
				headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
			}
			IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
			goto FurtherExamineStack;
		case aiDummyAftD2:
			if (headDec2Var->thisImplApp == 0)
				DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, cnf->lastCCBOfLastCCAdded);
			RecreateConfOfAISVarIfNeeded(cnf, cnf->varNumWithContr);
			while (OFF(headConfPartVar->implStat,IMPL_D2))
			{
				RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
				IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
				headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
			}
			IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
			goto FurtherExamineStack;
		case addAftConf:
			//added in CCC version
			if ( (headDec2Var) && (headDec2Var->implLevel > cnf->implLvlToSkipTill) )
				headDec2Var->thisImplApp = 0;				
			if (!(cnf->heur&128))
			{
				if (headDec2Var->thisImplApp <= headDec2Var->d2ImplStatBefCheckConf)
					DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, cnf->lastCCBOfLastCCAdded);
				else
					DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD2, cnf->lastCCBOfLastCCAdded);
			} else
			{
				if (headDec2Var->thisImplApp <= headDec2Var->d2ImplStatBefCheckConf)
					DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, ( (headDec2Var->lastCCBBefD2) ? (GetAddedCCList(headDec2Var->lastCCBBefD2)->next) : (cnf->lastCCBOfFirstCCAdded) ) );
			}
			while (OFF(headConfPartVar->implStat,IMPL_D2))
			{
				//added in CCC version
				if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
					headConfPartVar->thisImplApp = 0;
				RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
				IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
				headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
			}
			IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
			goto FurtherExamineStack;
		case aiDummyAftConf:
			if (!(cnf->heur&128))
			{
				if (headDec2Var->thisImplApp <= headDec2Var->d2ImplStatBefCheckConf)
					DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, cnf->lastCCBOfLastCCAdded);
				else
					DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD2, cnf->lastCCBOfLastCCAdded);
			} else
			{
				if (headDec2Var->thisImplApp <= headDec2Var->d2ImplStatBefCheckConf)
					DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, ( (headDec2Var->lastCCBBefD2) ? (GetAddedCCList(headDec2Var->lastCCBBefD2)->next) : (cnf->lastCCBOfFirstCCAdded) ) );
			}
			RecreateConfOfAISVarIfNeeded(cnf, cnf->varNumWithContr);
			while (OFF(headConfPartVar->implStat,IMPL_D2))
			{
				RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
				IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
				headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
			}
			IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
			goto FurtherExamineStack;
		case confReg:
		case confDummyRegRes:
		case confDummyAddImpl:
			if (PassToNextConfBefFuthExSt(cnf))
			{
				while (ALL_OFF_FOR_DISJ(headConfPartVar->implStat,IMPL_D2|IMPL_ADD_IMPL_ST))
				{
					IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, 0, UnimplyConfVar, UnimplyOtherVars);
					headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
				}
				
				//moving the next line here from PassToNextConfBefFuthExSt
				//in order not to put into lastCCBBefConfChk a conflict, s.t. ch->firstBox..->implLevel > d2var->implLevel
				//in this case the conflict already deleted to this point
				headDec2Var->lastCCBBefConfChk = cnf->lastCCBOfLastCCAdded;
				goto ImplyNext;
			} else
			{
				while (ALL_OFF_FOR_DISJ(headConfPartVar->implStat,IMPL_D2|IMPL_ADD_IMPL_ST))
				{
					IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, 0, UnimplyConfVar, UnimplyOtherVars);
					headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
				}
				while (OFF(headConfPartVar->implStat,IMPL_D2))
				{
					RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
					IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
					headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
				}				
				IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2VarDelRemD1Confs, 0, 0, UnimplyOtherVars);
				goto FurtherExamineStack;
			}
			}
FurtherExamineStack:
			while (cnf->ivs->decNum)
			{
				confVarsBefDecVar = FALSE;				
				EmptyStack(cnf->confVarsPart);
				headConfPartVar = ((ivs->cpNum) ? (&cnf->vars[labs(ivs->bs[ivs->cpHead].litNum)]) : (0));
				
				headDec2Var = ((ivs->d2Num) ? (&cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)]) : (0));
				
				if (ON(headConfPartVar->implStat,IMPL_D1))
				{
					//added in CCC version
					if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
						headConfPartVar->thisImplApp = 0;
#ifdef EXT2_PRINT
					if (TC(cnf, "EXT2_PRINT"))
					{
						sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
						OutputTrace(cnf, "\nAlgMain", 357, "", buff);
					}
#endif
					
					if (headConfPartVar->thisImplApp != 0) 
					{
						litNum = IVSDelTillLastDecVar(cnf, cnf->ivs, UnimplyChangeSide, 0,0,0, UnimplyOtherVars);						
						PrepVarToImpl(cnf, -litNum, 0, NOT_IMPLIED | IMPL_D2 | IMPL_SIGN(-litNum), cnf->lastLayerPerCpNum[cnf->ivs->cpNum]+1);
						goto ImplyNext;
					} else
					{
						
						IVSDelTillLastConfPartVar(cnf, cnf->ivs, UnimplyD1Var, 0, 0, 0, UnimplyOtherVars);
						goto GoOnWithExamineStack;
					}
				}
				
				while (ON(headConfPartVar->implStat,IMPL_CONF))
				{
					//added in CCC version
					if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
						headConfPartVar->thisImplApp = 0;
#ifdef EXT2_PRINT
					if (TC(cnf, "EXT2_PRINT"))
					{
						sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
						OutputTrace(cnf, "\nAlgMain", 357, "", buff);
					}
#endif
					
					if (headConfPartVar->thisImplApp)
						Push(cnf->confVarsPart, IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, 0, UnimplyConfVar, UnimplyOtherVars));
					else
						IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, 0, UnimplyConfVar, UnimplyOtherVars);
					headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
					//added in CCC version
					if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
						headConfPartVar->thisImplApp = 0;
					confVarsBefDecVar = TRUE;
				}
				
				if (ON(headDec2Var->implStat,IMPL_D2))
				{
					//added in CCC version
					if ( (headDec2Var) && (headDec2Var->implLevel > cnf->implLvlToSkipTill) )
						headDec2Var->thisImplApp = 0;
#ifdef EXT2_PRINT
					if (TC(cnf, "EXT2_PRINT"))
					{
						sprintf(buff, "\nPROCESSING D2 VAR : %ld(%lu apps.,%lu impl. lvl.)%c", ivs->bs[ivs->d2Head].litNum, headDec2Var->thisImplApp, headDec2Var->implLevel, GetAisLetter(cnf, ivs->bs[ivs->d2Head].litNum, headDec2Var->aisStatus));
						OutputTrace(cnf, "\nAlgMain", 357, "", buff);
					}
#endif
					
					if (headDec2Var->d2IfConfChck)
					{
						if (IsEmpty(cnf->confVarsPart))
						{
							if (headDec2Var->thisImplApp > headDec2Var->d2ImplStatBefCheckConf)
							{
								if (!(cnf->heur&128))
									DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD2, ( (headDec2Var->lastCCBBefConfChk) ? (GetAddedCCList(headDec2Var->lastCCBBefConfChk)->next) : (cnf->lastCCBOfFirstCCAdded) ) );
							}
							else
							{
								if (!(cnf->heur&128))
									DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, ( (headDec2Var->lastCCBBefConfChk) ? (GetAddedCCList(headDec2Var->lastCCBBefConfChk)->next) : (cnf->lastCCBOfFirstCCAdded) ) );
								else
									DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, ( (headDec2Var->lastCCBBefD2) ? (GetAddedCCList(headDec2Var->lastCCBBefD2)->next) : (cnf->lastCCBOfFirstCCAdded) ) );
								
							}
							
							//deleting added vars from stack
							while (OFF(headConfPartVar->implStat,IMPL_D2))
							{
								//added in CCC version
								if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
									headConfPartVar->thisImplApp = 0;
#ifdef EXT2_PRINT
								if (TC(cnf, "EXT2_PRINT"))
								{
									sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
									OutputTrace(cnf, "\nAlgMain", 357, "", buff);
								}
#endif
								RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
								IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
								headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
								//added in CCC version
								if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
									headConfPartVar->thisImplApp = 0;
							}
							
							IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
							goto GoOnWithExamineStack;								
						} else
						{							
							//There is at least 1 conf. variable participating in one of the conflicts
							if (cnf->heur&128)
							{
								RecClsFromConfIVSVars(cnf);
							}
							
							if (PassToNextConfAftFuthExSt(cnf))
								goto ImplyNext;
							else
							{
								//processing added vars
								while (OFF(headConfPartVar->implStat,IMPL_D2))
								{
									//added in CCC version
									if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
										headConfPartVar->thisImplApp = 0;
#ifdef EXT2_PRINT
									if (TC(cnf, "EXT2_PRINT"))
									{
										sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
										OutputTrace(cnf, "\nAlgMain", 357, "", buff);
									}
#endif
									
									RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
									IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
									headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
									//added in CCC version
									if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
										headConfPartVar->thisImplApp = 0;
								}
								
								IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2VarDelRemD1Confs, 0, 0, UnimplyOtherVars);
								goto GoOnWithExamineStack;								
							}
						}
					} else
					{
						if (headDec2Var->thisImplApp != 0)
						{
							//processing added vars
							while (OFF(headConfPartVar->implStat,IMPL_D2))
							{
								//added in CCC version
								if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
									headConfPartVar->thisImplApp = 0;
#ifdef EXT2_PRINT
								if (TC(cnf, "EXT2_PRINT"))
								{
									sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
									OutputTrace(cnf, "\nAlgMain", 357, "", buff);
								}
#endif
								RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
								IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
#ifdef PRINT_IVS
								if (TC(cnf, "PRINT_IVS"))
								{
									//									GoThrowAllLists(cnf);
								}
#endif
								headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
								//added in CCC version
								if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
									headConfPartVar->thisImplApp = 0;
							}
#ifdef PRINT_IVS
							if (TC(cnf, "PRINT_IVS"))
							{
								//								GoThrowAllLists(cnf);
							}
#endif
							
							IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
							goto GoOnWithExamineStack;								
						} else
						{
#ifdef EXT2_PRINT
							if (TC(cnf, "EXT2_PRINT"))
							{
								sprintf(buff, "\nBefore DelCCsNotIncAISLeave");
								OutputTrace(cnf, "\nAlgMain", 686, "", buff);
							}
#endif
							DelCCsNotIncAISLeave(cnf, headDec2Var->lastCCBBefD1, ( (headDec2Var->lastCCBBefD2) ? (GetAddedCCList(headDec2Var->lastCCBBefD2)->next) : (cnf->lastCCBOfFirstCCAdded) ));
#ifdef EXT2_PRINT
							if (TC(cnf, "EXT2_PRINT"))
							{
								sprintf(buff, "\nAfter DelCCsNotIncAISLeave");
								OutputTrace(cnf, "\nAlgMain", 686, "", buff);
							}
#endif
							//processing added vars
							while (OFF(headConfPartVar->implStat,IMPL_D2))
							{
								//added in CCC version
								if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
									headConfPartVar->thisImplApp = 0;
#ifdef EXT2_PRINT
								if (TC(cnf, "EXT2_PRINT"))
								{
									sprintf(buff, "\nCP TOP VAR : %ld(%lu apps.,%lu impl. lvl.)%c%c", ivs->bs[ivs->cpHead].litNum, headConfPartVar->thisImplApp, headConfPartVar->implLevel, GetStatLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->implStat), GetAisLetter(cnf, ivs->bs[ivs->cpHead].litNum, headConfPartVar->aisStatus));
									OutputTrace(cnf, "\nAlgMain", 357, "", buff);
								}
#endif
								
								RecreateConfOfAISVarIfNeeded(cnf, headConfPartVar->varNum);
								IVSDelTillLastConfPartVar(cnf, cnf->ivs, 0, 0, UnimplyAisVar, 0, UnimplyOtherVars);
								headConfPartVar = &vars[labs(ivs->bs[ivs->cpHead].litNum)];
								//added in CCC version
								if ( (headConfPartVar) && (headConfPartVar->implLevel > cnf->implLvlToSkipTill) )
									headConfPartVar->thisImplApp = 0;
							}
							
							IVSDelTillLastDecVar(cnf, cnf->ivs, 0, UnimplyD2Var, 0, 0, UnimplyOtherVars);
							goto GoOnWithExamineStack;								
						}
					}
				}
GoOnWithExamineStack:
#ifdef PRINT_IVS
				if (TC(cnf, "PRINT_IVS"))
				{
					//				GoThrowAllLists(cnf);
					//PQPrint(cnf, cnf->pq, GetLitsVal);
				}
#endif
				continue;
			}
			
			//we reach here only if the stack is empty and toBeImplied is empty as well
			return unsat;
			
		}  else
			//if there is no contradiction
		{			
			GetLitToJump(cnf);
		}	
ImplyNext: 
#ifdef PRINT_MIDDLE_IVS
		if (TC(cnf, "PRINT_MIDDLE_IVS"))
		{
			buffInd+=sprintf(buff+buffInd, "\n------------------------------------------");
			OutputTrace(cnf, "\nAlgMain", 668, "", buff);
		}
#endif
		continue;
	}
}
