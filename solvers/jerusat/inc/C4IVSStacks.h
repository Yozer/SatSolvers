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

#ifndef CCC_IVS_STACKS_H
#define CCC_IVS_STACKS_H

#include <stdio.h>

struct Cnf_;

typedef struct OneIVSBox_ 
{
	long prevD1Ind, prevD2Ind, prevDecInd, prevCPInd;
	long litNum;
} OneIVSBox;

typedef struct ImplVarsStacks_ {
	OneIVSBox* bs;
	unsigned long allNum, decNum, d1Num, d2Num, cpNum, totalCapacity, d1Depth;
	long allHead, d1Head, decHead, d2Head, cpHead;
} IVS;

IVS* IVSCreate(unsigned long litsNum);
void IVSDestroy(IVS*);

long IVSPush(struct Cnf_*, IVS*, long litNum);
//returns lastly deleted literal number
long IVSDelTillLastDecVar(struct Cnf_* cnf, IVS* ivs, void (*FreeD1Var)(struct Cnf_* cnf, long varNum), void (*FreeD2Var)(struct Cnf_* cnf, long varNum), void (*FreeAisVar)(struct Cnf_* cnf, long varNum), void (*FreeConfVar)(struct Cnf_* cnf, long varNum), void (*FreeOtherVars)(struct Cnf_* cnf, long varNum));
//returns lastly deleted literal number
long IVSDelTillLastConfPartVar(struct Cnf_* cnf, IVS* ivs, void (*FreeD1Var)(struct Cnf_* cnf, long varNum), void (*FreeD2Var)(struct Cnf_* cnf, long varNum), void (*FreeAisVar)(struct Cnf_* cnf, long varNum), void (*FreeConfVar)(struct Cnf_* cnf, long varNum), void (*FreeOtherVars)(struct Cnf_* cnf, long varNum));
void IVSPrint(struct Cnf_*, IVS*);
void IVSPrintSorted(struct Cnf_*, IVS*);

#endif