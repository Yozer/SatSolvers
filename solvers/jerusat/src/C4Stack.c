/*------------------------------------------------------------------------*\
Jerusat 1.3 SAT Solver
(C)3001-2003, Nadel Alexander.

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
#include <stdio.h>

#include "C4Globals.h"
#include "C4Stack.h"
#ifdef WIN32
#include <crtdbg.h>
#endif
Stack* CreateStack(long maxNumOfElem)
{
	Stack* stack;
	
	stack = malloc(sizeof(struct Stack_));
	stack->headInd = stack->printInd = -1;
	stack->elemArr = malloc(sizeof(long)*maxNumOfElem);
	stack->maxNumOfElem = maxNumOfElem;

	return stack;
}

void FreeStack(Stack* stack)
{
	free(stack->elemArr);
	free(stack);
}

void Push(Stack* stack, long elem)
{
	if (stack->headInd == stack->maxNumOfElem-1)
	{
		stack->elemArr = realloc(stack->elemArr, sizeof(long)*2*stack->maxNumOfElem);
		stack->maxNumOfElem*=2;
	}
	stack->elemArr[++stack->headInd] = elem;
}

long Pop(Stack* stack)
{
	if (IsEmpty(stack))
		return 0;
	return stack->elemArr[stack->headInd--];
}

long GetHead(Stack* stack)
{
	if (!IsEmpty(stack))
		return stack->elemArr[stack->headInd];
	return 0;
}

long IsEmpty(Stack* stack)
{
	return (stack->headInd < 0);
}

long ElemNum(Stack* stack)
{
	return stack->headInd+1;
}

long GetFirstForPrinting(Stack* stack)
{
	if (stack->headInd < 0) return -1;

	stack->printInd = stack->headInd;
	return stack->elemArr[stack->printInd--];
}

long GetNextForPrinting(Stack* stack)
{
	if (stack->printInd < 0) return -1;
	
	return stack->elemArr[stack->printInd--];
}

long IsEndOfPrinting(Stack* stack)
{
	return (stack->printInd < 0);
}


void PrintStack(Stack* st)
{
	long elem;
	
	buffInd+=sprintf(buff+buffInd, "\n{");
	for (elem = GetFirstForPrinting(st); elem != -1; elem = GetNextForPrinting(st))
	{
		buffInd+=sprintf(buff+buffInd, "%d", elem);
		
		if (!IsEndOfPrinting(st))
			buffInd+=sprintf(buff+buffInd, ";");
	}
	
	buffInd+=sprintf(buff+buffInd, "}");
//	OutputTrace(cnf, "\nPrintStack", 98, "", buff);
}

void EmptyStack(Stack* stack)
{
	stack->headInd = stack->printInd = -1;
}

