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
#include "C4IVSStacks.h"
#include "C4PQ.h"
#ifdef WIN32
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif

void PrepVarToImpl(Cnf* cnf, long litNum, CCBox* implClsPtr, unsigned long implStat, long implLayer)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];
	
	if (OFF(var->implStat,IMPLIED))
	{
		var->implCls = implClsPtr;
		var->implStat = implStat;
		//to make sorting in Imply easier
		var->cpImplLevel = cnf->ivs->cpNum;
		var->implLayer = implLayer;
		Enqueue(cnf->toBeImplied, litNum);
	} else
	{
		if ( ( (ON(var->implStat,IMPL_POS)) && (ON(implStat,IMPL_NEG)) )
			||
			( (ON(var->implStat,IMPL_NEG)) && (ON(implStat,IMPL_POS)) )
			)
		{
			Enqueue(cnf->toBeImplied, DUMMY_CONF_LIT_TO_JUMP);
			cnf->varNumWithContr = var->varNum;
		}
	}
}

void IVSPushOrPrepushToBeImplied(Cnf* cnf)
{
	while (!IsQEmpty(cnf->toBeImplied))
	{
		IVSPushOrPrepush(cnf, Dequeue(cnf->toBeImplied));
	}
}

CCBox* Imply(Cnf* cnf)
{
	long litToJump, currClsInd, pInd, ppInd, oldPInd, auxInd;
	unsigned long* ccNumSortedFrom;
	Var *secWV, *currVar, *prevVar, *prev2Var, *varHeur;
	CCBox *horizCCB, *nextHorizCCB, *secwCCB, *clsCCBoxes, *wccbOfCurrVar, *currCCBox, *ccbWithData;
	CCBox** lstBeg;
	Boolean ifSorted, ifNewlySorted;
	CCArrowsBox* ccArrB;
#ifdef FBGNEWRESTART
//	if (cnf->actualCCNum >= 35616)
//		DebugBreak();
#endif


#ifdef IMPLY_PRINT
	if (TC(cnf, "IMPLY_PRINT"))
		{
				buffInd+=sprintf(buff+buffInd, "\n---------------Imply--------------");
				}
#endif

	while (!IsQEmpty(cnf->toBeImplied))
	{
		litToJump = Dequeue(cnf->toBeImplied);
		if (litToJump == DUMMY_CONF_LIT_TO_JUMP)
		{
			EmptyQueue(cnf->toBeImplied);

#ifdef IMPLY_PRINT
			if (TC(cnf, "IMPLY_PRINT"))
				{
			buffInd+=sprintf(buff+buffInd, "\nDUMMY_CONF_LIT_TO_JUMP");
			OutputTrace(cnf, "\nImply", 62, "", buff);		
			}
#endif


			return &cnf->dummyConfBox;
		}	

		if ((litToJump == MAX_PQ_LIT)||(ON(cnf->vars[labs(litToJump)].implStat,NOT_IMPLIED)))
		{
			litToJump = IVSPushOrPrepush(cnf, litToJump);
			if (cnf->vars[labs(litToJump)].aisStatus == implIncons)
			{
				//there is a contradiction!
				IVSPushOrPrepushToBeImplied(cnf);
				cnf->varNumWithContr = labs(litToJump);

#ifdef IMPLY_PRINT
				if (TC(cnf, "IMPLY_PRINT"))
					{
				buffInd+=sprintf(buff+buffInd, "\nimplIncons - %ld", litToJump);
				OutputTrace(cnf, "\nImply", 88, "", buff);		
				}
#endif

				return &cnf->dummyAISBox;
			}

#ifdef IMPLY_PRINT
			if (TC(cnf, "IMPLY_PRINT"))
				{
				buffInd+=sprintf(buff+buffInd, "\n@@@Literal #%ld", litToJump);
				OutputTrace(cnf, "\nImply", 88, "", buff);		
				}
#endif

		

			lstBeg = ((litToJump<0) ? (&cnf->vars[-litToJump].watchedPos) : (&cnf->vars[litToJump].watchedNeg));
			//each var may be in one of the following 5 states : 
			//notImpl, toBeImplSAT, toBeImplUNS,impliedSAT,impliedUNS
#ifdef IMPLY_PRINT
			if (TC(cnf, "IMPLY_PRINT"))
				{
//			GoThrowAllLists(cnf);
				}
#endif

			for (horizCCB = *lstBeg; horizCCB; horizCCB = nextHorizCCB)
			{

#ifdef IMPLY_PRINT
				if (TC(cnf, "IMPLY_PRINT"))
					{
				PrintCCGivenCCBox(cnf, horizCCB, TRUE, FALSE);
				OutputTrace(cnf, "\nImply", 87, "", buff);
				}
#endif

				ccbWithData = CCB_DOWN_FROM_CC(horizCCB);
				clsCCBoxes = CLS_CCBOXES_BEG(horizCCB);
				wccbOfCurrVar = WCCB_BY_WDOWN_AND_LIT(horizCCB,-litToJump);
				ccArrB = WchFromCcBox(wccbOfCurrVar,horizCCB);
				nextHorizCCB = ccArrB->next;
				secwCCB = ((horizCCB == wccbOfCurrVar) ? (&clsCCBoxes[horizCCB->nextClsIndOrD1ArrOrd]) : (horizCCB) );
				secWV = &cnf->vars[labs(secwCCB->litNum)];

				//Note : secWV can't be implUNS, cause it's watched
				
				//if secWV is impliedSAT or toBeImpliedSAT
				if ( ( (ON(secWV->implStat,IMPL_POS)) && (secwCCB->litNum > 0) ) 
					  ||
					 ( (ON(secWV->implStat,IMPL_NEG)) && (secwCCB->litNum < 0) ) )			   
				{
					goto nextClause;
				}
				
				ccNumSortedFrom = CC_SORTED_FROM(horizCCB);
				ifSorted = ccbWithData->ifSorted;
				for (ifNewlySorted = TRUE, currClsInd = clsCCBoxes[horizCCB->nextClsIndOrD1ArrOrd].nextClsIndOrD1ArrOrd, pInd = clsCCBoxes[horizCCB->nextClsIndOrD1ArrOrd].ordFromUp0Ind, ppInd = horizCCB->ordFromUp0Ind; currClsInd != -1; currClsInd = clsCCBoxes[currClsInd].nextClsIndOrD1ArrOrd)		
				{
					currCCBox = &clsCCBoxes[currClsInd];
					currVar = &cnf->vars[labs(currCCBox->litNum)];
					
					if (!currCCBox->ifNextClsBoxInd)
					{
						if ((ifSorted)&&(ON(currVar->implStat,IMPLIED))&&(currVar->lastCCBefImplNum <= *ccNumSortedFrom))
						{
							//simulating going each time throw all CC performance
							if (!(cnf->heur&4))
							{
								for (; currClsInd > -1; currClsInd--)
								{
									 if (&clsCCBoxes[currClsInd])
									 {
										 clsCCBoxes[currClsInd];
									 }
								}
							}
							break;
						}

						*(D1EmergOrd(currCCBox)) = currCCBox->nextClsIndOrD1ArrOrd; 
						currCCBox->ifNextClsBoxInd = 1;
						currCCBox->nextClsIndOrD1ArrOrd = currClsInd-1;						
					}

					if ( (currVar->implStat == NOT_IMPLIED)
						    ||
						 ( ( (ON(currVar->implStat,IMPL_POS)) && (currCCBox->litNum > 0) ) 
							||
						 ( (ON(currVar->implStat,IMPL_NEG)) && (currCCBox->litNum < 0) ) )			   
					   )
					{
						//if secWV isn't implied or currVar is SAT
						//we put secWV or currVar appropriately instead
						//of the wactched var, that led us to this CC and go on to the next CC
						if ( (secWV->implStat == NOT_IMPLIED) 
							   || 
							   (currVar->implStat != NOT_IMPLIED)
							   )
						{		
							if (cnf->heur&(1024|2048|4096))
							{
								if ( (secWV->implStat == NOT_IMPLIED) 
									    && 
									 (currVar->implStat == NOT_IMPLIED)
									    &&
									 (
									   (currCCBox->nextClsIndOrD1ArrOrd == -1)
									     ||
									   ((!clsCCBoxes[currCCBox->nextClsIndOrD1ArrOrd].ifNextClsBoxInd)
									      &&
									    (ifSorted)
									      &&
									    (ON(cnf->vars[labs(clsCCBoxes[currCCBox->nextClsIndOrD1ArrOrd].litNum)].implStat,IMPLIED))
									      &&
									    (cnf->vars[labs(clsCCBoxes[currCCBox->nextClsIndOrD1ArrOrd].litNum)].lastCCBefImplNum <= *ccNumSortedFrom)
									   )
									 )
								   )
								{												
									PQRemVar(cnf, cnf->pq, secWV->varNum);
									PQRemVar(cnf, cnf->pq, currVar->varNum);
									
									IncrHeur(cnf, -secwCCB->litNum, 1);
									IncrHeur(cnf, -currCCBox->litNum, 1);									

									PQAddVar(cnf, cnf->pq, secWV->varNum, GetLitsVal, GetVarsSign);
									PQAddVar(cnf, cnf->pq, currVar->varNum, GetLitsVal, GetVarsSign);
								}								
							}
							if (wccbOfCurrVar == horizCCB)
							{						
//								wccbOfCurrVar->ifOnWchList = FALSE;
//								currCCBox->ifOnWchList = TRUE;
								RemWCCBoxFromVarsWList(cnf, wccbOfCurrVar, wccbOfCurrVar);
								RemWCCBoxFromVarsWList(cnf, secwCCB, wccbOfCurrVar);
								if (ccbWithData->ifDelCls)
									RemCCFromLst(cnf, &cnf->vars[labs(*GetDelClsLit(ccbWithData))].clssToDel, wccbOfCurrVar, GetDelCls);
								clsCCBoxes[pInd].nextClsIndOrD1ArrOrd = currCCBox->nextClsIndOrD1ArrOrd;
								currCCBox->nextClsIndOrD1ArrOrd = secwCCB->ordFromUp0Ind;
								wccbOfCurrVar->nextClsIndOrD1ArrOrd = secwCCB->nextClsIndOrD1ArrOrd;
								secwCCB->nextClsIndOrD1ArrOrd = wccbOfCurrVar->ordFromUp0Ind;
								InsWCCBoxIntoVarsWList(cnf, secwCCB, currCCBox);
								InsWCCBoxIntoVarsWList(cnf, currCCBox, currCCBox);
								if (ccbWithData->ifDelCls)
									InsCCIntoLstBeg(cnf, &cnf->vars[labs(*GetDelClsLit(ccbWithData))].clssToDel, currCCBox, GetDelCls);
							} else
							{
								RemWCCBoxFromVarsWList(cnf, wccbOfCurrVar, horizCCB);
								clsCCBoxes[pInd].nextClsIndOrD1ArrOrd = currCCBox->nextClsIndOrD1ArrOrd;
								currCCBox->nextClsIndOrD1ArrOrd = wccbOfCurrVar->ordFromUp0Ind;
								secwCCB->nextClsIndOrD1ArrOrd = currCCBox->ordFromUp0Ind;
								InsWCCBoxIntoVarsWList(cnf, currCCBox, secwCCB);
							}
//							ExchWchInfo(cnf, horizCCB, &clsCCBoxes[currClsInd]);

#ifdef IMPLY_PRINT
							if (TC(cnf, "IMPLY_PRINT"))
								{
							PrintCCGivenCCBox(cnf, horizCCB, TRUE, FALSE);
							OutputTrace(cnf, "\nImply", 87, "", buff);
							}
#endif			

							goto nextClause;
						}
						else
						{
							//here we should make currVar watched in this CC
							//instead of SecVW and go on with CC checking
							if (wccbOfCurrVar == horizCCB)
							{
								RemWCCBoxFromVarsWList(cnf, secwCCB, wccbOfCurrVar);
								
								wccbOfCurrVar->nextClsIndOrD1ArrOrd = currClsInd;
								clsCCBoxes[pInd].nextClsIndOrD1ArrOrd = currCCBox->nextClsIndOrD1ArrOrd;
								currCCBox->nextClsIndOrD1ArrOrd = secwCCB->ordFromUp0Ind;
					
								InsWCCBoxIntoVarsWList(cnf, currCCBox, wccbOfCurrVar);
							
								auxInd = currClsInd;								
								currClsInd = pInd;
								
								if (pInd == secwCCB->ordFromUp0Ind)
									ppInd = auxInd;				

								secwCCB = &clsCCBoxes[auxInd];
								secWV = &cnf->vars[labs(secwCCB->litNum)];							

#ifdef IMPLY_PRINT
								if (TC(cnf, "IMPLY_PRINT"))
									{
								PrintCCGivenCCBox(cnf, horizCCB, TRUE, FALSE);
								OutputTrace(cnf, "\nImply", 261, "", buff);
								}
#endif

								continue;
									
							} else
							{								
//								secwCCB->ifOnWchList = FALSE;
//								currCCBox->ifOnWchList = TRUE;
								RemWCCBoxFromVarsWList(cnf, wccbOfCurrVar, secwCCB);
								RemWCCBoxFromVarsWList(cnf, secwCCB, secwCCB);
								if (ccbWithData->ifDelCls)
									RemCCFromLst(cnf, &cnf->vars[labs(*GetDelClsLit(ccbWithData))].clssToDel, secwCCB, GetDelCls);

								if (pInd != wccbOfCurrVar->ordFromUp0Ind)
								{
									clsCCBoxes[pInd].nextClsIndOrD1ArrOrd = currCCBox->nextClsIndOrD1ArrOrd;
									currCCBox->nextClsIndOrD1ArrOrd = wccbOfCurrVar->ordFromUp0Ind;
									secwCCB->nextClsIndOrD1ArrOrd = wccbOfCurrVar->nextClsIndOrD1ArrOrd;
									wccbOfCurrVar->nextClsIndOrD1ArrOrd = secwCCB->ordFromUp0Ind;
									
									InsWCCBoxIntoVarsWList(cnf, currCCBox, currCCBox);
									InsWCCBoxIntoVarsWList(cnf, wccbOfCurrVar, currCCBox);								
									if (ccbWithData->ifDelCls)
										InsCCIntoLstBeg(cnf, &cnf->vars[labs(*GetDelClsLit(ccbWithData))].clssToDel, currCCBox, GetDelCls);

									currClsInd = pInd;
									if (ppInd == wccbOfCurrVar->ordFromUp0Ind)
										ppInd = secwCCB->ordFromUp0Ind;		
																		
									secwCCB = horizCCB = currCCBox;
									secWV = &cnf->vars[labs(secwCCB->litNum)];			


#ifdef IMPLY_PRINT
									if (TC(cnf, "IMPLY_PRINT"))
										{
									PrintCCGivenCCBox(cnf, horizCCB, TRUE, FALSE);
									OutputTrace(cnf, "\nImply", 261, "", buff);
									}
#endif

									continue;							
								} else
								{
									secwCCB->nextClsIndOrD1ArrOrd = currCCBox->nextClsIndOrD1ArrOrd;
									currCCBox->nextClsIndOrD1ArrOrd = wccbOfCurrVar->ordFromUp0Ind;
									wccbOfCurrVar->nextClsIndOrD1ArrOrd = secwCCB->ordFromUp0Ind;

									InsWCCBoxIntoVarsWList(cnf, currCCBox, currCCBox);
									InsWCCBoxIntoVarsWList(cnf, wccbOfCurrVar, currCCBox);								
									if (ccbWithData->ifDelCls)
										InsCCIntoLstBeg(cnf, &cnf->vars[labs(*GetDelClsLit(ccbWithData))].clssToDel, currCCBox, GetDelCls);
									
									secwCCB = horizCCB = currCCBox;
									secWV = &cnf->vars[labs(secwCCB->litNum)];			
									pInd = currClsInd = ppInd;
									ppInd = wccbOfCurrVar->ordFromUp0Ind;

#ifdef IMPLY_PRINT
									if (TC(cnf, "IMPLY_PRINT"))
										{
									PrintCCGivenCCBox(cnf, horizCCB, TRUE, FALSE);
									OutputTrace(cnf, "\nImply", 261, "", buff);
									}
#endif

									continue;							

								}
							}
						}
					}
					
					//if (!cnf->ifPrePushD1Block)
					{
						if (pInd != horizCCB->nextClsIndOrD1ArrOrd)
						{							
							prevVar = &cnf->vars[labs(clsCCBoxes[pInd].litNum)];
							
							if (currVar->cpImplLevel > prevVar->cpImplLevel)
							{
								if (cnf->heur&8)
								//if ((cnf->heur&8)&&((*CC_LITS_NUM(currCCBox))>17))
								{
									clsCCBoxes[pInd].nextClsIndOrD1ArrOrd = currCCBox->nextClsIndOrD1ArrOrd;
									currCCBox->nextClsIndOrD1ArrOrd = clsCCBoxes[pInd].ordFromUp0Ind;
									clsCCBoxes[ppInd].nextClsIndOrD1ArrOrd = currCCBox->ordFromUp0Ind;
									
									oldPInd = pInd;
									pInd = currCCBox->ordFromUp0Ind;
									currClsInd = oldPInd;
									
									if ((ppInd != horizCCB->nextClsIndOrD1ArrOrd)&&(ifNewlySorted))
									{									
										prev2Var = &cnf->vars[labs(clsCCBoxes[ppInd].litNum)];
										if (currVar->cpImplLevel > prev2Var->cpImplLevel)
											ifNewlySorted = FALSE;
									}
									
								} else
								{
									ifNewlySorted = FALSE;
								}		
							}
						}
					}/* else
					{
						ifNewlySorted = FALSE;
					}*/
					//ifNewlySorted = FALSE;					
					
					ppInd = pInd;
					pInd = currClsInd;
				}
				
				if (ifNewlySorted)
				{
					*ccNumSortedFrom = cnf->currCCNum;
					ccbWithData->ifSorted = 1;
				}
				

#ifdef IMPLY_PRINT
				if (TC(cnf, "IMPLY_PRINT"))
					{
				PrintCCGivenCCBox(cnf, horizCCB, TRUE, FALSE);
				OutputTrace(cnf, "\nImply", 87, "", buff);
				}
#endif

#ifdef IMPLY_PRINT
				if (TC(cnf, "IMPLY_PRINT"))
					{
//						GoThrowAllLists(cnf);
					}
#endif
				//in this case we add secWV to toBeImplied
				if (secWV->implStat == NOT_IMPLIED)
				{
#ifdef IMPLY_PRINT
					if (TC(cnf, "IMPLY_PRINT"))
						{
					buffInd+=sprintf(buff+buffInd, "\n~~~Literal #%ld is added to cnf->toBeImplied", secwCCB->litNum);
					OutputTrace(cnf, "\nImply", 87, "", buff);
					}
#endif
					PrepVarToImpl(cnf, secwCCB->litNum, secwCCB, NOT_IMPLIED|IMPL_SIGN(secwCCB->litNum)|IMPL_REG_RES, cnf->vars[labs(litToJump)].implLayer+1);
				} else
				{
					//there is a contradiction!
					IVSPushOrPrepushToBeImplied(cnf);
					return horizCCB;
				}
nextClause:	0;	   
			}
		}
	}
	
	return 0;
}
