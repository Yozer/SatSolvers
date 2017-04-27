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
#ifdef WIN32
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
//returns TRUE if no need to check conflicts after TreatSimilarityBefConfChk
/*Boolean TreatSimilarityBefConfChk(Cnf* cnf, long litNum)
{
Var *var, *varItr;
CCBox *ccb, *ccbNext, *ccbWithData, *ccbNextWithData, *itrCcb, *itrCcbNext, *cc1Arrow, *ccNext1Arrow, *ccbEq, *ccbFlipped;
CCBox *ccbUsedConf, *ccbNextUsedConf, *ccbWL, *ccbNextWL, *ccBeg, *ccNextBeg; 
CCBox *addedConfsPrev, *addedConfsNext, *newCcbWithData, *d1Prev, *d1Next, *nextCcbToChk;
unsigned long *ccLitsNum, *ccNextLitsNum;
Boolean notEq, ifBefUsedConfHead, resNoNeedToChkConfs;
CCArrowsBox* arrBox;
//short oldEmD1;
#ifdef EXT2_PRINT
if (TC(cnf, "EXT2_PRINT"))
{
buffInd = sprintf(buff, "\nTreatSimilarityBefConfChk:");
}
#endif

  resNoNeedToChkConfs = FALSE;
  var = &cnf->vars[labs(litNum)];
  
	for (ccb = var->d1ConfsList; ccb; ccb = nextCcbToChk)
	{
	ccbWithData = CCB_DOWN_FROM_CC(ccb);
	
	  if (ccbWithData->ifConfSimTreated)
	  {
	  //return resNoNeedToChkConfs;
	  if (!(nextCcbToChk = ccbNext = GetD1ArrFromCC(ccb)->next))
	  return resNoNeedToChkConfs;
	  continue;
	  }
	  
		ccbWithData->ifConfSimTreated = TRUE;
		
		  if (!(nextCcbToChk = ccbNext = GetD1ArrFromCC(ccb)->next))
		  return resNoNeedToChkConfs;
		  ccbNextWithData = CCB_DOWN_FROM_CC(ccbNext);
		  
			if (ccb->ordFromUp0Ind != ccbNext->ordFromUp0Ind)
			continue;
			
			  ccLitsNum = CC_LITS_NUM(ccb);
			  ccNextLitsNum = CC_LITS_NUM(ccbNext);
			  cc1Arrow = FRST_ARROW_FROM_CC(ccb);
			  ccNext1Arrow = FRST_ARROW_FROM_CC(ccbNext);
			  
				if (*ccLitsNum == *ccNextLitsNum)
				{
				//CC's may be equal or one flipped
				for (ccbWL = 0, ccbNextWL = 0, notEq = FALSE, ccbFlipped = 0, itrCcb = ccb + 1, itrCcbNext = ccbNext + 1; itrCcb != cc1Arrow; itrCcb++, itrCcbNext++)
				{
				if (labs(itrCcb->litNum) != labs(itrCcbNext->litNum))
				{
				notEq = TRUE;
				break;
				}
				if (itrCcb->litNum == -itrCcbNext->litNum)
				{
				if (!ccbFlipped)
				ccbFlipped = itrCcb;
				else
				{
				notEq = TRUE;
				break;
				}
				}
				if (itrCcb->ifOnWchList)
				ccbWL = itrCcb;
				if (itrCcbNext->ifOnWchList)
				ccbNextWL = itrCcbNext;
				}
				if (notEq)
				continue;
				if ((!ccbWL)&&(ccb->ifOnWchList))
				ccbWL = ccb;
				if ((!ccbNextWL)&&(ccbNext->ifOnWchList))
				ccbNextWL = ccbNext;
				} else
				{
				//TO BE CHANGED!!!
				continue;
				}
				
				  for (ccbUsedConf = ccbNextUsedConf = 0,notEq = FALSE, itrCcb = ccb-1, itrCcbNext = ccbNext-1; ((itrCcb+1)->ordFromUp0Ind) != 0; itrCcb--, itrCcbNext--)
				  {
				  if (itrCcb->litNum != itrCcbNext->litNum)
				  {
				  notEq = TRUE;
				  break;
				  }
				  if (itrCcb->ifUsedConfsDelHead)
				  ccbUsedConf = itrCcb;
				  if (itrCcbNext->ifUsedConfsDelHead)
				  ccbNextUsedConf = itrCcbNext;
				  }
				  if (notEq)
				  continue;
				  if ((!ccbUsedConf)&&(ccb->ifUsedConfsDelHead))
				  ccbUsedConf = ccb;
				  if ((!ccbNextUsedConf)&&(ccbNext->ifUsedConfsDelHead))
				  ccbNextUsedConf = ccbNext;
				  #ifdef EXT2_PRINT
				  if (TC(cnf, "EXT2_PRINT"))
				  {
				  buffInd+=sprintf(buff+buffInd, "\n2 equal or flipped CC's are found");
				  PrintCCGivenCCBox(cnf, ccb, TRUE, FALSE);
				  PrintCCGivenCCBox(cnf, ccbNext, TRUE, FALSE);
				  OutputTrace(cnf, "\nTreatSimilarityBefConfChk", 333, "", buff);
				  }
				  #endif
				  //if we reached this point one is flipped or the CC's are equal 
				  ccbWithData->ifConfSimTreated = FALSE;
				  ccBeg = CLS_CCBOXES_BEG(ccb);
				  ccNextBeg = CLS_CCBOXES_BEG(ccbNext);
				  //if one flipped
				  if (ccbFlipped)
				  {
				  //removing boths CC's from watched lists or bot2 lists
				  
					if ( (ccbWithData->ccType != CC_TYPE_CONF_ONLY) && (ccbWL) )
					{
					ccbWL->ifOnWchList = FALSE;
					RemWCCBoxFromVarsWList(cnf, ccbWL, ccbWL);
					RemWCCBoxFromVarsWList(cnf, &ccBeg[ccbWL->nextClsIndOrD1ArrOrd], ccbWL);
					} else
					RemCCFromLst(cnf, &cnf->vars[labs((ccbWithData-1)->litNum)].bot2CC, ccbWithData-1, Bot2FromCcBox);
					if ( (ccbNextWithData->ccType != CC_TYPE_CONF_ONLY) && (ccbNextWL) )
					{
					ccbNextWL->ifOnWchList = FALSE;
					RemWCCBoxFromVarsWList(cnf, ccbNextWL, ccbNextWL);
					RemWCCBoxFromVarsWList(cnf, &ccNextBeg[ccbNextWL->nextClsIndOrD1ArrOrd], ccbNextWL);
					} else
					RemCCFromLst(cnf, &cnf->vars[labs((ccbNextWithData-1)->litNum)].bot2CC, ccbNextWithData-1, Bot2FromCcBox);
					
					  //removing del. cls. from both clssToDel
					  if (ccbWithData->ifDelClsDataField)
					  RemCCFromLst(cnf, &cnf->vars[labs((ccbWithData-cnf->maxClsLenToRec)->litNum)].clssToDel, ccbWithData, GetDelCls);
					  if (ccbNextWithData->ifDelClsDataField)
					  RemCCFromLst(cnf, &cnf->vars[labs((ccbNextWithData-cnf->maxClsLenToRec)->litNum)].clssToDel, ccbNextWithData, GetDelCls);
					  
						//removing both used confs del
						RemCCFromLst(cnf, &cnf->vars[labs(ccbUsedConf->litNum)].usedConfToDel, ccbUsedConf, GetDelUsedConf);
						RemCCFromLst(cnf, &cnf->vars[labs(ccbNextUsedConf->litNum)].usedConfToDel, ccbNextUsedConf, GetDelUsedConf);
						
						  //removing both from "added confs list arrows", saving prev and aft for ccb
						  RemFromConfLst(cnf, ccbNextWithData);
						  addedConfsPrev = GetAddedCCList(ccbWithData)->prev;
						  addedConfsNext = GetAddedCCList(ccbWithData)->next;
						  RemFromConfLst(cnf, ccbWithData);
						  
							//preparing new last box of CC1
							newCcbWithData = ccbWithData-1;
							newCcbWithData->ifDelClsDataField = ( (((*ccLitsNum)-1) > cnf->maxClsLenToRec) ? (1) : (0) );
							newCcbWithData->ifPlcForEmD1Ord = ccbWithData->ifPlcForEmD1Ord;
							newCcbWithData->ifSorted = ccbWithData->ifSorted;
							newCcbWithData->ifWatchedArrowsDataFields = ccbWithData->ifWatchedArrowsDataFields;
							newCcbWithData->ccType = ccbWithData->ccType;
							newCcbWithData->ifConfSimTreated = ccbWithData->ifConfSimTreated;
							
							  if (ccb->ifNextClsBoxInd)
							  {
							  ccb->ifNextClsBoxInd = FALSE;
							  ccb->nextClsIndOrD1ArrOrd = *(D1EmergOrd(ccb));
							  }
							  
								for (ifBefUsedConfHead = TRUE, itrCcb = CLS_CCBOXES_BEG(ccbWithData), itrCcbNext = CLS_CCBOXES_BEG(ccbNextWithData); (itrCcb-1) != ccb; itrCcb++, itrCcbNext++)
								{
								varItr = &cnf->vars[labs(itrCcbNext->litNum)];
								
								  if (itrCcbNext->ifUsedConfsDelHead)
								  ifBefUsedConfHead = FALSE;
								  
									varItr->thisImplApp--;
									if (!ifBefUsedConfHead)
									varItr->thisImplCCBox--;
									
									  if ((itrCcbNext->ifNextClsBoxInd)||(itrCcbNext->nextClsIndOrD1ArrOrd != -1))
									  {
									  varItr->thisImplD1Box--;
									  RemCCFromLst(cnf, &varItr->d1ConfsList, itrCcbNext, GetD1ArrFromCC);
									  }
									  
										if ((!itrCcb->ifNextClsBoxInd)&&(itrCcb->nextClsIndOrD1ArrOrd != -1))
										{
										d1Prev = GetD1ArrFromCC(itrCcb)->prev;					
										d1Next = GetD1ArrFromCC(itrCcb)->next;
										(*ccLitsNum)--;
										GetD1ArrFromCC(itrCcb)->prev = d1Prev;
										GetD1ArrFromCC(itrCcb)->next = d1Next;
										(*ccLitsNum)++;					
										}
										}
										
										  //if (!ccb->ifNextClsBoxInd)
										  {
										  (*ccLitsNum)--;
										  *(D1EmergOrd(ccb)) = ccb->nextClsIndOrD1ArrOrd; 
										  (*ccLitsNum)++;
										  ccb->ifNextClsBoxInd = TRUE;
										  ccb->nextClsIndOrD1ArrOrd = *ccLitsNum - 2;						
										  }/* else
										  {
										  d1Prev = GetD1ArrFromCC(ccb)->prev;					
										  d1Next = GetD1ArrFromCC(ccb)->next;
										  (*ccLitsNum)--;
										  *(D1EmergOrd(ccb)) = oldEmD1;
										  GetD1ArrFromCC(ccb)->prev = d1Prev;
										  GetD1ArrFromCC(ccb)->next = d1Next;
										  (*ccLitsNum)++;	
										  ccb->ifNextClsBoxInd = TRUE;
										  ccb->nextClsIndOrD1ArrOrd = *ccLitsNum - 2;						
										  } // * /was here
										  
											for (itrCcb = ccb+1; itrCcb != ccbFlipped; itrCcb++)
											{
											if (itrCcb != (ccb+1))
											itrCcb->nextClsIndOrD1ArrOrd = itrCcb->ordFromUp0Ind-1;
											else
											itrCcb->nextClsIndOrD1ArrOrd = ((ccb->ordFromUp0Ind != 0) ? (ccb->ordFromUp0Ind-1) : (-1)); 
											itrCcb->ifNextClsBoxInd = TRUE;
											}
											
											  for (; itrCcb != (cc1Arrow-1); itrCcb++)
											  {
											  itrCcb->litNum = (itrCcb+1)->litNum;
											  if (itrCcb != (ccb+1))
											  itrCcb->nextClsIndOrD1ArrOrd = itrCcb->ordFromUp0Ind-1;
											  else
											  itrCcb->nextClsIndOrD1ArrOrd = ((ccb->ordFromUp0Ind != 0) ? (ccb->ordFromUp0Ind-1) : (-1)); 
											  itrCcb->ifNextClsBoxInd = TRUE;
											  }
											  
												//freeing CC2
												free(ccNextLitsNum);
												
												  //finally setting the new number of vars
												  (*ccLitsNum)--;
												  
													if ((*(ccLitsNum)) > 1)
													{
													GetAddedCCList(newCcbWithData)->prev = addedConfsPrev;
													GetAddedCCList(newCcbWithData)->next = addedConfsNext;
													if (!addedConfsPrev)
													cnf->lastCCBOfFirstCCAdded = newCcbWithData;
													else
													GetAddedCCList(addedConfsPrev)->next = newCcbWithData;
													if (!addedConfsNext)
													cnf->lastCCBOfLastCCAdded = newCcbWithData;
													else
													GetAddedCCList(addedConfsNext)->prev = newCcbWithData;
													//set "delete cls arrows", if needed
													if (newCcbWithData->ifDelClsDataField)
													InsCCIntoLstBeg(cnf, &cnf->vars[labs((newCcbWithData-cnf->maxClsLenToRec)->litNum)].clssToDel, newCcbWithData, GetDelCls);
													
													  //set "delete used conf. arrows" into appropriate list
													  
														//changing the placement of ccUsedConf if needed
														if (ccbUsedConf->ordFromUp0Ind != 0)
														{
														ccbUsedConf->ifUsedConfsDelHead = 0;
														ccbUsedConf--;
														ccbUsedConf->ifUsedConfsDelHead = 1;
														cnf->vars[labs(ccbUsedConf->litNum)].thisImplCCBox++;
														}
														InsCCIntoLstBeg(cnf, &cnf->vars[labs(ccbUsedConf->litNum)].usedConfToDel, ccbUsedConf, GetDelUsedConf);
														
														  if ( (newCcbWithData->ccType != CC_TYPE_CONF_ONLY) && ((newCcbWithData - ccb)>=2) )
														  {
														  ccb->ifOnWchList = TRUE;
														  InsWCCBoxIntoVarsWList(cnf, ccb, ccb);
														  InsWCCBoxIntoVarsWList(cnf, &ccBeg[ccb->nextClsIndOrD1ArrOrd], ccb);
														  if (GetD1ArrFromCC(ccb)->prev)
														  nextCcbToChk = GetD1ArrFromCC(ccb)->prev;
														  else
														  nextCcbToChk = ccb;
														  } else
														  {
														  if (newCcbWithData - ccb == 1)
														  {
														  InsCCIntoLstBeg(cnf, &cnf->vars[labs(ccb->litNum)].bot2CC, ccb, Bot2FromCcBox);
														  if (GetD1ArrFromCC(ccb)->prev)
														  nextCcbToChk = GetD1ArrFromCC(ccb)->prev;
														  else
														  nextCcbToChk = ccb;
														  }
														  else
														  {						
														  //RemCCFromLst(cnf, &var->d1ConfsList, ccb, GetD1ArrFromCC);
														  var->thisImplD1Box--;
														  InsCCIntoLstBeg(cnf, &cnf->vars[labs((ccb-1)->litNum)].bot2CC, (ccb-1), Bot2FromCcBox);
														  UpdateAddImplStInfo(cnf, newCcbWithData, newCcbWithData->litNum);
														  resNoNeedToChkConfs = TRUE;
														  if (GetD1ArrFromCC(ccb)->prev)
														  nextCcbToChk = GetD1ArrFromCC(ccb)->prev;
														  else
														  nextCcbToChk = ccb;
														  }
														  }
														  #ifdef EXT2_PRINT
														  if (TC(cnf, "EXT2_PRINT"))
														  {
														  buffInd+=sprintf(buff+buffInd, "\nThe first CC is now:");
														  PrintCCGivenCCBox(cnf, ccb, TRUE, FALSE);
														  OutputTrace(cnf, "\nTreatSimilarityBefConfChk", 333, "", buff);
														  }
														  #endif
														  } else
														  {
														  resNoNeedToChkConfs = TRUE;
														  UpdateAddImplStInfo(cnf, 0, newCcbWithData->litNum);
														  if (GetD1ArrFromCC(ccb)->prev)
														  nextCcbToChk = GetD1ArrFromCC(ccb)->prev;
														  else
														  nextCcbToChk = GetD1ArrFromCC(ccb)->next;
														  free(ccLitsNum);
														  var->thisImplD1Box--;
														  #ifdef EXT2_PRINT
														  if (TC(cnf, "EXT2_PRINT"))
														  {
														  buffInd+=sprintf(buff+buffInd, "\nThe first CC has been destroyed");
														  OutputTrace(cnf, "\nTreatSimilarityBefConfChk", 333, "", buff);
														  }
														  #endif
														  
															}
															if (nextCcbToChk)
															CCB_DOWN_FROM_CC(nextCcbToChk)->ifConfSimTreated = FALSE;
															cnf->actualCCNum--;
															}
															}
															
															  return resNoNeedToChkConfs;
															  }
*/
/*void ChkAndTreat1ConfEachSideForD2(Cnf* cnf, long litNum)
{
Var* var;
CCBox *itrCC1, *itrCC2, *lastCC1B, *lastCC2B, *usedConfCC1, *usedConfCC2, *newLastCC1B;
unsigned long *sortedFromCC1, *sortedFromCC2, *litsNumCC1, *litsNumCC2;
Boolean ifBefUsedConfHead;
#ifdef EXT2_PRINT
if (TC(cnf, "EXT2_PRINT"))
{
buffInd = sprintf(buff, "\nChkAndTreat1ConfEachSideForD2 for %ld", litNum);
OutputTrace(cnf, "\nChkAndTreat1ConfEachSideForD2", 147, "", buff);
}
#endif

  var = &cnf->vars[labs(litNum)];
  
	if (!(lastCC2B = cnf->lastCCBOfLastCCAdded))
	return;
	if (!(lastCC1B = GetAddedCCList(lastCC2B)->prev))
	return;
	
	  
		if (!( (lastCC2B->litNum == -litNum) && (lastCC1B->litNum == litNum) ))
		return;
		
		  litsNumCC1 = CC_LITS_NUM(lastCC1B);
		  litsNumCC2 = CC_LITS_NUM(lastCC2B);
		  if ( (*litsNumCC1) != (*litsNumCC2) )
		  return;
		  
			for (sortedFromCC1 = CC_SORTED_FROM(lastCC1B), itrCC1 = lastCC1B-1, itrCC2 = lastCC2B-1; (itrCC1+1)->ordFromUp0Ind != 0; itrCC1--, itrCC2--)
			{
			if (itrCC1->litNum != itrCC2->litNum)
			return;
			if (itrCC1->ifUsedConfsDelHead)
			usedConfCC1 = itrCC1;
			if (itrCC2->ifUsedConfsDelHead)
			usedConfCC2 = itrCC2;
			}
			
			  /*	if (lastCC1B->ifUsedConfsDelHead)
			  usedConfCC1 = lastCC1B;
			  if (lastCC2B->ifUsedConfsDelHead)
			  usedConfCC2 = lastCC2B;* /
			  #ifdef EXT2_PRINT
			  if (TC(cnf, "EXT2_PRINT"))
			  {
			  buffInd += sprintf(buff, " : CC's are equal");
			  }
			  #endif	
			  //CC's are equal if we reached this point
			  sortedFromCC2 = CC_SORTED_FROM(lastCC2B);
			  
				//removing boths CC's from bot2
				RemCCFromLst(cnf, &cnf->vars[labs((lastCC1B-1)->litNum)].bot2CC, lastCC1B-1, Bot2FromCcBox);
				RemCCFromLst(cnf, &cnf->vars[labs((lastCC2B-1)->litNum)].bot2CC, lastCC2B-1, Bot2FromCcBox);
				
				  //removing del. cls. from both clssToDel
				  if (lastCC1B->ifDelCls)
				  RemCCFromLst(cnf, &cnf->vars[labs(*(GetDelClsAr(lastCC1B)))].clssToDel, lastCC1B, GetDelCls);
				  if (lastCC2B->ifDelCls)
				  RemCCFromLst(cnf, &cnf->vars[labs(*(GetDelClsAr(lastCC2B)))].clssToDel, lastCC2B, GetDelCls);
				  
					//removing both used confs del
					//if (lastCC1B->ccType != CC_TYPE_CLS_ONLY) 
					RemCCFromLst(cnf, &cnf->vars[labs(usedConfCC1->litNum)].usedConfToDel, usedConfCC1, GetDelUsedConf);
					//if (lastCC2B->ccType != CC_TYPE_CLS_ONLY) 
					RemCCFromLst(cnf, &cnf->vars[labs(usedConfCC2->litNum)].usedConfToDel, usedConfCC2, GetDelUsedConf);
					
					  //removing both from "added confs list arrows"
					  RemFromConfLst(cnf, lastCC1B);
					  RemFromConfLst(cnf, lastCC2B);
					  
						//preparing new last box of CC1
						newLastCC1B = lastCC1B-1;
						newLastCC1B->ifDelCls = ( (((*litsNumCC1)-1) > cnf->maxClsLenToRec) ? (1) : (0) );
						newLastCC1B->ifSorted = lastCC1B->ifSorted;
						newLastCC1B->ccType = lastCC1B->ccType;
						newLastCC1B->ifConfSimTreated = lastCC1B->ifConfSimTreated;
						
						  for (ifBefUsedConfHead = TRUE, itrCC1 = CLS_CCBOXES_BEG(lastCC1B), itrCC2 = CLS_CCBOXES_BEG(lastCC2B); itrCC1 != lastCC1B; itrCC1++, itrCC2++)
						  {
						  var = &cnf->vars[labs(itrCC2->litNum)];
						  
							if (itrCC2->ifUsedConfsDelHead)
							ifBefUsedConfHead = FALSE;
							
							  var->thisImplApp--;
							  if (!ifBefUsedConfHead)
							  var->thisImplCCBox--;
							  
								if (itrCC2->nextClsIndOrD1ArrOrd != -1)
								{
								var->thisImplD1Box--;
								RemCCFromLst(cnf, &var->d1ConfsList, itrCC2, GetD1ArrFromCC);
								}
								
								  if (itrCC1->nextClsIndOrD1ArrOrd != -1)
								  {
								  RemCCFromLst(cnf, &var->d1ConfsList, itrCC1, GetD1ArrFromCC);
								  if (itrCC1 != newLastCC1B)
								  {
								  (*litsNumCC1)--;
								  InsCCIntoLstBeg(cnf, &var->d1ConfsList, itrCC1, GetD1ArrFromCC);
								  (*litsNumCC1)++;
								  } else
								  {
								  var->thisImplD1Box--;
								  itrCC1->nextClsIndOrD1ArrOrd = -1;
								  }
								  }
								  }
								  
									//freeing CC2
									free(litsNumCC2);
									
									  //finally setting the new number of vars
									  (*litsNumCC1)--;
									  
										if ((*(litsNumCC1)) > 1)
										{
										InsToEndOfConfLst(cnf, newLastCC1B);
										//set "delete cls arrows", if needed
										if (newLastCC1B->ifDelCls)
										InsCCIntoLstBeg(cnf, &cnf->vars[labs((newLastCC1B-cnf->maxClsLenToRec)->litNum)].clssToDel, newLastCC1B, GetDelCls);
										
										  //set "delete used conf. arrows" into appropriate list
										  
											//changing the placement of usedConfCC1 if needed
											if (usedConfCC1->ordFromUp0Ind != 0)
											{
											usedConfCC1->ifUsedConfsDelHead = 0;
											usedConfCC1--;
											usedConfCC1->ifUsedConfsDelHead = 1;
											cnf->vars[labs(usedConfCC1->litNum)].thisImplCCBox++;
											}
											//if (newLastCC1B->ccType != CC_TYPE_CLS_ONLY)
											InsCCIntoLstBeg(cnf, &cnf->vars[labs(usedConfCC1->litNum)].usedConfToDel, usedConfCC1, GetDelUsedConf);
											
											  //update 2nd from bottom var list to ensure that 2 bottommost CCBox's
											  //will be added appropriate watched box lists
											  InsCCIntoLstBeg(cnf, &cnf->vars[labs((newLastCC1B-1)->litNum)].bot2CC, newLastCC1B-1, Bot2FromCcBox);
											  if (newLastCC1B->ifPlcForEmD1Ord)
											  *(D1EmergOrd(newLastCC1B)) = -1;
											  
												UpdateAddImplStInfo(cnf, newLastCC1B, newLastCC1B->litNum);
												} else
												{
												UpdateAddImplStInfo(cnf, 0, newLastCC1B->litNum);
												free(litsNumCC1);
												}
												cnf->actualCCNum--;
												
												  #ifdef EXT2_PRINT
												  if (TC(cnf, "EXT2_PRINT"))
												  {
												  PrintCCGivenCCBox(cnf, newLastCC1B, TRUE, TRUE);
												  OutputTrace(cnf, "\nChkAndTreat1ConfEachSideForD2", 147, "", buff);
												  }
												  #endif	
												  
													}
*/
void InsIntoWListsOnBot2Unimply(Cnf* cnf, long litUnimpl)
{
	Var* var;
	CCBox *bot2Ccb, *bot2CcbNext;
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nInsIntoWListsOnBot2Unimply of %ld", litUnimpl);
	}
#endif
	var = &cnf->vars[labs(litUnimpl)];
	
	for (bot2Ccb = var->bot2CC; bot2Ccb; bot2Ccb = bot2CcbNext)
	{
		bot2CcbNext = Bot2FromCcBox(bot2Ccb)->next;
		
		bot2Ccb->nextClsIndOrD1ArrOrd = bot2Ccb->ordFromUp0Ind-1;
		(bot2Ccb+1)->nextClsIndOrD1ArrOrd = bot2Ccb->ordFromUp0Ind;
		//		(bot2Ccb+1)->ifOnWchList = TRUE;
		bot2Ccb->ifNextClsBoxInd = (bot2Ccb+1)->ifNextClsBoxInd = 1;
		InsWCCBoxIntoVarsWList(cnf, bot2Ccb, bot2Ccb+1);
		InsWCCBoxIntoVarsWList(cnf, bot2Ccb+1, bot2Ccb+1);
#ifdef DEL_PRINT
		if (TC(cnf, "DEL_PRINT"))
		{
			PrintCCGivenCCBox(cnf, bot2Ccb+1, TRUE, FALSE);
		}
#endif
	}
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		OutputTrace(cnf, "\nInsIntoWListsOnBot2Unimply", 39, "", buff);
	}
#endif
	
	var->bot2CC = 0;
}

void DelClssFromList(Cnf* cnf, long litNum)
{
	Var* var;
	CCBox *ccb, *downBox, *ccArr;
#ifdef DEL_PRINT
	long i;
	
#endif
	var = &cnf->vars[labs(litNum)];
	
	ccb = var->clssToDel; 
	
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		var = &cnf->vars[labs(litNum)];
		//GoThrowList(&var->usedConfToDel, GetDelUsedConf);
//		GoThrowList(&var->clssToDel, GetDelCls);
//		GoThrowList(&var->remFromAIS, RfaisFromCcBox);
//		GoThrowList(&var->bot2CC, Bot2FromCcBox);
		//	GoThrowD1ConfsList(cnf, var->varNum, var->d1ConfsList);
//		GoThrowWathedList(cnf, var->varNum, var->watchedPos);
//		GoThrowWathedList(cnf, -(long)var->varNum, var->watchedNeg);
//		GoThrowList(&cnf->lastCCBOfFirstCCAdded, GetAddedCCList);
		
		buffInd+=sprintf(buff+buffInd, "\nDelClssFromList of %ld", litNum);
	}
#endif
	
	while (ccb)
	{
		downBox = CCB_DOWN_FROM_CC(ccb);
		ccArr = CLS_CCBOXES_BEG(ccb);
#ifdef DEL_PRINT
		if (TC(cnf, "DEL_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nBefore");
			PrintCCGivenCCBox(cnf, downBox, TRUE, FALSE);
			OutputTrace(cnf, "\nDelClssFromList", 35, "", buff);
//			for (i = 1; i <= cnf->initVarsNum; i++)
//			{
//				GoThrowWathedList(cnf, -(long)i, cnf->vars[i].watchedNeg);
//				GoThrowWathedList(cnf, i, cnf->vars[i].watchedPos);
//			}
		}
#endif
		if (downBox->ifBot2)
		{
			RemWCCBoxFromVarsWList(cnf, ccb, ccb);
			RemWCCBoxFromVarsWList(cnf, &ccArr[ccb->nextClsIndOrD1ArrOrd], ccb);
		}
		RemCCFromLst(cnf, &var->clssToDel, ccb, GetDelCls);
#ifdef DEL_PRINT
		if (TC(cnf, "DEL_PRINT"))
		{
		/*		buffInd+=sprintf(buff+buffInd, "\nInbetween");
		PrintCCGivenCCBox(cnf, downBox, TRUE, FALSE);
			OutputTrace(cnf, "\nDelClssFromList", 61, "", buff);*/
		}
#endif
		if (downBox->ccType == CC_TYPE_CONF_CLS)
		{
			downBox->ccType = CC_TYPE_CONF_ONLY;
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\nAfter");
				PrintCCGivenCCBox(cnf, downBox, TRUE, FALSE);
			}
#endif
			
			//downBox->ifWatchedArrowsDataFields = TRUE;
		} else
		{
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				//			buffInd+=sprintf(buff+buffInd, "\nBefore destroying %#x", CC_LITS_NUM(ccb));
				//			OutputTrace(cnf, "\nDelClssFromList", 89, "", buff);
			}
#endif
			
			free(CC_LITS_NUM(ccb));
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\nDestroyed");
				OutputTrace(cnf, "\nDelClssFromList", 95, "", buff);
			}
#endif
		}
		
		ccb = var->clssToDel; 
	}	
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		OutputTrace(cnf, "\nDelClssFromList", 61, "", buff);
	}
#endif
	
}

void DelUsedConfsFromList(Cnf* cnf, long litNum)
{
	Var* var;
	CCBox *ccb, *ccbWithData;
#ifdef DEL_PRINT
		long i;
	
#endif
	
	var = &cnf->vars[labs(litNum)];
	
	ccb = var->usedConfToDel; 
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nDelUsedConfsFromList of %ld", litNum);
		OutputTrace(cnf, "\nDelUsedConfsFromList", 117, "", buff);
	}
#endif
	
	while (ccb)
	{
		ccbWithData = CCB_DOWN_FROM_CC(ccb);
#ifdef DEL_PRINT
		if (TC(cnf, "DEL_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nBefore");
			PrintCCGivenCCBox(cnf, ccbWithData, TRUE, FALSE);
			OutputTrace(cnf, "\nDelUsedConfsFromList", 117, "", buff);
		}
#endif
#ifdef	DEL_CONFS_PRINT
		if (TC(cnf, "DEL_CONFS_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "\nDEL_CONF: ");
			PrintCCGivenCCBox(cnf, ccbWithData, TRUE, FALSE);
			OutputTrace(cnf, "\nDelUsedConfsFromList", 117, "", buff);
		}
#endif
		//removing current ccBox from var->usedConfToDel
		RemCCFromLst(cnf, &var->usedConfToDel, ccb, GetDelUsedConf);
		
		//removing appropriate box from cnf->ccList
		RemFromConfLst(cnf, ccbWithData);
		
		if (ccbWithData->ccType == CC_TYPE_CONF_CLS)
		{
			ccbWithData->ccType = CC_TYPE_CLS_ONLY;
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\nAfter");
				PrintCCGivenCCBox(cnf, ccbWithData, TRUE, FALSE);
				OutputTrace(cnf, "\nDelUsedConfsFromList", 117, "", buff);
			}
#endif
		}
		else
		{
			free(CC_LITS_NUM(ccb));
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, "\nDestroyed");
			}
#endif
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
//				for (i = 1; i <= cnf->initVarsNum; i++)
//				{
//					GoThrowWathedList(cnf, -(long)i, cnf->vars[i].watchedNeg);
//					GoThrowWathedList(cnf, i, cnf->vars[i].watchedPos);
//				}
			}
#endif
		}
		
		ccb = var->usedConfToDel; 
	}
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		OutputTrace(cnf, "\nDelUsedConfsFromList", 109, "", buff);
	}
#endif
	
}

void DelConfFromAddedCCList(Cnf* cnf, CCBox* lastCCBOfCC, long implLvlMmTill)
{
	CCBox *frstCCBox, *ccb, *oldCCBeg, *newCCBeg, *oldWDown, *oldWUp, *oldDelCls;
	CCBox* *oldWDownLst, *oldWUpLst, *oldDelClsLst, *frstArrowFromCC;
	Var* var;
	Boolean ifBefUsedConfHead;
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nDelConfFromAddedCCList - %#x, MM till %ld", lastCCBOfCC, implLvlMmTill);
		PrintCCGivenCCBox(cnf, lastCCBOfCC, TRUE, FALSE);
		OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
	}
#endif
#ifdef DEL_CONFS_PRINT
	if (TC(cnf, "DEL_CONFS_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nDEL_CONF,DelConfFromAddedCCList - %#x, MM till %ld", lastCCBOfCC, implLvlMmTill);
		PrintCCGivenCCBox(cnf, lastCCBOfCC, TRUE, FALSE);
		OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
	}
#endif

	RemFromConfLst(cnf, lastCCBOfCC);
	
	frstCCBox = CLS_CCBOXES_BEG(lastCCBOfCC);
	
	frstArrowFromCC = FRST_ARROW_FROM_CC(frstCCBox);
	
	for (ifBefUsedConfHead = TRUE, ccb = frstCCBox; ((ccb != frstArrowFromCC)&&(ON(cnf->vars[labs(ccb->litNum)].implStat,IMPLIED))&&(cnf->vars[labs(ccb->litNum)].implLevel < implLvlMmTill)); ccb++)
	{
		var = &cnf->vars[labs(ccb->litNum)];
		
		if (ccb->ifUsedConfsDelHead)
		{
			RemCCFromLst(cnf, &var->usedConfToDel, ccb, GetDelUsedConf);
			ifBefUsedConfHead = FALSE;
		}
		
		var->thisImplApp--;
		if (!ifBefUsedConfHead)
		{
			var->thisImplCCBox--;
		}
		if ((!ccb->ifNextClsBoxInd)&&(ccb->nextClsIndOrD1ArrOrd != -1))
		{
			//			PrintRecConfsOfVar(cnf, var);
			var->thisImplD1Box--;
			RemCCFromLst(cnf, &var->d1ConfsList, ccb, GetD1ArrFromCC);
		}
	}
	
	while (ifBefUsedConfHead)
	{
		var = &cnf->vars[labs(ccb->litNum)];
		if (ccb->ifUsedConfsDelHead)
		{
			RemCCFromLst(cnf, &var->usedConfToDel, ccb, GetDelUsedConf);
			ifBefUsedConfHead = FALSE;
		}
		ccb++;
	}
	
	if (lastCCBOfCC->ccType == CC_TYPE_CONF_CLS)
	{
		lastCCBOfCC->ccType = CC_TYPE_CLS_ONLY;
		
#ifdef EXT3_PRINT
		if (TC(cnf, "EXT3_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "After lastCCBOfCC->ccType = CC_TYPE_CLS_ONLY;\n");
			OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
		}
#endif
		
		/*if (cnf->heur&32)
		{
		oldCCBeg = CC_LITS_NUM(lastCCBOfCC);
		oldWUpLst = ( ((lastCCBOfCC)->litNum > 0) ? (&cnf->vars[labs((lastCCBOfCC)->litNum)].watchedPos) : (&cnf->vars[labs((lastCCBOfCC)->litNum)].watchedNeg) );
		oldWDownLst = ( ((lastCCBOfCC)->litNum > 0) ? (&cnf->vars[labs((lastCCBOfCC)->litNum)].watchedPos) : (&cnf->vars[labs((lastCCBOfCC)->litNum)].watchedNeg) );
		oldDelClsLst = &cnf->vars[labs(oldCCBeg[(*(CC_LITS_NUM(oldCCBeg)))-cnf->maxClsLenToRec-1].litNum)].clssToDel;
		newCCBeg = realloc(oldCCBeg, 2*sizeof(unsigned long)+(*((unsigned long*)oldCCBeg))*sizeof(CCBox)+(2+lastCCBOfCC->ifDelClsDataField)*sizeof(CCArrowsBox));
		if (oldCCBeg != newCCBeg)
		{
		//GO ON HERE!!!	
		//var = 
		}
	}*/
		//PrintCCGivenCCBox(cnf, lastCCBOfCC, TRUE, FALSE);
	} else
	{
		free(CC_LITS_NUM(lastCCBOfCC));
	}
#ifdef EXT3_PRINT
	if (TC(cnf, "EXT3_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nEnd of DelConfFromAddedCCList");
		OutputTrace(cnf, "\nDelConfFromAddedCCList", 117, "", buff);
	}
#endif
}

void DelCCsNotIncAISLeave(Cnf* cnf, CCBox* ccbFrom, CCBox* ccbTill)
{
	CCBox *lastCCB, *lastCCBNext;
	Var* var;
	long implLvlOfTopDecVar;
	
	if (ccbFrom == ccbTill)
		return;
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, "\nDelCCsNotIncAISLeave from %#x to %#x : ", ccbFrom, ccbTill);
		//	OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 234, "", buff);
		//if (ccbFrom)
		//	PrintCCGivenCCBox(cnf, ccbFrom, FALSE, FALSE);
		//	OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 234, "", buff);
	}
#endif
	
	implLvlOfTopDecVar = cnf->vars[labs(cnf->ivs->bs[cnf->ivs->decHead].litNum)].implLevel;
	
	for (lastCCB = ((ccbFrom) ? (GetAddedCCList(ccbFrom)->next) : (cnf->lastCCBOfFirstCCAdded)); lastCCB != ccbTill; lastCCB = lastCCBNext)
	{
#ifdef DEL_PRINT
		if (TC(cnf, "DEL_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, "%#x", lastCCB);
			//		OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 243, "", buff);
		}
#endif
		lastCCBNext = GetAddedCCList(lastCCB)->next;
		
		var = &cnf->vars[labs(lastCCB->litNum)];
		
		if (!( (lastCCB == cnf->lastCCBoxOfCCNotToDel) || 
			/*(var->aisLastCCBox->clsFromConfStat) ||*/
			(( (ON(var->implStat,IMPL_ADD_IMPL_ST)) 
			|| 
			( (ON(var->implStat,IMPL_REG_RES)) && (var->aisStatus == implIncons) )
			)
			&&
			(var->implLevel>implLvlOfTopDecVar)
			&&
			(var->aisLastCCBox == lastCCB)
			&&
			(var->implLevel <= cnf->implLvlToSkipTill)
			&&
			(var->thisImplApp))
			) )
		{
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, " DL");
				//			OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 266, "", buff);
			}
#endif
			DelConfFromAddedCCList(cnf, lastCCB, implLvlOfTopDecVar); 	
#ifdef DEL_PRINT
			if (TC(cnf, "DEL_PRINT"))
			{
				buffInd+=sprintf(buff+buffInd, " After DL");
				//			OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 266, "", buff);
			}
#endif
			
		}
#ifdef DEL_PRINT
		if (TC(cnf, "DEL_PRINT"))
		{
			buffInd+=sprintf(buff+buffInd, " LV,");
			//			OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 274, "", buff);
			
		}
#endif
	}
#ifdef DEL_PRINT
	if (TC(cnf, "DEL_PRINT"))
	{
		buffInd+=sprintf(buff+buffInd, " End of DelCCsNotIncAISLeave function");
		OutputTrace(cnf, "\nDelCCsNotIncAISLeave", 161, "", buff);
	}
#endif
}
