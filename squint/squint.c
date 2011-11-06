
#line	2	"squint.y"
#include "store.h"
#include "node.h"
#include "symbol.h"
#include <u.h>
#include <lib9.h>
#include <bio.h>
#include "fns.h"

#define	yyerror	error
#define	Z	(Node *)0

extern Node	unittype;
extern int	iflag;

#line	17	"squint.y"
typedef union {
	Node	*n;
	Symbol	*s;
	Store	*st;
	long	l;
	int	i;
} YYSTYPE;
extern	int	yyerrflag;
#ifndef	YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE	yylval;
YYSTYPE	yyval;
#define	ID	57346
#define	NUM	57347
#define	STRING	57348
#define	UNIT	57349
#define	INT	57350
#define	CHAR	57351
#define	CONST	57352
#define	BUILTIN	57353
#define	ARRAY	57354
#define	OF	57355
#define	CHAN	57356
#define	STRUCT	57357
#define	PROG	57358
#define	TYPE	57359
#define	IF	57360
#define	ELSE	57361
#define	FOR	57362
#define	WHILE	57363
#define	DO	57364
#define	SELECT	57365
#define	SWITCH	57366
#define	CASE	57367
#define	DEFAULT	57368
#define	PRINT	57369
#define	REC	57370
#define	BEGIN	57371
#define	BECOME	57372
#define	RESULT	57373
#define	MK	57374
#define	LEN	57375
#define	REF	57376
#define	DEF	57377
#define	BREAK	57378
#define	CONTINUE	57379
#define	VAL	57380
#define	ANDAND	57381
#define	OROR	57382
#define	CAT	57383
#define	DEL	57384
#define	EQ	57385
#define	NE	57386
#define	LE	57387
#define	GE	57388
#define	LSH	57389
#define	RSH	57390
#define	UMINUS	57391
#define	ARROW	57392
#define	SND	57393
#define	RCV	57394
#define	DEC	57395
#define	INC	57396
#define YYEOFCODE 1
#define YYERRCODE 2
short	yyexca[] =
{-1, 1,
	1, -1,
	-2, 0,
-1, 47,
	70, 26,
	76, 26,
	-2, 88,
-1, 241,
	70, 137,
	-2, 70,
-1, 297,
	70, 139,
	-2, 70,
};
#define	YYNPROD	145
#define	YYPRIVATE 57344
#define	YYLAST	1612
short	yyact[] =
{
   8, 167, 257, 198,  38, 273,  43, 144, 266,  15,
  80,  81,  83,   6,   4, 151,   5, 183, 222, 201,
 322, 155, 314, 311, 299, 211, 188, 226, 116,  86,
  95, 101, 102, 103, 104, 105, 106, 107, 108,   3,
 303,  96,  51, 285, 286, 281, 116, 117, 188, 116,
 307, 111, 210, 188, 119, 120, 121, 122, 123, 124,
 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
 135, 136, 137, 138, 282, 247, 140, 231, 188, 188,
 227, 188, 114, 145, 215, 220, 116, 280, 149, 150,
 152, 153, 275,  79,  25,  26,  27, 157, 189, 225,
 145, 145, 188, 116, 212,  50, 115, 161, 163, 191,
 164, 160, 116, 118, 159,  96,  28, 179, 267, 268,
 214,  40,  31,  32,  33, 185, 176,  41, 110, 173,
 174, 175, 100,  99, 169,  90, 170, 171, 172,  89,
 186,  88,  87,  35, 324, 321,  94, 145, 287,  37,
  36, 264,  30, 177, 190,  34, 245, 204, 230, 205,
 146, 203, 156,  46, 162, 290, 313, 145, 209, 194,
 200, 142, 208,  85, 213, 207, 197,  84,  94,  52,
  42, 223,  74, 252,  48,  49,  73, 312,  76, 228,
 295,  23, 232, 319, 236, 152,  91, 305, 269, 240,
 204, 237,  24, 241, 203, 300, 217, 233, 234, 196,
 235, 272, 200, 238, 301, 242, 223, 221, 250, 239,
 251, 139, 223, 109, 156, 145, 148, 185, 263, 260,
 145, 258, 256, 259, 248, 267, 268, 261, 253, 249,
 254,  44, 219, 164, 218, 140, 206,  94,  39, 274,
  74, 276, 238, 181,  73, 270,  76,  74,  78, 279,
 284,  73,  75,  76,  93, 152,  21, 283, 294, 141,
 292, 140, 297, 298, 291, 274, 296,  22, 288, 184,
 289, 200, 113, 304,   2, 302,   1, 306, 176,  92,
  45, 173, 174, 175, 309, 308, 169,  94, 170, 171,
 172, 164,  94,  48,  49, 315, 200, 238,  48,  49,
  23,  29, 278, 140, 158, 154, 320, 274, 318, 265,
 323,  98, 316, 238,  47,  25,  26,  27, 168, 216,
  48,  49,   0,   0,   0,   0,  50,  23,  16,   0,
  18,  17,  14,  20,  19,   0,   0,  28,  24,   9,
  10,  11,  40,  31,  32,  33,  12,  13,  41, 246,
   0,  53,  54,  71,  72,  55,  56,  57,   0,   0,
   0,  74,   0,   0,  35,  73,  75,  76,   0,   0,
  37,  36,   0,  30,   0,   0,  34,   0,   0,   0,
   0,  42, 178,   7,  46,  47,  25,  26,  27,   0,
   0,  48,  49,   0,   0,   0,   0,  50,  23,  16,
   0,  18,  17,  14,  20,  19,   0,   0,  28,  24,
   9,  10,  11,  40,  31,  32,  33,  12,  13,  41,
   0,  94,   0,   0,  94,   0,   0,  48,  49,   0,
  48,  49,   0,   0,  23,  35,   0,  23,   0,   0,
   0,  37,  36,   0,  30,  24,   0,  34,  98,   0,
   0,   0,  42, 112,   7,  46,  47,  25,  26,  27,
   0,   0,  48,  49,   0,   0,   0,   0,  50,  23,
  16,   0,  18,  17,  14,  20,  19,   0,   0,  28,
  24,   9,  10,  11,  40,  31,  32,  33,  12,  13,
  41,  97,  55,  56,  57,   0,   0,   0,  74,   0,
   0,   0,  73,  75,  76,   0,  35,   0,   0,   0,
   0,   0,  37,  36,   0,  30,   0,   0,  34,  79,
  25,  26,  27,  42,   0,   7,  46,   0,   0,   0,
   0,  50,   0,  16,   0,  18,  17,  14,  20,  19,
 293,   0,  28,   0,   9,  10,  11,  40,  31,  32,
  33,  12,  13,  41,   0, 176,   0,   0, 173, 174,
 175,   0,   0, 169,   0, 170, 171, 172,   0,  35,
   0,   0,   0,   0,   0,  37,  36,   0,  30,   0,
   0,  34,  79,  25,  26,  27,  42,   0,   7,  46,
 180,   0,   0,   0,  50,   0,  16,   0,  18,  17,
  14,  20,  19, 199,   0,  28,   0,   9,  10,  11,
  40,  31,  32,  33,  12,  13,  41, 176,   0,   0,
 173, 174, 175,   0,   0, 169,   0, 170, 171, 172,
   0,   0,  35,   0,   0,   0,   0,   0,  37,  36,
   0,  30,   0,   0,  34,  79,  25,  26,  27,  42,
   0,   7,  46,   0,   0,   0,   0,  50,   0,  16,
   0,  18,  17,  14,  20,  19,   0,   0,  28,   0,
   9,  10,  11,  40,  31,  32,  33,  12,  13,  41,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  35,   0,   0,   0,   0,
   0,  37,  36,   0,  30,   0,   0,  34,   0,   0,
   0,   0,  42,   0,   7,  46,  61,  62,  63,  64,
  58,  59,  60,  65,  66,  69,  67,  70,  68,  53,
  54,  71,  72,  55,  56,  57,   0,   0,   0,  74,
   0,   0,   0,  73,  75,  76,   0,   0,   0,   0,
   0,   0,   0, 277,  61,  62,  63,  64,  58,  59,
  60,  65,  66,  69,  67,  70,  68,  53,  54,  71,
  72,  55,  56,  57,   0,   0,   0,  74,   0,   0,
   0,  73,  75,  76,   0,   0,   0,   0,   0,   0,
   0, 187,  61,  62,  63,  64,  58,  59,  60,  65,
  66,  69,  67,  70,  68,  53,  54,  71,  72,  55,
  56,  57,   0,   0,   0,  74,   0,   0,   0,  73,
  75,  76,   0,   0,   0,   0,   0, 262,  61,  62,
  63,  64,  58,  59,  60,  65,  66,  69,  67,  70,
  68,  53,  54,  71,  72,  55,  56,  57,   0,   0,
   0,  74,   0,   0,   0,  73,  75,  76,   0,   0,
   0,   0,   0, 195,  61,  62,  63,  64,  58,  59,
  60,  65,  66,  69,  67,  70,  68,  53,  54,  71,
  72,  55,  56,  57,   0,   0,   0,  74,   0,   0,
   0,  73,  75,  76,   0,   0,   0,   0,   0, 193,
  61,  62,  63,  64,  58,  59,  60,  65,  66,  69,
  67,  70,  68,  53,  54,  71,  72,  55,  56,  57,
   0,   0,   0,  74,   0,   0,   0,  73,  75,  76,
   0,   0,   0,   0,   0, 192,  61,  62,  63,  64,
  58,  59,  60,  65,  66,  69,  67,  70,  68,  53,
  54,  71,  72,  55,  56,  57,   0,   0,   0,  74,
   0,   0,   0,  73,  75,  76,   0,   0,   0,   0,
   0, 182,  61,  62,  63,  64,  58,  59,  60,  65,
  66,  69,  67,  70,  68,  53,  54,  71,  72,  55,
  56,  57,   0,   0,   0,  74,   0,   0,   0,  73,
  75,  76,   0,   0,   0, 147,  61,  62,  63,  64,
  58,  59,  60,  65,  66,  69,  67,  70,  68,  53,
  54,  71,  72,  55,  56,  57,   0,   0,   0,  74,
   0,   0,   0,  73,  75,  76,   0,   0,   0, 143,
  61,  62,  63,  64,  58,  59,  60,  65,  66,  69,
  67,  70,  68,  53,  54,  71,  72,  55,  56,  57,
   0,   0,   0,  74,   0,   0,   0,  73,  75,  76,
   0,   0,   0,  77,  61,  62,  63,  64,  58,  59,
  60,  65,  66,  69,  67,  70,  68,  53,  54,  71,
  72,  55,  56,  57,   0,   0,   0,  74,   0,   0,
   0,  73,  75,  76, 317,  61,  62,  63,  64,  58,
  59,  60,  65,  66,  69,  67,  70,  68,  53,  54,
  71,  72,  55,  56,  57,   0,   0,   0,  74,   0,
   0,   0,  73,  75,  76, 310,  61,  62,  63,  64,
  58,  59,  60,  65,  66,  69,  67,  70,  68,  53,
  54,  71,  72,  55,  56,  57,   0,   0,   0,  74,
   0,   0,   0,  73,  75,  76,  61,  62,  63,  64,
  58,  59,  60,  65,  66,  69,  67,  70,  68,  53,
  54,  71,  72,  55,  56,  57,   0,   0,   0, 243,
   0,   0,   0,  73, 244,  76,  79,  25,  26,  27,
   0,   0,   0,   0,   0,   0,   0,   0,  50,   0,
   0,  79,  25,  26,  27,   0,   0,   0,   0,  28,
   0,   0,   0,  50, 165,  31,  32,  33,   0,   0,
  41,   0,   0,   0,  28,   0,   0,   0,   0, 165,
  31,  32,  33,   0,   0,  41,  35,   0,   0,   0,
   0,   0,  37,  36,   0,  30,   0,   0,  34,   0,
   0,  35,   0, 166,   0,   0,  46,  37,  36,   0,
 271,   0,   0,  34,  79,  25,  26,  27, 166,   0,
   0,  46,   0,   0,   0,   0,  50,   0,   0,  79,
  25,  26,  27,   0,   0,   0,   0,  28,   0,   0,
   0,  50,  40,  31,  32,  33,   0,   0,  41,   0,
   0,   0,  28,   0,   0,   0,   0, 255,  31,  32,
  33,   0,   0,  41,  35,   0,   0,   0,   0,   0,
  37,  36,   0,  30,   0,   0,  34,   0,   0,  35,
   0, 146,   0,   0,  46,  37,  36,   0,  30,   0,
   0,  34,  79,  25,  26,  27, 224,   0,   0,  46,
   0,   0,   0,   0,  50,   0,   0,  79,  25,  26,
  27,   0,   0,   0,   0,  28,   0,   0,   0,  50,
  40,  31,  32,  33,   0,   0,  41,   0,   0,   0,
  28,   0,   0,   0,   0,  40,  31,  32,  33,   0,
   0,  41,  35,   0,   0,   0,   0,   0,  37,  36,
   0,  30,   0,   0,  34,   0,   0,  35,   0, 224,
   0,   0,  46,  37,  36,   0,  30,   0,   0,  34,
  79,  25,  26,  27, 229,   0,   0,  46,   0,   0,
   0,   0,  50,   0,   0,  79,  25,  26,  27,   0,
   0,   0,   0,  28,   0,   0,   0,  50,  40,  31,
  32,  33,   0,   0,  41,   0,   0,   0,  28,   0,
   0,   0,   0,  40,  31,  32,  33,   0,   0,  41,
  35,   0,  79,  25,  26,  27,  37,  36,   0,  30,
   0,   0,  34,   0,  50,  35,   0,  82,   0,   0,
  46,  37,  36,   0,  30,  28,   0,  34,   0,   0,
  40,  31,  32,  33,   0,  46,  41,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  35,   0,   0,   0,   0,   0,  37,  36,
   0, 202,   0,   0,  34,   0,   0,   0,   0,   0,
   0,   0,  46,  58,  59,  60,  65,  66,  69,  67,
  70,  68,  53,  54,  71,  72,  55,  56,  57,   0,
   0,   0,  74,   0,   0,   0,  73,  75,  76,  65,
  66,  69,  67,  70,  68,  53,  54,  71,  72,  55,
  56,  57,   0,   0,   0,  74,   0,   0,   0,  73,
  75,  76
};
short	yypact[] =
{
 462,-1000, 462,-1000,-1000, 106,-1000,-1000,1010,1451,
1436,1451, 104, 100, 651,-1000,  68,  67,  65,  61,
 125,-1000, 298, 243, 430,-1000,-1000,-1000,  59,  58,
1451,1451,1451,1451,1451,1451,1451,1451, 184,-1000,
  54, 109, 391,  36,-1000,-1000,1451,-1000,-1000,-1000,
  39,-1000,-1000,1451,1451,1451,1451,1451,1451,1451,
1451,1451,1451,1451,1451,1451,1451,1451,1451,1451,
1451,1451,1451,-1000, 182,1451, 265,-1000,  98,-1000,
1106, 976,1280, 942,-1000,-1000, 205,1451,1451,1451,
1451, 199,-1000,-1000,-1000,  27,-1000, 427, 293,1280,
  89, 194, 194, 194, 194, 194, 194, 194, 194,1202,
 623,-1000,-1000, 320,-1000, 561, 249, 906, 243, 445,
 445, 194, 194, 194,1542,1542,1542,1519,1519,1519,
1519, 308, 308, 308, 308, 308, 308, 445, 445,1451,
 724,-1000,-1000,-1000,  26,1106,1280,-1000,  35, 870,
 834,  96,1106, 798, 137, 588,1488, 623, 174,  95,
-1000, -23,-1000, -50,1106,  30,1280,  45,-1000, 138,
 231, 229,  11,-1000,-1000,-1000,-1000,-1000,-1000, 178,
1358,-1000,-1000,  24, -49,  10,1106,-1000,1373,  85,
   5,1451, 651, 651,1451, 123,-1000, 588, 651,1488,
-1000, 129,1451, 176,1136,-1000,-1000,  83,-1000,-1000,
-1000,-1000, 284,   3,-1000,1358, 226,1451, 623, 112,
 243,1295,-1000,1106,1280, 218, 243, 623,1106,1280,
-1000,-1000, 762, 209,-1000,  78, 210, 651,-1000, 128,
-1000, 187,1217, 172,1451,-1000,-1000,-1000,  17, 623,
 686,-1000, 243,  12,-1000, -29,   2, 109, 623, -27,
-1000, -28,  75, 651,1451,  93, 525,1451, 120,-1000,
1451,1451,1451, -53, 166,-1000,-1000,-1000, 142, -30,
 218, 122,-1000,-1000,-1000, 623,-1000,-1000,-1000, -25,
-1000, 525, 651,1451,1075,-1000, -54, 119,1106, 103,
1202,-1000, -48, 623,-1000,-1000,-1000, 651, 651,1044,
-1000,-1000,1451, 154, 623,  72,-1000,-1000, -57,1451,
  71,-1000,-1000,1106,-1000
};
short	yypgo[] =
{
   0, 329, 328, 248, 319,  13, 315,  19,  21,   9,
 314, 312,  14,   3, 311,   0,   7,  15,  17,   6,
  18,   8,   4, 266,   2, 290, 286, 284,   5,  16,
  39, 282,   1, 279, 277, 241
};
short	yyr1[] =
{
   0,  26,  26,  27,  27,  31,  31,  13,  13,  29,
  29,  29,  29,   5,  10,  10,  10,  10,  23,  23,
  23,  23,  34,  34,  35,  35,  19,  19,  32,  32,
  32,  32,  32,   2,   2,   2,   2,   1,   1,  18,
  18,  33,  33,  11,  11,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  22,  22,
  22,   3,   3,  14,  14,  14,  20,  20,  16,  16,
  16,  16,  25,  24,  24,  30,  30,  30,  12,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  12,  12,  12,  12,   9,   9,  17,  17,   4,   4,
  21,  21,  21,   6,   6,   8,   8,   7,   7,   7,
   7,   7,   7,  28,  28
};
short	yyr2[] =
{
   0,   0,   1,   1,   2,   1,   2,   1,   2,   1,
   2,   4,   2,   4,   2,   1,   3,   2,   3,   4,
   5,   7,   1,   2,   1,   1,   1,   3,   1,   4,
   3,   5,   5,   1,   1,   1,   1,   0,   3,   0,
   1,   3,   5,   4,   5,   1,   1,   1,   4,   1,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   3,
   5,   5,   4,   2,   1,   4,   6,   2,   1,   4,
   3,   3,   4,   1,   1,   3,   1,   3,   1,   3,
   3,   5,   6,   0,   2,   1,   2,   1,   1,   2,
   3,   3,   5,   3,   2,   2,   7,   1,   5,   7,
   5,   9,   7,   4,   2,   3,   0,   1,   2,   3,
   3,   2,   4,   2,   3,   3,   4,   2,   5,   4,
   7,   4,   7,   0,   2
};
short	yychk[] =
{
-1000, -26, -27, -30, -12, -29,  -5,  73, -15,  29,
  30,  31,  36,  37,  22,  -9,  18,  21,  20,  24,
  23, -23, -34,  17,  28,   5,   6,   7,  27, -14,
  63,  33,  34,  35,  66,  54,  61,  60, -22,  -3,
  32,  38,  71, -19, -35, -25,  74,   4,  10,  11,
  16, -30,  73,  53,  54,  57,  58,  59,  44,  45,
  46,  40,  41,  42,  43,  47,  48,  50,  52,  49,
  51,  55,  56,  67,  63,  68,  69,  73,  -3,   4,
 -15, -15,  71, -15,  73,  73, -12,  74,  74,  74,
  74,  71, -23, -35,   4, -19, -29,  71,  28,  74,
  74, -15, -15, -15, -15, -15, -15, -15, -15,  39,
  74,  -9,  72, -31, -30,  70,  76, -15,  74, -15,
 -15, -15, -15, -15, -15, -15, -15, -15, -15, -15,
 -15, -15, -15, -15, -15, -15, -15, -15, -15,  39,
 -15,   4,  73,  73, -16, -15,  71,  73,  21, -15,
 -15, -17, -15, -15,  -6,  -8,  25,  70, -10, -29,
  -5, -16,  75, -16, -15,  32,  71, -32,  -2,  12,
  14,  15,  16,   7,   8,   9,   4, -30,  72, -32,
  39,   4,  75, -18, -33, -19, -15,  77,  76,  72,
 -16,  74,  75,  75,  73,  75,  72,  -8, -13,  25,
 -12,  -7,  63, -22, -15, -32,  72, -29,  -5,  73,
  75,  75,  74, -16,  75,  39,  -1,  68,  13,  13,
  74,  39, -20, -15,  71,  75,  76,  70, -15,  71,
  73,  72, -15, -12, -12, -17,  71, -13, -12,  -7,
  70, -15,  39,  63,  68,  73,  75,  72, -20,  13,
 -15, -32,  71, -18, -20,  32, -16, -24,  13, -19,
 -32, -16,  75,  19,  73,  -4, -21,  25,  26,  70,
  68,  63,  39, -28, -22,  75, -32,  77, -11, -19,
  75,  74,  72,  -9, -32,  70,  72,  73, -12, -17,
  72, -21, -13,  25, -15,  70, -28, -15, -15,  77,
  39,  72, -19,  70, -24,  75, -32,  75, -13, -15,
  70,  77,  68,  63,  70, -32, -12,  70, -28,  39,
 -32,  73,  77, -15,  73
};
short	yydef[] =
{
   1,  -2,   2,   3, 105,   0, 107, 108,   0,   0,
   0,   0,   0,   0,   0, 117,   0,   0,   0,   0,
   0,   9,   0,   0,   0,  45,  46,  47,   0,  49,
   0,   0,   0,   0,   0,   0,   0,   0,  93,  84,
   0,   0,   0,   0,  22,  94,   0,  -2,  24,  25,
   0,   4, 106,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  75,  83,   0,   0, 109,  84,  88,
   0,   0,   0,   0, 114, 115,   0,   0,   0, 126,
   0,   0,  10,  23,  26,   0,  12,   0,   0,   0,
   0,  70,  71,  72,  73,  74,  76,  77,  78,   0,
   0,  87, 124,   0,   5,   0,   0,   0,  39,  50,
  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
  61,  62,  63,  64,  65,  66,  67,  68,  69,   0,
   0,  90, 110, 111,   0,  98,   0, 113,   0,   0,
   0,   0, 127,   0,   0,   0,   0,   0,   0,   0,
  15,   0,  91,   0,  79,   0,   0,   0,  28,  37,
   0,   0,   0,  33,  34,  35,  36,   6, 125,  18,
   0,  27,  95,   0,  40,   0,  82,  89,   0,   0,
   0,   0,   0,   0, 126,   0, 123,   0, 133,   0,
   7,   0,   0,  93,   0,  11,  13,   0,  17,  14,
  48,  92,   0,   0,  85,   0,   0,   0,   0,   0,
  39,   0,  19,  96,   0, 103,   0,   0, 100,   0,
 112,  99,   0, 118, 120,   0,   0, 134,   8,   0,
 135,  -2,   0,  83, 143,  16,  80,  81,   0,   0,
   0,  30,   0,   0,  20,   0,   0,   0,   0,   0,
  41,   0,   0,   0, 126,   0,   0,   0,   0, 136,
 143,   0,   0,   0,  93,  86,  29,  38,   0,   0,
 103,   0,  97, 102, 104,   0, 101, 116, 119,   0,
 122,   0, 128,   0,   0, 131,   0,  -2, 141,   0,
 144,  31,   0,   0,  32,  21,  42,   0, 129,   0,
 130, 138, 143,   0,   0,   0, 121, 132,   0,   0,
   0,  43, 140, 142,  44
};
short	yytok1[] =
{
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  61,   0,   0,   0,  59,  44,   0,
  74,  75,  57,  53,  76,  54,  69,  58,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  70,  73,
  50,  39,  52,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  68,   0,  77,  46,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  71,  45,  72,  60
};
short	yytok2[] =
{
   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,
  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,
  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38,  40,  41,  42,
  43,  47,  48,  49,  51,  55,  56,  62,  63,  64,
  65,  66,  67
};
long	yytok3[] =
{
   0
};
#define YYFLAG 		-1000
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0

#ifdef	yydebug
#include	"y.debug"
#else
#define	yydebug		0
#endif

/*	parser for yacc output	*/

int	yynerrs = 0;		/* number of errors */
int	yyerrflag = 0;		/* error recovery flag */

char*	yytoknames[1];		/* for debugging */
char*	yystates[1];		/* for debugging */

char*
yytokname(int yyc)
{
	static char x[10];

	if(yyc > 0 && yyc <= sizeof(yytoknames)/sizeof(yytoknames[0]))
	if(yytoknames[yyc-1])
		return yytoknames[yyc-1];
	sprint(x, "<%d>", yyc);
	return x;
}

char*
yystatname(int yys)
{
	static char x[10];

	if(yys >= 0 && yys < sizeof(yystates)/sizeof(yystates[0]))
	if(yystates[yys])
		return yystates[yys];
	sprint(x, "<%d>\n", yys);
	return x;
}

long
yylex1(void)
{
	long yychar;
	long *t3p;
	int c;

	yychar = yylex();
	if(yychar < sizeof(yytok1)/sizeof(yytok1[0])) {
		if(yychar <= 0) {
			c = yytok1[0];
			goto out;
		}
		c = yytok1[yychar];
		goto out;
	}
	if(yychar >= YYPRIVATE)
		if(yychar < YYPRIVATE+sizeof(yytok2)/sizeof(yytok2[0])) {
			c = yytok2[yychar-YYPRIVATE];
			goto out;
		}
	for(t3p=yytok3;; t3p+=2) {
		c = t3p[0];
		if(c == yychar) {
			c = t3p[1];
			goto out;
		}
		if(c == 0)
			break;
	}
	c = 0;

out:
	if(c == 0)
		c = yytok2[1];	/* unknown char */
	if(yydebug >= 3)
		fprint(2, "lex %.4lux %s\n", yychar, yytokname(c));
	return c;
}

int
yyparse(void)
{
	struct
	{
		YYSTYPE	yyv;
		int	yys;
	} yys[YYMAXDEPTH], *yyp, *yypt;
	short *yyxi;
	int yyj, yym, yystate, yyn, yyg;
	long yychar;
	YYSTYPE save1, save2;
	int save3, save4;

	save1 = yylval;
	save2 = yyval;
	save3 = yynerrs;
	save4 = yyerrflag;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyp = &yys[-1];
	goto yystack;

ret0:
	yyn = 0;
	goto ret;

ret1:
	yyn = 1;
	goto ret;

ret:
	yylval = save1;
	yyval = save2;
	yynerrs = save3;
	yyerrflag = save4;
	return yyn;

yystack:
	/* put a state and value onto the stack */
	if(yydebug >= 4)
		fprint(2, "char %s in %s", yytokname(yychar), yystatname(yystate));

	yyp++;
	if(yyp >= &yys[YYMAXDEPTH]) {
		yyerror("yacc stack overflow");
		goto ret1;
	}
	yyp->yys = yystate;
	yyp->yyv = yyval;

yynewstate:
	yyn = yypact[yystate];
	if(yyn <= YYFLAG)
		goto yydefault; /* simple state */
	if(yychar < 0)
		yychar = yylex1();
	yyn += yychar;
	if(yyn < 0 || yyn >= YYLAST)
		goto yydefault;
	yyn = yyact[yyn];
	if(yychk[yyn] == yychar) { /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if(yyerrflag > 0)
			yyerrflag--;
		goto yystack;
	}

yydefault:
	/* default state action */
	yyn = yydef[yystate];
	if(yyn == -2) {
		if(yychar < 0)
			yychar = yylex1();

		/* look through exception table */
		for(yyxi=yyexca;; yyxi+=2)
			if(yyxi[0] == -1 && yyxi[1] == yystate)
				break;
		for(yyxi += 2;; yyxi += 2) {
			yyn = yyxi[0];
			if(yyn < 0 || yyn == yychar)
				break;
		}
		yyn = yyxi[1];
		if(yyn < 0)
			goto ret0;
	}
	if(yyn == 0) {
		/* error ... attempt to resume parsing */
		switch(yyerrflag) {
		case 0:   /* brand new error */
			yyerror("syntax error");
			yynerrs++;
			if(yydebug >= 1) {
				fprint(2, "%s", yystatname(yystate));
				fprint(2, "saw %s\n", yytokname(yychar));
			}

		case 1:
		case 2: /* incompletely recovered error ... try again */
			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */
			while(yyp >= yys) {
				yyn = yypact[yyp->yys] + YYERRCODE;
				if(yyn >= 0 && yyn < YYLAST) {
					yystate = yyact[yyn];  /* simulate a shift of "error" */
					if(yychk[yystate] == YYERRCODE)
						goto yystack;
				}

				/* the current yyp has no shift onn "error", pop stack */
				if(yydebug >= 2)
					fprint(2, "error recovery pops state %d, uncovers %d\n",
						yyp->yys, (yyp-1)->yys );
				yyp--;
			}
			/* there is no state on the stack with an error shift ... abort */
			goto ret1;

		case 3:  /* no shift yet; clobber input char */
			if(yydebug >= 2)
				fprint(2, "error recovery discards %s\n", yytokname(yychar));
			if(yychar == YYEOFCODE)
				goto ret1;
			yychar = -1;
			goto yynewstate;   /* try again in the same state */
		}
	}

	/* reduction by production yyn */
	if(yydebug >= 2)
		fprint(2, "reduce %d in:\n\t%s", yyn, yystatname(yystate));

	yypt = yyp;
	yyp -= yyr2[yyn];
	yyval = (yyp+1)->yyv;
	yym = yyn;

	/* consult goto table to find next state */
	yyn = yyr1[yyn];
	yyg = yypgo[yyn];
	yyj = yyg + yyp->yys + 1;

	if(yyj >= YYLAST || yychk[yystate=yyact[yyj]] != -yyn)
		yystate = yyact[yyg];
	switch(yym) {
		
case 1:
#line	55	"squint.y"
{
		yyval.n=Z;
	} break;
case 3:
#line	62	"squint.y"
{
		yyval.n=yypt[-0].yyv.n;
    Compile:
		ndump(yyval.n);
		istart();
		if(yyval.n) switch(yyval.n->t){
		int pable;
		case NDecl:
		case NDeclsc:
			declare(yyval.n, 0, 1, 1);	/* BUG: freenode won't get called on error! */
			freenode(yyval.n);
			break;
		case NRec:
			recrewrite(yyval.n);
			declare(yyval.n->l, 0, 1, 1);	/* BUG: freenode won't get called on error! */
			compile(yyval.n->r);
			if(iflag)
				idump();
			if(executeallowed)
				execute();
			break;
		default:
			type(yyval.n, 1);
			pable=printable(yyval.n);
			if(pable)
				yyval.n=newi(NExpr, yyval.n, Z, PRINT);
			compile(yyval.n);
			if(iflag)
				idump();
			if(executeallowed){
				execute();
				if(pable)
					write(1, "\n", 1);
			}
		}
	} break;
case 4:
#line	99	"squint.y"
{
		yyval.n=yypt[-0].yyv.n;
		goto Compile;
	} break;
case 6:
#line	107	"squint.y"
{
		yyval.n=new(NList, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 8:
#line	114	"squint.y"
{
		yyval.n=new(NList, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 10:
#line	121	"squint.y"
{
		yyval.n=newi(NDeclsc, yypt[-0].yyv.n, Z, yypt[-1].yyv.i);
	} break;
case 11:
#line	125	"squint.y"
{
		yyval.n=newi(NType, yypt[-0].yyv.n, Z, TType);
		yyval.n=new(NDecl, yypt[-2].yyv.n, yyval.n, Z);
	} break;
case 12:
#line	130	"squint.y"
{
		yyval.n=new(NRec, yypt[-0].yyv.n, Z, Z);
	} break;
case 13:
#line	136	"squint.y"
{
		yyval.n=new(NRec, yypt[-1].yyv.n, Z, Z);
	} break;
case 16:
#line	144	"squint.y"
{
		yyval.n=new(NList, yypt[-2].yyv.n, yypt[-1].yyv.n, Z);
	} break;
case 17:
#line	148	"squint.y"
{
		yyval.n=new(NList, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 18:
#line	154	"squint.y"
{
		yyval.n=new(NDecl, yypt[-2].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 19:
#line	158	"squint.y"
{
		yyval.n=new(NDecl, yypt[-3].yyv.n, Z, yypt[-0].yyv.n);
	} break;
case 20:
#line	162	"squint.y"
{
		yyval.n=new(NDecl, yypt[-4].yyv.n, yypt[-2].yyv.n, yypt[-0].yyv.n);
	} break;
case 21:
#line	166	"squint.y"
{
		yyval.n=new(NDecl, yypt[-6].yyv.n, yypt[-4].yyv.n, new(NMk, dupnode(yypt[-4].yyv.n), Z, Z));
	} break;
case 23:
#line	173	"squint.y"
{
		yyval.i=yypt[-1].yyv.i|yypt[-0].yyv.i;
	} break;
case 24:
#line	179	"squint.y"
{
		yyval.i=SCconst;
	} break;
case 25:
#line	183	"squint.y"
{
		yyval.i=SCbltin;
	} break;
case 26:
#line	188	"squint.y"
{
		yyval.n=idnode(yypt[-0].yyv.s);
	} break;
case 27:
#line	192	"squint.y"
{
		yyval.n=idnode(yypt[-0].yyv.s);
		yyval.n=new(NList, yypt[-2].yyv.n, yyval.n, Z);
	} break;
case 29:
#line	200	"squint.y"
{
		yyval.n=newi(NType, yypt[-2].yyv.n, yypt[-0].yyv.n, TArray);
	} break;
case 30:
#line	204	"squint.y"
{
		yyval.n=newi(NType, Z, yypt[-0].yyv.n, TChan);
	} break;
case 31:
#line	208	"squint.y"
{
		yyval.n=newi(NType, yypt[-1].yyv.n, Z, TStruct);
	} break;
case 32:
#line	212	"squint.y"
{
		yyval.n=newi(NType, yypt[-2].yyv.n, yypt[-0].yyv.n, TProg);
	} break;
case 33:
#line	218	"squint.y"
{
		yyval.n=newi(NType, Z, Z, TUnit);
	} break;
case 34:
#line	222	"squint.y"
{
		yyval.n=newi(NType, Z, Z, TInt);
	} break;
case 35:
#line	226	"squint.y"
{
		yyval.n=newi(NType, Z, Z, TChar);
	} break;
case 36:
#line	230	"squint.y"
{
		yyval.n=idnode(yypt[-0].yyv.s);
		yyval.n=newi(NType, yyval.n, Z, TID);
	} break;
case 37:
#line	236	"squint.y"
{
		yyval.n=Z;
	} break;
case 38:
#line	240	"squint.y"
{
		yyval.n=yypt[-1].yyv.n;
	} break;
case 39:
#line	245	"squint.y"
{
		yyval.n=Z;
	} break;
case 41:
#line	252	"squint.y"
{
		yyval.n=elemrewr(yypt[-2].yyv.n, yypt[-0].yyv.n, Z, NFormal);
	} break;
case 42:
#line	256	"squint.y"
{
		yyval.n=elemrewr(yypt[-2].yyv.n, yypt[-0].yyv.n, yypt[-4].yyv.n, NFormal);
	} break;
case 43:
#line	262	"squint.y"
{
		yyval.n=elemrewr(yypt[-3].yyv.n, yypt[-1].yyv.n, Z, NElem);
	} break;
case 44:
#line	266	"squint.y"
{
		yyval.n=elemrewr(yypt[-3].yyv.n, yypt[-1].yyv.n, yypt[-4].yyv.n, NElem);
	} break;
case 45:
#line	272	"squint.y"
{
		yyval.n=newl(NNum, Z, Z, yypt[-0].yyv.l);
	} break;
case 46:
#line	276	"squint.y"
{
		yyval.n=newst(NString, Z, Z, yypt[-0].yyv.st);
	} break;
case 47:
#line	280	"squint.y"
{
		yyval.n=new(NUnit, Z, Z, Z);
	} break;
case 48:
#line	284	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-1].yyv.n, Z, PRINT);
	} break;
case 50:
#line	289	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '+');
	} break;
case 51:
#line	293	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '-');
	} break;
case 52:
#line	297	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '*');
	} break;
case 53:
#line	301	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '/');
	} break;
case 54:
#line	305	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '%');
	} break;
case 55:
#line	309	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '&');
	} break;
case 56:
#line	313	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '|');
	} break;
case 57:
#line	317	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '^');
	} break;
case 58:
#line	321	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, ANDAND);
	} break;
case 59:
#line	325	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, OROR);
	} break;
case 60:
#line	329	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, CAT);
	} break;
case 61:
#line	333	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, DEL);
	} break;
case 62:
#line	337	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, EQ);
	} break;
case 63:
#line	341	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, NE);
	} break;
case 64:
#line	345	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '<');
	} break;
case 65:
#line	349	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '>');
	} break;
case 66:
#line	353	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, LE);
	} break;
case 67:
#line	357	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, GE);
	} break;
case 68:
#line	361	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, LSH);
	} break;
case 69:
#line	365	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, RSH);
	} break;
case 70:
#line	369	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, RCV);
	} break;
case 71:
#line	373	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, LEN);
	} break;
case 72:
#line	377	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, REF);
	} break;
case 73:
#line	381	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, DEF);
	} break;
case 74:
#line	385	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, DEC);
	} break;
case 75:
#line	389	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-1].yyv.n, Z, INC);
	} break;
case 76:
#line	393	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, UMINUS);
	} break;
case 77:
#line	397	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, '!');
	} break;
case 78:
#line	401	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, '~');
	} break;
case 79:
#line	405	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-2].yyv.n, yypt[-0].yyv.n, '=');
	} break;
case 80:
#line	409	"squint.y"
{
		yyval.n=new(NMk, Z, Z, Z);
		yyval.n=newi(NExpr, yypt[-4].yyv.n, yyval.n, '=');
	} break;
case 81:
#line	414	"squint.y"
{
		yyval.n=new(NMk, dupnode(etypeof(yypt[-1].yyv.n)), yypt[-1].yyv.n, Z);
		yyval.n=newi(NExpr, yypt[-4].yyv.n, yyval.n, '=');
	} break;
case 82:
#line	419	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-3].yyv.n, yypt[-0].yyv.n, SND);
	} break;
case 83:
#line	423	"squint.y"
{
		yyval.n=new(NUnit, Z, Z, Z);
		yyval.n=newi(NExpr, yypt[-1].yyv.n, yyval.n, SND);
	} break;
case 85:
#line	429	"squint.y"
{
		yyval.n=new(NMk, yypt[-1].yyv.n, Z, Z);
	} break;
case 86:
#line	433	"squint.y"
{
		yyval.n=new(NMk, yypt[-3].yyv.n, yypt[-1].yyv.n, Z);
	} break;
case 87:
#line	437	"squint.y"
{
		yyval.n=new(NVal, yypt[-0].yyv.n, Z, Z);
	} break;
case 88:
#line	443	"squint.y"
{
		yyval.n=idnode(yypt[-0].yyv.s);
	} break;
case 89:
#line	447	"squint.y"
{
		yyval.n=new(NArrayref, yypt[-3].yyv.n, yypt[-1].yyv.n, Z);
	} break;
case 90:
#line	451	"squint.y"
{
		yyval.n=idnode(yypt[-0].yyv.s);
		yyval.n=new(NStructref, yypt[-2].yyv.n, yyval.n, Z);
	} break;
case 91:
#line	458	"squint.y"
{
		yyval.n=new(NCall, yypt[-2].yyv.n, Z, Z);
	} break;
case 92:
#line	462	"squint.y"
{
		yyval.n=new(NCall, yypt[-3].yyv.n, yypt[-1].yyv.n, Z);
	} break;
case 95:
#line	470	"squint.y"
{
		yyval.n=yypt[-1].yyv.n;
	} break;
case 97:
#line	477	"squint.y"
{
		yyval.n=new(NMk, dupnode(etypeof(yypt[-1].yyv.n)), yypt[-1].yyv.n, Z);
	} break;
case 99:
#line	484	"squint.y"
{
		yyval.n=new(NMk, dupnode(etypeof(yypt[-1].yyv.n)), yypt[-1].yyv.n, Z);
	} break;
case 100:
#line	488	"squint.y"
{
		yyval.n=new(NExprlist, yypt[-2].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 101:
#line	492	"squint.y"
{
		yyval.n=new(NMk, dupnode(etypeof(yypt[-1].yyv.n)), yypt[-1].yyv.n, Z);
		yyval.n=new(NExprlist, yypt[-4].yyv.n, yyval.n, Z);
	} break;
case 102:
#line	499	"squint.y"
{
		yyval.n=newi(NType, yypt[-3].yyv.n, yypt[-1].yyv.n, TProg);
		yyval.n=new(NProg, yyval.n, yypt[-0].yyv.n, Z);
	} break;
case 103:
#line	505	"squint.y"
{
		yyval.n=dupnode(&unittype);
	} break;
case 104:
#line	509	"squint.y"
{
		yyval.n=yypt[-0].yyv.n;
	} break;
case 108:
#line	520	"squint.y"
{
		yyval.n=Z;
	} break;
case 109:
#line	524	"squint.y"
{
		yyval.n=yypt[-1].yyv.n;
		if(yyval.n->t==NMk)
			yyval.n=new(NComplete, yyval.n, Z, Z);
	} break;
case 110:
#line	530	"squint.y"
{
		yyval.n=new(NBegin, yypt[-1].yyv.n, Z, Z);
	} break;
case 111:
#line	534	"squint.y"
{
		yyval.n=new(NBecome, yypt[-1].yyv.n, Z, Z);
	} break;
case 112:
#line	538	"squint.y"
{
		yyval.n=new(NMk, dupnode(etypeof(yypt[-2].yyv.n)), yypt[-2].yyv.n, Z);
		yyval.n=new(NBecome, yyval.n, Z, Z);
	} break;
case 113:
#line	543	"squint.y"
{
		yyval.n=new(NResult, yypt[-1].yyv.n, Z, Z);
	} break;
case 114:
#line	547	"squint.y"
{
		yyval.n=new(NBreak, Z, Z, Z);
	} break;
case 115:
#line	551	"squint.y"
{
		yyval.n=new(NContinue, Z, Z, Z);
	} break;
case 116:
#line	555	"squint.y"
{
		yyval.n=new(NLoopexpr, Z, yypt[-2].yyv.n, Z);
		yyval.n=newi(NLoop, yypt[-5].yyv.n, yyval.n, 1);
	} break;
case 118:
#line	561	"squint.y"
{
		yyval.n=new(NIf, yypt[-0].yyv.n, Z, yypt[-2].yyv.n);
	} break;
case 119:
#line	565	"squint.y"
{
		yyval.n=new(NIf, yypt[-2].yyv.n, yypt[-0].yyv.n, yypt[-4].yyv.n);
	} break;
case 120:
#line	569	"squint.y"
{
		yyval.n=new(NLoopexpr, Z, yypt[-2].yyv.n, Z);
		yyval.n=newi(NLoop, yypt[-0].yyv.n, yyval.n, 0);
	} break;
case 121:
#line	574	"squint.y"
{
		yyval.n=new(NLoopexpr, yypt[-6].yyv.n, yypt[-4].yyv.n, yypt[-2].yyv.n);
		yyval.n=newi(NLoop, yypt[-0].yyv.n, yyval.n, 0);
	} break;
case 122:
#line	579	"squint.y"
{
		yyval.n=new(NSwitch, yypt[-1].yyv.n, yypt[-4].yyv.n, Z);
	} break;
case 123:
#line	583	"squint.y"
{
		yyval.n=new(NSelect, yypt[-1].yyv.n, Z, Z);
	} break;
case 124:
#line	589	"squint.y"
{
		yyval.n=Z;
	} break;
case 125:
#line	593	"squint.y"
{
		yyval.n=new(NScope, yypt[-1].yyv.n, Z, Z);
	} break;
case 126:
#line	598	"squint.y"
{
		yyval.n=Z;
	} break;
case 128:
#line	605	"squint.y"
{
		yyval.n=new(NCase, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 129:
#line	609	"squint.y"
{
		yyval.n=new(NCase, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
		yyval.n=new(NList, yypt[-2].yyv.n, yyval.n, Z);
	} break;
case 130:
#line	616	"squint.y"
{
		yyval.n=new(NLabel, yypt[-1].yyv.n, Z, Z);
	} break;
case 131:
#line	620	"squint.y"
{
		yyval.n=new(NDefault, Z, Z, Z);
	} break;
case 132:
#line	624	"squint.y"
{
		yyval.n=new(NLabel, yypt[-1].yyv.n, Z, Z);
		yyval.n=new(NList, yypt[-3].yyv.n, yyval.n, Z);
	} break;
case 133:
#line	631	"squint.y"
{
		yyval.n=new(NCase, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
	} break;
case 134:
#line	635	"squint.y"
{
		yyval.n=new(NCase, yypt[-1].yyv.n, yypt[-0].yyv.n, Z);
		yyval.n=new(NList, yypt[-2].yyv.n, yyval.n, Z);
	} break;
case 135:
#line	642	"squint.y"
{
		yyval.n=new(NLabel, yypt[-1].yyv.n, Z, Z);
	} break;
case 136:
#line	646	"squint.y"
{
		yyval.n=new(NLabel, yypt[-1].yyv.n, Z, Z);
		yyval.n=new(NList, yypt[-3].yyv.n, yyval.n, Z);
	} break;
case 137:
#line	653	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, RCV);
	} break;
case 138:
#line	657	"squint.y"
{
		yyval.n=new(NArraycom, yypt[-3].yyv.n, yypt[-1].yyv.n, Z);
		yyval.n=newi(NExpr, yyval.n, Z, RCV);
	} break;
case 139:
#line	662	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-0].yyv.n, Z, RCV);
		yyval.n=newi(NExpr, yypt[-3].yyv.n, yyval.n, '=');
	} break;
case 140:
#line	667	"squint.y"
{
		yyval.n=new(NArraycom, yypt[-3].yyv.n, yypt[-1].yyv.n, Z);
		yyval.n=newi(NExpr, yyval.n, Z, RCV);
		yyval.n=newi(NExpr, yypt[-6].yyv.n, yyval.n, '=');
	} break;
case 141:
#line	673	"squint.y"
{
		yyval.n=newi(NExpr, yypt[-3].yyv.n, yypt[-0].yyv.n, SND);
	} break;
case 142:
#line	677	"squint.y"
{
		yyval.n=new(NArraycom, yypt[-6].yyv.n, yypt[-4].yyv.n, Z);
		yyval.n=newi(NExpr, yyval.n, yypt[-0].yyv.n, SND);
	} break;
case 143:
#line	683	"squint.y"
{
		yyval.n=Z;
	} break;
case 144:
#line	687	"squint.y"
{
		yyval.n=yypt[-1].yyv.n;
	} break;
	}
	goto yystack;  /* stack new state and value */
}
