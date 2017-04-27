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
#include "C4PQ.h"
#ifdef WIN32
#include <crtdbg.h>
#pragma intrinsic(labs,memcpy,abs,strcpy,strcmp,strlen)
#endif
void PrepFirstConfForChk(Cnf* cnf, long headD2VarNum)
{
	Var* d2Var;

	d2Var = &cnf->vars[labs(headD2VarNum)];

	d2Var->d2ConfToBeChecked = d2Var->d1ConfsList;
	d2Var->d2CbToBeImplied = d2Var->d2ConfToBeChecked+1;
	d2Var->d2ImplStatBefCheckConf = d2Var->thisImplApp;
	if (GetD1ArrFromCC(d2Var->d2ConfToBeChecked)->next)
	{
		cnf->nextChCbEqTill = (GetD1ArrFromCC(d2Var->d2ConfToBeChecked)->next)+1;
		cnf->ifNextConfAlsoChked = TRUE;
	} else
	{
		cnf->nextChCbEqTill = 0;
		cnf->ifNextConfAlsoChked = FALSE;
	}
#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	PrintConfToBeChecked(cnf, d2Var);
	}
#endif
}

Boolean PassToNextConfAftFuthExSt(Cnf* cnf)
{
	IVS* ivs;
	Var *d2Var;
	CCBox *checkedConf, *nextConf;
	
	ivs = cnf->ivs;
	d2Var = &cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)];
	checkedConf = d2Var->d2ConfToBeChecked;
	nextConf = GetD1ArrFromCC(checkedConf)->next;
	
	if (d2Var->d2ImplStatBefCheckConf < d2Var->thisImplApp)
		RemCCFromLst(cnf, &d2Var->d1ConfsList, checkedConf, GetD1ArrFromCC);
		
	if (!nextConf)
		return FALSE;

	d2Var->d2NumOfCurrChkConf++;
	d2Var->d2ConfToBeChecked = nextConf;
	d2Var->d2CbToBeImplied = nextConf+1;
	d2Var->lastCCBBefConfChk = cnf->lastCCBOfLastCCAdded;
	d2Var->d2ImplStatBefCheckConf = d2Var->thisImplApp;
	if (GetD1ArrFromCC(d2Var->d2ConfToBeChecked)->next)
	{
		cnf->nextChCbEqTill = (GetD1ArrFromCC(d2Var->d2ConfToBeChecked)->next)+1;
		cnf->ifNextConfAlsoChked = TRUE;
	} else
	{
		cnf->nextChCbEqTill = 0;
		cnf->ifNextConfAlsoChked = FALSE;
	}

#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	PrintConfToBeChecked(cnf, d2Var);
	}
#endif
	
	return TRUE;
	
}

//returns TRUE if next conf should be checked
//returns FALSE if checking conflicts is done
//supposes, that conflict was checked for cnf->ivs->headDec2Var
//d2 var is processed before FurtherExamineStack in AlgMain
Boolean PassToNextConfBefFuthExSt(Cnf* cnf)
{
	IVS* ivs;
	Var *d2Var;
	CCBox *checkedConf, *nextConf, *cbCurr, *oldNextCb;
	Boolean ifReplBy2SideBlck;
	CCBox *curr1ArrBox, *next1ArrBox;
	
	ivs = cnf->ivs;
	d2Var = &cnf->vars[labs(ivs->bs[ivs->d2Head].litNum)];
	checkedConf = d2Var->d2ConfToBeChecked;
	nextConf = GetD1ArrFromCC(checkedConf)->next;

	if (d2Var->d2ImplStatBefCheckConf < d2Var->thisImplApp)
	{
		RemCCFromLst(cnf, &d2Var->d1ConfsList, checkedConf, GetD1ArrFromCC);
		ifReplBy2SideBlck = FALSE;
	}
	else
		ifReplBy2SideBlck = TRUE;

	//if next conflict is equal to this till d2CbToBeImplied
	//we are going till the conflict which his head isn't implied
	//and choosing appropriate cnf->nextChCbEqTill from its vars
	if ((cnf->ifNextConfAlsoChked)&&(nextConf))
	{
		for (oldNextCb = cnf->nextChCbEqTill, checkedConf = nextConf, nextConf = GetD1ArrFromCC(nextConf)->next; 
		nextConf; oldNextCb = cnf->nextChCbEqTill,checkedConf = nextConf,nextConf = GetD1ArrFromCC(nextConf)->next)
		{	
			if (!ifReplBy2SideBlck)
				RemCCFromLst(cnf, &d2Var->d1ConfsList, checkedConf, GetD1ArrFromCC);

			curr1ArrBox = FRST_ARROW_FROM_CC(checkedConf);
			next1ArrBox = FRST_ARROW_FROM_CC(nextConf);

			for ( cbCurr = checkedConf+1, cnf->nextChCbEqTill = nextConf+1; 
			      ( (cbCurr!=curr1ArrBox)&&(cnf->nextChCbEqTill!=next1ArrBox)&&
				    (cbCurr!=oldNextCb)&&(cnf->ifNextConfAlsoChked) );
				  cbCurr++, cnf->nextChCbEqTill++ 
				)
			{
				if (cbCurr->litNum != cnf->nextChCbEqTill->litNum)
				{
					cnf->ifNextConfAlsoChked = FALSE;
					goto OutOfFors;
				}
			}
			
			if (cnf->ifNextConfAlsoChked)
			{
				//if next conf. is shorter, it wasn't checked
				if ( (!cnf->nextChCbEqTill) && (cbCurr) && (cbCurr != oldNextCb) )
				{
					cnf->ifNextConfAlsoChked = FALSE;
					goto OutOfFors;
				}	
			}
			
		} 

		if (!nextConf)
		{
			if (!ifReplBy2SideBlck)
				RemCCFromLst(cnf, &d2Var->d1ConfsList, checkedConf, GetD1ArrFromCC);
		}
	}
	
OutOfFors:	
	if (!nextConf)
		return FALSE;
	
	d2Var->d2NumOfCurrChkConf++;
	d2Var->d2ConfToBeChecked = nextConf;

	d2Var->d2CbToBeImplied = d2Var->d2ConfToBeChecked + 1; 
	d2Var->d2ImplStatBefCheckConf = d2Var->thisImplApp;
	if (GetD1ArrFromCC(d2Var->d2ConfToBeChecked)->next)
	{
		cnf->nextChCbEqTill = (GetD1ArrFromCC(d2Var->d2ConfToBeChecked)->next) + 1;
		cnf->ifNextConfAlsoChked = TRUE;

	} else
	{
		cnf->nextChCbEqTill = 0;
		cnf->ifNextConfAlsoChked = FALSE;
	}

#ifdef PRINT_IVS
	if (TC(cnf, "PRINT_IVS"))
		{
	PrintConfToBeChecked(cnf, d2Var);
	}
#endif
	
	return TRUE;
	
}
