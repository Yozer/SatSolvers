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
#include <limits.h>
#ifdef _DEBUG
#include <windows.h>
#endif

#include "C4Globals.h"
#include "C4PQ.h"

#ifdef WIN32
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
long IVSPushOrPrepush(struct Cnf_* cnf, long litNum)
{
	unsigned long *implStat;
	Var* var;

	if (cnf->ifPrePushD1Block)
	{
		if (litNum == MAX_PQ_LIT)
		{
			litNum = PQRemMaxVar(cnf, cnf->pq, GetLitsVal);
			var = &cnf->vars[labs(litNum)];
			var->implStat = IMPLIED | IMPL_D1 | IMPL_SIGN(litNum);
			var->implCls = 0;
		}
		else
		{
			var = &cnf->vars[labs(litNum)];
			PQRemVar(cnf, cnf->pq, litNum);
		}

		//to enforce var->implLevel be bigger than cnf->initAisVarsImplLvlIncl
		var->implLevel = LONG_MAX;
		var->lastCCBefImplNum = cnf->currCCNum;
		
		implStat = &var->implStat;

		TURN_OFF(*implStat,NOT_IMPLIED);
		TURN_ON(*implStat,IMPLIED);

		Push(cnf->prePushSt, litNum);

		return litNum;
	} else
	{
		return IVSPush(cnf, cnf->ivs, litNum);
	}
}

void PrePushGetVarsInConf_(Cnf* cnf, CCBox* clsWithContradiction)
{
	Var *currVar;
	CCBox* vertBox;
#ifdef PRINT_IVS
	unsigned long* currImplStat;
	long litNumAsImplied;
	
#endif
	for (vertBox = CLS_CCBOXES_BEG(clsWithContradiction); (vertBox != FRST_ARROW_FROM_CC(clsWithContradiction)); vertBox++)
	{
		if (cnf->lastPrepushConfNum[labs(vertBox->litNum)] != (cnf->currCCNum+1))
		{
			currVar = &cnf->vars[labs(vertBox->litNum)];
			if (currVar->implLevel > cnf->initAisVarsImplLvlIncl)
			{
				//if the variable is implied independently of others
				//or is implied in this clause
				if ( (!currVar->implCls) || (CLS_CCBOXES_BEG(currVar->implCls) == CLS_CCBOXES_BEG(clsWithContradiction)) )
				{
					cnf->lastPrepushConfNum[labs(vertBox->litNum)] = cnf->currCCNum+1;
#ifdef PRINT_IVS
					if (TC(cnf, "PRINT_IVS"))
						{
					currImplStat = &cnf->vars[labs(vertBox->litNum)].implStat;
					
					if (ON(*currImplStat,IMPL_POS))
						litNumAsImplied = labs(vertBox->litNum);
					else
						litNumAsImplied = -labs(vertBox->litNum);
					
					buffInd+=sprintf(buff+buffInd, "%d%c,", litNumAsImplied, GetStatLetter(cnf, litNumAsImplied, *currImplStat));fflush(outFile);
					}
#endif					
				}
				else
					PrePushGetVarsInConf_(cnf, currVar->implCls);
			} 
		}
	}
}

void PrePushGetVarsInConf(Cnf* cnf, CCBox* clsWithContradiction)
{
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	buffInd = 0;
	buffInd = sprintf(buff, "\nPRECONFLICT: {");
	}
#endif
	PrePushGetVarsInConf_(cnf, clsWithContradiction);
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	buffInd+=sprintf(buff+buffInd, "}");fflush(outFile);
	OutputTrace(cnf, "\nPrePushGetVarsInConf", 22, "", buff);
	}
#endif
}

CCBox* PushD1Block(Cnf* cnf, CCBox* clsWithContr)
{
	Var* var;
	CCBox* newCCWithContr;
	long litNum;

	PrePushGetVarsInConf(cnf, clsWithContr);

	EmptyStack(cnf->pushD1BlockSt);
	
	while (!(IsEmpty(cnf->prePushSt)))
	{
		litNum = Pop(cnf->prePushSt);
		var = &cnf->vars[labs(litNum)];
		
		var->implCls = 0;
		var->implLevel = 0;
		PQAddVar(cnf, cnf->pq, var->varNum, GetLitsVal, GetVarsSign);
		
		if ( (cnf->lastPrepushConfNum[var->varNum] == (cnf->currCCNum+1))
			   &&
			 (ON(var->implStat,IMPL_D1)) )
		{
			var->implStat = NOT_IMPLIED;
			Push(cnf->pushD1BlockSt, litNum);
		} else
		{
			var->implStat = NOT_IMPLIED;
		}
	}	

	cnf->currCCNum++;
	cnf->ifPrePushD1Block = FALSE;
	while (!(IsEmpty(cnf->pushD1BlockSt)))
	{
		litNum = Pop(cnf->pushD1BlockSt);
		PrepVarToImpl(cnf, litNum, 0, NOT_IMPLIED | IMPL_D1 | IMPL_SIGN(litNum), cnf->lastLayerPerCpNum[cnf->ivs->cpNum]+1);
		if (newCCWithContr = Imply(cnf))
		{
			return newCCWithContr;
		}
		
	}	
#ifdef _DEBUG
	DebugBreak();
#endif
}
