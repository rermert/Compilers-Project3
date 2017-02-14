/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_Void = 258,
     T_Bool = 259,
     T_Int = 260,
     T_Float = 261,
     T_LessEqual = 262,
     T_GreaterEqual = 263,
     T_EQ = 264,
     T_NE = 265,
     T_LeftAngle = 266,
     T_RightAngle = 267,
     T_And = 268,
     T_Or = 269,
     T_Equal = 270,
     T_MulAssign = 271,
     T_DivAssign = 272,
     T_AddAssign = 273,
     T_SubAssign = 274,
     T_While = 275,
     T_For = 276,
     T_If = 277,
     T_Else = 278,
     T_Return = 279,
     T_Break = 280,
     T_Const = 281,
     T_Uniform = 282,
     T_Layout = 283,
     T_Continue = 284,
     T_Do = 285,
     T_Inc = 286,
     T_Dec = 287,
     T_Switch = 288,
     T_Case = 289,
     T_Default = 290,
     T_In = 291,
     T_Out = 292,
     T_InOut = 293,
     T_Mat2 = 294,
     T_Mat3 = 295,
     T_Mat4 = 296,
     T_Vec2 = 297,
     T_Vec3 = 298,
     T_Vec4 = 299,
     T_Ivec2 = 300,
     T_Ivec3 = 301,
     T_Ivec4 = 302,
     T_Bvec2 = 303,
     T_Bvec3 = 304,
     T_Bvec4 = 305,
     T_Uint = 306,
     T_Uvec2 = 307,
     T_Uvec3 = 308,
     T_Uvec4 = 309,
     T_Struct = 310,
     T_Semicolon = 311,
     T_Dot = 312,
     T_Colon = 313,
     T_Question = 314,
     T_Comma = 315,
     T_Dash = 316,
     T_Plus = 317,
     T_Star = 318,
     T_Slash = 319,
     T_LeftParen = 320,
     T_RightParen = 321,
     T_LeftBracket = 322,
     T_RightBracket = 323,
     T_LeftBrace = 324,
     T_RightBrace = 325,
     T_Identifier = 326,
     T_IntConstant = 327,
     T_FloatConstant = 328,
     T_BoolConstant = 329
   };
#endif
/* Tokens.  */
#define T_Void 258
#define T_Bool 259
#define T_Int 260
#define T_Float 261
#define T_LessEqual 262
#define T_GreaterEqual 263
#define T_EQ 264
#define T_NE 265
#define T_LeftAngle 266
#define T_RightAngle 267
#define T_And 268
#define T_Or 269
#define T_Equal 270
#define T_MulAssign 271
#define T_DivAssign 272
#define T_AddAssign 273
#define T_SubAssign 274
#define T_While 275
#define T_For 276
#define T_If 277
#define T_Else 278
#define T_Return 279
#define T_Break 280
#define T_Const 281
#define T_Uniform 282
#define T_Layout 283
#define T_Continue 284
#define T_Do 285
#define T_Inc 286
#define T_Dec 287
#define T_Switch 288
#define T_Case 289
#define T_Default 290
#define T_In 291
#define T_Out 292
#define T_InOut 293
#define T_Mat2 294
#define T_Mat3 295
#define T_Mat4 296
#define T_Vec2 297
#define T_Vec3 298
#define T_Vec4 299
#define T_Ivec2 300
#define T_Ivec3 301
#define T_Ivec4 302
#define T_Bvec2 303
#define T_Bvec3 304
#define T_Bvec4 305
#define T_Uint 306
#define T_Uvec2 307
#define T_Uvec3 308
#define T_Uvec4 309
#define T_Struct 310
#define T_Semicolon 311
#define T_Dot 312
#define T_Colon 313
#define T_Question 314
#define T_Comma 315
#define T_Dash 316
#define T_Plus 317
#define T_Star 318
#define T_Slash 319
#define T_LeftParen 320
#define T_RightParen 321
#define T_LeftBracket 322
#define T_RightBracket 323
#define T_LeftBrace 324
#define T_RightBrace 325
#define T_Identifier 326
#define T_IntConstant 327
#define T_FloatConstant 328
#define T_BoolConstant 329




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 41 "parser.y"
{
    int integerConstant;
    bool boolConstant;
    float floatConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
    Decl *decl;
    List<Decl*> *declList;
}
/* Line 1529 of yacc.c.  */
#line 206 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
