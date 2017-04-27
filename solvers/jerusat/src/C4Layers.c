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
#ifdef WIN32
#include <crtdbg.h>
#include <windows.h>
#endif
#include "C4Globals.h"
#include "C4FBG.h"
#ifdef WIN32
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
void InsertToLLL(Cnf* cnf, LLBox** lst, LLBox* llb)
{
	LLBox* auxLLB;
	
	if (*lst)
	{
		auxLLB = *lst;
		*lst = llb;
		llb->next = auxLLB;
		llb->prev = 0;
		if (auxLLB)
			auxLLB->prev = llb;
	} else
	{
		llb->prev = llb->next = 0;
		*lst = llb;
	}
	
}

void InsertSecToLLL(Cnf* cnf, LLBox** lst, LLBox* llb)
{
	LLBox* auxLLB;
	
	auxLLB = (*lst)->next;
	(*lst)->next = llb;
	llb->prev = (*lst);
	llb->next = auxLLB;
	if (auxLLB)
		auxLLB->prev = llb;
}

//returns number of variables distributed
long DistribClsToLayers(Cnf* cnf, CCBox* cls, long* thisCPLvlVars)
{
	CCBox* currBox;
	LLBox* auxLLB;
	Var* var;
	long varsNum;

	for (varsNum = 0, currBox = CLS_CCBOXES_BEG(cls); (currBox != FRST_ARROW_FROM_CC(cls)); currBox++)
	{
		var = &cnf->vars[labs(currBox->litNum)];
		if (labs(var->lastLayerCountVal) != cnf->layerPartCount+1)
		{
			varsNum++;
			if ((cnf->ifAdd1UipClause == 1) || (var->implLayer >= cnf->vars[labs(cnf->ivs->bs[cnf->ivs->cpHead].litNum)].implLayer))
			{
				InsertToLLL(cnf, &cnf->layersArr[var->implLayer], &var->layerBox);
			} else
			{
				if ((!cnf->layerBoxesBefLastCPLvl)||(var->cpImplLevel >= cnf->vars[labs(cnf->layerBoxesBefLastCPLvl->litNum)].cpImplLevel))
					InsertToLLL(cnf, &cnf->layerBoxesBefLastCPLvl, &var->layerBox);
				else
					InsertSecToLLL(cnf, &cnf->layerBoxesBefLastCPLvl, &var->layerBox);

			}
			var->lastLayerCountVal = cnf->layerPartCount+1;
			cnf->lastLayerCoPerCpLvl[var->cpImplLevel] = var->lastLayerCountVal;
			if (var->cpImplLevel == cnf->ivs->cpNum)
				(*thisCPLvlVars)++;
		}
	}

	return varsNum;
}

CCBox* Add1UIPClause(Cnf* cnf, CCBox* clsWithContradiction, long* litNum)
{
	long lastLayer, currLayer, varsNumAll, varsNumBefLayer, litsNum, litsInd, ivsInd, prevImplInd, implInd, thisCPLvlVars;
	LLBox* currBox;
	Var *var, *var2, *varToDelCls;
	char* cc;
	CCBox* litsArr;
	IVS* ivs;
	OneIVSBox *ib, *ib2;
#ifdef FBGNEWRESTART
	Boolean fbgOneSat;
#endif
	int value_to_incr_heur;

#ifdef EXT2_PRINT
	if (TC(cnf, "EXT2_PRINT"))
		{
	buffInd+=sprintf(buff+buffInd, "\nAdd1UIPClause : ");
	}
#endif

	lastLayer = cnf->lastLayerPerCpNum[cnf->ivs->cpNum];
	varsNumAll = varsNumBefLayer = thisCPLvlVars = 0;
	cnf->layerBoxesBefLastCPLvl = 0;
	//distribute the clause with contradiction to layers
	varsNumAll+=DistribClsToLayers(cnf, clsWithContradiction, &thisCPLvlVars);
#ifdef PRINT_LAYERS
	if (TC(cnf, "PRINT_LAYERS"))
		{
	PrintLayers(cnf);
	}
#endif
	//finding the 1UIP   
	for (currLayer = lastLayer; ((currLayer == lastLayer) || (!cnf->layersArr[currLayer]) || (cnf->layersArr[currLayer]->next) || (thisCPLvlVars != 1)); currLayer--)
	{
#ifdef WIN32
		_ASSERT(currLayer > cnf->lastLayerPerCpNum[cnf->ivs->cpNum-1]);
#endif
		for (currBox = cnf->layersArr[currLayer]; currBox; currBox = cnf->layersArr[currLayer])
		{
			cnf->layersArr[currLayer] = currBox->next;
			if (cnf->layersArr[currLayer])
				cnf->layersArr[currLayer]->prev = 0;
			currBox->prev = currBox->next = 0;

			var = &cnf->vars[labs(currBox->litNum)];
			var->lastLayerCountVal*=-1;
			varsNumBefLayer++;
			if (var->implCls)
			{
				varsNumAll+=DistribClsToLayers(cnf, var->implCls, &thisCPLvlVars);
#ifdef PRINT_LAYERS
				if (TC(cnf, "PRINT_LAYERS"))
					{
				PrintLayers(cnf);
				}
#endif
			}
			thisCPLvlVars--;
			if (cnf->incrHeur1UIP == 1)
				IncrHeur(cnf, (var->implStat&IMPL_POS)?(var->varNum):(-(long)(var->varNum)), 1);
		}
	}

/*
#ifdef PRINT_LAYERS
if (TC(cnf, "PRINT_LAYERS"))
{
				PrintLayers(cnf);
				}
#endif
*/

	cnf->layerPartCount++;
	
	//building the new clause
	litsNum = varsNumAll - varsNumBefLayer;

	switch(litsNum) {
	case 1:
		value_to_incr_heur = 8;
		break;
	case 2:
		value_to_incr_heur = 4;
		break;
	case 3:
		value_to_incr_heur = 2;
	default:
		value_to_incr_heur = 1;
	}
	
	if (litsNum < cnf->max1UIPLenToRec) 
		cnf->small_confs++;
	else
		cnf->large_confs++;

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
		//sorted from
		(*((unsigned long*)(&cc[sizeof(unsigned long)]))) = cnf->currCCNum;

		//setting watch CC boxes info
		
		litsArr[litsNum-1].ccType = CC_TYPE_CLS_ONLY;
		litsArr[litsNum-1].ifSorted = (cnf->ifAdd1UipClause == 1);
		litsArr[litsNum-1].ifConfSimTreated = FALSE;
		litsArr[litsNum-1].ifDelCls = (litsNum > cnf->max1UIPLenToRec);
		litsArr[litsNum-1].ifRecClsFromConfCreated = 0;
		litsArr[litsNum-1].ifBot2 = 1;
		litsArr[litsNum-1].clsFromConfStat = 0;
		litsArr[litsNum-1].ifAdd1UIP = 1;
		
		ivs = cnf->ivs;
#ifdef FBGNEWRESTART
		fbgOneSat = FALSE;
#endif
		if (cnf->ifAdd1UipClause == 1)
		{
			for (prevImplInd = ivs->allHead, litsInd = litsNum-1, ivsInd = ivs->cpHead; litsInd >= 0; ivsInd = ib->prevCPInd)
			{
				ib = &ivs->bs[ivsInd];
				var = &cnf->vars[labs(ib->litNum)];
				
				if (cnf->lastLayerCoPerCpLvl[var->cpImplLevel] == cnf->layerPartCount)
				{
					//for (implInd = ((ivsInd == ivs->cpHead) ? (cnf->vars[labs(cnf->layersArr[currLayer]->litNum)].ivsBox - ivs->bs) : (prevImplInd)); ((implInd != ivsInd)&&(litsInd >= 0)); implInd--)
					for (implInd = prevImplInd; ((implInd != ivsInd-1)&&(litsInd >= 0)); implInd--)
					{
						ib2 = &ivs->bs[implInd];
						var2 = &cnf->vars[labs(ib2->litNum)];
						
						if (var2->lastLayerCountVal == cnf->layerPartCount)
						{
							cnf->layersArr[var2->implLayer] = 0;
							litsArr[litsInd].litNum = -ib2->litNum;
#ifdef FBGNEWRESTART
							if (litsArr[litsInd].litNum == fbgNewRest[cnf->vars[labs(litsArr[litsInd].litNum)].varNum])
								fbgOneSat = TRUE;
#endif

							litsArr[litsInd].ordFromUp0Ind = litsInd;
							litsArr[litsInd].ifNextClsBoxInd = 1;
							litsArr[litsInd].ifUsedConfsDelHead = 0;
							litsArr[litsInd].nextClsIndOrD1ArrOrd = litsInd-1;		
							litsInd--;
							if (cnf->incrHeur1UIP >= 1)
								IncrHeur(cnf, (var2->implStat&IMPL_POS)?(var2->varNum):(-(long)(var2->varNum)), value_to_incr_heur);
						}
					}
					
				}		
				prevImplInd = ivsInd-1;
			}
		} else
		{
			InsertToLLL(cnf, &cnf->layerBoxesBefLastCPLvl, cnf->layersArr[currLayer]);
			for (currBox = cnf->layerBoxesBefLastCPLvl, litsInd = litsNum-1; currBox; currBox = currBox->next, litsInd--)
			{
				litsArr[litsInd].litNum = -cnf->vars[labs(currBox->litNum)].ivsBox->litNum;
#ifdef FBGNEWRESTART
				if (litsArr[litsInd].litNum == fbgNewRest[cnf->vars[labs(litsArr[litsInd].litNum)].varNum])
					fbgOneSat = TRUE;
#endif

				litsArr[litsInd].ordFromUp0Ind = litsInd;
				litsArr[litsInd].ifNextClsBoxInd = 1;
				litsArr[litsInd].ifUsedConfsDelHead = 0;
				litsArr[litsInd].nextClsIndOrD1ArrOrd = litsInd-1;
				//IncrHeur(cnf, (var2->implStat&IMPL_POS)?(var2->varNum):(-(long)(var2->varNum)), 1);
			}

		}
#ifdef FBGNEWRESTART
		if (!fbgOneSat)
			DebugBreak();
#endif

		//set "delete cls arrows", if needed
		if (litsArr[litsNum-1].ifDelCls)
		{
			varToDelCls = &cnf->vars[labs(litsArr[litsNum-cnf->max1UIPLenToRec-1].litNum)];			
			InsCCIntoLstBeg(cnf, &varToDelCls->clssToDel, &litsArr[litsNum-1], GetDelCls);
			*GetDelClsLit(litsArr) = varToDelCls->varNum;
		}
		
		if ((!(litsArr[litsNum-1].ifDelCls))||(varToDelCls->varNum != labs(litsArr[litsNum-2].litNum)))
			InsCCIntoLstBeg(cnf, &cnf->vars[labs(litsArr[litsNum-2].litNum)].bot2CC, &litsArr[litsNum-2], Bot2FromCcBox);
		else
			litsArr[litsNum-1].ifBot2 = 0;		
		*(D1EmergOrd(litsArr)) = -1;

#ifdef FBG1UIP
		for (fbgCcb = litsArr; fbgCcb != FRST_ARROW_FROM_CC(litsArr); fbgCcb++)
		{
			if (fbgCcb->litNum*seqFBG[labs(fbgCcb->litNum)] > 0)
				goto AftDebugBreak;
		}
		DebugBreak();
AftDebugBreak:
#endif
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
			{
		PrintCCGivenCCBox(cnf, litsArr, TRUE, TRUE);
		OutputTrace(cnf, "\nAdd1UIPClause", 39, "", buff);
		}
#endif
		*litNum = litsArr[litsNum-1].litNum;
		return &litsArr[litsNum-1];
	} else
	{
		/*
		cnf->vars[labs(cnf->layersArr[currLayer]->litNum)].aisLastCCBox = 0;
				PushIntoAddImplSt(cnf, -cnf->vars[labs(cnf->layersArr[currLayer]->litNum)].ivsBox->litNum);*/
		*litNum = cnf->layersArr[currLayer]->litNum;
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
			{
		buffInd+=sprintf(buff+buffInd, "\nAdd1UIPClause : no CC - ais %ld added", -cnf->vars[labs(cnf->layersArr[currLayer]->litNum)].ivsBox->litNum);
		OutputTrace(cnf, "\nAdd1UIPClause", 179, "", buff);
		}
#endif
		return 0;
	}
	
}

void UpdAISInfoFor1UIP(Cnf* cnf, CCBox* lastCcb, long litNum)
{
	Var *varToChk, *vrInImplStTill;
	long existImplLvlTill;
	
	if (lastCcb)
	{
		varToChk = &cnf->vars[labs(lastCcb->litNum)];
		vrInImplStTill = &cnf->vars[labs((lastCcb-1)->litNum)];

		if (vrInImplStTill->cpImplLevel == varToChk->cpImplLevel)
			return;
		
		if (ON(varToChk->implStat, IMPL_REG_RES))
		{
#ifdef WIN32
			_ASSERT((varToChk->aisStatus == notImplOut)||(varToChk->aisStatus == implCons));
#endif	
			if (varToChk->aisStatus == notImplOut)
			{
				varToChk->aisLastCCBox = lastCcb;
				varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
			} else
			{
				existImplLvlTill = ((varToChk->aisLastCCBox) ? (cnf->vars[labs((varToChk->aisLastCCBox-1)->litNum)].implLevel) : (-1));
				if (vrInImplStTill->implLevel > existImplLvlTill)
				{
					varToChk->aisCCOfVarToSkipTill = lastCcb;
					cnf->implLvlToSkipTill = vrInImplStTill->implLevel;
				}
				if (vrInImplStTill->implLevel < existImplLvlTill)
				{
					cnf->implLvlToSkipTill = existImplLvlTill;
					varToChk->aisCCOfVarToSkipTill = varToChk->aisLastCCBox;
					varToChk->aisLastCCBox = lastCcb;
					varToChk->aisStatus = addImplChngSide;
				}
				if (vrInImplStTill->implLevel == existImplLvlTill)
				{
					cnf->implLvlToSkipTill = existImplLvlTill;
					varToChk->aisCCOfVarToSkipTill = lastCcb;
				}

			}
		}
	} else
	{
		varToChk = &cnf->vars[labs(litNum)];

		if (ON(varToChk->implStat, IMPL_REG_RES))
		{		
			if (varToChk->aisStatus == notImplOut)
			{
				varToChk->aisLastCCBox = lastCcb;
				varToChk->aisStatus = AIS_CONF_PART_ADD_STAT(varToChk);
			} else
			{
				
				varToChk->aisStatus = addImplChngSide;
				cnf->lastCCBoxOfCCNotToDel = varToChk->aisCCOfVarToSkipTill = varToChk->aisLastCCBox;
				cnf->implLvlToSkipTill = -1;
				varToChk->aisLastCCBox = 0;
			}
		}

	}
}


