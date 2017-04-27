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

#ifndef CCC_DATA_STRUCT_H
#define CCC_DATA_STRUCT_H

#include "C4HeurData.h"
#include "C4Stack.h"
#include "C4IVSStacks.h"
#include "C4Queue.h"

#ifndef WIN32
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define MAX_PQ_LIT 1000000000
#define DUMMY_CONF_LIT_TO_JUMP 0

#define CC_TYPE_INIT_CLS 0
#define CC_TYPE_CONF_CLS 1
#define CC_TYPE_CONF_ONLY 2
#define CC_TYPE_CLS_ONLY 3

#define WCCB_BY_WDOWN_AND_LIT(wDown,lit) ( ((wDown)->litNum == (lit)) ? ((wDown)) : ((wDown)-((wDown)->ordFromUp0Ind)+((wDown)->nextClsIndOrD1ArrOrd)) )
#define CLS_CCBOXES_BEG(ccBox) ((ccBox) - (ccBox)->ordFromUp0Ind) 
#define CC_SORTED_FROM(ccBox) (((unsigned long*)((ccBox) - (ccBox)->ordFromUp0Ind)) - 1)
#define CC_LITS_NUM(ccBox) (((unsigned long*)((ccBox) - (ccBox)->ordFromUp0Ind)) - 2)
#define FRST_ARROW_FROM_CC(ccBox) ( (ccBox) - (ccBox)->ordFromUp0Ind + (*((unsigned long*)((ccBox) - (ccBox)->ordFromUp0Ind) - 2)) )
#define CCB_DOWN_FROM_CC(ccBox) ( (ccBox) - (ccBox)->ordFromUp0Ind + ((*((unsigned long*)((ccBox) - (ccBox)->ordFromUp0Ind) - 2)) - 1) )

//implied or not implied
#define NOT_IMPLIED 1
#define IMPLIED 2
//implied positeively or negatively
#define IMPL_POS 4
#define IMPL_NEG 8

#define POS_APP 0
#define NEG_APP 1

#define OVERALL_CONF_APP_1_PP(var) ( (var->implStat&4) ? (var->overallApp[0]++) : (var->overallApp[1]++))
#define ONES_CONF_APP_1_PP(lit) ( (lit>0) ? (cnf->vars[labs(lit)].overallApp[0]++) : (cnf->vars[labs(lit)].overallApp[1]++) )

//implication status
#define IMPL_D1 16
#define IMPL_D2 32
#define IMPL_CONF 64
#define IMPL_REG_RES 128
#define IMPL_ADD_IMPL_ST 256

#define IMPL_SIGN(lit) ( ((lit)>0) ? (IMPL_POS) : (IMPL_NEG) ) 

#define TURN_ON(implStat,flag) ((implStat) |= (flag))
#define TURN_OFF(implStat,flag) ((implStat) &= (~(flag)))
#define ON(implStat,flag) ((implStat) == ((implStat) | (flag)))
#define ALL_ON_FOR_DISJ(implStat,flags) ((implStat) == ((implStat) | (flags)))
#define ONE_ON_FOR_DISJ(implStat,flags) ((implStat) & (flags))
#define OFF(implStat,flag) ((implStat) != ((implStat) | (flag)))
#define ALL_OFF_FOR_DISJ(implStat,flags) (!((implStat) & (flags)))
#define ONE_OFF_FOR_DISJ(implStat,flags) (!((implStat) == ((implStat) | (flags))))

typedef struct CCBox_
{
  //struct {
		/*
		this should have been, but the compiler gives it too much bytes
		union {
			unsigned long d1ArrOrdFromUp : 16;
			unsigned long nextClsIndOrD1ArrOrd : 16;
		};*/		
		long nextClsIndOrD1ArrOrd;
		unsigned long ordFromUp0Ind;
		long litNum;
//		unsigned long ifOnWchList : 1;
		unsigned long ccType : 2;
		unsigned long ifSorted : 1;
//		unsigned long ifDelClsDataField : 1;	
//		unsigned long ifWatchedArrowsDataFields : 1;
		unsigned long ifNextClsBoxInd : 1;
		unsigned long ifUsedConfsDelHead : 1;
//		unsigned long ifPlcForEmD1Ord : 1;
		unsigned long ifConfSimTreated : 1;
		unsigned long ifDelCls : 1;
		unsigned long clsFromConfStat : 1;
		unsigned long ifBot2 : 1;
		unsigned long ifRecClsFromConfCreated : 1;
		unsigned long ifAdd1UIP : 1;
//	};
} CCBox;

typedef struct CCArrowsBox_
{
	CCBox* prev;
	CCBox* next;
} CCArrowsBox;

typedef struct OneQBox_
{
	long litNum;
	struct OneQBox_* prev;
	struct OneQBox_* next;
} OneQBox;

typedef enum{d1,d2NoOrBefConfChk,addAftD2,addAftConf,confReg,confDummyRegRes,aiDummyAftD2,aiDummyAftConf,confDummyAddImpl} ConfCircum;
typedef enum{notImplOut, notImplIn, confPartNotAdd, confPartAddPos, confPartAddNeg, addImplImpl, addImplChngSide, implCons, implIncons} AddImplStat;
#define AIS_CONF_PART_ADD_STAT(var) ( ((var)->ivsBox->litNum > 0) ? (confPartAddNeg) : (confPartAddPos) )

typedef struct LLBox_
{
	struct LLBox_* prev;
	struct LLBox_* next;
	long litNum;
} LLBox;


typedef struct Var_
{
	unsigned long varNum;
	CCBox *watchedPos, *watchedNeg;
	//var's box in priority queue
	OneQBox* pqBox;
	//implication status
	unsigned long implStat;
	//number of last CC before implication
	unsigned long lastCCBefImplNum;	
	//recording cnf->actualCCNum before D1 implication
	unsigned long actualCCNumBefD1Impl;
	//implication level
	long implLevel;
	//conflict part vars' implication level
	long cpImplLevel;
	//pointer to implication clause
	CCBox* implCls;
	//pointer to IVS box in IVS stack
	OneIVSBox* ivsBox;
	//number of previous D2 var in IVS stack
	long prevImplD2Num;
	//number of CC boxes to record CC's even if the var 
	//enlarges the CC beyond cnf->maxCCBoxesInCC
	unsigned long cbNumToRecAnyway;	
	//number of CC boxes in current implication 
	unsigned long thisImplCCBox;
	//number of D1 boxes in current implication 
	unsigned long thisImplD1Box;
	//number of appearences in current implication
	//equal to thisImplCCBox for complete var
	unsigned long thisImplApp;
	//number of overall appearances, used for heuristics
	double overallApp[2];
  //union
  //{
		//list of D1 conflicts to check
		CCBox* d1ConfsList;
		//while checking the conflicts we'll leave in d1ConfsList and therefore in d2ConfsToReplBy2SideConfBlcks
		//only those that would be changed by d2 blocks if some conditions are true
  //	CCBox* d2ConfsToReplBy2SideConfBlcks;
  //};
	//list of clauses to delete after deimplication of the var
	CCBox* clssToDel;
	//list of used conflicts to delete after deimplication of the var
	CCBox* usedConfToDel;
	//may be one of {decImplNotAdd, decImplAdd1stSide, 
	//addImplImpl, notImplOut, notImplIn, implCons, implIncons}
	AddImplStat aisStatus;
	//cc to be the implCls for a var participating in addImplSt
	//each time it is IMPL_ADD_IMPL_ST implied
	CCBox* aisLastCCBox;
	CCBox* aisCCOfVarToSkipTill;
	//box in AIS list
	LLBox* aisBox;
	//list of CC's for 2nd from bottom vars to include 2 bottom CCBoxes
	//in appropriate watched lists
	CCBox* bot2CC;
	//list to remove the bottom var from AIS
	CCBox* remFromAIS;
	//list of conflicts to be checked
	CCBox* d2ConfToBeChecked;
	//conflict box to be implied
	CCBox* d2CbToBeImplied;
	//indicates whether conflicts are checked for the current D2 var
	char/*Boolean*/ d2IfConfChck;
	CCBox* lastCCBBefD1;
	CCBox* lastCCBBefD2;
	CCBox* lastCCBBefConfChk;
	unsigned long d2ImplStatBefCheckConf;
	long d2NumOfCurrChkConf;
	//used in RecClsAndConf_ only
	char/*Boolean*/ ifD1LiveInCurrCC;
	long prevPowOf2ToDiv;
	long randNumPerVar;
	unsigned long d1ImplApp;
	
	//for effective clause recording (1UIP etc.)
	long implLayer;
	LLBox layerBox;
	long lastLayerCountVal;
} Var;

char/*Boolean*/ ifUnsAftBuildCnf;
struct PQ_;
typedef enum {NoClsLinear, ClsLinear, NoClsExp, ClsExp1, ClsExp2, ClsLikeVer1, NoClsLikeVer1} RestartsStrategy;
typedef struct Cnf_
{
	Var* vars;
	char* initCCBuff;
	unsigned long initClssNum;
	unsigned long initVarsNum;
	unsigned long currCCNum;
	unsigned long actualCCNum;
	unsigned long heur;
	unsigned long maxClsLenToRec;
	unsigned long maxCCBoxesInCC;
	unsigned long cutOff;
	struct PQ_* pq;
	//queue of variables to be implied
	Queue* toBeImplied;
	CCBox dummyConfBox;
	CCBox dummyAISBox;
	char/*Boolean*/ ifPrePushD1Block;
	IVS* ivs;
	unsigned long currNodeNum;
	unsigned long varNumWithContr;
	Stack* prePushSt;
	//for each var holds last conf. num, where the var participated(used in GetVarsInConf)
	unsigned long* lastConfNum;
	//for each var holds last prepush conf. num, where the var participated(used in PrePushGetVarsInConf)
	unsigned long* lastPrepushConfNum;
//	long implLvlNotCPFromDown;
	long implLvlMinInCC;
	double notToChkConfsFrom;
	//literals participating in new CC, used in RecClsAndConf
	Stack* rccCPLits;
	//used to prevent checking not complete D1 vars when needed
	//(when there is a conflict of AIS var implied to both sides)
	long implLvlToSkipTill;
	//linked list holding vars which are to be implied before each implication
	LLBox* ais;
	//pointer to member of addImplSt to be implied next (initialized when IVSPushing D2 var) 
	LLBox* aisPtr;	
	//beginning of list of all conflicts
	CCBox* lastCCBOfFirstCCAdded;
	//end of list of all conflicts
	CCBox* lastCCBOfLastCCAdded;
	//holds pointer to one of conf. boxes of next conflict of uppermost D2 var 
	//till nextChCbEqTill, all conf. boxes are equal to conflict currently being checked 
	CCBox* nextChCbEqTill;
	char/*Boolean*/ ifNextConfAlsoChked;
	Stack* confVarsPart;
	CCBox* lastCCBoxOfCCNotToDel;
	long initAisVarsImplLvlIncl;
	unsigned long d2VarsConfsCheck;
	unsigned long d2VarsConfsNotCheck;
	unsigned long d2VarsConfsCheckMinImplLvl;
	Stack* pushD1BlockSt;
	double heurPartOfS1;
	long powOf2ToDiv;
	unsigned long restartAft;
	unsigned long restartAdd;
	unsigned long restartsNum;
	char ifRestarting;
	long* varsNumForRestart;
	long varsNumForRestartInd;
	unsigned long sizeOfBuffForCnf;
	long varToDelCls;
	long clsDensFromNotInc;

	long* lastLayerPerCpNum;
	LLBox** layersArr;
	long layerPartCount;
	long* lastLayerCoPerCpLvl;
	char/*Boolean*/ ifAdd1UipClause;
	LLBox* layerBoxesBefLastCPLvl;
	long max1UIPLenToRec;
	int incrHeur1UIP;

	//parameters for new restarts strategies (for version 2)
	RestartsStrategy restartsStrategy;
	unsigned long restartAftInit;
	unsigned long restartAddInit;
	
	int restartDelayTimesNow;
	int restartDelayTimes;

	unsigned long restartsClsAddNum;
	unsigned long restartsClsNotAddNum;

	unsigned long restartAftWhenDelay;
	unsigned long restartAddWhenDelay;
	
	int restartsMaxClssToRec;

	unsigned long minD1Depth;
	
	char/*Boolean*/ ifGetFirstBackAftRestart;

	int small_confs;
	int large_confs;
} Cnf;

#endif
