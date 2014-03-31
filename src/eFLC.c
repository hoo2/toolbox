/**

	eFLC.c - embedded Fuzzy Logic Control System

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


#include "eFLC.h"

/* ================     Data types       ======================*/

flc_InOut_t    FLC_InOutBuffer[eFLC_VARIABLE_CAPACITY];

// If no FCL define here and set from user's code
#if eFLC_ENABLE_FCL != 1
FLC_IO_t       FLC_InOutSetup[eFLC_VARIABLE_CAPACITY];
#endif




/* ================     Private Data      ======================*/

FLC_MF_Var_t         FLC_Variables[eFLC_VARIABLE_CAPACITY];
FLC_RB_RuleBlock_t   FLC_RuleBlock_Table[eFLC_RULEBLOCK_CAPACITY];
FLC_Defuzz_t         FLC_Defuzz_Table[eFLC_VARIABLE_CAPACITY];








/* =======================  Module I/O   ========================*/

flc_InOut_t io_SetOutput (flc_Name_t *nm, flc_InOut_t out)
{
   int i;
   for ( i=0 ; i<eFLC_VARIABLE_CAPACITY ; ++i)
      if (!strcmp (nm, FLC_InOutSetup[i].VarName))
         return (FLC_InOutBuffer[FLC_InOutSetup[i].BufferVarIdx] = out);
   return 0;
}

flc_InOut_t io_GetInput (flc_Name_t *nm)
{
   int i;
   for ( i=0 ; i<eFLC_VARIABLE_CAPACITY ; ++i)
      if (!strcmp (nm, FLC_InOutSetup[i].VarName))
         return FLC_InOutBuffer[FLC_InOutSetup[i].BufferVarIdx];
   return 0;
}




/* ===============     FCL File Parser Functions     ==============*/

#if eFLC_ENABLE_FCL == 1
static __file_t * fcl_loadfile (FCL_FILE *fcl)
{
   __file_t *file;

   if ((file = (__file_t*)malloc(sizeof(__file_t))) != NULL )
   {
      file->fd = (flc_Char_t*)fcl;
      file->cursor = (flc_Char_t*)fcl;
      return file;
   }
   return NULL;
}

static flc_Char_t fcl_getc (__file_t *file)
{
   return *file->cursor++;
}

static flc_Char_t* fcl_gets (flc_Char_t* str, __file_t *file)
{
   flc_Char_t ch, i=0;

   while ( (ch = fcl_getc (file)) != EOF )
   {
      str[i++] = ch;
      if (ch == ';')
      {
         str[i]='\0';
         return str;
      }
   }
   if (i)
   {
      str[i] = '\0';
      return str;
   }
   else
      return NULL;
}



/* Return pointer to first non-whitespace char in given string. */
static flc_Char_t* fcl_skip(const flc_Char_t* s)
{
    while (*s && isspace (*s))   s++;
    return (flc_Char_t*)s;
}


static flc_Char_t* fcl_gettoken (flc_Char_t* t, flc_Char_t* s, flc_Char_t** rest)
{
   int8_t i = 0;

   // Scan for first non-whitespace
   s = fcl_skip(s);

   // if alpha wait the first non-NAME
   if (isalpha (*s))
      while ( isalnum (*s) || *s == '_')           t[i++] = *s++;
   // if digit wait the first non-NUMBER
   else if (isdigit (*s))
      while (isdigit (*s) || *s == '.')            t[i++] = *s++;
   // If punct wait the first non-punct exept ;
   else if (*s == ':' || *s == '=')
      while (*s == ':' || *s == '=')               t[i++] = *s++;
   else if (*s == '.')
      while (*s == '.')                            t[i++] = *s++;

   else if (*s == '-')                             t[i++] = *s++;
   else if (*s == '+')                             t[i++] = *s++;
   else if (*s == ',')                             t[i++] = *s++;
   else if (*s == '(')                             t[i++] = *s++;
   else if (*s == ')')                             t[i++] = *s++;
   else if (*s == ';')                             t[i++] = *s++;

   t[i] = '\0';      //NULL term the token
   *rest = s;          //skip token to rest of the sentence
   return t;
}




fcl_Exit_Status_t fcl_parse (void)
{
   flc_Char_t fcl_line_buf[eFLC_FCL_LINE_WIDTH];
   __file_t *fcl = fcl_loadfile (FCL);
   enum {
      NO_OP_Block = 0, Var_Block, Fuz_Block, Defuz_Block, Rule_Block
   }FCL_Block = NO_OP_Block;
   flc_Flag_t rule_f = 0;

   flc_Char_t token[12], *sentence, *crap = NULL;
   int item, point, par, i;  //token number/order

   //temporary parser data holders
   FLC_MF_Var_t v;
   FLC_MF_TERM_Node_t t;
   FLC_Defuzz_t d;
   flc_Index_t rb_idx = -1;  //current rule block [index]
   FLC_RL_Cond_t cond;
   FLC_RL_Concl_t concl;

   /* Scan through file line by line. Line delimiter is ';' */
   while (sentence = fcl_line_buf, fcl_gets(sentence, fcl) != NULL )
   {
      fcl_gettoken (token, sentence, &crap);

      if (!strcmp (token, "VAR_INPUT") || !strcmp (token, "VAR_OUTPUT"))
         FCL_Block = Var_Block;
      else if ((!strcmp (token, "END_VAR")))
         FCL_Block = NO_OP_Block;

      else if (!strcmp (token, "FUZZIFY"))
         FCL_Block = Fuz_Block;
      else if ((!strcmp (token, "END_FUZZIFY")))
         FCL_Block = NO_OP_Block;

      else if (!strcmp (token, "DEFUZZIFY"))
         FCL_Block = Defuz_Block;
      else if ((!strcmp (token, "END_DEFUZZIFY")))
         FCL_Block = NO_OP_Block;

      else if (!strcmp (token, "RULEBLOCK"))
         FCL_Block = Rule_Block;
      else if ((!strcmp (token, "END_RULEBLOCK")))
         FCL_Block = NO_OP_Block;


      switch (FCL_Block)
      {
         case Var_Block:
            // tokenize each line untill ';'
            v.VarMIN = v.VarMAX = 1;
            item = 0;
            while (strcmp (fcl_gettoken (token, sentence, &sentence), ";"))
               switch (item++)
               {
                  case 0:
                     if ( !strcmp (token, "VAR_INPUT")  || !strcmp (token, "VAR_OUTPUT") )
                        --item;
                     else
                        strcpy (v.VarName, token);
                     break;
                  case 1:
                     if ( strcmp (token, "RANGE") )   return FLC_PARSE_ERROR;
                     break;
                  case 2:
                     if ( strcmp (token, "(") )       return FLC_PARSE_ERROR;
                     break;
                  case 3:
                     if ( !strcmp (token, "-"))       { v.VarMIN *= -1; --item; }
                     else if ( !strcmp (token, "+"))  --item;
                     else                             v.VarMIN *= atof (token);
                     break;
                  case 4:
                     if ( strcmp (token, "..") )      return FLC_PARSE_ERROR;
                     break;
                  case 5:
                     if ( !strcmp (token, "-"))       { v.VarMAX *= -1; --item; }
                     else if (!strcmp (token, "+"))   --item;
                     else                             v.VarMAX *= atof (token);
                     break;
                  case 6:
                     if ( strcmp (token, ")") )       return FLC_PARSE_ERROR;
                     break;
                  default:
                     return FLC_PARSE_ERROR;
               }
            if (MF_NewVariable (v.VarName, v.VarMIN, v.VarMAX) != FLC_SUCCESS)
               return FLC_MEM_ERROR;
            break;
         case Fuz_Block:
            // tokenize each line untill ';'
            for (i=0 ; i<eFLC_FORM_SHAPE_POINTS ; ++i)
               t.MF_Form_Shape[i][0] = t.MF_Form_Shape[i][1] = 1;
            point = 0;
            item = 0;
            while (strcmp (fcl_gettoken (token, sentence, &sentence), ";"))
               switch (item++)
               {
                  case 0:
                     if ( !strcmp (token, "FUZZIFY") )
                     {
                        strcpy (v.VarName, fcl_gettoken (token, sentence, &sentence));
                        --item;
                     }
                     else if (strcmp (token, "TERM"))
                        return FLC_PARSE_ERROR;
                     break;
                  case 1:
                     strcpy (t.TermName, token);      break;
                  case 2:
                     if (!strcmp (token, "SHOULDER"))          t.MF_FormType = Shoulder;
                     else if (!strcmp (token, "TRAPEZOIDAL"))  t.MF_FormType = Trapezoidal;
                     else if (!strcmp (token, "TRIANGLE"))     t.MF_FormType = Triangle;
                     else if (!strcmp (token, "RECTANGLE"))    t.MF_FormType = Rectangle;
                     else if (!strcmp (token, "SINGLETON"))    t.MF_FormType = Singleton;
                     else  return FLC_PARSE_ERROR;
                     break;
                  case 3:
                     if ( strcmp (token, ":=") )      return FLC_PARSE_ERROR;
                     break;
                  case 4:
                     if ( !strcmp (token, "(") )      break;
                     else if ( strcmp (token, ";") && point)
                        { item = 9; break; }
                     else
                        return FLC_PARSE_ERROR;
                  case 5:
                     if (!strcmp (token, "-"))        { t.MF_Form_Shape[point][0] *= -1; --item; }
                     else if (!strcmp (token, "+"))   --item;
                     else                             t.MF_Form_Shape[point][0] *= atof (token);
                     break;
                  case 6:
                     if ( strcmp (token, ",") )       return FLC_PARSE_ERROR;
                     break;
                  case 7:
                     if (!strcmp (token, "-"))        { t.MF_Form_Shape[point][1] *= -1; --item; }
                     else if (!strcmp (token, "+"))   --item;
                     else                             t.MF_Form_Shape[point][1] *= atof (token);
                     break;
                  case 8:
                     if ( strcmp (token, ")") )       return FLC_PARSE_ERROR;
                     else                             { ++point; item = 4; }
                     break;
                  case 9:
                     break;
                  default:
                     return FLC_PARSE_ERROR;
               }
            if (MF_NewTerm (v.VarName, t.TermName, t.MF_FormType, t.MF_Form_Shape) != FLC_SUCCESS)
               return FLC_MEM_ERROR;
            break;

         case Defuz_Block:
            item = 0;
            while (strcmp (fcl_gettoken (token, sentence, &sentence), ";"))
               switch (item++)
               {
                  case 0:
                     if ( !strcmp (token, "DEFUZZIFY") )
                     {
                        strcpy (v.VarName, fcl_gettoken (token, sentence, &sentence));
                        --item;
                     }
                     else if (strcmp (token, "METHOD"))
                        return FLC_PARSE_ERROR;
                     break;
                  case 1:
                     if ( strcmp (token, ":") )       return FLC_PARSE_ERROR;
                     break;
                  case 2:
                     if (!strcmp (token, "COG"))         d.DefMethod = COG;
                     else if (!strcmp (token, "COGS"))   d.DefMethod = COGS;
                     else if (!strcmp (token, "COA"))    d.DefMethod = COA;
                     else if (!strcmp (token, "MOM"))    d.DefMethod = MOM;
                     else if (!strcmp (token, "LM"))     d.DefMethod = LM;
                     else if (!strcmp (token, "RM"))     d.DefMethod = RM;
                     else  return FLC_PARSE_ERROR;
                     break;
                  case 3:
                     break;
               }
            FLC_NewDefuzz_Variable (v.VarName, d.DefMethod);
            break;
         case Rule_Block:
            item = 0;
            par = 0;
            rule_f = 0;
            cond.OP = NO_OP;
            cond.SubC_A.GenNOT = cond.SubC_A.VarIdx = cond.SubC_A.TermNOT = cond.SubC_A.TermIdx = 0;
            cond.SubC_B.GenNOT = cond.SubC_B.VarIdx = cond.SubC_B.TermNOT = cond.SubC_B.TermIdx = 0;
            concl.VarIdx = concl.TermIdx = 0; concl.Factor = 1;

            while (strcmp (fcl_gettoken (token, sentence, &sentence), ";"))
            {
               if ( !strcmp (token, "RULEBLOCK") )
               {
                  fcl_gettoken (token, sentence, &sentence); //discard rule block name
                  ++rb_idx;   //New rule block
                  //Load defaults
                  if (RB_NewRuleBlock(MIN, MAX, ACCU_MAX) != FLC_SUCCESS)
                     return FLC_MEM_ERROR;
                  continue;
               }
               else if (!strcmp (token, "AND") && !rule_f ) //Be aware for RULE operator AND
               {
                  if ( strcmp (fcl_gettoken (token, sentence, &sentence), ":") )  //discard ':'
                     return FLC_PARSE_ERROR;
                  fcl_gettoken (token, sentence, &sentence); //Get method name
                  if (!strcmp (token, "MIN"))         FLC_RuleBlock_Table[rb_idx].AND_Algo = MIN;
                  else if (!strcmp (token, "PROD"))   FLC_RuleBlock_Table[rb_idx].AND_Algo = PROD;
                  else if (!strcmp (token, "BDIF"))   FLC_RuleBlock_Table[rb_idx].AND_Algo = BDIF;
                  continue;
               }
               else if (!strcmp (token, "OR") && !rule_f) //Be aware for RULE operator OR
               {
                  if ( strcmp (fcl_gettoken (token, sentence, &sentence), ":") )  //discard ':'
                     return FLC_PARSE_ERROR;
                  fcl_gettoken (token, sentence, &sentence); //Get method name
                  if (!strcmp (token, "MAX"))         FLC_RuleBlock_Table[rb_idx].OR_Algo = MAX;
                  else if (!strcmp (token, "ASUM"))   FLC_RuleBlock_Table[rb_idx].OR_Algo = ASUM;
                  else if (!strcmp (token, "BSUM"))   FLC_RuleBlock_Table[rb_idx].OR_Algo = BSUM;
                  continue;
               }
               else if (!strcmp (token, "ACCU"))
               {
                  if ( strcmp (fcl_gettoken (token, sentence, &sentence), ":") )  //discard ':'
                     return FLC_PARSE_ERROR;
                  fcl_gettoken (token, sentence, &sentence); //Get method name
                  if (!strcmp (token, "ACCU_MAX"))         FLC_RuleBlock_Table[rb_idx].ACCU_Method = ACCU_MAX;
                  else if (!strcmp (token, "ACCU_BSUM"))   FLC_RuleBlock_Table[rb_idx].ACCU_Method = ACCU_BSUM;
                  else if (!strcmp (token, "ACCU_NSUM"))   FLC_RuleBlock_Table[rb_idx].ACCU_Method = ACCU_NSUM;
                  continue;
               }
               switch (item++)
               {
                  case 0:
                     if ( strcmp (token, "RULE") )       return FLC_PARSE_ERROR;
                     else
                     {
                        fcl_gettoken (token, sentence, &sentence); //discard rule number
                        rule_f = 1;    //Enter Rule mode (AND - OR operator lock)
                     }
                     break;
                  case 1:
                     if ( strcmp (token, ":") )          return FLC_PARSE_ERROR;
                     break;
                  case 2:
                     if ( strcmp (token, "IF") )         return FLC_PARSE_ERROR;
                     break;
                  case 3:
                     if ( strcmp (token, "(") )         return FLC_PARSE_ERROR;
                     else ++par;
                     break;
                  case 4:
                     if ( !strcmp (token, "NOT") )      { cond.SubC_A.GenNOT = 1;  --item; }
                     else if ( !strcmp (token, "(") && cond.SubC_A.GenNOT)       { ++par; --item;}
                     else  cond.SubC_A.VarIdx = mf_getVar_Index (token);
                     break;
                  case 5:
                     if ( strcmp (token, "IS") )         return FLC_PARSE_ERROR;
                     break;
                  case 6:
                     if ( !strcmp (token, "NOT") )      { cond.SubC_A.TermNOT = 1;  --item; }
                     else if ( !strcmp (token, "(")  && cond.SubC_A.TermNOT)     { ++par; --item; }
                     else  cond.SubC_A.TermIdx = mf_getTerm_Index (cond.SubC_A.VarIdx, token);
                     break;
                  case 7:
                     if ( !strcmp (token, ")") && (par-1))   { --item; --par;}
                     break;
                  case 8:
                     if ( !strcmp (token, "AND") )       cond.OP = AND;
                     else if ( !strcmp (token, "OR") )   cond.OP = OR;
                     else if ( !strcmp (token, "THEN") ) { cond.OP = NO_OP; item = 15; }
                     else
                        return FLC_PARSE_ERROR;
                     break;
                  case 9:
                     par = 0;
                     if ( strcmp (token, "(") )         return FLC_PARSE_ERROR;
                     else ++par;
                     break;
                  case 10:
                     if ( !strcmp (token, "NOT") )      { cond.SubC_B.GenNOT = 1;  --item; }
                     else if ( !strcmp (token, "(") && cond.SubC_B.GenNOT)       { ++par; --item;}
                     else  cond.SubC_B.VarIdx = mf_getVar_Index (token);
                     break;
                  case 11:
                     if ( strcmp (token, "IS") )         return FLC_PARSE_ERROR;
                     break;
                  case 12:
                     if ( !strcmp (token, "NOT") )      { cond.SubC_B.TermNOT = 1;  --item; }
                     else if ( !strcmp (token, "(")  && cond.SubC_B.TermNOT)     { ++par; --item; }
                     else  cond.SubC_B.TermIdx = mf_getTerm_Index (cond.SubC_B.VarIdx, token);
                     break;
                  case 13:
                     if ( !strcmp (token, ")") && (par-1))   { --item; --par;}
                     break;
                  case 14:
                     if ( strcmp (token, "THEN") )         return FLC_PARSE_ERROR;
                     break;
                  case 15:
                     if ( strcmp (token, "(") )         return FLC_PARSE_ERROR;
                     break;
                  case 16:
                     concl.VarIdx = mf_getVar_Index (token);
                     break;
                  case 17:
                     if ( strcmp (token, "IS") )         return FLC_PARSE_ERROR;
                     break;
                  case 18:
                     concl.TermIdx = mf_getTerm_Index (concl.VarIdx, token);
                     break;
                  case 19:
                     if ( !strcmp (token, ")") )
                     {
                        if (RB_NewRule(rb_idx, cond, concl) != FLC_SUCCESS)
                           return FLC_MEM_ERROR;
                     }
                     else if ( !strcmp (token, "WITH") )
                     {
                        concl.Factor = atof (fcl_gettoken (token, sentence, &sentence));
                        --item;
                     }
                     else
                        return FLC_PARSE_ERROR;
                     break;
                  case 20:
                     break;
               }
            }
            break;
      }
    }
    return FLC_SUCCESS;
}

#endif   //#if eFLC_ENABLE_FCL == 1




/* ================     Variable Functions     ======================*/

static flc_Index_t mf_getVar_Index (flc_Name_t * vnm)
{
   flc_Index_t i;
   for (i=0 ; i<eFLC_VARIABLE_CAPACITY ; ++i)
      if (!strcmp(FLC_Variables[i].VarName, vnm))
         return FLC_Variables[i].VarIdx;
   return (flc_Index_t) -1;
}

static flc_Index_t mf_getTerm_Index (flc_Index_t vidx, flc_Name_t * tnm)
{
   FLC_MF_TERM_Node_t *node;
   //Seek tnm(desired name) in list @ index vidx
   node = FLC_Variables[vidx].MF;
   while (node)
   {
      if (!strcmp (node->TermName, tnm))
         return node->TermIdx;
      node = node->next;
   }
   return (flc_Index_t) -1;
}

static int mf_get_ShapePoints (flc_Index_t vidx, flc_Index_t tidx)
{
   FLC_MF_TERM_Node_t *node = FLC_Variables[vidx].MF;

   //Seek Term Index into Term list
   while (node && (node->TermIdx != tidx))    node = node->next;
   if (!node)  return 0;   //If no termidx return 0 points for error

   // Learn how many poits are useful in node
   switch (node->MF_FormType)
   {
      default:
      case Shoulder:
      case Triangle:    return 3;
      case Trapezoidal: return 4;
      case Rectangle:   return 2;
      case Singleton:   return 1;
   }
}

static flc_Truth_t
mf_mu(flc_Index_t vidx, flc_Index_t tidx, flc_InOut_t in)
{
   int i, points;
   float Px, Py, P2Px, P2Py;   //point P[x,y] and Point-2-point [x,y]
   float dx;
   flc_Truth_t mu;
   FLC_MF_TERM_Node_t *node = FLC_Variables[vidx].MF;

   //Seek Term Index into Term list
   while (node && (node->TermIdx != tidx))
      node = node->next;
   if (!node)  return -1;   //If no termidx return -1 for error

   //Seek Where InValue belongs among the x-axis
   points = mf_get_ShapePoints(vidx, tidx);
   if ( in <= node->MF_Form_Shape[0][0] || in >= node->MF_Form_Shape[points-1][0] )
       return 0;
   else
   {
      for (i=0 ; i<points ; ++i)
         if ( in < node->MF_Form_Shape[i][0] )
         {
            Px = node->MF_Form_Shape[i-1][0];
            Py = node->MF_Form_Shape[i-1][1];
            P2Px = node->MF_Form_Shape[i][0] - Px;
            P2Py = node->MF_Form_Shape[i][1] - Py;
            dx = in - Px;
            mu = Py + dx * P2Py / P2Px;
            return mu;
         }
      return -1;  //Error with points
   }
}


static fcl_Exit_Status_t mf_reset_mU (void)
{
   int i;
   FLC_MF_TERM_Node_t *node;

   // Scan every non NULL Variable
   for (i=0 ; i<eFLC_VARIABLE_CAPACITY && FLC_Variables[i].MF; ++i)
      for (node = FLC_Variables[i].MF ; node ; node=node->next)
         node->mU = 0;
   return FLC_SUCCESS;
}



fcl_Exit_Status_t
MF_NewVariable(flc_Name_t *nm, flc_InOut_t from, flc_InOut_t to)
{
   int i;
   // Seek first empty spot (empty name)
   for (i=0 ; (i<eFLC_VARIABLE_CAPACITY) && *FLC_Variables[i].VarName ; ++i);
   // Create tVariable
   if ( i<eFLC_VARIABLE_CAPACITY )
   {
      strcpy (FLC_Variables[i].VarName, nm); //Get name
      FLC_Variables[i].VarIdx = i;           // Index same as array position
      FLC_Variables[i].VarMIN = from;        // Get Range
      FLC_Variables[i].VarMAX = to;
      FLC_Variables[i].MF = NULL;            // NULL indicates last item
      return FLC_SUCCESS;
   }
   return FLC_MEM_ERROR;
}


fcl_Exit_Status_t
MF_NewTerm(flc_Name_t *vnm, flc_Name_t *tnm, FLC_MF_Form_t ft, mf_Form_Shape_t fs[][2])
{
   FLC_MF_TERM_Node_t *node, *new_node;
   int i,j,d=0, new_list = 0;

   //Seek last object (the one includes Zeros)
   if ((node = FLC_Variables[mf_getVar_Index(vnm)].MF) == NULL)
      new_list = 1;
   else
   {
      ++d;
      while (node->next)
      {
         ++d;  //for indexing
         node = node->next;
      }
   }
   //Make new Term node
   if ((new_node = (FLC_MF_TERM_Node_t *) malloc(sizeof(FLC_MF_TERM_Node_t)) ) != NULL)
   {
      // Get Values
      strcpy (new_node->TermName, tnm);
      new_node->TermIdx = d;
      new_node->MF_FormType = ft;
      for (i=0 ; i<eFLC_FORM_SHAPE_POINTS ; ++i)
         for (j=0 ; j<2 ; ++j)
            new_node->MF_Form_Shape[i][j] = fs[i][j];
      //Link the new data
      if (new_list)
         FLC_Variables[mf_getVar_Index(vnm)].MF = new_node;
      else
         node->next = new_node;
      new_node->next = NULL;   //NULL indicates last item
      return FLC_SUCCESS;
   }
   else
      return FLC_MEM_ERROR;
}









/* ===================     Rule Functions      ===================*/
fcl_Exit_Status_t
RB_NewRuleBlock(flc_AND_Algo_t and, flc_OR_Algo_t or, flc_ACCU_Method_t acc)
{
   int i;

   // Seek first empty spot (NULL RB pointer)
   for (i=0 ; (i<eFLC_RULEBLOCK_CAPACITY) && FLC_RuleBlock_Table[i].RB ; ++i);
   // Create RuleBlock
   if (i<eFLC_RULEBLOCK_CAPACITY)
   {
      //Table Data set
      FLC_RuleBlock_Table[i].RuleBlockIdx = i;     //Index same as array position
      FLC_RuleBlock_Table[i].AND_Algo = and;
      FLC_RuleBlock_Table[i].OR_Algo = or;
      FLC_RuleBlock_Table[i].ACCU_Method = acc;
      FLC_RuleBlock_Table[i].RB = NULL;            //NULL indicates last item
      return FLC_SUCCESS;
   }
   return FLC_MEM_ERROR;

}


fcl_Exit_Status_t
RB_NewRule(flc_Index_t rb_idx, FLC_RL_Cond_t cond, FLC_RL_Concl_t concl)
{
   FLC_RB_Rule_Node_t *node, *new_node;
   int d=0, new_list = 0;

   //Seek last object (the one includes Zeros)
   if ((node = FLC_RuleBlock_Table[rb_idx].RB) == NULL)
      new_list = 1;
   else
   {
      ++d;
      while (node->next)
      {
         ++d;  //for indexing
         node = node->next;
      }
   }
   //Make new Rule node
   if ((new_node = (FLC_RB_Rule_Node_t *) malloc(sizeof(FLC_RB_Rule_Node_t)) ) !=NULL)
   {
      // Get Values
      new_node->RuleIdx = d;
      new_node->COND = cond;
      new_node->CONCL = concl;
      //Link the new data
      if (new_list)
         FLC_RuleBlock_Table[rb_idx].RB = new_node;
      else
         node->next = new_node;
      new_node->next = NULL;    //NULL indicates last item
      return FLC_SUCCESS;
   }
   else
      return FLC_MEM_ERROR;
}




/* ===============   Fuzzy Logic Methods and Algorithms   ==============*/

/**
   static fcl_Exit_Status_t meth_COG (flc_Index_t vidx)

  * @brief  This function Enable and initialize
            and calibrates the ADC.
  * @param  none
  * @retval none
*/
static fcl_Exit_Status_t meth_COG (flc_Index_t vidx)
{
   FLC_MF_TERM_Node_t *node;
   flc_InOut_t step, v, envelop_val, mu_v, Int_Num = 0, Int_Denum = 0;  //Accumulators for Integral

   // For each step get envelop of mU and calculate Numerator/Denumerator
   step = (FLC_Variables[vidx].VarMAX - FLC_Variables[vidx].VarMIN)/eFLC_DEFUZZmeth_COG_STEPS;
   for ( v = FLC_Variables[vidx].VarMIN ; v < FLC_Variables[vidx].VarMAX ; v += step)
   {
      //Get Envelop Value
      node = FLC_Variables[vidx].MF;
      envelop_val = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU);
      do{
         if ( (mu_v = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU)) > envelop_val )
            envelop_val = mu_v;
         node = node->next;
      } while (node);

      //Accumulate
      Int_Num += envelop_val * v;
      Int_Denum += envelop_val;
   }
   // Calc the COG
   if (Int_Denum)
      io_SetOutput (FLC_Variables[vidx].VarName, (Int_Num / Int_Denum));
   else
      io_SetOutput (FLC_Variables[vidx].VarName, 0);
   return FLC_SUCCESS;
}

static fcl_Exit_Status_t meth_COGS (flc_Index_t vidx)
{
   FLC_MF_TERM_Node_t *node;
   flc_InOut_t Int_Num = 0, Int_Denum = 0;  //Accumulators for Integral

   // For each Singleton's mU in Term's list calculate Numerator/Denumerator
   for (node = FLC_Variables[vidx].MF ; node ; node = node->next)
      if (node->MF_FormType == Singleton)
      {
         Int_Num += node->mU * node->MF_Form_Shape[0][0];
         Int_Denum += node->mU;
      }
   // Calc the COGS
   if (Int_Denum)
      io_SetOutput (FLC_Variables[vidx].VarName, (Int_Num / Int_Denum));
   else
      io_SetOutput (FLC_Variables[vidx].VarName, 0);
   return FLC_SUCCESS;
}

static fcl_Exit_Status_t meth_MOM (flc_Index_t vidx)
{
   FLC_MF_TERM_Node_t *node;
   flc_InOut_t step, v, envelop_val, mu_v;
   flc_InOut_t lm, rm, max;

   // For each step get envelop of mU and calculate Numerator/Denumerator
   step = (FLC_Variables[vidx].VarMAX - FLC_Variables[vidx].VarMIN)/eFLC_DEFUZZmeth_COG_STEPS;
   lm = rm = max = 0;
   for ( v = FLC_Variables[vidx].VarMIN ; v < FLC_Variables[vidx].VarMAX ; v += step)
   {
      //Get Envelop Value
      node = FLC_Variables[vidx].MF;
      envelop_val = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU);
      do{
         if ( (mu_v = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU)) > envelop_val )
            envelop_val = mu_v;
         node = node->next;
      } while (node);

      //Get maximum
      if (max < envelop_val)
      {
         max = envelop_val;
         lm = v;
      }
      if (max == envelop_val && max)
         rm = v;
   }
   io_SetOutput (FLC_Variables[vidx].VarName, ((rm + lm) / 2));
   return FLC_SUCCESS;
}


static fcl_Exit_Status_t meth_LM (flc_Index_t vidx)
{
   FLC_MF_TERM_Node_t *node;
   flc_InOut_t step, v, envelop_val, mu_v;
   flc_InOut_t lm, max;

   // For each step get envelop of mU and calculate Numerator/Denumerator
   step = (FLC_Variables[vidx].VarMAX - FLC_Variables[vidx].VarMIN)/eFLC_DEFUZZmeth_COG_STEPS;
   lm = max = 0;
   for ( v = FLC_Variables[vidx].VarMIN ; v < FLC_Variables[vidx].VarMAX ; v += step)
   {
      //Get Envelop Value
      node = FLC_Variables[vidx].MF;
      envelop_val = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU);
      do{
         if ( (mu_v = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU)) > envelop_val )
            envelop_val = mu_v;
         node = node->next;
      } while (node);

      //Get maximum
      if (max < envelop_val)
      {
         max = envelop_val;
         lm = v;
      }
   }
   io_SetOutput (FLC_Variables[vidx].VarName, lm);
   return FLC_SUCCESS;
}

static fcl_Exit_Status_t meth_RM (flc_Index_t vidx)
{
   FLC_MF_TERM_Node_t *node;
   flc_InOut_t step, v, envelop_val, mu_v;
   flc_InOut_t rm, max;

   // For each step get envelop of mU and calculate Numerator/Denumerator
   step = (FLC_Variables[vidx].VarMAX - FLC_Variables[vidx].VarMIN)/eFLC_DEFUZZmeth_COG_STEPS;
   rm = max = 0;
   for ( v = FLC_Variables[vidx].VarMIN ; v < FLC_Variables[vidx].VarMAX ; v += step)
   {
      //Get Envelop Value
      node = FLC_Variables[vidx].MF;
      envelop_val = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU);
      do{
         if ( (mu_v = meth_MIN (mf_mu (vidx, node->TermIdx, v), node->mU)) > envelop_val )
            envelop_val = mu_v;
         node = node->next;
      } while (node);

      //Get maximum
      if (max < envelop_val)
         max = envelop_val;
      if (max == envelop_val && max)
         rm = v;
   }
   io_SetOutput (FLC_Variables[vidx].VarName, rm);
   return FLC_SUCCESS;
}


#ifndef   __INLINE
static flc_Truth_t meth_NOT(flc_Truth_t a){
   return (1 - a);
}
static flc_Truth_t meth_MAX (flc_Truth_t a, flc_Truth_t b){
   if (a>b) return a;
   else     return b;
}
static flc_Truth_t meth_MIN (flc_Truth_t a, flc_Truth_t b){
   if (a<b) return a;
   else     return b;
}
static flc_Truth_t meth_ASUM (flc_Truth_t a, flc_Truth_t b){
   return (a + b - a*b);
}
static flc_Truth_t meth_PROD (flc_Truth_t a, flc_Truth_t b){
   return a*b;
}
static flc_Truth_t meth_BSUM (flc_Truth_t a, flc_Truth_t b){
   if (a+b>1)  return 1;
   else        return (a+b);
}
static flc_Truth_t meth_BDIF (flc_Truth_t a, flc_Truth_t b){
   if (a+b-1>0)   return a+b-1;
   else           return 0;
}
static flc_Truth_t meth_NSUM (flc_Truth_t a, flc_Truth_t b){
   return meth_BSUM(a,b);
}

/* ------- Headges ------- */
static flc_Truth_t hdg_Little (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 0.3);
}
static flc_Truth_t hdg_Somewhat (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 0.5);
}
static flc_Truth_t hdg_Slightly (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 0.7);
}
static flc_Truth_t hdg_Very (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 2);
}
static flc_Truth_t hdg_Extremely (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 3);
}
static flc_Truth_t hdg_Very_Very (flc_Truth_t mu){
   return (flc_Truth_t)pow(mu, 4);
}
#endif   //#ifndef   __INLINE





/* ================     Inference Functions      ======================*/

fcl_Exit_Status_t FLC_Inference (void)
{
   int i;
   flc_Truth_t mu=0, mu_a = 0, mu_b = 0;
   flc_InOut_t in;  //Capture input variables
   FLC_RB_Rule_Node_t *r_node;
   FLC_MF_TERM_Node_t *t_node;
   ACCU_Method_pt accu_meth;
   OP_Method_pt   and_algo, or_algo;

   // Scan every non NULL RuleBlock
   mf_reset_mU ();
   for (i=0 ; i<eFLC_RULEBLOCK_CAPACITY && FLC_RuleBlock_Table[i].RB; ++i)
   {
      r_node = FLC_RuleBlock_Table[i].RB;
      // Methods assignment
      switch (FLC_RuleBlock_Table[i].AND_Algo)
      {
         case MIN:
         default:    and_algo = meth_MIN; break;
         case PROD:  and_algo = meth_PROD; break;
         case BDIF:  and_algo = meth_BDIF; break;
      }
      switch (FLC_RuleBlock_Table[i].OR_Algo)
      {
         case MAX:
         default:    or_algo = meth_MAX; break;
         case ASUM:  or_algo = meth_ASUM; break;
         case BSUM:  or_algo = meth_BSUM; break;
      }
      switch (FLC_RuleBlock_Table[i].ACCU_Method)
      {
         case ACCU_MAX:
         default:          accu_meth = meth_MAX; break;
         case ACCU_BSUM:   accu_meth = meth_BSUM; break;
         case ACCU_NSUM:   accu_meth = meth_NSUM; break;
      }

      // Scan every r_node in the list @ RuleBlock[i]
      while (r_node)
      {
         // Calc sub-conditions
         if (r_node->COND.OP != NO_OP)
         {
            in = io_GetInput (FLC_Variables[r_node->COND.SubC_B.VarIdx].VarName);   //Capture In Value
            mu_b = mf_mu (r_node->COND.SubC_B.VarIdx, r_node->COND.SubC_B.TermIdx, in);
            if (r_node->COND.SubC_B.TermNOT)   mu_b = meth_NOT (mu_b);
            if (r_node->COND.SubC_B.GenNOT)    mu_b = meth_NOT (mu_b);
         }
         in = io_GetInput (FLC_Variables[r_node->COND.SubC_A.VarIdx].VarName);   //Capture In Value
         mu_a = mf_mu (r_node->COND.SubC_A.VarIdx, r_node->COND.SubC_A.TermIdx, in);
         if (r_node->COND.SubC_A.TermNOT)   mu_a = meth_NOT (mu_a);
         if (r_node->COND.SubC_A.GenNOT)    mu_a = meth_NOT (mu_a);

         // Calculate result of COND
         switch (r_node->COND.OP)
         {
            default:
            case NO_OP: mu = r_node->CONCL.Factor * mu_a; break;
            case NOT:   mu = r_node->CONCL.Factor * meth_NOT(mu_a); break;
            case AND:   mu = r_node->CONCL.Factor * and_algo(mu_a, mu_b); break;
            case OR:    mu = r_node->CONCL.Factor * or_algo(mu_a, mu_b); break;
         }

         // Accumulate to Conclusion

         //Seek Term Index into Term list
         t_node = FLC_Variables[r_node->CONCL.VarIdx].MF;
         while (t_node && (t_node->TermIdx != r_node->CONCL.TermIdx))
            t_node = t_node->next;
         if (!t_node)   //Error
            return FLC_INF_ERROR;
         t_node->mU = accu_meth (t_node->mU, mu);

         //Next r_node
         r_node = r_node->next;
      }
   }
   return FLC_SUCCESS;
}




/* ================     Defuzzification Functions     ======================*/

static fcl_Exit_Status_t FLC_NewDefuzz_Variable (flc_Name_t *vnm, flc_DEFUZ_Method_t def_meth)
{
   int i;
   // Scan Defuzz Table for empty spot
   for (i=0 ; i<eFLC_DEFUZZ_CAPACITY && FLC_Defuzz_Table[i].ThereIs; ++i);

   //New Entry
   if (i<eFLC_DEFUZZ_CAPACITY)
   {
      FLC_Defuzz_Table[i].VarIdx = mf_getVar_Index (vnm);
      FLC_Defuzz_Table[i].DefMethod = def_meth;
      FLC_Defuzz_Table[i].ThereIs = 1;
      return FLC_SUCCESS;
   }
   else
      return FLC_MEM_ERROR;
}

fcl_Exit_Status_t FLC_Defuzzification (void)
{
   int i;
   // Seek the existing defuzz requests
   for (i=0 ; i<eFLC_DEFUZZ_CAPACITY && FLC_Defuzz_Table[i].ThereIs; ++i)
      switch (FLC_Defuzz_Table[i].DefMethod)
      {
         default:
         case COG:
         case COA:
            if (meth_COG (FLC_Defuzz_Table[i].VarIdx) == FLC_DEF_ERROR)  return FLC_DEF_ERROR;
            else  break;
         case COGS:
            if (meth_COGS (FLC_Defuzz_Table[i].VarIdx) == FLC_DEF_ERROR) return FLC_DEF_ERROR;
            else  break;
         case MOM:
            if (meth_MOM (FLC_Defuzz_Table[i].VarIdx) == FLC_DEF_ERROR)  return FLC_DEF_ERROR;
            else  break;
         case LM:
            if (meth_LM (FLC_Defuzz_Table[i].VarIdx) == FLC_DEF_ERROR)  return FLC_DEF_ERROR;
            else  break;
         case RM:
            if (meth_RM (FLC_Defuzz_Table[i].VarIdx) == FLC_DEF_ERROR)  return FLC_DEF_ERROR;
            else  break;
      }
   return FLC_SUCCESS;
}


/* =============  Embedded Fuzzy Controller Main Functions  ==============*/

#if eFLC_ENABLE_FCL == 1
fcl_Exit_Status_t FLC_Init (void)
{
   return fcl_parse ();
}
#else
fcl_Exit_Status_t FLC_Init (FLC_IO_t *io_set)
{
   int i;
   for (i=0 ; *io_set[i].VarName && i<eFLC_VARIABLE_CAPACITY; ++i)
   {
      FLC_InOutSetup[i].BufferVarIdx = io_set[i].BufferVarIdx;
      strcpy (FLC_InOutSetup[i].VarName, io_set[i].VarName);
   }
   return FLC_SUCCESS0;
}
#endif   //#if eFLC_ENABLE_FCL == 1

fcl_Exit_Status_t FCL_Loop (void)
{
   fcl_Exit_Status_t ret;
   if ((ret = FLC_Inference ()) != FLC_SUCCESS)
      return ret;
   if ((ret = FLC_Defuzzification ()) != FLC_SUCCESS)
      return ret;
   return FLC_SUCCESS;
}


