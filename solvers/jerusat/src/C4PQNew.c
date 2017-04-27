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
#include <math.h>
#include <string.h>
#include <limits.h>
#include "C4PQ.h"
#ifdef WIN32
#include <windows.h>
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
void IncrHeur(Cnf* cnf, long litNum, int value)
{
	Var* var;

	var = &cnf->vars[labs(litNum)];

	if (var->prevPowOf2ToDiv != cnf->powOf2ToDiv)
	{
		if (cnf->powOf2ToDiv - var->prevPowOf2ToDiv >= 31)
		{
			var->overallApp[POS_APP] = var->overallApp[NEG_APP]	= 0.0;
		} else
		{
			var->overallApp[POS_APP]/=(double)(((unsigned long)1)<<(cnf->powOf2ToDiv - var->prevPowOf2ToDiv));
			var->overallApp[NEG_APP]/=(double)(((unsigned long)1)<<(cnf->powOf2ToDiv - var->prevPowOf2ToDiv));
		}
		var->prevPowOf2ToDiv = cnf->powOf2ToDiv;
	}
	((litNum>0) ? (var->overallApp[POS_APP]+=value) : (var->overallApp[NEG_APP]+=value));
}

void ReduceInitHeurToMax(Cnf* cnf, long litNum, double maxVal)
{
	Var* var;
	double mx, mn;
	double *appPos, *appNeg;

	var = &cnf->vars[labs(litNum)];

	appPos = var->overallApp + POS_APP;
	appNeg = var->overallApp + NEG_APP;

	mx = max(*appPos, *appNeg);
	mn = min(*appPos, *appNeg);
	

	if (2*(cnf->heurPartOfS1*mx+(1-cnf->heurPartOfS1)*mn) > maxVal)
	{
		*appPos *= (maxVal/(2*(cnf->heurPartOfS1*mx+(1-cnf->heurPartOfS1)*mn)));
		*appNeg *= (maxVal/(2*(cnf->heurPartOfS1*mx+(1-cnf->heurPartOfS1)*mn)));
	}
}

double GetLitsVal(Cnf* cnf, long litNum)
{
	Var* var;
	double mx, mn;
	double *appPos, *appNeg;
	
	var = &cnf->vars[labs(litNum)];

	appPos = var->overallApp + POS_APP;
	appNeg = var->overallApp + NEG_APP;
	
	if (var->prevPowOf2ToDiv != cnf->powOf2ToDiv)
	{
		if (cnf->powOf2ToDiv - var->prevPowOf2ToDiv >= 31)
		{
			*appPos = *appNeg	= 0.0;
		} else
		{
			*appPos/=(double)(((unsigned long)1)<<(cnf->powOf2ToDiv - var->prevPowOf2ToDiv));
			*appNeg/=(double)(((unsigned long)1)<<(cnf->powOf2ToDiv - var->prevPowOf2ToDiv));
		}
		var->prevPowOf2ToDiv = cnf->powOf2ToDiv;
	}

	mx = max(*appPos, *appNeg);
	mn = min(*appPos, *appNeg);
	return 2*(cnf->heurPartOfS1*mx+(1-cnf->heurPartOfS1)*mn);
	//return *appPos+*appNeg;
}

double GetLitsValForPrint(Cnf* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];

	return (var->overallApp[POS_APP] + var->overallApp[NEG_APP]);
}


char GetVarsSign(Cnf* cnf, long litNum)
{
	Var* var;
	var = &cnf->vars[labs(litNum)];

	if (!(cnf->heur&64))
		return ( /*(1-(char)(((cnf->heur&64)>>6)<<1))**/(((var->overallApp[POS_APP]) > (var->overallApp[NEG_APP])) ? (1) : (-1)));
	else
		return ( /*(1-(char)(((cnf->heur&64)>>6)<<1))**/(((var->overallApp[POS_APP]) < (var->overallApp[NEG_APP])) ? (1) : (-1)));
}

PQ* PQCreate(Cnf* cnf, unsigned short qsNum, double maxInterval, Boolean ifGetFirst, double(*GetLitsVal)(Cnf* cnf, long litNum), char(*GetVarsSign)(Cnf* cnf, long litNum), void(*ReduceInitHeurToMax)(Cnf* cnf, long litNum, double maxVal))
{
	PQ* pq;
	OneQBox *qb, *frstBox;
	unsigned short currQNum;
	long i;
	
	pq = malloc(sizeof(PQ));
	pq->ifGetFirst = ifGetFirst;
	pq->maxInterval = maxInterval;
	pq->qsNum = qsNum;
	pq->qs = calloc(qsNum, sizeof(OneQ));
	pq->currMaxQNum = -1;
	pq->oneQIntrvl = maxInterval / (double)qsNum;
	
	//filling the PQ
		
	for (currQNum = 0; currQNum < qsNum; currQNum++)
	{		
		pq->qs[currQNum].max = maxInterval/qsNum*(currQNum+1);
		
		//adding dummy first and last qb's
		pq->qs[currQNum].firstBox = calloc(1, sizeof(OneQBox));
		pq->qs[currQNum].lastBox = calloc(1, sizeof(OneQBox));
		pq->qs[currQNum].firstBox->next = pq->qs[currQNum].lastBox;
		pq->qs[currQNum].lastBox->prev = pq->qs[currQNum].firstBox;
	}	
		
	for (i=1; i <= cnf->initVarsNum; i++)
	{
		qb = cnf->vars[i].pqBox;
		ReduceInitHeurToMax(cnf, i, maxInterval);
		currQNum = /*floor*/(GetLitsVal(cnf, i)/pq->oneQIntrvl);
#ifdef WIN32
		_ASSERT(currQNum<=qsNum);
#endif
		if (currQNum >= qsNum)
			currQNum = qsNum-1;
		frstBox = pq->qs[currQNum].firstBox;
		qb->prev = frstBox;
		frstBox->next->prev = qb;
		qb->next = frstBox->next;
		frstBox->next = qb;
		
		qb->litNum*=GetVarsSign(cnf, qb->litNum);		

		if (currQNum > pq->currMaxQNum)
			pq->currMaxQNum = currQNum;
	}
	
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
	PQPrint(cnf, pq, GetLitsVal);
	}
#endif
	return pq;
}

void PQDestroy(PQ* pq)
{
	unsigned short i;
	
	for (i = 0; i < pq->qsNum; i++)
	{
		free(pq->qs[i].firstBox);
		free(pq->qs[i].lastBox);
	}
	
	free(pq->qs);
	free(pq);
}

short FindNewMax(OneQ* qs, short from, short to)
{
	unsigned short middle;
	div_t dRes;
	short res;

	if (to == -1)
		return -1;
	if (qs[to].firstBox->next != qs[to].lastBox)
		return to;

	if (from == to)
		return -1;

	dRes = div(to+from,2);
	middle = ((dRes.rem) ? (dRes.quot + 1) : (dRes.quot));
	
	res = FindNewMax(qs, middle, to);
	if (res != -1)
		return res;
	else
		return FindNewMax(qs, from, middle-1);

}

void PQSetNewMaxQIfNeeded(PQ* pq)
{
	if (pq->qs[pq->currMaxQNum].firstBox->next != pq->qs[pq->currMaxQNum].lastBox)
		return;
	pq->currMaxQNum =  FindNewMax(pq->qs, 0, pq->currMaxQNum-1);
	
}

long PQRemMaxVar(Cnf* cnf, PQ* pq, double(*GetLitsVal)(Cnf* cnf, long litNum))
{
	OneQBox *retBox, *qb;
	long retVal;
	double maxVal;
	OneQ* maxQ;
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
//	PQPrint(cnf, pq, GetLitsVal);
		}
#endif
	
	if (pq->currMaxQNum == -1)
		return 0;
	
	maxQ = &pq->qs[pq->currMaxQNum];
	if (pq->ifGetFirst)
	{
		if ((!(cnf->heur&2))||(maxQ->firstBox->next == maxQ->lastBox->prev)||(rand() < div(RAND_MAX,2).quot))
		{
			retVal = maxQ->firstBox->next->litNum;
			retBox = maxQ->firstBox->next;
		} else 
		{
			retVal = maxQ->firstBox->next->next->litNum;
			retBox = maxQ->firstBox->next->next;
		}
	} else
	{
		retBox = maxQ->firstBox->next;
		retVal = retBox->litNum;
		maxVal = GetLitsVal(cnf, retVal);
		for (qb = maxQ->firstBox->next->next; qb != maxQ->lastBox; qb = qb->next)
		{
			if (GetLitsVal(cnf, qb->litNum) > maxVal)
			{
				maxVal = GetLitsVal(cnf, qb->litNum);
				retVal = qb->litNum;
				retBox = qb;
			}
		}
		
		retBox->prev->next = retBox->next;
	}

	retBox->prev->next = retBox->next;
	retBox->next->prev = retBox->prev;

	PQSetNewMaxQIfNeeded(pq);
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
	PQPrint(cnf, pq, GetLitsVal);
	}
#endif	
	return retVal;
}

void PQInsQb(Cnf* cnf, PQ* pq, OneQBox* qb, unsigned short qInd)
{
//	if ((!cnf->heur&16)||(rand() < div(RAND_MAX,2).quot))
//	{
		qb->prev = pq->qs[qInd].firstBox;
		qb->next = pq->qs[qInd].firstBox->next;
		pq->qs[qInd].firstBox->next = qb;
		qb->next->prev = qb;
/*	} else
	{
		qb->next = pq->qs[qInd].lastBox;
		qb->prev = pq->qs[qInd].lastBox->prev;
		pq->qs[qInd].lastBox->prev = qb;
		qb->prev->next = qb;
	}
*/
	if (pq->currMaxQNum < qInd)
		pq->currMaxQNum = qInd;
}

void PQAddVar(Cnf* cnf, PQ* pq, long litNum, double(*GetLitsVal)(Cnf* cnf, long litNum), char(*GetVarsSign)(Cnf* cnf, long litNum))
{
	unsigned short newQInd, botQIndToUnite, qResInd;
	double newVal;
	Var* var;
	OneQBox *qb, *qNewAft1, *qNewBefLast, *qOldAft1;
	OneQ *q, *qNext, *qRes;
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
//	PQPrint(cnf, pq, GetLitsVal);
		}
#endif

	var = &cnf->vars[labs(litNum)];
	qb = var->pqBox;
	qb->litNum = GetVarsSign(cnf, litNum)*var->varNum;

	newVal = GetLitsVal(cnf, litNum);
		
	if (newVal > pq->maxInterval)
	{
		for (botQIndToUnite = qResInd = 0; botQIndToUnite < pq->qsNum; botQIndToUnite+=2, qResInd++)
		{
			q = &pq->qs[botQIndToUnite];
			qNext = &pq->qs[botQIndToUnite+1];
			qRes = &pq->qs[qResInd];
			if (botQIndToUnite == 0)
			{
				if (qNext->firstBox->next != qNext->lastBox)
				{
					if (q->firstBox->next != q->lastBox)
					{						
						qNewAft1 = qNext->firstBox->next;
						qNewBefLast = qNext->lastBox->prev;
						qOldAft1 = q->firstBox->next;
						
						q->firstBox->next = qNewAft1;
						qNewAft1->prev = q->firstBox;
						qNewBefLast->next = qOldAft1;
						qOldAft1->prev = qNewBefLast;
					} else
					{
						q->firstBox->next = qNext->firstBox->next;
						q->firstBox->next->prev = q->firstBox;
						q->lastBox->prev = qNext->lastBox->prev;
						q->lastBox->prev->next = q->lastBox;
					}
											
					qNext->firstBox->next = qNext->lastBox;
					qNext->lastBox->prev = qNext->firstBox;
				}
				
			} else	
			{
				if (qNext->firstBox->next != qNext->lastBox)
				{
					qRes->firstBox->next = qNext->firstBox->next;
					qRes->firstBox->next->prev = qRes->firstBox;
					if (q->firstBox->next != q->lastBox)
					{
						qNext->lastBox->prev->next = q->firstBox->next;
						qNext->lastBox->prev->next->prev = qNext->lastBox->prev;
						qRes->lastBox->prev = q->lastBox->prev;
						qRes->lastBox->prev->next = qRes->lastBox;
					} else
					{
						qRes->lastBox->prev = qNext->lastBox->prev;
						qRes->lastBox->prev->next = qRes->lastBox;
					}
				} else
				{
					if (q->firstBox->next != q->lastBox)
					{
						qRes->firstBox->next = q->firstBox->next;
						qRes->firstBox->next->prev = qRes->firstBox;
						qRes->lastBox->prev = q->lastBox->prev;
						qRes->lastBox->prev->next = qRes->lastBox;
					}
				}
				
				qNext->firstBox->next = qNext->lastBox;
				qNext->lastBox->prev = qNext->firstBox;
				q->firstBox->next = q->lastBox;
				q->lastBox->prev = q->firstBox;
			}
		}
		cnf->powOf2ToDiv++;
		pq->currMaxQNum = div(pq->qsNum,2).quot - 1;	
		newVal = GetLitsVal(cnf, litNum);
	}	

	newQInd = /*floor*/(newVal/pq->oneQIntrvl);
	if (newQInd >= pq->qsNum)
		newQInd = pq->qsNum-1;
	PQInsQb(cnf, pq, qb, newQInd);

#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
	PQPrint(cnf, pq, GetLitsVal);
	}
#endif
}

void PQRemVar(Cnf* cnf, PQ* pq, long litNum)
{
	Var* var;
	OneQBox* qb;
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
//	PQPrint(cnf, pq, GetLitsVal);
		}
#endif
	
	var = &cnf->vars[labs(litNum)];
	qb = var->pqBox;
	
	qb->prev->next = qb->next;
	qb->next->prev = qb->prev;
	
	PQSetNewMaxQIfNeeded(pq);
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
	PQPrint(cnf, pq, GetLitsVal);
	}
#endif
}

Boolean PQIfEmpty(PQ* pq)
{
	return (pq->currMaxQNum == -1);
}

void PQPrint(Cnf* cnf, PQ* pq, double(*GetLitsVal)(Cnf* cnf, long litNum))
{
	short i;
	OneQBox *qb, *qbPrev;
#ifdef PQ_PRINT
	if (TC(cnf, "PQ_PRINT"))
		{
//	if (cnf->actualCCNum > 1901)
	{
		buffInd = sprintf(buff, "\nPQ : QsNum = %d, MaxInterval = %3.3f, ifGetFirst = %d(", pq->qsNum, pq->maxInterval, pq->ifGetFirst);
		
		for (i = pq->qsNum - 1; i >= 0; i--)
		{
			buffInd+=sprintf(buff+buffInd, "\n%d(%3.3f)", i, pq->qs[i].max);
		/*	if ((i == (pq->qsNum-1))&&(pq->qs[i].max != pq->maxVal))
				buffInd+=sprintf(buff+buffInd, "INCONSISTENCY - pq->qs[pq->qsNum-1].max != pq->maxVal)");*/
			if (i == pq->currMaxQNum)
				buffInd+=sprintf(buff+buffInd, "* : ");
			else
				buffInd+=sprintf(buff+buffInd, " : ");
			
			for (qbPrev = pq->qs[i].firstBox, qb = pq->qs[i].firstBox->next; qb != pq->qs[i].lastBox; qb = qb->next, qbPrev = qbPrev->next)
			{
				buffInd+=sprintf(buff+buffInd, "%ld(%3.3f),", qb->litNum, GetLitsValForPrint(cnf, qb->litNum));
				if (qb->prev != qbPrev)
				{
					DebugBreak();
				}
			}
		}
		
		OutputTrace(cnf, "\nPQPrint", 387, "", buff);
	}
	}
#endif
}
