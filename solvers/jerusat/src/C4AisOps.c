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
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
CCArrowsBox* RfaisFromCcBox(CCBox* rfais2FromDownCCBox)
{
	return ( (CCArrowsBox*)(rfais2FromDownCCBox+2) );
}

CCArrowsBox* Bot2FromCcBox(CCBox* bot2FromDownCCBox)
{
	return ( ((CCArrowsBox*)(bot2FromDownCCBox+2)) + 1 );
}

void PushIntoAddImplSt(Cnf* cnf, long litNum)
{
	Var *var, *varInImplStTill;
	
	var = &cnf->vars[labs(litNum)];
#ifdef EXT2_PRINT
	if (TC(cnf, "EXT2_PRINT"))
		{
	buffInd+=sprintf(buff+buffInd, "\nPushIntoAddImplSt:%ld", litNum);
	}
#endif
	var->aisStatus = notImplIn;
	var->aisBox->prev = 0;
	var->aisBox->next = cnf->ais;
	var->aisBox->litNum = litNum;
	if (cnf->ais)
		cnf->ais->prev = var->aisBox;
	cnf->ais = var->aisBox;

	if (var->aisLastCCBox)
	{
		varInImplStTill = &cnf->vars[labs(((var->aisLastCCBox)-1)->litNum)];
		/*RemWCCBoxFromVarsWList(cnf, var->aisLastCCBox);
		RemWCCBoxFromVarsWList(cnf, var->aisLastCCBox-1);*/
		InsCCIntoLstBeg(cnf, &varInImplStTill->remFromAIS, ((var->aisLastCCBox)-1), RfaisFromCcBox);
#ifdef EXT2_PRINT
		if (TC(cnf, "EXT2_PRINT"))
			{
		PrintCCGivenCCBox(cnf, var->aisLastCCBox, TRUE, TRUE);
		}
#endif

	}

	if (!cnf->aisPtr)
		cnf->aisPtr = cnf->ais;
#ifdef PRINT_AIS
	if (TC(cnf, "PRINT_AIS"))
		{
	OutputTrace(cnf, "\nPushIntoAddImplSt:", 51, "", buff);
	PrintAis(cnf);
	}
#endif
}

void RemFromAisOnUnimply(Cnf* cnf, long litUnimpl)
{
	Var *varUnimpl, *aisVar;
	CCBox* botCCB;

	varUnimpl = &cnf->vars[labs(litUnimpl)];
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
		{
	sprintf(buff, "\nRemFromAisOnUnimply");
	OutputTrace(cnf, "\nRemFromAisOnUnimply", 257, "", buff);
	//GoThrowAllLists(cnf);
	}
#endif
	while (varUnimpl->remFromAIS)
	{
		aisVar = &cnf->vars[labs((varUnimpl->remFromAIS+1)->litNum)];

		if (cnf->aisPtr == aisVar->aisBox)
			cnf->aisPtr = cnf->aisPtr->prev;
		
		if (cnf->ais == aisVar->aisBox)
			cnf->ais = aisVar->aisBox->next;
		if (aisVar->aisBox->prev)
			aisVar->aisBox->prev->next = aisVar->aisBox->next;
		if (aisVar->aisBox->next)
			aisVar->aisBox->next->prev = aisVar->aisBox->prev;
#ifdef EXT3_PRINT
		if (TC(cnf, "EXT3_PRINT"))
			{
	sprintf(buff, "\nBefore RemCCFromLst");
	OutputTrace(cnf, "\nRemFromAisOnUnimply", 257, "", buff);
	//GoThrowAllLists(cnf);
	}
#endif
		botCCB = varUnimpl->remFromAIS+1;
		RemCCFromLst(cnf, &(varUnimpl->remFromAIS), varUnimpl->remFromAIS, RfaisFromCcBox);
#ifdef EXT3_PRINT
		if (TC(cnf, "EXT3_PRINT"))
			{
	sprintf(buff, "\nAfter RemCCFromLst");
	OutputTrace(cnf, "\nRemFromAisOnUnimply", 257, "", buff);
	//GoThrowAllLists(cnf);
	}
#endif
		
		aisVar->aisStatus = notImplOut;
		/*if (aisVar->aisLastCCBox)
		{
			InsWCCBoxIntoVarsWList(cnf, aisVar->aisLastCCBox);
			InsWCCBoxIntoVarsWList(cnf, aisVar->aisLastCCBox-1);
		}*/
		aisVar->aisLastCCBox = aisVar->aisCCOfVarToSkipTill = 0;
		aisVar->aisBox->prev = aisVar->aisBox->next = 0;
		/*if (botCCB->clsFromConfStat)
		{
			InsWCCBoxIntoVarsWList(cnf, botCCB, botCCB);
			InsWCCBoxIntoVarsWList(cnf, botCCB-1, botCCB);
		}*/
	}
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
		{
	sprintf(buff, "\nExiting RemFromAisOnUnimply");
	OutputTrace(cnf, "\nRemFromAisOnUnimply", 257, "", buff);
	//GoThrowAllLists(cnf);
	}
#endif
}

Boolean IfAddToAis(Cnf* cnf, long varNumToChk, long varNumInImplStTill)
{
	Var *varToChk, *varInImplStTill;
	IVS* ivs;

	varToChk = &cnf->vars[labs(varNumToChk)];
	varInImplStTill = &cnf->vars[labs(varNumInImplStTill)];
	
	ivs = cnf->ivs;
	
	if (ONE_ON_FOR_DISJ(varToChk->implStat,IMPL_D1|IMPL_D2))
	{
		if (ONE_ON_FOR_DISJ(varInImplStTill->implStat,IMPL_D1|IMPL_D2))
		{
			//if (varToChk->ivsBox->prevConfPartVar == varInImplStTill->ivsBox)
			if (&(ivs->bs[varToChk->ivsBox->prevCPInd]) == varInImplStTill->ivsBox)
				return FALSE;
			else
				return TRUE;
		}

		if (ON(varInImplStTill->implStat,IMPL_CONF))
		{
			//if (varInImplStTill->implLevel > cnf->vars[labs(varToChk->ivsBox->prevDecVar->varNum)].implLevel)
			if (varInImplStTill->implLevel > cnf->vars[labs(ivs->bs[varToChk->ivsBox->prevDecInd].litNum)].implLevel)
				return FALSE;
			else
				return TRUE;
		}		
		
		if (ON(varInImplStTill->implStat,IMPL_ADD_IMPL_ST))
			return TRUE;
	} else

	if (ON(varToChk->implStat,IMPL_CONF))
	{
		if (ONE_ON_FOR_DISJ(varInImplStTill->implStat,IMPL_D1|IMPL_D2|IMPL_ADD_IMPL_ST))
				return TRUE;
		
		if (ON(varInImplStTill->implStat,IMPL_CONF))
		{
			if (varInImplStTill->implLevel > cnf->vars[labs(varToChk->prevImplD2Num)].implLevel)
				return FALSE;
			else
				return TRUE;
		}		
	} else

	if (ON(varToChk->implStat,IMPL_ADD_IMPL_ST))
	{
		if (ONE_ON_FOR_DISJ(varInImplStTill->implStat,IMPL_D1|IMPL_CONF))
			return TRUE;
		
		if (ON(varInImplStTill->implStat,IMPL_D2))
		{
			if (varInImplStTill->varNum == labs(varToChk->prevImplD2Num) )
				return FALSE;
			else
				return TRUE;
		}

		if (ON(varInImplStTill->implStat,IMPL_ADD_IMPL_ST))
		{
			if (varInImplStTill->implLevel > cnf->vars[labs(varToChk->prevImplD2Num)].implLevel)
				return FALSE;
			else
				return TRUE;	
		}
	}

	return FALSE;
	
}

void RecreateConfOfAISVarIfNeeded(Cnf* cnf, long varNum)
{
	Var *aisVar, *var;
	CCBox* litsArr;
	unsigned long litsNum;
	long litsInd;
	Boolean ifBefUsedConfHead;

	aisVar = &cnf->vars[labs(varNum)];

	if (!((aisVar->aisLastCCBox)&&(aisVar->aisLastCCBox->ccType == CC_TYPE_CLS_ONLY)&&(aisVar->thisImplApp)&&(aisVar->aisLastCCBox->ifRecClsFromConfCreated == 0)&&(!aisVar->aisLastCCBox->ifAdd1UIP)))
		return;

//	if (!(cnf->heur&32))
	{
		litsArr = CLS_CCBOXES_BEG(aisVar->aisLastCCBox);
		litsNum = (*(CC_LITS_NUM(litsArr)));
		var = &cnf->vars[labs(litsArr[0].litNum)];
		
		//InsCCIntoLstBeg(cnf, &cnf->vars[labs(litsArr[0].litNum)].usedConfToDel, &litsArr[0], GetDelUsedConf);
		//set "added confs list arrows"
		InsToEndOfConfLst(cnf, &litsArr[litsNum-1]);
		
		for (ifBefUsedConfHead = TRUE, litsInd = 0; (litsInd < litsNum); litsInd++)
		{
			var = &cnf->vars[labs(litsArr[litsInd].litNum)];
			if (litsArr[litsInd].ifUsedConfsDelHead)
			{
				//set "delete used conf. arrows" into appropriate list
				InsCCIntoLstBeg(cnf, &var->usedConfToDel, &litsArr[litsInd], GetDelUsedConf);
				ifBefUsedConfHead = FALSE;
			}
			if (!ifBefUsedConfHead)
				var->thisImplCCBox++;
			var->thisImplApp++;
			if (litsArr[litsInd].nextClsIndOrD1ArrOrd != -1)
			{
				InsCCIntoLstBeg(cnf, &var->d1ConfsList, &litsArr[litsInd], GetD1ArrFromCC);
				var->thisImplD1Box++;
			}
		}

		cnf->currCCNum++;

		cnf->lastCCBOfLastCCAdded = &litsArr[litsNum-1];
		
		aisVar->aisLastCCBox->ccType = CC_TYPE_CONF_CLS;
		
		return;
	}

}
