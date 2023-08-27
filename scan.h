/* Definitions the scanner shares with the parser.
   Michael L. Scott, 2008-2021.
*/

enum token {t_read, t_write, t_id, t_literal, t_gets, t_add,
              t_sub, t_mul, t_div, t_equals, t_notequals, t_greater,
              t_lessthan, t_greaterequals, t_lessthanequals, t_lparen,
              t_rparen, t_if, t_fi, t_do, t_od, t_check, t_eof};

#define MAX_TOKEN_LEN 200
extern char token_image[MAX_TOKEN_LEN];

extern token scan();