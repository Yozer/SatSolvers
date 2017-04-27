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

#ifndef CCC_PQ_H
#define CCC_PQ_H

#include "C4Globals.h"


typedef struct OneQ_
{
	double max;
	OneQBox *firstBox, *lastBox;
} OneQ;

typedef struct PQ_
{
	unsigned short qsNum;
	short currMaxQNum;
	double maxInterval;
	char/*Boolean*/ ifGetFirst;
	OneQ* qs;
	double oneQIntrvl;
} PQ;

PQ* PQCreate(Cnf* cnf, unsigned short qsNum, double maxInterval, Boolean ifGetFirst, double(*GetLitsVal)(Cnf* cnf, long litNum), char(*GetVarsSign)(Cnf* cnf, long litNum), void(*ReduceInitHeurToMax)(Cnf* cnf, long litNum, double maxVal));
void PQDestroy(PQ* pq);
long PQRemMaxVar(Cnf* cnf, PQ* pq, double(*GetLitsVal)(Cnf* cnf, long litNum));
void PQAddVar(Cnf* cnf, PQ* pq, long litNum, double(*GetLitsVal)(Cnf* cnf, long litNum), char(*GetVarsSign)(Cnf* cnf, long litNum));
void PQRemVar(Cnf* cnf, PQ* pq, long litNum);
Boolean PQIfEmpty(PQ* pq);
void PQPrint(Cnf* cnf, PQ* pq, double(*GetLitsVal)(Cnf* cnf, long litNum));

/*
typedef struct OneQ_
{
	double max;
	OneQBox *firstBox, *lastBox;
} OneQ;

typedef enum{FOMGetFirst, FOMGetMax, FOMGetFirstForAllButLast, FOMGetMaxForAllButLast} GetFirstOrMax;

typedef struct PQ_
{
	unsigned short qsNum;
	short currMaxQNum;
	double maxInterval;
	double maxVal;
	GetFirstOrMax  getFirstOrMax;
	OneQ* qs;
} PQ;

PQ* PQCreate(Cnf* cnf, unsigned short qsNum, double maxInterval, GetFirstOrMax getFirstOrMax, double(*GetLitsVal)(Cnf* cnf, long litNum), char(*GetVarsSign)(Cnf* cnf, long litNum), double maxVal);
void PQDestroy(PQ* pq);
long PQRemMaxVar(Cnf* cnf, PQ* pq, double(*GetLitsVal)(Cnf* cnf, long litNum));
void PQAddVar(Cnf* cnf, PQ* pq, long litNum, double(*GetLitsVal)(Cnf* cnf, long litNum), char(*GetVarsSign)(Cnf* cnf, long litNum));
void PQRemVar(Cnf* cnf, PQ* pq, long litNum);
Boolean PQIfEmpty(PQ* pq);
void PQPrint(Cnf* cnf, PQ* pq, double(*GetLitsVal)(Cnf* cnf, long litNum));
*/
#endif