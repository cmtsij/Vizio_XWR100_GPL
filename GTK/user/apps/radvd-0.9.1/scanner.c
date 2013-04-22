/*
 *   The license which is distributed with this software in the file COPYRIGHT
 *   applies to this software. If your distribution is missing this file, you
 *   may request it from <pekkas@netcore.fi>.
 *
 */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5

#include <stdio.h>
#include <unistd.h>


/* cfront 1.2 defines "c_plusplus" instead of "__cplusplus" */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif


#ifdef __cplusplus

#include <stdlib.h>

/* Use prototypes in function declarations. */
#define YY_USE_PROTOS

/* The "const" storage-class-modifier is valid. */
#define YY_USE_CONST

#else	/* ! __cplusplus */

#if __STDC__

#define YY_USE_PROTOS
#define YY_USE_CONST

#endif	/* __STDC__ */
#endif	/* ! __cplusplus */

#ifdef __TURBOC__
 #pragma warn -rch
 #pragma warn -use
#include <io.h>
#include <stdlib.h>
#define YY_USE_CONST
#define YY_USE_PROTOS
#endif

#ifdef YY_USE_CONST
#define yyconst const
#else
#define yyconst
#endif


#ifdef YY_USE_PROTOS
#define YY_PROTO(proto) proto
#else
#define YY_PROTO(proto) ()
#endif

/* Returned upon end-of-file. */
#define YY_NULL 0

/* Promotes a possibly negative, possibly signed char to an unsigned
 * integer for use as an array index.  If the signed char is negative,
 * we want to instead treat it as an 8-bit unsigned char, hence the
 * double cast.
 */
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c)

/* Enter a start condition.  This macro really ought to take a parameter,
 * but we do it the disgusting crufty way forced on us by the ()-less
 * definition of BEGIN.
 */
#define BEGIN yy_start = 1 + 2 *

/* Translate the current start state into a value that can be later handed
 * to BEGIN to return to the state.  The YYSTATE alias is for lex
 * compatibility.
 */
#define YY_START ((yy_start - 1) / 2)
#define YYSTATE YY_START

/* Action number for EOF rule of a given start state. */
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)

/* Special action meaning "start processing a new file". */
#define YY_NEW_FILE yyrestart( yyin )

#define YY_END_OF_BUFFER_CHAR 0

/* Size of default input buffer. */
#define YY_BUF_SIZE 16384

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern int yyleng;
extern FILE *yyin, *yyout;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2

/* The funky do-while in the following #define is used to turn the definition
 * int a single C statement (which needs a semi-colon terminator).  This
 * avoids problems with code like:
 *
 * 	if ( condition_holds )
 *		yyless( 5 );
 *	else
 *		do_something_else();
 *
 * Prior to using the do-while the compiler would get upset at the
 * "else" because it interpreted the "if" statement as being all
 * done when it reached the ';' after the yyless() call.
 */

/* Return all but the first 'n' matched characters back to the input stream. */

#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
		*yy_cp = yy_hold_char; \
		YY_RESTORE_YY_MORE_OFFSET \
		yy_c_buf_p = yy_cp = yy_bp + n - YY_MORE_ADJ; \
		YY_DO_BEFORE_ACTION; /* set up yytext again */ \
		} \
	while ( 0 )

#define unput(c) yyunput( c, yytext_ptr )

/* The following is because we cannot portably get our hands on size_t
 * (without autoconf's help, which isn't available because we want
 * flex-generated scanners to compile on their own).
 */
typedef unsigned int yy_size_t;


struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2
	};

static YY_BUFFER_STATE yy_current_buffer = 0;

/* We provide macros for accessing buffer states in case in the
 * future we want to put the buffer states in a more general
 * "scanner state".
 */
#define YY_CURRENT_BUFFER yy_current_buffer


/* yy_hold_char holds the character lost when yytext is formed. */
static char yy_hold_char;

static int yy_n_chars;		/* number of characters read into yy_ch_buf */


int yyleng;

/* Points to current character in buffer. */
static char *yy_c_buf_p = (char *) 0;
static int yy_init = 1;		/* whether we need to initialize */
static int yy_start = 0;	/* start state number */

/* Flag which is used to allow yywrap()'s to do buffer switches
 * instead of setting up a fresh yyin.  A bit of a hack ...
 */
static int yy_did_buffer_switch_on_eof;

void yyrestart YY_PROTO(( FILE *input_file ));

void yy_switch_to_buffer YY_PROTO(( YY_BUFFER_STATE new_buffer ));
void yy_load_buffer_state YY_PROTO(( void ));
YY_BUFFER_STATE yy_create_buffer YY_PROTO(( FILE *file, int size ));
void yy_delete_buffer YY_PROTO(( YY_BUFFER_STATE b ));
void yy_init_buffer YY_PROTO(( YY_BUFFER_STATE b, FILE *file ));
void yy_flush_buffer YY_PROTO(( YY_BUFFER_STATE b ));
#define YY_FLUSH_BUFFER yy_flush_buffer( yy_current_buffer )

YY_BUFFER_STATE yy_scan_buffer YY_PROTO(( char *base, yy_size_t size ));
YY_BUFFER_STATE yy_scan_string YY_PROTO(( yyconst char *yy_str ));
YY_BUFFER_STATE yy_scan_bytes YY_PROTO(( yyconst char *bytes, int len ));

static void *yy_flex_alloc YY_PROTO(( yy_size_t ));
static void *yy_flex_realloc YY_PROTO(( void *, yy_size_t ));
static void yy_flex_free YY_PROTO(( void * ));

#define yy_new_buffer yy_create_buffer

#define yy_set_interactive(is_interactive) \
	{ \
	if ( ! yy_current_buffer ) \
		yy_current_buffer = yy_create_buffer( yyin, YY_BUF_SIZE ); \
	yy_current_buffer->yy_is_interactive = is_interactive; \
	}

#define yy_set_bol(at_bol) \
	{ \
	if ( ! yy_current_buffer ) \
		yy_current_buffer = yy_create_buffer( yyin, YY_BUF_SIZE ); \
	yy_current_buffer->yy_at_bol = at_bol; \
	}

#define YY_AT_BOL() (yy_current_buffer->yy_at_bol)

typedef unsigned char YY_CHAR;
FILE *yyin = (FILE *) 0, *yyout = (FILE *) 0;
typedef int yy_state_type;
extern char *yytext;
#define yytext_ptr yytext

static yy_state_type yy_get_previous_state YY_PROTO(( void ));
static yy_state_type yy_try_NUL_trans YY_PROTO(( yy_state_type current_state ));
static int yy_get_next_buffer YY_PROTO(( void ));
static void yy_fatal_error YY_PROTO(( yyconst char msg[] ));

/* Done after the current pattern has been matched and before the
 * corresponding action - sets up yytext.
 */
#define YY_DO_BEFORE_ACTION \
	yytext_ptr = yy_bp; \
	yyleng = (int) (yy_cp - yy_bp); \
	yy_hold_char = *yy_cp; \
	*yy_cp = '\0'; \
	yy_c_buf_p = yy_cp;

#define YY_NUM_RULES 49
#define YY_END_OF_BUFFER 50
static yyconst short int yy_accept[483] =
    {   0,
        0,    0,   50,   48,    3,    2,   48,   48,   47,   37,
       48,   48,   48,   48,   48,   48,   48,   48,   46,   46,
       46,   46,   46,   46,   46,   46,   46,    3,    0,    1,
       38,    0,   37,    0,    0,   36,    0,    0,    0,    0,
        0,    0,    0,    0,   46,    0,   46,   46,   46,   46,
       46,   46,   41,   46,   46,   39,   37,    0,    0,   36,
        0,    0,    0,    0,    0,    0,    0,   46,   46,   46,
       46,   46,   46,   43,   46,   42,   46,   46,   37,    0,
        0,    0,   36,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,

        0,    0,   46,   46,   45,   46,   46,   46,   46,   46,
       37,    0,    0,   36,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,   46,   46,
       46,   46,   46,    6,    0,    0,    0,   36,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,   46,   46,   46,   44,    5,    0,    0,   36,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,

        0,    0,    0,    0,   46,   46,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,   40,   46,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,   20,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    4,    0,    0,    0,    0,    0,    0,
        0,    0,   13,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,

        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       28,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,   21,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,   24,    8,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       16,    0,    0,    0,    0,   26,   11,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,

        0,    0,    0,    0,   36,    0,    0,    0,    0,    0,
        0,    0,    0,   15,    0,    0,    0,    0,    0,    0,
        0,    7,    0,    0,    0,    0,   36,    0,    0,    0,
       25,   27,    0,    0,    0,   14,   33,    0,    0,   22,
        0,    0,    0,    0,    0,    0,   36,    0,    0,    0,
        0,    0,    0,    0,    0,   29,   31,    0,    9,    0,
       10,   36,    0,   17,    0,    0,   12,    0,   32,   19,
        0,   34,    0,    0,    0,    0,   30,    0,   18,   35,
       23,    0
    } ;

static yyconst int yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    1,    1,    4,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    5,    6,    7,    8,    8,    8,
        8,    9,    8,   10,    8,    8,    8,   11,   12,    1,
        1,    1,    1,    1,   13,   14,   15,   16,   17,   18,
        1,   19,   20,    1,    1,   21,   22,    1,   23,   24,
        1,   25,   26,   27,   28,   29,    1,    1,    1,    1,
        1,    1,    1,    1,   30,    1,   31,   32,   33,   34,

       35,   36,   37,   38,   39,   40,   41,   42,   43,   44,
       45,   46,   40,   47,   48,   49,   50,   51,   52,   53,
       54,   40,   55,    1,   56,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;

static yyconst int yy_meta[57] =
    {   0,
        1,    1,    1,    1,    1,    2,    1,    3,    3,    3,
        4,    1,    5,    5,    5,    5,    5,    5,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    6,
        3,    3,    3,    3,    3,    3,    6,    6,    6,    6,
        6,    6,    6,    6,    6,    6,    6,    6,    6,    6,
        6,    6,    6,    6,    1,    1
    } ;

static yyconst short int yy_base[524] =
    {   0,
        0,    0,  853,  854,  850,  854,  848,   49,  854,   54,
      839,   55,   61,   62,  804,  811,   38,  803,   87,   64,
       68,   65,  100,   75,   72,  136,  137,  844,  842,  854,
      104,  141,  146,  833,   74,  854,  147,  153,  800,  798,
      788,  796,  800,  152,  194,  157,   76,  163,  169,  165,
      179,  178,   88,  180,  166,  184,  213,  167,  827,    0,
      218,  802,  801,  790,  809,  154,  800,  187,  240,  251,
      240,  257,  258,  259,  271,  273,  275,  274,  281,  821,
      820,  819,  818,  778,  777,  791,  780,  780,  784,  175,
      239,  775,  200,  259,  790,  810,  806,  771,  768,  781,

      766,  783,  289,  297,  291,  292,  303,  306,  307,  309,
      316,  802,  801,  800,    0,  761,  762,  772,  764,  757,
      761,  760,  771,  781,  763,  765,  132,  749,  754,  747,
      754,  746,  757,  758,  745,  749,  743,  741,  319,  310,
      326,  327,  328,  329,  777,  776,  775,  774,  739,  764,
      751,  746,  745,  738,  747,  752,  737,  740,  738,  740,
      725,  722,  736,  722,  729,  722,  731,  745,  751,  732,
      749,  712,  343,  336,  337,  349,  352,  749,  748,  747,
      713,  711,  705,  741,  706,  730,  714,  701,  705,  701,
      712,  708,  714,  709,  730,  709,  707,  731,  695,  702,

      703,  682,  701,  711,  353,  355,  722,  721,  720,  685,
      683,  686,  690,  675,  698,  689,  676,  681,  706,  673,
      688,  674,  348,  683,  681,  694,  694,  664,  690,  660,
      696,  658,  664,  365,  367,  696,  695,  662,  683,  654,
      667,  670,  672,  665,  672,  854,  652,  649,  663,  646,
      654,  645,  678,  639,  668,  649,  643,   67,  647,  665,
      649,  663,  640,  368,  670,  669,  668,  633,  638,  356,
      632,  633,  854,  656,  623,  628,  636,  628,  642,  632,
      632,  632,  630,  643,  627,  613,  622,  613,  611,  614,
      608,  612,  601,  643,  642,  602,  608,  611,  602,  599,

      624,  604,  599,  608,  609,  607,  602,  605,  603,  604,
      590,  623,  600,  599,  597,  597,  583,  581,  577,  579,
      854,  616,  615,  614,  576,  584,  586,  586,  126,  574,
      588,  572,  578,  595,  588,  571,  564,  577,  564,  561,
      575,  559,  560,  571,  569,  565,  568,  567,  566,  589,
      588,  854,  549,  562,  560,  553,  550,  544,  555,  546,
      553,  550,  549,  552,  547,  538,  854,  854,  550,  544,
      546,  532,  545,  535,  531,  542,  529,  564,  563,  562,
      854,  523,  536,  539,  533,  854,  854,  521,  549,  530,
      522,  517,  520,  527,  514,  517,  528,  519,  510,  519,

      504,  510,  502,  541,  540,  511,  502,  511,  502,  497,
      503,  509,  508,  854,  507,  497,  505,  504,  405,  378,
      371,  854,  374,  379,  372,  390,  389,  388,  355,  362,
      854,  854,  378,  363,  344,  854,  854,  359,  343,  854,
      355,  354,  343,  344,  371,  341,  371,  364,  332,  321,
      315,  308,  302,  296,  271,  854,  854,  268,  854,  251,
      854,  256,  255,  854,  219,  205,  854,  189,  854,  854,
      164,  854,  177,  152,   78,   55,  854,   73,  854,  854,
      854,  854,  407,  412,  416,  421,  424,  428,  431,  434,
      437,  440,  443,  446,  449,  452,  455,  458,  461,  464,

      467,  470,  473,  476,  479,  482,  485,  488,  491,  494,
      497,  500,  503,  506,  509,  512,  515,  518,  521,  524,
      527,  530,  533
    } ;

static yyconst short int yy_def[524] =
    {   0,
      482,    1,  482,  482,  482,  482,  483,  482,  482,  484,
      482,  484,  484,  484,  482,  482,  482,  482,  482,  485,
      485,  485,  485,  485,  485,  485,  485,  482,  483,  482,
      482,  482,  486,  487,  486,  482,  486,  486,  482,  482,
      482,  482,  482,  482,   19,  487,  485,  485,  485,  485,
      485,  485,  485,  485,  485,  482,  488,  488,  489,  490,
      482,  482,  482,  482,  482,  482,  482,  482,   19,  489,
      485,  485,  485,  485,  485,  485,  485,  485,  482,  482,
      491,  492,  493,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,

      482,  482,  485,  491,  485,  485,  485,  485,  485,  485,
      482,  494,  495,  496,  490,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  494,  485,
      485,  485,  485,  485,  482,  497,  498,  499,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  485,  485,  485,  485,  500,  501,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,

      482,  482,  482,  482,  485,  485,  482,  502,  503,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  485,  485,  504,  505,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  485,  482,  506,  507,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  508,  509,  482,  482,  482,  482,  482,

      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  510,  511,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  512,
      513,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  514,  515,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,

      482,  482,  482,  516,  517,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  518,  519,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  520,  521,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  522,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  523,  482,  482,  482,  482,  482,  482,  482,
      482,    0,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,

      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482
    } ;

static yyconst short int yy_nxt[911] =
    {   0,
        4,    5,    6,    7,    8,    4,    9,   10,   10,   10,
       11,    9,   12,   13,   14,   14,   14,   14,   15,   16,
        4,   17,    4,    4,    4,    4,    4,   18,    4,    4,
       19,   19,   19,   19,   19,   19,   20,   21,   22,   20,
       20,   23,   24,   20,   25,   26,   27,   20,   20,   20,
       20,   20,   20,   20,    9,    9,   31,   31,   31,   32,
      482,   33,   33,   33,   34,   34,  482,  482,   41,   44,
       44,   34,   34,   44,   44,   44,   42,   44,   44,  482,
       44,   44,   44,  428,   34,   44,   44,  287,   37,  481,
      288,   38,   44,   44,   45,   45,   45,   46,   44,   35,

       35,   35,   35,   35,   35,   44,   48,   52,   49,   51,
       44,   31,   31,   31,  480,   53,   47,   45,   45,   45,
       45,   45,   45,   47,   47,   47,   47,   47,   47,   47,
       47,   47,   47,   47,   47,   47,   47,   47,   47,   47,
       47,   44,   44,  356,   50,  357,   44,   44,   56,   56,
       56,   32,  482,   57,   57,   57,   34,   34,  482,   68,
       68,   68,  160,   34,   70,   70,   70,   60,   44,  100,
       44,   44,  482,   44,   44,   44,   44,   34,  101,   44,
      161,   55,   54,   44,   44,   44,  479,  428,   44,   44,
       44,   56,   56,   56,   68,   68,   68,   61,  477,   71,

       62,   69,   69,   69,   72,  122,   58,   58,   58,   58,
       58,   58,   75,   76,   77,   78,   74,   73,   32,  123,
       79,   79,   79,   34,   69,   69,   69,   69,   69,   69,
       84,  476,   85,   86,  127,  475,   87,   88,   89,   90,
       91,   92,   93,   94,  128,   44,   95,  103,  103,  103,
       44,  474,   80,   80,   80,   80,   80,   80,  104,  104,
      104,   82,   44,   44,   44,  428,  428,   44,   44,   44,
      103,  103,  103,  103,  103,  103,   44,  105,   44,   44,
       44,   44,  124,   44,   44,   44,   32,  125,  111,  111,
      111,   34,  107,  129,   44,  106,   44,   44,  472,   46,

      471,   44,   44,  130,  139,  139,  139,   82,   44,  108,
      109,   44,   44,   44,   44,   44,   44,   44,  470,   44,
       44,   32,  110,  111,  111,  111,  173,  173,  173,   82,
      469,   44,   44,   44,   44,  140,   44,   44,   44,   44,
      468,   44,   44,  144,  467,  143,   44,   44,  174,  141,
       68,   68,   68,   82,   44,  142,  466,   44,   44,   44,
       44,  175,   44,   44,  465,   44,  464,  206,  251,  176,
       44,  252,   44,   44,  428,   44,  298,   44,   44,  299,
      177,  428,  461,  460,  205,  459,  458,  235,  457,  456,
      455,  454,  453,  452,  253,  451,  450,  449,   60,  428,

      380,  264,  446,  445,  444,  443,  234,   29,   29,   29,
       29,   29,   29,   35,   35,   35,   35,   47,   47,   47,
      442,   47,   58,   58,   58,   58,   59,   59,   59,   80,
       80,   80,   80,   81,   81,   81,   83,  441,   83,  112,
      112,  112,  113,  113,  113,  114,  114,  114,  145,  145,
      145,  146,  146,  146,  148,  148,  148,  178,  178,  178,
      179,  179,  179,  180,  180,  180,  207,  207,  207,  208,
      208,  208,  236,  236,  236,  237,  237,  237,  265,  265,
      265,  266,  266,  266,  294,  294,  294,  295,  295,  295,
      322,  322,  322,  323,  323,  323,  350,  350,  350,  351,

      351,  351,  378,  378,  378,  379,  379,  379,  404,  404,
      404,  405,  405,  405,  426,  426,  426,  427,  427,  427,
      447,  447,  447,  448,  448,  448,  462,  462,  462,  463,
      463,  463,  473,  473,  473,  478,  478,  478,  440,  439,
      438,  437,  436,  435,  434,  433,  432,  431,  430,  429,
      428,  380,  425,  424,  423,  422,  421,  420,  419,  418,
      417,  416,  415,  414,  413,  412,  411,  410,  409,  408,
      407,  406,   60,  380,  324,  403,  402,  401,  400,  399,
      398,  397,  396,  395,  394,  393,  392,  391,  390,  389,
      388,  387,  386,  385,  384,  383,  382,  381,  380,  324,

      377,  376,  375,  374,  373,  372,  371,  370,  369,  368,
      367,  366,  365,  364,  363,  362,  361,  360,  359,  358,
      355,  354,  353,  352,   60,  324,  267,  349,  348,  347,
      346,  345,  344,  343,  342,  341,  340,  339,  338,  337,
      336,  335,  334,  333,  332,  331,  330,  329,  328,  327,
      326,  325,  324,  267,  321,  320,  319,  318,  317,  316,
      315,  314,  313,  312,  311,  310,  309,  308,  307,  306,
      305,  304,  303,  302,  301,  300,  297,  296,   60,  267,
      209,  293,  292,  291,  290,  289,  286,  285,  284,  283,
      282,  281,  280,  279,  278,  277,  276,  275,  274,  273,

      272,  271,  270,  269,  268,  267,  209,  263,  262,  261,
      260,  259,  258,  257,  256,  255,  254,  250,  249,  248,
      247,  246,  245,  244,  243,  242,  241,  240,  239,  238,
       60,  209,  147,  233,  232,  231,  230,  229,  228,  227,
      226,  225,  224,  223,  222,  221,  220,  219,  218,  217,
      216,  215,  214,  213,  212,  211,  210,  115,  209,  147,
      204,  203,  202,  201,  200,  199,  198,  197,  196,  195,
      194,  193,  192,  191,  190,  189,  188,  187,  186,  185,
      184,  183,  182,  181,  115,   60,  147,   82,  172,  171,
      170,  169,  168,  167,  166,  165,  164,  163,  162,  159,

      158,  157,  156,  155,  154,  153,  152,  151,  150,  149,
      115,  147,   82,  138,  137,  136,  135,  134,  133,  132,
      131,  126,  121,  120,  119,  118,  117,  116,  115,   60,
       82,   34,  102,   99,   98,   97,   96,   82,   67,   66,
       65,   64,   63,   60,   30,   28,   43,   40,   39,   36,
       30,   28,  482,    3,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,

      482,  482,  482,  482,  482,  482,  482,  482,  482,  482
    } ;

static yyconst short int yy_chk[911] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    8,    8,    8,   10,
       12,   10,   10,   10,   10,   12,   13,   14,   17,   20,
       22,   13,   14,   21,   20,   22,   17,   25,   21,   35,
       24,   47,   25,  478,   35,   24,   47,  258,   12,  476,
      258,   13,   19,   53,   19,   19,   19,   19,   53,   19,

       19,   19,   19,   19,   19,   23,   21,   25,   22,   24,
       23,   31,   31,   31,  475,   25,   19,   19,   19,   19,
       19,   19,   19,   19,   19,   19,   19,   19,   19,   19,
       19,   19,   19,   19,   19,   19,   19,   19,   19,   19,
       19,   26,   27,  329,   23,  329,   26,   27,   32,   32,
       32,   33,   37,   33,   33,   33,   33,   37,   38,   44,
       44,   44,  127,   38,   46,   46,   46,   46,   48,   66,
       50,   55,   58,   48,   49,   50,   55,   58,   66,   49,
      127,   27,   26,   52,   51,   54,  474,  473,   52,   51,
       54,   56,   56,   56,   68,   68,   68,   37,  471,   48,

       38,   45,   45,   45,   49,   90,   45,   45,   45,   45,
       45,   45,   51,   52,   54,   55,   50,   49,   57,   90,
       57,   57,   57,   57,   45,   45,   45,   45,   45,   45,
       61,  468,   61,   61,   93,  466,   61,   61,   61,   61,
       61,   61,   61,   61,   93,   71,   61,   69,   69,   69,
       71,  465,   69,   69,   69,   69,   69,   69,   70,   70,
       70,   70,   72,   73,   74,  463,  462,   72,   73,   74,
       69,   69,   69,   69,   69,   69,   75,   71,   76,   78,
       77,   75,   91,   76,   78,   77,   79,   91,   79,   79,
       79,   79,   73,   94,  103,   72,  105,  106,  460,  103,

      458,  105,  106,   94,  104,  104,  104,  104,  107,   75,
       77,  108,  109,  107,  110,  140,  108,  109,  455,  110,
      140,  111,   78,  111,  111,  111,  139,  139,  139,  139,
      454,  141,  142,  143,  144,  106,  141,  142,  143,  144,
      453,  174,  175,  110,  452,  109,  174,  175,  140,  107,
      173,  173,  173,  173,  176,  108,  451,  177,  205,  176,
      206,  141,  177,  205,  450,  206,  449,  175,  223,  142,
      234,  223,  235,  264,  448,  234,  270,  235,  264,  270,
      143,  447,  446,  445,  174,  444,  443,  206,  442,  441,
      439,  438,  435,  434,  223,  433,  430,  429,  428,  427,

      426,  235,  425,  424,  423,  421,  205,  483,  483,  483,
      483,  483,  483,  484,  484,  484,  484,  485,  485,  485,
      420,  485,  486,  486,  486,  486,  487,  487,  487,  488,
      488,  488,  488,  489,  489,  489,  490,  419,  490,  491,
      491,  491,  492,  492,  492,  493,  493,  493,  494,  494,
      494,  495,  495,  495,  496,  496,  496,  497,  497,  497,
      498,  498,  498,  499,  499,  499,  500,  500,  500,  501,
      501,  501,  502,  502,  502,  503,  503,  503,  504,  504,
      504,  505,  505,  505,  506,  506,  506,  507,  507,  507,
      508,  508,  508,  509,  509,  509,  510,  510,  510,  511,

      511,  511,  512,  512,  512,  513,  513,  513,  514,  514,
      514,  515,  515,  515,  516,  516,  516,  517,  517,  517,
      518,  518,  518,  519,  519,  519,  520,  520,  520,  521,
      521,  521,  522,  522,  522,  523,  523,  523,  418,  417,
      416,  415,  413,  412,  411,  410,  409,  408,  407,  406,
      405,  404,  403,  402,  401,  400,  399,  398,  397,  396,
      395,  394,  393,  392,  391,  390,  389,  388,  385,  384,
      383,  382,  380,  379,  378,  377,  376,  375,  374,  373,
      372,  371,  370,  369,  366,  365,  364,  363,  362,  361,
      360,  359,  358,  357,  356,  355,  354,  353,  351,  350,

      349,  348,  347,  346,  345,  344,  343,  342,  341,  340,
      339,  338,  337,  336,  335,  334,  333,  332,  331,  330,
      328,  327,  326,  325,  324,  323,  322,  320,  319,  318,
      317,  316,  315,  314,  313,  312,  311,  310,  309,  308,
      307,  306,  305,  304,  303,  302,  301,  300,  299,  298,
      297,  296,  295,  294,  293,  292,  291,  290,  289,  288,
      287,  286,  285,  284,  283,  282,  281,  280,  279,  278,
      277,  276,  275,  274,  272,  271,  269,  268,  267,  266,
      265,  263,  262,  261,  260,  259,  257,  256,  255,  254,
      253,  252,  251,  250,  249,  248,  247,  245,  244,  243,

      242,  241,  240,  239,  238,  237,  236,  233,  232,  231,
      230,  229,  228,  227,  226,  225,  224,  222,  221,  220,
      219,  218,  217,  216,  215,  214,  213,  212,  211,  210,
      209,  208,  207,  204,  203,  202,  201,  200,  199,  198,
      197,  196,  195,  194,  193,  192,  191,  190,  189,  188,
      187,  186,  185,  184,  183,  182,  181,  180,  179,  178,
      172,  171,  170,  169,  168,  167,  166,  165,  164,  163,
      162,  161,  160,  159,  158,  157,  156,  155,  154,  153,
      152,  151,  150,  149,  148,  147,  146,  145,  138,  137,
      136,  135,  134,  133,  132,  131,  130,  129,  128,  126,

      125,  124,  123,  122,  121,  120,  119,  118,  117,  116,
      114,  113,  112,  102,  101,  100,   99,   98,   97,   96,
       95,   92,   89,   88,   87,   86,   85,   84,   83,   82,
       81,   80,   67,   65,   64,   63,   62,   59,   43,   42,
       41,   40,   39,   34,   29,   28,   18,   16,   15,   11,
        7,    5,    3,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,
      482,  482,  482,  482,  482,  482,  482,  482,  482,  482,

      482,  482,  482,  482,  482,  482,  482,  482,  482,  482
    } ;

static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;

/* The intent behind this definition is that it'll catch
 * any uses of REJECT which flex missed.
 */
#define REJECT reject_used_but_not_detected
#define yymore() yymore_used_but_not_detected
#define YY_MORE_ADJ 0
#define YY_RESTORE_YY_MORE_OFFSET
char *yytext;
#line 1 "scanner.l"
#define INITIAL 0
/*
 *   $Id: scanner.l,v 1.11 2005/10/18 19:17:29 lutchann Exp $
 *
 *   Authors:
 *    Pedro Roque		<roque@di.fc.ul.pt>
 *    Lars Fenneberg		<lf@elemental.net>	 
 *
 *   This software is Copyright 1996-2000 by the above mentioned author(s), 
 *   All Rights Reserved.
 *
 *   The license which is distributed with this software in the file COPYRIGHT
 *   applies to this software. If your distribution is missing this file, you
 *   may request it from <pekkas@netcore.fi>.
 *
 */
#line 17 "scanner.l"
#include <config.h>
#include <includes.h>
#include <radvd.h>
#include <gram.h>

extern char *conf_file;

int num_lines = 1;
#line 756 "lex.yy.c"

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap YY_PROTO(( void ));
#else
extern int yywrap YY_PROTO(( void ));
#endif
#endif

#ifndef YY_NO_UNPUT
static void yyunput YY_PROTO(( int c, char *buf_ptr ));
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy YY_PROTO(( char *, yyconst char *, int ));
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen YY_PROTO(( yyconst char * ));
#endif

#ifndef YY_NO_INPUT
#ifdef __cplusplus
static int yyinput YY_PROTO(( void ));
#else
static int input YY_PROTO(( void ));
#endif
#endif

#if YY_STACK_USED
static int yy_start_stack_ptr = 0;
static int yy_start_stack_depth = 0;
static int *yy_start_stack = 0;
#ifndef YY_NO_PUSH_STATE
static void yy_push_state YY_PROTO(( int new_state ));
#endif
#ifndef YY_NO_POP_STATE
static void yy_pop_state YY_PROTO(( void ));
#endif
#ifndef YY_NO_TOP_STATE
static int yy_top_state YY_PROTO(( void ));
#endif

#else
#define YY_NO_PUSH_STATE 1
#define YY_NO_POP_STATE 1
#define YY_NO_TOP_STATE 1
#endif

#ifdef YY_MALLOC_DECL
YY_MALLOC_DECL
#else
#if __STDC__
#ifndef __cplusplus
#include <stdlib.h>
#endif
#else
/* Just try to get by without declaring the routines.  This will fail
 * miserably on non-ANSI systems for which sizeof(size_t) != sizeof(int)
 * or sizeof(void*) != sizeof(int).
 */
#endif
#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
#endif

/* Copy whatever the last rule matched to the standard output. */

#ifndef ECHO
/* This used to be an fputs(), but since the string might contain NUL's,
 * we now use fwrite().
 */
#define ECHO (void) fwrite( yytext, yyleng, 1, yyout )
#endif

/* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
 * is returned in "result".
 */
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( yy_current_buffer->yy_is_interactive ) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < max_size && \
			     (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else if ( ((result = fread( buf, 1, max_size, yyin )) == 0) \
		  && ferror( yyin ) ) \
		YY_FATAL_ERROR( "input in flex scanner failed" );
#endif

/* No semi-colon after return; correct usage is to write "yyterminate();" -
 * we don't want an extra ';' after the "return" because that will cause
 * some compilers to complain about unreachable statements.
 */
#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Report a fatal error. */
#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) yy_fatal_error( msg )
#endif

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL int yylex YY_PROTO(( void ))
#endif

/* Code executed at the beginning of each rule, after yytext and yyleng
 * have been set up.
 */
#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif

/* Code executed at the end of each rule. */
#ifndef YY_BREAK
#define YY_BREAK break;
#endif

#define YY_RULE_SETUP \
	YY_USER_ACTION

YY_DECL
	{
	register yy_state_type yy_current_state;
	register char *yy_cp = NULL, *yy_bp = NULL;
	register int yy_act;

#line 38 "scanner.l"


#line 910 "lex.yy.c"

	if ( yy_init )
		{
		yy_init = 0;

#ifdef YY_USER_INIT
		YY_USER_INIT;
#endif

		if ( ! yy_start )
			yy_start = 1;	/* first start state */

		if ( ! yyin )
			yyin = stdin;

		if ( ! yyout )
			yyout = stdout;

		if ( ! yy_current_buffer )
			yy_current_buffer =
				yy_create_buffer( yyin, YY_BUF_SIZE );

		yy_load_buffer_state();
		}

	while ( 1 )		/* loops until end-of-file is reached */
		{
		yy_cp = yy_c_buf_p;

		/* Support of yytext. */
		*yy_cp = yy_hold_char;

		/* yy_bp points to the position in yy_ch_buf of the start of
		 * the current run.
		 */
		yy_bp = yy_cp;

		yy_current_state = yy_start;
yy_match:
		do
			{
			register YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)];
			if ( yy_accept[yy_current_state] )
				{
				yy_last_accepting_state = yy_current_state;
				yy_last_accepting_cpos = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 483 )
					yy_c = yy_meta[(unsigned int) yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
			++yy_cp;
			}
		while ( yy_base[yy_current_state] != 854 );

yy_find_action:
		yy_act = yy_accept[yy_current_state];
		if ( yy_act == 0 )
			{ /* have to back up */
			yy_cp = yy_last_accepting_cpos;
			yy_current_state = yy_last_accepting_state;
			yy_act = yy_accept[yy_current_state];
			}

		YY_DO_BEFORE_ACTION;


do_action:	/* This label is used only to access EOF actions. */


		switch ( yy_act )
	{ /* beginning of action switch */
			case 0: /* must back up */
			/* undo the effects of YY_DO_BEFORE_ACTION */
			*yy_cp = yy_hold_char;
			yy_cp = yy_last_accepting_cpos;
			yy_current_state = yy_last_accepting_state;
			goto yy_find_action;

case 1:
*yy_cp = yy_hold_char; /* undo effects of setting up yytext */
yy_c_buf_p = yy_cp -= 1;
YY_DO_BEFORE_ACTION; /* set up yytext again */
YY_RULE_SETUP
#line 40 "scanner.l"
{/* ignore comments */}
	YY_BREAK
case 2:
YY_RULE_SETUP
#line 41 "scanner.l"
{num_lines++;}
	YY_BREAK
case 3:
YY_RULE_SETUP
#line 42 "scanner.l"
{}
	YY_BREAK
case 4:
YY_RULE_SETUP
#line 44 "scanner.l"
{ return T_INTERFACE; }
	YY_BREAK
case 5:
YY_RULE_SETUP
#line 45 "scanner.l"
{ return T_PREFIX; }
	YY_BREAK
case 6:
YY_RULE_SETUP
#line 46 "scanner.l"
{ return T_ROUTE; }
	YY_BREAK
case 7:
YY_RULE_SETUP
#line 48 "scanner.l"
{ return T_IgnoreIfMissing; }
	YY_BREAK
case 8:
YY_RULE_SETUP
#line 49 "scanner.l"
{ return T_AdvSendAdvert; }
	YY_BREAK
case 9:
YY_RULE_SETUP
#line 50 "scanner.l"
{ return T_MaxRtrAdvInterval; }
	YY_BREAK
case 10:
YY_RULE_SETUP
#line 51 "scanner.l"
{ return T_MinRtrAdvInterval; }
	YY_BREAK
case 11:
YY_RULE_SETUP
#line 52 "scanner.l"
{ return T_AdvManagedFlag; }
	YY_BREAK
case 12:
YY_RULE_SETUP
#line 53 "scanner.l"
{ return T_AdvOtherConfigFlag; }
	YY_BREAK
case 13:
YY_RULE_SETUP
#line 54 "scanner.l"
{ return T_AdvLinkMTU; }
	YY_BREAK
case 14:
YY_RULE_SETUP
#line 55 "scanner.l"
{ return T_AdvReachableTime; }
	YY_BREAK
case 15:
YY_RULE_SETUP
#line 56 "scanner.l"
{ return T_AdvRetransTimer; }
	YY_BREAK
case 16:
YY_RULE_SETUP
#line 57 "scanner.l"
{ return T_AdvCurHopLimit; }
	YY_BREAK
case 17:
YY_RULE_SETUP
#line 58 "scanner.l"
{ return T_AdvDefaultLifetime; }
	YY_BREAK
case 18:
YY_RULE_SETUP
#line 59 "scanner.l"
{ return T_AdvDefaultPreference; }
	YY_BREAK
case 19:
YY_RULE_SETUP
#line 60 "scanner.l"
{ return T_AdvSourceLLAddress; }
	YY_BREAK
case 20:
YY_RULE_SETUP
#line 62 "scanner.l"
{ return T_AdvOnLink; }
	YY_BREAK
case 21:
YY_RULE_SETUP
#line 63 "scanner.l"
{ return T_AdvAutonomous; }
	YY_BREAK
case 22:
YY_RULE_SETUP
#line 64 "scanner.l"
{ return T_AdvValidLifetime; }
	YY_BREAK
case 23:
YY_RULE_SETUP
#line 65 "scanner.l"
{ return T_AdvPreferredLifetime; }
	YY_BREAK
case 24:
YY_RULE_SETUP
#line 67 "scanner.l"
{ return T_AdvRouterAddr; }
	YY_BREAK
case 25:
YY_RULE_SETUP
#line 68 "scanner.l"
{ return T_AdvHomeAgentFlag; }
	YY_BREAK
case 26:
YY_RULE_SETUP
#line 69 "scanner.l"
{ return T_AdvIntervalOpt; }
	YY_BREAK
case 27:
YY_RULE_SETUP
#line 70 "scanner.l"
{ return T_AdvHomeAgentInfo; }
	YY_BREAK
case 28:
YY_RULE_SETUP
#line 71 "scanner.l"
{ return T_UnicastOnly; }
	YY_BREAK
case 29:
YY_RULE_SETUP
#line 73 "scanner.l"
{ return T_Base6to4Interface; }
	YY_BREAK
case 30:
YY_RULE_SETUP
#line 75 "scanner.l"
{ return T_HomeAgentPreference; }
	YY_BREAK
case 31:
YY_RULE_SETUP
#line 76 "scanner.l"
{ return T_HomeAgentLifetime; }
	YY_BREAK
case 32:
YY_RULE_SETUP
#line 78 "scanner.l"
{ return T_AdvRoutePreference; }
	YY_BREAK
case 33:
YY_RULE_SETUP
#line 79 "scanner.l"
{ return T_AdvRouteLifetime; }
	YY_BREAK
case 34:
YY_RULE_SETUP
#line 81 "scanner.l"
{ return T_MinDelayBetweenRAs; }
	YY_BREAK
case 35:
YY_RULE_SETUP
#line 83 "scanner.l"
{ return T_AdvMobRtrSupportFlag; }
	YY_BREAK
case 36:
YY_RULE_SETUP
#line 85 "scanner.l"
{
			static struct in6_addr addr;
			int i;
				
			i = inet_pton(AF_INET6, yytext, &addr);

			dlog(LOG_DEBUG, 4, "inet_pton returned %d", i);

			/* BSD API draft and NRL's code don't aggree on
			 * this. the draft specifies a return value of 1 on 
			 * success, NRL's code returns the address length in 
			 * bytes on success (16 for an IPv6 address)
			 */
			if (i < 1) {
				flog(LOG_ERR, "invalid address in %s, line %d", conf_file,
					num_lines);
				return T_BAD_TOKEN;
			}

			yylval.addr = &addr;
			return IPV6ADDR;
		}
	YY_BREAK
case 37:
YY_RULE_SETUP
#line 108 "scanner.l"
{ yylval.num = atoi(yytext); return NUMBER; }
	YY_BREAK
case 38:
YY_RULE_SETUP
#line 110 "scanner.l"
{ yylval.snum = atoi(yytext); return SIGNEDNUMBER; }
	YY_BREAK
case 39:
YY_RULE_SETUP
#line 112 "scanner.l"
{ yylval.dec = atof(yytext); return DECIMAL; }
	YY_BREAK
case 40:
YY_RULE_SETUP
#line 114 "scanner.l"
{ return INFINITY; }
	YY_BREAK
case 41:
YY_RULE_SETUP
#line 116 "scanner.l"
{ yylval.bool = 1; return SWITCH; }
	YY_BREAK
case 42:
YY_RULE_SETUP
#line 118 "scanner.l"
{ yylval.bool = 0; return SWITCH; }
	YY_BREAK
case 43:
YY_RULE_SETUP
#line 120 "scanner.l"
{ yylval.snum = -1; return SIGNEDNUMBER; }
	YY_BREAK
case 44:
YY_RULE_SETUP
#line 122 "scanner.l"
{ yylval.snum = 0; return SIGNEDNUMBER; }
	YY_BREAK
case 45:
YY_RULE_SETUP
#line 124 "scanner.l"
{ yylval.snum = 1; return SIGNEDNUMBER; }
	YY_BREAK
case 46:
YY_RULE_SETUP
#line 126 "scanner.l"
{
			static char name[IFNAMSIZ];
				
			strncpy(name, yytext, IFNAMSIZ-1);
			name[IFNAMSIZ-1] = '\0';
			yylval.str = name;
			return STRING;
		}
	YY_BREAK
case 47:
YY_RULE_SETUP
#line 135 "scanner.l"
{ return *yytext; }
	YY_BREAK
case 48:
YY_RULE_SETUP
#line 137 "scanner.l"
{ return T_BAD_TOKEN; }
	YY_BREAK
case 49:
YY_RULE_SETUP
#line 138 "scanner.l"
ECHO;
	YY_BREAK
#line 1269 "lex.yy.c"
case YY_STATE_EOF(INITIAL):
	yyterminate();

	case YY_END_OF_BUFFER:
		{
		/* Amount of text matched not including the EOB char. */
		int yy_amount_of_matched_text = (int) (yy_cp - yytext_ptr) - 1;

		/* Undo the effects of YY_DO_BEFORE_ACTION. */
		*yy_cp = yy_hold_char;
		YY_RESTORE_YY_MORE_OFFSET

		if ( yy_current_buffer->yy_buffer_status == YY_BUFFER_NEW )
			{
			/* We're scanning a new file or input source.  It's
			 * possible that this happened because the user
			 * just pointed yyin at a new source and called
			 * yylex().  If so, then we have to assure
			 * consistency between yy_current_buffer and our
			 * globals.  Here is the right place to do so, because
			 * this is the first action (other than possibly a
			 * back-up) that will match for the new input source.
			 */
			yy_n_chars = yy_current_buffer->yy_n_chars;
			yy_current_buffer->yy_input_file = yyin;
			yy_current_buffer->yy_buffer_status = YY_BUFFER_NORMAL;
			}

		/* Note that here we test for yy_c_buf_p "<=" to the position
		 * of the first EOB in the buffer, since yy_c_buf_p will
		 * already have been incremented past the NUL character
		 * (since all states make transitions on EOB to the
		 * end-of-buffer state).  Contrast this with the test
		 * in input().
		 */
		if ( yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			{ /* This was really a NUL. */
			yy_state_type yy_next_state;

			yy_c_buf_p = yytext_ptr + yy_amount_of_matched_text;

			yy_current_state = yy_get_previous_state();

			/* Okay, we're now positioned to make the NUL
			 * transition.  We couldn't have
			 * yy_get_previous_state() go ahead and do it
			 * for us because it doesn't know how to deal
			 * with the possibility of jamming (and we don't
			 * want to build jamming into it because then it
			 * will run more slowly).
			 */

			yy_next_state = yy_try_NUL_trans( yy_current_state );

			yy_bp = yytext_ptr + YY_MORE_ADJ;

			if ( yy_next_state )
				{
				/* Consume the NUL. */
				yy_cp = ++yy_c_buf_p;
				yy_current_state = yy_next_state;
				goto yy_match;
				}

			else
				{
				yy_cp = yy_c_buf_p;
				goto yy_find_action;
				}
			}

		else switch ( yy_get_next_buffer() )
			{
			case EOB_ACT_END_OF_FILE:
				{
				yy_did_buffer_switch_on_eof = 0;

				if ( yywrap() )
					{
					/* Note: because we've taken care in
					 * yy_get_next_buffer() to have set up
					 * yytext, we can now set up
					 * yy_c_buf_p so that if some total
					 * hoser (like flex itself) wants to
					 * call the scanner after we return the
					 * YY_NULL, it'll still work - another
					 * YY_NULL will get returned.
					 */
					yy_c_buf_p = yytext_ptr + YY_MORE_ADJ;

					yy_act = YY_STATE_EOF(YY_START);
					goto do_action;
					}

				else
					{
					if ( ! yy_did_buffer_switch_on_eof )
						YY_NEW_FILE;
					}
				break;
				}

			case EOB_ACT_CONTINUE_SCAN:
				yy_c_buf_p =
					yytext_ptr + yy_amount_of_matched_text;

				yy_current_state = yy_get_previous_state();

				yy_cp = yy_c_buf_p;
				yy_bp = yytext_ptr + YY_MORE_ADJ;
				goto yy_match;

			case EOB_ACT_LAST_MATCH:
				yy_c_buf_p =
				&yy_current_buffer->yy_ch_buf[yy_n_chars];

				yy_current_state = yy_get_previous_state();

				yy_cp = yy_c_buf_p;
				yy_bp = yytext_ptr + YY_MORE_ADJ;
				goto yy_find_action;
			}
		break;
		}

	default:
		YY_FATAL_ERROR(
			"fatal flex scanner internal error--no action found" );
	} /* end of action switch */
		} /* end of scanning one token */
	} /* end of yylex */


/* yy_get_next_buffer - try to read in a new buffer
 *
 * Returns a code representing an action:
 *	EOB_ACT_LAST_MATCH -
 *	EOB_ACT_CONTINUE_SCAN - continue scanning from current position
 *	EOB_ACT_END_OF_FILE - end of file
 */

static int yy_get_next_buffer()
	{
	register char *dest = yy_current_buffer->yy_ch_buf;
	register char *source = yytext_ptr;
	register int number_to_move, i;
	int ret_val;

	if ( yy_c_buf_p > &yy_current_buffer->yy_ch_buf[yy_n_chars + 1] )
		YY_FATAL_ERROR(
		"fatal flex scanner internal error--end of buffer missed" );

	if ( yy_current_buffer->yy_fill_buffer == 0 )
		{ /* Don't try to fill the buffer, so this is an EOF. */
		if ( yy_c_buf_p - yytext_ptr - YY_MORE_ADJ == 1 )
			{
			/* We matched a single character, the EOB, so
			 * treat this as a final EOF.
			 */
			return EOB_ACT_END_OF_FILE;
			}

		else
			{
			/* We matched some text prior to the EOB, first
			 * process it.
			 */
			return EOB_ACT_LAST_MATCH;
			}
		}

	/* Try to read more data. */

	/* First move last chars to start of buffer. */
	number_to_move = (int) (yy_c_buf_p - yytext_ptr) - 1;

	for ( i = 0; i < number_to_move; ++i )
		*(dest++) = *(source++);

	if ( yy_current_buffer->yy_buffer_status == YY_BUFFER_EOF_PENDING )
		/* don't do the read, it's not guaranteed to return an EOF,
		 * just force an EOF
		 */
		yy_current_buffer->yy_n_chars = yy_n_chars = 0;

	else
		{
		int num_to_read =
			yy_current_buffer->yy_buf_size - number_to_move - 1;

		while ( num_to_read <= 0 )
			{ /* Not enough room in the buffer - grow it. */
#ifdef YY_USES_REJECT
			YY_FATAL_ERROR(
"input buffer overflow, can't enlarge buffer because scanner uses REJECT" );
#else

			/* just a shorter name for the current buffer */
			YY_BUFFER_STATE b = yy_current_buffer;

			int yy_c_buf_p_offset =
				(int) (yy_c_buf_p - b->yy_ch_buf);

			if ( b->yy_is_our_buffer )
				{
				int new_size = b->yy_buf_size * 2;

				if ( new_size <= 0 )
					b->yy_buf_size += b->yy_buf_size / 8;
				else
					b->yy_buf_size *= 2;

				b->yy_ch_buf = (char *)
					/* Include room in for 2 EOB chars. */
					yy_flex_realloc( (void *) b->yy_ch_buf,
							 b->yy_buf_size + 2 );
				}
			else
				/* Can't grow it, we don't own it. */
				b->yy_ch_buf = 0;

			if ( ! b->yy_ch_buf )
				YY_FATAL_ERROR(
				"fatal error - scanner input buffer overflow" );

			yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];

			num_to_read = yy_current_buffer->yy_buf_size -
						number_to_move - 1;
#endif
			}

		if ( num_to_read > YY_READ_BUF_SIZE )
			num_to_read = YY_READ_BUF_SIZE;

		/* Read in more data. */
		YY_INPUT( (&yy_current_buffer->yy_ch_buf[number_to_move]),
			yy_n_chars, num_to_read );

		yy_current_buffer->yy_n_chars = yy_n_chars;
		}

	if ( yy_n_chars == 0 )
		{
		if ( number_to_move == YY_MORE_ADJ )
			{
			ret_val = EOB_ACT_END_OF_FILE;
			yyrestart( yyin );
			}

		else
			{
			ret_val = EOB_ACT_LAST_MATCH;
			yy_current_buffer->yy_buffer_status =
				YY_BUFFER_EOF_PENDING;
			}
		}

	else
		ret_val = EOB_ACT_CONTINUE_SCAN;

	yy_n_chars += number_to_move;
	yy_current_buffer->yy_ch_buf[yy_n_chars] = YY_END_OF_BUFFER_CHAR;
	yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = YY_END_OF_BUFFER_CHAR;

	yytext_ptr = &yy_current_buffer->yy_ch_buf[0];

	return ret_val;
	}


/* yy_get_previous_state - get the state just before the EOB char was reached */

static yy_state_type yy_get_previous_state()
	{
	register yy_state_type yy_current_state;
	register char *yy_cp;

	yy_current_state = yy_start;

	for ( yy_cp = yytext_ptr + YY_MORE_ADJ; yy_cp < yy_c_buf_p; ++yy_cp )
		{
		register YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			yy_last_accepting_state = yy_current_state;
			yy_last_accepting_cpos = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 483 )
				yy_c = yy_meta[(unsigned int) yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
		}

	return yy_current_state;
	}


/* yy_try_NUL_trans - try to make a transition on the NUL character
 *
 * synopsis
 *	next_state = yy_try_NUL_trans( current_state );
 */

#ifdef YY_USE_PROTOS
static yy_state_type yy_try_NUL_trans( yy_state_type yy_current_state )
#else
static yy_state_type yy_try_NUL_trans( yy_current_state )
yy_state_type yy_current_state;
#endif
	{
	register int yy_is_jam;
	register char *yy_cp = yy_c_buf_p;

	register YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		yy_last_accepting_state = yy_current_state;
		yy_last_accepting_cpos = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 483 )
			yy_c = yy_meta[(unsigned int) yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
	yy_is_jam = (yy_current_state == 482);

	return yy_is_jam ? 0 : yy_current_state;
	}


#ifndef YY_NO_UNPUT
#ifdef YY_USE_PROTOS
static void yyunput( int c, register char *yy_bp )
#else
static void yyunput( c, yy_bp )
int c;
register char *yy_bp;
#endif
	{
	register char *yy_cp = yy_c_buf_p;

	/* undo effects of setting up yytext */
	*yy_cp = yy_hold_char;

	if ( yy_cp < yy_current_buffer->yy_ch_buf + 2 )
		{ /* need to shift things up to make room */
		/* +2 for EOB chars. */
		register int number_to_move = yy_n_chars + 2;
		register char *dest = &yy_current_buffer->yy_ch_buf[
					yy_current_buffer->yy_buf_size + 2];
		register char *source =
				&yy_current_buffer->yy_ch_buf[number_to_move];

		while ( source > yy_current_buffer->yy_ch_buf )
			*--dest = *--source;

		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		yy_current_buffer->yy_n_chars =
			yy_n_chars = yy_current_buffer->yy_buf_size;

		if ( yy_cp < yy_current_buffer->yy_ch_buf + 2 )
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}

	*--yy_cp = (char) c;


	yytext_ptr = yy_bp;
	yy_hold_char = *yy_cp;
	yy_c_buf_p = yy_cp;
	}
#endif	/* ifndef YY_NO_UNPUT */


#ifndef YY_NO_INPUT
#ifdef __cplusplus
static int yyinput()
#else
static int input()
#endif
	{
	int c;

	*yy_c_buf_p = yy_hold_char;

	if ( *yy_c_buf_p == YY_END_OF_BUFFER_CHAR )
		{
		/* yy_c_buf_p now points to the character we want to return.
		 * If this occurs *before* the EOB characters, then it's a
		 * valid NUL; if not, then we've hit the end of the buffer.
		 */
		if ( yy_c_buf_p < &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			/* This was really a NUL. */
			*yy_c_buf_p = '\0';

		else
			{ /* need more input */
			int offset = yy_c_buf_p - yytext_ptr;
			++yy_c_buf_p;

			switch ( yy_get_next_buffer() )
				{
				case EOB_ACT_LAST_MATCH:
					/* This happens because yy_g_n_b()
					 * sees that we've accumulated a
					 * token and flags that we need to
					 * try matching the token before
					 * proceeding.  But for input(),
					 * there's no matching to consider.
					 * So convert the EOB_ACT_LAST_MATCH
					 * to EOB_ACT_END_OF_FILE.
					 */

					/* Reset buffer status. */
					yyrestart( yyin );

					/* fall through */

				case EOB_ACT_END_OF_FILE:
					{
					if ( yywrap() )
						return EOF;

					if ( ! yy_did_buffer_switch_on_eof )
						YY_NEW_FILE;
#ifdef __cplusplus
					return yyinput();
#else
					return input();
#endif
					}

				case EOB_ACT_CONTINUE_SCAN:
					yy_c_buf_p = yytext_ptr + offset;
					break;
				}
			}
		}

	c = *(unsigned char *) yy_c_buf_p;	/* cast for 8-bit char's */
	*yy_c_buf_p = '\0';	/* preserve yytext */
	yy_hold_char = *++yy_c_buf_p;


	return c;
	}
#endif /* YY_NO_INPUT */

#ifdef YY_USE_PROTOS
void yyrestart( FILE *input_file )
#else
void yyrestart( input_file )
FILE *input_file;
#endif
	{
	if ( ! yy_current_buffer )
		yy_current_buffer = yy_create_buffer( yyin, YY_BUF_SIZE );

	yy_init_buffer( yy_current_buffer, input_file );
	yy_load_buffer_state();
	}


#ifdef YY_USE_PROTOS
void yy_switch_to_buffer( YY_BUFFER_STATE new_buffer )
#else
void yy_switch_to_buffer( new_buffer )
YY_BUFFER_STATE new_buffer;
#endif
	{
	if ( yy_current_buffer == new_buffer )
		return;

	if ( yy_current_buffer )
		{
		/* Flush out information for old buffer. */
		*yy_c_buf_p = yy_hold_char;
		yy_current_buffer->yy_buf_pos = yy_c_buf_p;
		yy_current_buffer->yy_n_chars = yy_n_chars;
		}

	yy_current_buffer = new_buffer;
	yy_load_buffer_state();

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	yy_did_buffer_switch_on_eof = 1;
	}


#ifdef YY_USE_PROTOS
void yy_load_buffer_state( void )
#else
void yy_load_buffer_state()
#endif
	{
	yy_n_chars = yy_current_buffer->yy_n_chars;
	yytext_ptr = yy_c_buf_p = yy_current_buffer->yy_buf_pos;
	yyin = yy_current_buffer->yy_input_file;
	yy_hold_char = *yy_c_buf_p;
	}


#ifdef YY_USE_PROTOS
YY_BUFFER_STATE yy_create_buffer( FILE *file, int size )
#else
YY_BUFFER_STATE yy_create_buffer( file, size )
FILE *file;
int size;
#endif
	{
	YY_BUFFER_STATE b;

	b = (YY_BUFFER_STATE) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_buf_size = size;

	/* yy_ch_buf has to be 2 characters longer than the size given because
	 * we need to put in 2 end-of-buffer characters.
	 */
	b->yy_ch_buf = (char *) yy_flex_alloc( b->yy_buf_size + 2 );
	if ( ! b->yy_ch_buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_is_our_buffer = 1;

	yy_init_buffer( b, file );

	return b;
	}


#ifdef YY_USE_PROTOS
void yy_delete_buffer( YY_BUFFER_STATE b )
#else
void yy_delete_buffer( b )
YY_BUFFER_STATE b;
#endif
	{
	if ( ! b )
		return;

	if ( b == yy_current_buffer )
		yy_current_buffer = (YY_BUFFER_STATE) 0;

	if ( b->yy_is_our_buffer )
		yy_flex_free( (void *) b->yy_ch_buf );

	yy_flex_free( (void *) b );
	}



#ifdef YY_USE_PROTOS
void yy_init_buffer( YY_BUFFER_STATE b, FILE *file )
#else
void yy_init_buffer( b, file )
YY_BUFFER_STATE b;
FILE *file;
#endif


	{
	yy_flush_buffer( b );

	b->yy_input_file = file;
	b->yy_fill_buffer = 1;

#if YY_ALWAYS_INTERACTIVE
	b->yy_is_interactive = 1;
#else
#if YY_NEVER_INTERACTIVE
	b->yy_is_interactive = 0;
#else
	b->yy_is_interactive = file ? (isatty( fileno(file) ) > 0) : 0;
#endif
#endif
	}


#ifdef YY_USE_PROTOS
void yy_flush_buffer( YY_BUFFER_STATE b )
#else
void yy_flush_buffer( b )
YY_BUFFER_STATE b;
#endif

	{
	if ( ! b )
		return;

	b->yy_n_chars = 0;

	/* We always need two end-of-buffer characters.  The first causes
	 * a transition to the end-of-buffer state.  The second causes
	 * a jam in that state.
	 */
	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	b->yy_at_bol = 1;
	b->yy_buffer_status = YY_BUFFER_NEW;

	if ( b == yy_current_buffer )
		yy_load_buffer_state();
	}


#ifndef YY_NO_SCAN_BUFFER
#ifdef YY_USE_PROTOS
YY_BUFFER_STATE yy_scan_buffer( char *base, yy_size_t size )
#else
YY_BUFFER_STATE yy_scan_buffer( base, size )
char *base;
yy_size_t size;
#endif
	{
	YY_BUFFER_STATE b;

	if ( size < 2 ||
	     base[size-2] != YY_END_OF_BUFFER_CHAR ||
	     base[size-1] != YY_END_OF_BUFFER_CHAR )
		/* They forgot to leave room for the EOB's. */
		return 0;

	b = (YY_BUFFER_STATE) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = size - 2;	/* "- 2" to take care of EOB's */
	b->yy_buf_pos = b->yy_ch_buf = base;
	b->yy_is_our_buffer = 0;
	b->yy_input_file = 0;
	b->yy_n_chars = b->yy_buf_size;
	b->yy_is_interactive = 0;
	b->yy_at_bol = 1;
	b->yy_fill_buffer = 0;
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer( b );

	return b;
	}
#endif


#ifndef YY_NO_SCAN_STRING
#ifdef YY_USE_PROTOS
YY_BUFFER_STATE yy_scan_string( yyconst char *yy_str )
#else
YY_BUFFER_STATE yy_scan_string( yy_str )
yyconst char *yy_str;
#endif
	{
	int len;
	for ( len = 0; yy_str[len]; ++len )
		;

	return yy_scan_bytes( yy_str, len );
	}
#endif


#ifndef YY_NO_SCAN_BYTES
#ifdef YY_USE_PROTOS
YY_BUFFER_STATE yy_scan_bytes( yyconst char *bytes, int len )
#else
YY_BUFFER_STATE yy_scan_bytes( bytes, len )
yyconst char *bytes;
int len;
#endif
	{
	YY_BUFFER_STATE b;
	char *buf;
	yy_size_t n;
	int i;

	/* Get memory for full buffer, including space for trailing EOB's. */
	n = len + 2;
	buf = (char *) yy_flex_alloc( n );
	if ( ! buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_bytes()" );

	for ( i = 0; i < len; ++i )
		buf[i] = bytes[i];

	buf[len] = buf[len+1] = YY_END_OF_BUFFER_CHAR;

	b = yy_scan_buffer( buf, n );
	if ( ! b )
		YY_FATAL_ERROR( "bad buffer in yy_scan_bytes()" );

	/* It's okay to grow etc. this buffer, and we should throw it
	 * away when we're done.
	 */
	b->yy_is_our_buffer = 1;

	return b;
	}
#endif


#ifndef YY_NO_PUSH_STATE
#ifdef YY_USE_PROTOS
static void yy_push_state( int new_state )
#else
static void yy_push_state( new_state )
int new_state;
#endif
	{
	if ( yy_start_stack_ptr >= yy_start_stack_depth )
		{
		yy_size_t new_size;

		yy_start_stack_depth += YY_START_STACK_INCR;
		new_size = yy_start_stack_depth * sizeof( int );

		if ( ! yy_start_stack )
			yy_start_stack = (int *) yy_flex_alloc( new_size );

		else
			yy_start_stack = (int *) yy_flex_realloc(
					(void *) yy_start_stack, new_size );

		if ( ! yy_start_stack )
			YY_FATAL_ERROR(
			"out of memory expanding start-condition stack" );
		}

	yy_start_stack[yy_start_stack_ptr++] = YY_START;

	BEGIN(new_state);
	}
#endif


#ifndef YY_NO_POP_STATE
static void yy_pop_state()
	{
	if ( --yy_start_stack_ptr < 0 )
		YY_FATAL_ERROR( "start-condition stack underflow" );

	BEGIN(yy_start_stack[yy_start_stack_ptr]);
	}
#endif


#ifndef YY_NO_TOP_STATE
static int yy_top_state()
	{
	return yy_start_stack[yy_start_stack_ptr - 1];
	}
#endif

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif

#ifdef YY_USE_PROTOS
static void yy_fatal_error( yyconst char msg[] )
#else
static void yy_fatal_error( msg )
char msg[];
#endif
	{
	(void) fprintf( stderr, "%s\n", msg );
	exit( YY_EXIT_FAILURE );
	}



/* Redefine yyless() so it works in section 3 code. */

#undef yyless
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
		yytext[yyleng] = yy_hold_char; \
		yy_c_buf_p = yytext + n; \
		yy_hold_char = *yy_c_buf_p; \
		*yy_c_buf_p = '\0'; \
		yyleng = n; \
		} \
	while ( 0 )


/* Internal utility routines. */

#ifndef yytext_ptr
#ifdef YY_USE_PROTOS
static void yy_flex_strncpy( char *s1, yyconst char *s2, int n )
#else
static void yy_flex_strncpy( s1, s2, n )
char *s1;
yyconst char *s2;
int n;
#endif
	{
	register int i;
	for ( i = 0; i < n; ++i )
		s1[i] = s2[i];
	}
#endif

#ifdef YY_NEED_STRLEN
#ifdef YY_USE_PROTOS
static int yy_flex_strlen( yyconst char *s )
#else
static int yy_flex_strlen( s )
yyconst char *s;
#endif
	{
	register int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
	}
#endif


#ifdef YY_USE_PROTOS
static void *yy_flex_alloc( yy_size_t size )
#else
static void *yy_flex_alloc( size )
yy_size_t size;
#endif
	{
	return (void *) malloc( size );
	}

#ifdef YY_USE_PROTOS
static void *yy_flex_realloc( void *ptr, yy_size_t size )
#else
static void *yy_flex_realloc( ptr, size )
void *ptr;
yy_size_t size;
#endif
	{
	/* The cast to (char *) in the following accommodates both
	 * implementations that use char* generic pointers, and those
	 * that use void* generic pointers.  It works with the latter
	 * because both ANSI C and C++ allow castless assignment from
	 * any pointer type to void*, and deal with argument conversions
	 * as though doing an assignment.
	 */
	return (void *) realloc( (char *) ptr, size );
	}

#ifdef YY_USE_PROTOS
static void yy_flex_free( void *ptr )
#else
static void yy_flex_free( ptr )
void *ptr;
#endif
	{
	free( ptr );
	}

#if YY_MAIN
int main()
	{
	yylex();
	return 0;
	}
#endif
#line 138 "scanner.l"

