/**

	eFLC.h - embedded Fuzzy Logic Control System
	
	API for embedded Fuzzy Logic Control System

   Copyright (C) 2011 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       9/2011
   Version:    0.1

 */


#ifndef	__eFLC_h__
#define	__eFLC_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f10x.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ================    User Defines      ======================*/

// This enables the FCL parser module
#define  eFLC_ENABLE_FCL               (1)

#define  eFLC_TERMNAME_WIDTH           (10)
#define  eFLC_VARNAME_WIDTH            (10)
#define  eFLC_RULENAME_WIDTH           (10)
#define  eFLC_FORM_SHAPE_POINTS        (4)
#define  eFLC_VARIABLE_CAPACITY        (16)
#define  eFLC_RULEBLOCK_CAPACITY       (8)
#define  eFLC_DEFUZZ_CAPACITY          (16)
/* Heap memory calculator :
   
   Heap Size = 

 */


#define  eFLC_DEFUZZmeth_COG_STEPS          (100)

#if eFLC_ENABLE_FCL == 1
#define  eFLC_FCL_LINE_WIDTH            (80)
#endif   //#if eFLC_ENABLE_FCL == 1


/* ================    General Data types    ====================*/

// Inner module naming
typedef  int8_t         flc_Index_t;
typedef  char           flc_Name_t;
typedef  uint8_t        flc_Flag_t;
typedef  char           flc_Char_t;

typedef  float          mf_Form_Shape_t;
typedef  uint8_t        rb_NotFlag_t;
typedef  float          rb_Factor_t;

typedef  volatile float flc_InOut_t;
typedef  float          flc_Truth_t;

typedef  enum { FLC_SUCCESS, FLC_MEM_ERROR, FLC_INF_ERROR, FLC_DEF_ERROR, FLC_PARSE_ERROR} fcl_Exit_Status_t;

typedef  flc_Truth_t  (*ACCU_Method_pt) (flc_Truth_t, flc_Truth_t);
typedef  flc_Truth_t  (*OP_Method_pt) (flc_Truth_t, flc_Truth_t);


typedef enum  { NO_OP = 0, NOT, AND, OR}flc_Operator_t;
typedef enum  { MIN = 0, PROD, BDIF }flc_AND_Algo_t;
typedef enum  { MAX = 0, ASUM, BSUM }flc_OR_Algo_t;
typedef enum  { ACCU_MAX = 0, ACCU_BSUM, ACCU_NSUM }flc_ACCU_Method_t;
typedef enum  { COG = 0, COGS, COA, MOM, LM, RM }flc_DEFUZ_Method_t;
typedef enum  { NO_HEDGE = 0, LITTLE, SOMEWHAT, SLIGHTLY, VERY, EXTREMELY, VERY_VERY }flc_Hedges_t; 


/* =======================  Module I/O   ========================*/

#define  IN          (0)
#define  OUT         (1)

typedef struct eFLC_Input
{
   flc_Index_t       BufferVarIdx;
   flc_Name_t        VarName[eFLC_VARNAME_WIDTH];
}FLC_IO_t;

//Public
extern FLC_IO_t      FLC_InOutSetup[];
extern flc_InOut_t   FLC_InOutBuffer[eFLC_VARIABLE_CAPACITY];

static flc_InOut_t   io_SetOutput (flc_Name_t *nm, flc_InOut_t out);
static flc_InOut_t   io_GetInput (flc_Name_t *nm);



/* ===============  FCL File Parser Functions and Types  ==============*/

#if eFLC_ENABLE_FCL == 1

#ifndef EOF
#define  EOF      (0xFF)
#endif   //#ifndef EOF
typedef  const flc_Char_t   FCL_FILE;

extern FCL_FILE FCL[];

typedef struct
{
   flc_Char_t*       fd;
   flc_Char_t*       cursor;
}__file_t;


static __file_t *    fcl_loadfile (FCL_FILE *fcl);
static flc_Char_t    fcl_getc (__file_t *file);
static flc_Char_t*   fcl_gets (flc_Char_t* str, __file_t *file);
//static char*         strncpy_null (char* dest, const char* src, size_t size);

/* Return pointer to first non-whitespace char in given string. */
static flc_Char_t*   fcl_skip(const flc_Char_t* s);
static flc_Char_t*   fcl_gettoken (flc_Char_t* t, flc_Char_t* s, flc_Char_t** rest);

//Public
fcl_Exit_Status_t    fcl_parse (void);

#endif   //#if eFLC_ENABLE_FCL == 1





/* ================  Variable Functions and Types  ======================*/
typedef enum
{
   Shoulder = 0,
   Trapezoidal,
   Triangle,
   Rectangle,
   Singleton
}FLC_MF_Form_t;

struct MF_TERM_Node_s
{
   flc_Name_t              TermName[eFLC_TERMNAME_WIDTH];
   flc_Index_t             TermIdx;
   FLC_MF_Form_t           MF_FormType;
   mf_Form_Shape_t         MF_Form_Shape[eFLC_FORM_SHAPE_POINTS][2];
   flc_Truth_t             mU;      //result after inference/accu 
   struct MF_TERM_Node_s   *next;   //self referenced [ term list ]
};
typedef struct MF_TERM_Node_s FLC_MF_TERM_Node_t;


typedef struct MF_Var_s
{
   flc_Name_t              VarName[eFLC_VARNAME_WIDTH];
   flc_Index_t             VarIdx;
   flc_InOut_t             VarMIN;
   flc_InOut_t             VarMAX;
   FLC_MF_TERM_Node_t      *MF;  //Term list start
}FLC_MF_Var_t;

//Public
//extern FLC_MF_Var_t  FLC_Variables[eFLC_VARIABLE_CAPACITY];


static flc_Index_t   mf_getVar_Index (flc_Name_t * vnm);
static flc_Index_t   mf_getTerm_Index (flc_Index_t vidx, flc_Name_t * tnm);
static int           mf_get_ShapePoints (flc_Index_t vidx, flc_Index_t tidx);

static flc_Truth_t   mf_mu(flc_Index_t vidx, flc_Index_t tidx, flc_InOut_t in); 
static fcl_Exit_Status_t
                     mf_reset_mU (void);

fcl_Exit_Status_t    MF_NewVariable(flc_Name_t *nm, flc_InOut_t from, flc_InOut_t to);
fcl_Exit_Status_t    MF_NewTerm(flc_Name_t *vnm, flc_Name_t *nm, FLC_MF_Form_t ft, mf_Form_Shape_t fs[][2]);



/* ===================  Rule Functions and Types  ===================*/
typedef struct
{
   rb_NotFlag_t      GenNOT :1;
   flc_Index_t       VarIdx;
   rb_NotFlag_t      TermNOT :1;
   flc_Index_t       TermIdx;
}FLC_RL_Subc_t;

typedef struct
{
   flc_Operator_t    OP;
   FLC_RL_Subc_t     SubC_A;
   FLC_RL_Subc_t     SubC_B;
}FLC_RL_Cond_t;

typedef struct
{
   flc_Index_t       VarIdx;
   flc_Index_t       TermIdx;
   rb_Factor_t       Factor;
}FLC_RL_Concl_t;


struct RB_Rule_Node_s
{
   flc_Index_t       RuleIdx;
   FLC_RL_Cond_t     COND;
   FLC_RL_Concl_t    CONCL;
   struct RB_Rule_Node_s   *next;
};
typedef struct RB_Rule_Node_s FLC_RB_Rule_Node_t; 


typedef struct
{
   flc_Index_t          RuleBlockIdx;
   //flc_Name_t           RuleBlockName[eFLC_RULENAME_WIDTH];
   flc_AND_Algo_t       AND_Algo;
   flc_OR_Algo_t        OR_Algo;
   flc_ACCU_Method_t    ACCU_Method;
   FLC_RB_Rule_Node_t   *RB;
}FLC_RB_RuleBlock_t;

//Public
//extern FLC_RB_RuleBlock_t FLC_RuleBlock_Table[eFLC_RULEBLOCK_CAPACITY];


fcl_Exit_Status_t       RB_NewRuleBlock(flc_AND_Algo_t and, flc_OR_Algo_t or, flc_ACCU_Method_t acc);
fcl_Exit_Status_t       RB_NewRule(flc_Index_t rb_idx, FLC_RL_Cond_t cond, FLC_RL_Concl_t concl);





/* ===============   Fuzzy Logic Methods and Algorithms   ================*/

static fcl_Exit_Status_t meth_COG (flc_Index_t vidx);
static fcl_Exit_Status_t meth_COGS (flc_Index_t vidx);
static fcl_Exit_Status_t meth_MOM (flc_Index_t vidx);
static fcl_Exit_Status_t meth_LM (flc_Index_t vidx);
static fcl_Exit_Status_t meth_RM (flc_Index_t vidx);

#ifdef   __INLINE
__INLINE flc_Truth_t meth_NOT (flc_Truth_t a){
   return (1 - a);
}
__INLINE  flc_Truth_t meth_MAX (flc_Truth_t a, flc_Truth_t b){
   if (a>b) return a;
   else     return b;
}
__INLINE flc_Truth_t meth_MIN (flc_Truth_t a, flc_Truth_t b){
   if (a<b) return a;
   else     return b;
}
__INLINE flc_Truth_t meth_ASUM (flc_Truth_t a, flc_Truth_t b){
   return a + b - a*b;
}
__INLINE flc_Truth_t meth_PROD (flc_Truth_t a, flc_Truth_t b){
   return a*b;
}
__INLINE flc_Truth_t meth_BSUM (flc_Truth_t a, flc_Truth_t b){
   if (a+b>1)  return 1;
   else        return (a+b);
}
__INLINE flc_Truth_t meth_BDIF (flc_Truth_t a, flc_Truth_t b){
   if (a+b-1>0)   return a+b-1;
   else           return 0;
}
__INLINE flc_Truth_t meth_NSUM (flc_Truth_t a, flc_Truth_t b){
   return meth_BSUM (a,b);  //lag of Normalized sum TODO   
}

/* ------- Headges ------- */
__INLINE flc_Truth_t hdg_Little (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 0.3);
}
__INLINE flc_Truth_t hdg_Somewhat (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 0.5);
}
__INLINE flc_Truth_t hdg_Slightly (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 0.7);
}
__INLINE flc_Truth_t hdg_Very (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 2);
}
__INLINE flc_Truth_t hdg_Extremely (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 3);
}
__INLINE flc_Truth_t hdg_Very_Very (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 4);
}

#else

static flc_Truth_t meth_NOT(flc_Truth_t a);
static flc_Truth_t meth_MAX (flc_Truth_t a, flc_Truth_t b);
static flc_Truth_t meth_MIN (flc_Truth_t a, flc_Truth_t b);
static flc_Truth_t meth_ASUM (flc_Truth_t a, flc_Truth_t b);
static flc_Truth_t meth_PROD (flc_Truth_t a, flc_Truth_t b);
static flc_Truth_t meth_BSUM (flc_Truth_t a, flc_Truth_t b);
static flc_Truth_t meth_BDIF (flc_Truth_t a, flc_Truth_t b);
static flc_Truth_t meth_NSUM (flc_Truth_t a, flc_Truth_t b);

/* ------- Headges ------- */
static flc_Truth_t hdg_Little (flc_Truth_t mu);
static flc_Truth_t hdg_Somewhat (flc_Truth_t mu);
static flc_Truth_t hdg_Slightly (flc_Truth_t mu);
static flc_Truth_t hdg_Very (flc_Truth_t mu);
static flc_Truth_t hdg_Extremely (flc_Truth_t mu);
static flc_Truth_t hdg_Very_Very (flc_Truth_t mu);

#endif   //#ifdef   __INLINE



/* ================  Inference Functions and Types  ======================*/

//Public
fcl_Exit_Status_t FLC_Inference (void);


/* ================  Defuzzification Functions and Types  =================*/

typedef struct
{
   flc_Index_t          VarIdx;
   flc_DEFUZ_Method_t   DefMethod;
   flc_Flag_t           ThereIs :1;
}FLC_Defuzz_t;

//Public
//extern FLC_Defuzz_t FLC_Defuzz_Table[eFLC_VARIABLE_CAPACITY];


static fcl_Exit_Status_t   FLC_NewDefuzz_Variable (flc_Name_t *vnm, flc_DEFUZ_Method_t def_meth);

//Public
fcl_Exit_Status_t          FLC_Defuzzification (void);



/* =============  Embedded Fuzzy Controller Main Functions  ==============*/

#if eFLC_ENABLE_FCL == 1
fcl_Exit_Status_t FLC_Init (void);
#else
fcl_Exit_Status_t FLC_Init (FLC_IO_t *io_set);
#endif   //#if eFLC_ENABLE_FCL == 1


fcl_Exit_Status_t FCL_Loop (void);


#ifdef __cplusplus
}
#endif

#endif	//#ifndef	__eFLC_h__

