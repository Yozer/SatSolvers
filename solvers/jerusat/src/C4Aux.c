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
#include <memory.h>
#include <string.h>
#ifdef WIN32
#include <crtdbg.h>
#endif
#include "C4Globals.h"
#ifdef WIN32
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
#ifdef FBGNEWRESTART
#include "C4FBG.h"
#endif

#define EXT2_PRINT_COND 0//((cnf->currCCNum >= 49228))//&&(cnf->actualCCNum <= 13022))
#define PRINT_AIS_COND 0//((cnf->currCCNum >= 49228))//&&(cnf->actualCCNum <= 13022))
#define EXT3_PRINT_COND 0
#define PRINT_IVS_COND 0//((cnf->currCCNum >= 49228))//&&(cnf->actualCCNum <= 13022))
#define PQ_PRINT_COND 0
#define THROW_WL_PRINT_COND 0
#define DEL_PRINT_COND 0//((cnf->currCCNum >= 49228))//&&(cnf->actualCCNum <= 13022))
#define IMPLY_PRINT_COND 0//((cnf->currCCNum >= 49228))//&&(cnf->actualCCNum <= 13022))
#define PRINT_LAYERS_COND 0
#define RESTART_CLS_PRINT_COND 0
#define PRINT_MIDDLE_IVS_COND 0
#define DEL_CONFS_PRINT_COND 0//((cnf->currCCNum >= 49228))
Boolean TC(Cnf* cnf, char* pc)
{
	if (!strcmp(pc, "PRINT_IVS"))
		return ((PRINT_IVS_COND) ? (1) : (0));
	else if (!strcmp(pc, "EXT2_PRINT"))
		return ((EXT2_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "PRINT_AIS"))
		return ((PRINT_AIS_COND) ? (1) : (0));
	else if (!strcmp(pc, "EXT3_PRINT"))
		return ((EXT3_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "PQ_PRINT"))
		return ((PQ_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "THROW_WL_PRINT"))
		return ((THROW_WL_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "DEL_PRINT"))
		return ((DEL_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "IMPLY_PRINT"))
		return ((IMPLY_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "PRINT_LAYERS"))
		return ((PRINT_LAYERS_COND) ? (1) : (0));
	else if (!strcmp(pc, "RESTART_CLS_PRINT"))
		return ((RESTART_CLS_PRINT_COND) ? (1) : (0));
	else if (!strcmp(pc, "PRINT_MIDDLE_IVS"))
		return ((PRINT_MIDDLE_IVS_COND) ? (1) : (0));
	else if (!strcmp(pc, "DEL_CONFS_PRINT"))
		return ((DEL_CONFS_PRINT_COND) ? (1) : (0));
	return 1;
}

//This function returns a random number from 0 to n-1
long RandFrom0ToNMin1(long n)
{
	long res;
	
	res = ldiv(rand(), ldiv((RAND_MAX-0+1),n).quot).quot;
	if (res > n-1) 
		res = n-1;
	return res;
}

char GetStatLetter(Cnf* cnf, long litNum, unsigned long implStat)
{
	char stat;
	
	if ON(implStat,IMPL_REG_RES)
		stat = 'R';
	if ON(implStat,IMPL_ADD_IMPL_ST)
		stat = 'A';
	if ON(implStat,IMPL_D1)
		stat = 'F';
	if ON(implStat,IMPL_D2)
		stat = 'S';
	if ON(implStat,IMPL_CONF)
		stat = 'C';
	if (implStat == NOT_IMPLIED)
		stat = 'N';
	else
		if ON(implStat,NOT_IMPLIED)
			stat = 'T'; 
		
		return stat;
}

char GetAisLetter(Cnf* cnf, long litNum, int aisStat)
{
	char stat;
	switch (aisStat)
	{
	case notImplOut: stat = 'o'; break;
	case notImplIn: stat = 'i'; break;
	case confPartNotAdd: stat = 'N'; break;
	case confPartAddPos: stat = '+'; break;
	case confPartAddNeg: stat = '-'; break;
	case addImplImpl: stat = 'A'; break;
	case addImplChngSide: stat = 'S'; break;
	case implCons: stat = 'C'; break;
	case implIncons: stat = 'I'; break;
	}
	
	return stat;
}

void PrintAis(Cnf* cnf)
{
#ifdef PRINT_AIS
	LLBox* itr;
	Var *varIn;
	
	if (TC(cnf, "PRINT_AIS"))
	{
		
		buffInd = sprintf(buff, "\nAIS : ");
		
		for (itr = cnf->ais; itr; itr = itr->next)
		{
			varIn = &cnf->vars[labs(itr->litNum)];
			
			if (cnf->aisPtr == itr)
				buffInd+=sprintf(buff+buffInd, "@");
			buffInd+=sprintf(buff+buffInd, "%ld(%ld),", itr->litNum, ((varIn->aisLastCCBox)?((varIn->aisLastCCBox-1)->litNum):(0))); fflush(outFile);
		}
		
		OutputTrace(cnf, "\nPrintAis", 56, "", buff);
	}
#endif
}

void PrintConfToBeChecked(Cnf* cnf, Var* var)
{
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
	{
		CCBox* cb;
		buffInd = sprintf(buff, "\nPrintConfToBeChecked : prep.conf.: var %ld, c. blk.:%ld, conf.:%#x:{", var->varNum, var->d2NumOfCurrChkConf, var->d2ConfToBeChecked);
		PrintCCGivenCCBox(cnf, var->d2ConfToBeChecked, TRUE, FALSE);
		OutputTrace(cnf, "\nPrintConfToBeChecked", 88, "", buff);
	}
#endif
}
void PrintRecConfsOfVar(Cnf* cnf, Var* var)
{
#ifdef EXT2_PRINT
	CCBox* ccb;
	long i;
	
	if (TC(cnf, "EXT2_PRINT"))
	{
		
		buffInd = sprintf(buff, "\nPrintRecConfsOfVar : var %ld", var->varNum);
		
		for (i=0,ccb = var->d1ConfsList; ccb; ccb = GetD1ArrFromCC(ccb)->next,i++)
			PrintCCGivenCCBox(cnf, ccb, TRUE, FALSE);
		
		OutputTrace(cnf, "\nPrintRecConfsOfVar", 104, "", buff);
	}
#endif
}

void PrintCCGivenCCBox(Cnf* cnf, CCBox* cc, Boolean contPrint, Boolean noWchChk)
{
	//#ifdef PRINT_IVS
	CCBox *ccbWithInfo, *wUp, *litsArr;
	unsigned long ccType, d1ArrOrd, d1ArrNum;
	unsigned long sortedFrom;
	unsigned long* litsNum;
	long i, litNum;
	Var* var;
	Boolean ifSorted, ifWAD, ifDCD;
	CCArrowsBox* arrBox;
	char stat;
#ifndef SAT2003	
	if (!contPrint)
		buffInd = sprintf(buff, "\n");
	else
		buffInd+=sprintf(buff+buffInd, "\n");
	
	ccbWithInfo = CCB_DOWN_FROM_CC(cc);
	ccType = ccbWithInfo->ccType;
	
	if (ccbWithInfo->ifAdd1UIP == 1)
		buffInd+=sprintf(buff+buffInd, "1UIP");
	else
	{	
		switch (ccType) 
		{
		case CC_TYPE_CLS_ONLY : buffInd+=sprintf(buff+buffInd, "CLS");break;
		case CC_TYPE_CONF_ONLY : buffInd+=sprintf(buff+buffInd, "CONF");break;
		case CC_TYPE_CONF_CLS : buffInd+=sprintf(buff+buffInd, "CC");break;
		case CC_TYPE_INIT_CLS : buffInd+=sprintf(buff+buffInd, "CINIT");break;
		}
	}
	
	
	litsNum = CC_LITS_NUM(cc);
	buffInd+=sprintf(buff+buffInd, ":(%#x,%#x)", litsNum, ccbWithInfo);
	buffInd+=sprintf(buff+buffInd, "(%lu lits)", *litsNum);
	
	if (ifSorted = ccbWithInfo->ifSorted)
	{
		sortedFrom = *(CC_SORTED_FROM(cc));
		buffInd+=sprintf(buff+buffInd, "(S+%lu) : ", sortedFrom); 
	}
	else
		buffInd+=sprintf(buff+buffInd, "(S-) : "); 
	
	litsArr = CLS_CCBOXES_BEG(cc);
	
	for (d1ArrNum = 0, i = 0; &litsArr[i-1] != ccbWithInfo; i++)
	{
		if (i != litsArr[i].ordFromUp0Ind)
		{
			buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : ordFromUp0Ind");
			_ASSERT(0);
		}
		
		litNum = litsArr[i].litNum;
		var = &cnf->vars[labs(litNum)];		
		stat = GetStatLetter(cnf, litNum, var->implStat);
#ifndef FBGNEWRESTART		
		if (stat != 'N')
			buffInd+=sprintf(buff+buffInd, "%ld%c%ld,%lu", litNum, stat, var->cpImplLevel, var->lastCCBefImplNum);
		else
			buffInd+=sprintf(buff+buffInd, "%ld%c", litNum, stat);
		
		if ( (ifSorted) && (var->lastCCBefImplNum <= sortedFrom) )
		{
			buffInd+=sprintf(buff+buffInd, "+");
		}
#else
		buffInd+=sprintf(buff+buffInd, "%ld%c", litNum, ((litNum==fbgNewRest[labs(litNum)]) ? ('!') : ('~')));
#endif
		d1ArrOrd = litsArr[i].nextClsIndOrD1ArrOrd;
		if (!litsArr[i].ifNextClsBoxInd)
		{
			if (d1ArrOrd != -1)
			{
				//ALT+430
				d1ArrNum = d1ArrOrd+1;
				buffInd+=sprintf(buff+buffInd, "|D%ld", d1ArrOrd);
			}
		} else
		{
			buffInd+=sprintf(buff+buffInd, "|CL%d", (long)d1ArrOrd);
		}
		((&litsArr[i] != ccbWithInfo) ? (buffInd+=sprintf(buff+buffInd, ",,")) : (buffInd+=sprintf(buff+buffInd, ";")));
	}
	
	//	ifWAD = ccbWithInfo->ifWatchedArrowsDataFields;
	ifDCD = ccbWithInfo->ifDelCls;
	
	arrBox = &litsArr[i];
	arrBox+=2;
	/*
	if ((noWchChk)&&(ifWAD))
	{
	arrBox+=2;
	}  else
	{
	if (ifWAD)
	{
	buffInd+=sprintf(buff+buffInd, "W:");
	if ((arrBox->prev)&&(arrBox->prev->litNum != wUp->litNum))
	{
				buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : wUp->prev");
				}
				if ((arrBox->next)&&(arrBox->next->litNum != wUp->litNum))
				{
				buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : wUp->next");
				}
				
				  buffInd+=sprintf(buff+buffInd, "«%#x,%#x»,", arrBox->prev, arrBox->next);
				  
					arrBox++;
					
					  if ((arrBox->prev)&&(arrBox->prev->litNum != ccbWithInfo->litNum))
					  {
					  buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : ccbWithInfo->prev");
					  }
					  if ((arrBox->next)&&(arrBox->next->litNum != ccbWithInfo->litNum))
					  {
					  buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : ccbWithInfo->next");
					  }
					  
						buffInd+=sprintf(buff+buffInd, "«%#x,%#x»,", arrBox->prev, arrBox->next);
						
						  arrBox++;
						  }
						  }
	*/
	if (ifDCD)
	{
		if (*litsNum <= cnf->maxClsLenToRec)
			buffInd+=sprintf(buff+buffInd, "DC:%ld", litsArr[*litsNum-cnf->maxClsLenToRec-1].litNum);
		else
			buffInd+=sprintf(buff+buffInd, "DC:%ld", 0);
	}
	arrBox++;
	
	if ((ccType == CC_TYPE_CONF_CLS)||(ccType == CC_TYPE_CONF_ONLY))
	{
		buffInd+=sprintf(buff+buffInd, "DUsC:");
		/*if ((arrBox->prev)&&(labs(arrBox->prev->litNum) != labs(litsArr->litNum)))
		{
		buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : Delete used confs list -> prev");
		}
		if ((arrBox->next)&&(labs(arrBox->next->litNum) != labs(litsArr->litNum)))
		{
		buffInd+=sprintf(buff+buffInd, "INCONSISTENCY : Delete used confs list -> next");
	}*/
		
		buffInd+=sprintf(buff+buffInd, "«%#x,%#x»,", arrBox->prev, arrBox->next);
		
		arrBox++;
		
		buffInd+=sprintf(buff+buffInd, "ACC:");		
		buffInd+=sprintf(buff+buffInd, "«%#x,%#x»,", arrBox->prev, arrBox->next);
		
		arrBox++;	
	}
	
	if (ccbWithInfo->ccType != CC_TYPE_INIT_CLS)
		buffInd+=sprintf(buff+buffInd, "D1EM - %d", *((long*)arrBox));
	else
		buffInd+=sprintf(buff+buffInd, "NO D1EM");
	
	if (!contPrint)
		OutputTrace(cnf, "PrintCC", 220, "", buff);
	
#endif
}

void PrintLayers(Cnf* cnf)
{
	LLBox* llb;
	long lyr;
	
	buffInd += sprintf(buff+buffInd, "\n");
	
	for (lyr = cnf->lastLayerPerCpNum[cnf->ivs->cpNum]; lyr; lyr--)
	{
		if (cnf->layersArr[lyr])
		{
			buffInd += sprintf(buff+buffInd, "%ld : ", lyr);
			for (llb = cnf->layersArr[lyr]; llb; llb = llb->next)
			{
				buffInd += sprintf(buff+buffInd, "%ld, ", llb->litNum);
			}
		}
	}
	
	OutputTrace(cnf, "\nPrintLayers", 289, "", buff);
}




void OutputTrace(Cnf* cnf, char* proc, int line, char* module, char* buff)
{
	long ind, prevInd;
	char chSv;
	Boolean goOut;
	
	//if (cnf->actualCCNum >= 4633)
	{
		
		for (goOut = FALSE, prevInd = 0, ind = min(buffInd,BUFF_PRINT_PORTION); !goOut; prevInd = ind, ind+=min(buffInd-ind,BUFF_PRINT_PORTION))
		{
			chSv = buff[ind];
			buff[ind] = '\0';
			fprintf(outFile, "%s", buff+prevInd);fflush(outFile);
			buff[ind] = chSv;
			
			if (ind == buffInd)
				goOut = TRUE;
#ifdef _DEBUG
			if ((prevInd == 0)&&(buff[0] == '\n'))
				_CrtDbgReport(_CRT_WARN, proc, line, module, buff+1);
			else
				_CrtDbgReport(_CRT_WARN, proc, line, module, buff+prevInd);
#endif
			
		}
	}
	buffInd = 0;
}


void GoThrowAllLists(Cnf* cnf)
{
#ifndef NOT_GO_THROW_LISTS
#ifdef _DEBUG
	long i;
	Var* var;
	
	for (i=1; i<=cnf->initVarsNum; i++)
	{
		var = &cnf->vars[i];
		GoThrowList(&var->usedConfToDel, GetDelUsedConf);
		GoThrowList(&var->clssToDel, GetDelCls);
		GoThrowList(&var->remFromAIS, RfaisFromCcBox);
		GoThrowList(&var->bot2CC, Bot2FromCcBox);
		//		GoThrowD1ConfsList(cnf, var->varNum, var->d1ConfsList);
		GoThrowWathedList(cnf, var->varNum, var->watchedPos);
		GoThrowWathedList(cnf, -(long)var->varNum, var->watchedNeg);	
	}
	
	GoThrowList(&cnf->lastCCBOfFirstCCAdded, GetAddedCCList);
#endif
#endif
}

void GoThrowList(CCBox** lstBeg, CCArrowsBox*(*GetArrFromCC)(CCBox* ccb))
{
	CCBox *ccb, *ccbPrev;
	
	for (ccbPrev = 0, ccb = *lstBeg; ccb; ccb = GetArrFromCC(ccb)->next)
	{
		if (ccbPrev)
		{
#ifdef WIN32
			_ASSERT(GetArrFromCC(ccbPrev)->next == ccb);		
#endif
		}
#ifdef WIN32
		_ASSERT(GetArrFromCC(ccb)->prev == ccbPrev);		
#endif
		ccbPrev = ccb;	
	}
}

void GoThrowD1ConfsList(Cnf* cnf, long varNum, CCBox* d1ConfsList)
{
	CCBox *ccb, *ccbPrev;
	long ccNum;
	Var* var;
	
	for (ccNum = 0, ccbPrev = 0, ccb = d1ConfsList; ccb; ccb = GetD1ArrFromCC(ccb)->next, ccNum++)
	{
		if (ccbPrev)
		{
#ifdef WIN32
			_ASSERT(GetD1ArrFromCC(ccbPrev)->next == ccb);		
#endif
		}
		
#ifdef WIN32
		_ASSERT(GetD1ArrFromCC(ccb)->prev == ccbPrev);		
#endif		
		ccbPrev = ccb;	
	}
	
	var = &cnf->vars[labs(varNum)];
	
	/*	if (var->thisImplApp == var->thisImplD1Box)
	_ASSERT(var->thisImplD1Box == ccNum);*/
}

void GoThrowWathedList(Cnf* cnf, long litNum, CCBox* frstBox)
{
	CCBox *ccb, *secwCCB, *horizCCB, *ccbWithData, *clsCCBoxes, *wccbOfCurrVar, *nextHorizCCB;
	unsigned long litsNum, i;
	long currCcbInd;
	
	for (horizCCB = frstBox; horizCCB; horizCCB = nextHorizCCB)
	{
#ifdef THROW_WL_PRINT
		if (TC(cnf, "THROW_WL_PRINT"))
		{
			PrintCCGivenCCBox(cnf, horizCCB, FALSE, FALSE);
			OutputTrace(cnf, "\nGoThrowWathedList", 319, "", buff);
		}
#endif
		ccbWithData = CCB_DOWN_FROM_CC(horizCCB);
		clsCCBoxes = CLS_CCBOXES_BEG(horizCCB);
		litsNum = *(CC_LITS_NUM(horizCCB));
		if (horizCCB->litNum == litNum)
		{
			wccbOfCurrVar = horizCCB;
			secwCCB = &clsCCBoxes[horizCCB->nextClsIndOrD1ArrOrd];
		} else
		{
			if (clsCCBoxes[horizCCB->nextClsIndOrD1ArrOrd].litNum == litNum)
			{
				wccbOfCurrVar = &clsCCBoxes[horizCCB->nextClsIndOrD1ArrOrd];
				secwCCB = horizCCB;
			} else
			{
#ifdef WIN32
				_ASSERT(0);
#endif
			}
		}
		nextHorizCCB = (WchFromCcBox(wccbOfCurrVar,horizCCB))->next;
		
		for (i = 1, currCcbInd = horizCCB->ordFromUp0Ind; i <= litsNum; i++, currCcbInd = ((ccb->ifNextClsBoxInd)?(ccb->nextClsIndOrD1ArrOrd):(currCcbInd-1)))
		{
#ifdef WIN32
			_ASSERT(currCcbInd != -1);
#endif
			ccb = &clsCCBoxes[currCcbInd];
			
		}
#ifdef WIN32
		_ASSERT(currCcbInd == -1);
#endif
	}
}

Boolean CheckModel(Cnf* cnf, long* model)
{
	CCBox* ccb;
	unsigned long* ccLitNum; 	
	long clsNum;
	
	for (clsNum = 0, ccLitNum = cnf->initCCBuff; ccLitNum != (((char*)cnf->initCCBuff)+cnf->sizeOfBuffForCnf); ccLitNum = (((char*)(ccLitNum+2))+(*ccLitNum)*sizeof(CCBox)+2*sizeof(CCArrowsBox)), clsNum++ )
	{
		for (ccb = ccLitNum+2; ccb != (((CCBox*)(ccLitNum+2))+*ccLitNum); ccb++)
		{
			if (ccb->litNum == model[labs(ccb->litNum)])
				break;
#ifdef WIN32
			_ASSERT(labs(ccb->litNum) == labs(model[labs(ccb->litNum)]));
#endif
		}
		
		if ( ccb == (((CCBox*)(ccLitNum+2))+*ccLitNum))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}
