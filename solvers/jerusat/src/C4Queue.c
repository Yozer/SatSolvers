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

#include "C4Queue.h"
//#include "N5Globals.h"
#ifdef WIN32
#include <crtdbg.h>
#endif
long Modulo(long num, long mod)
{
	ldiv_t divRes;

	divRes = ldiv(num, mod);
	
	if (divRes.rem < 0)
		return mod+divRes.rem;
	else
		return divRes.rem;
}

Queue* CreateQueue(long maxElemNum)
{
	Queue* q;

	q = (Queue*)malloc(sizeof(Queue));

	q->elemArr = (long*)malloc(maxElemNum*sizeof(long));
	
	if (maxElemNum < 2) maxElemNum = 2;
	q->maxNumOfElem = maxElemNum;

	q->tailInd = 0;
	q->headInd = 1;

	return q;
}

char IsQEmpty(Queue* q)
{
	return (Modulo(q->tailInd+1, q->maxNumOfElem) == q->headInd);
}

void Enqueue(Queue* q, long el)
{
	q->tailInd = Modulo(q->tailInd+1, q->maxNumOfElem);
	q->elemArr[q->tailInd] = el;
}

long Dequeue(Queue* q)
{
	if (IsQEmpty(q)) return 0;

	q->headInd = Modulo(q->headInd+1, q->maxNumOfElem);
	return q->elemArr[Modulo(q->headInd-1, q->maxNumOfElem)];
}

void EmptyQueue(Queue* q)
{
	q->tailInd = 0;
	q->headInd = 1;
}

void FreeQueue(Queue* q)
{
	free(q->elemArr);
	free(q);
}

/*
void PrintQueue(Queue* q)
{
	long i;
	
	if (IsQEmpty(q)) 
		fprintf(outFile, "{}");
	else
	{
		for (i = q->headInd; i != q->tailInd; i = Modulo(i+1, q->maxNumOfElem))
			if (i == q->headInd)
				fprintf(outFile, "{%d;", q->elemArr[i]);
			else
				if (i != q->tailInd)
					fprintf(outFile, "%d;", q->elemArr[i]);
				else
					fprintf(outFile, "%d}", q->elemArr[i]);
	}
}*/
