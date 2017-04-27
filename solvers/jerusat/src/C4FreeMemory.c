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
#include "C4Globals.h"
#include "C4PQ.h"
#ifdef WIN32
#include <crtdbg.h>
//#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
void FreeCnf(Cnf* cnf)
{
	long i;
	CCBox *ccb, *ccbNext, *wccbDown, *ccbWithInfo, *ccbArr;
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
		{
	//GoThrowAllLists(cnf);
		}
#endif
	if (!cnf)
		return;
	for (i = 1; i <= cnf->initVarsNum; i++)
	{
		InsIntoWListsOnBot2Unimply(cnf, i);
		free(cnf->vars[i].pqBox);
		free(cnf->vars[i].aisBox);
	}

	
	for (i = 1; i <= cnf->initVarsNum; i++)
	{
		for (ccb = cnf->vars[i].watchedPos; ccb; ccb = ccbNext)
		{
			ccbArr = CLS_CCBOXES_BEG(ccb);
			ccbNext = WchFromCcBox(ccb, ccb)->next;
			ccbWithInfo = CCB_DOWN_FROM_CC(ccb);
			if (ccbWithInfo->ccType != CC_TYPE_INIT_CLS)
			{
				RemWCCBoxFromVarsWList(cnf, &ccbArr[ccb->nextClsIndOrD1ArrOrd], ccb);
				RemWCCBoxFromVarsWList(cnf, ccb, ccb);
				if (ccbWithInfo->ccType == CC_TYPE_CLS_ONLY)
					free(CC_LITS_NUM(ccb));
				else
				{
					RemCCFromLst(cnf, &cnf->lastCCBOfFirstCCAdded, ccbWithInfo, GetAddedCCList);
					free(CC_LITS_NUM(ccb));
				}
#ifdef EXT3_PRINT
				if (TC(cnf, "EXT3_PRINT"))
					{
				//GoThrowAllLists(cnf);
					}
#endif

			}
		}
		cnf->vars[i].watchedPos = 0;

		for (ccb = cnf->vars[i].watchedNeg; ccb; ccb = ccbNext)
		{
			ccbArr = CLS_CCBOXES_BEG(ccb);
			ccbNext = WchFromCcBox(ccb, ccb)->next;
			ccbWithInfo = CCB_DOWN_FROM_CC(ccb);
			if (ccbWithInfo->ccType != CC_TYPE_INIT_CLS)
			{
				RemWCCBoxFromVarsWList(cnf, &ccbArr[ccb->nextClsIndOrD1ArrOrd], ccb);
				RemWCCBoxFromVarsWList(cnf, ccb, ccb);
				if (ccbWithInfo->ccType == CC_TYPE_CLS_ONLY)
					free(CC_LITS_NUM(ccb));
				else
				{
					RemCCFromLst(cnf, &cnf->lastCCBOfFirstCCAdded, ccbWithInfo, GetAddedCCList);
					free(CC_LITS_NUM(ccb));
				}
#ifdef EXT3_PRINT
				if (TC(cnf, "EXT3_PRINT"))
					{
				//GoThrowAllLists(cnf);
					}
#endif

			}
		}
		cnf->vars[i].watchedNeg = 0;
	}

	for (ccb = cnf->lastCCBOfFirstCCAdded; ccb; ccb = ccbNext)
	{
		ccbNext = GetAddedCCList(ccb)->next;
		free(CC_LITS_NUM(ccb));
	}
	
	free(cnf->initCCBuff);	
	free(cnf->vars);
	PQDestroy(cnf->pq);
	FreeQueue(cnf->toBeImplied);
	FreeStack(cnf->prePushSt);
	FreeStack(cnf->pushD1BlockSt);
	IVSDestroy(cnf->ivs);
	free(cnf->lastConfNum);
	free(cnf->lastPrepushConfNum);
	FreeStack(cnf->rccCPLits);
	FreeStack(cnf->confVarsPart);
	free(cnf->varsNumForRestart);
	free(cnf->lastLayerPerCpNum);
	free(cnf->layersArr);
	free(cnf->lastLayerCoPerCpLvl);
	free(cnf);
}
