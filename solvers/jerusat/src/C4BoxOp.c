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
#ifdef WIN32
#include <crtdbg.h>
//#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
CCBox* CreateCCBox(char ccType, long litNum, unsigned long ordFromUp0Ind, char wchStat)
{
	CCBox* ccb;

	ccb = malloc(sizeof(CCBox));

	ccb->ccType = ccType;
	ccb->litNum = litNum;
	ccb->ordFromUp0Ind = ordFromUp0Ind;

	return ccb;
}

void FillCCBox(CCBox* ccb, char ccType, Boolean ifSorted, long litNum, unsigned long ordFromUp0Ind, char wchStat)
{
	ccb->ccType = ccType;
	ccb->ifSorted = ifSorted;
	ccb->litNum = litNum;
	ccb->ordFromUp0Ind = ordFromUp0Ind;
}
