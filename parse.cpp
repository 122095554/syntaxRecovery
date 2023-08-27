#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <regex>

using namespace std;

#include "scan.h"

const string names[] = {"read", "write", "id", "literal", "gets", "add",
                       "sub", "mul", "div", "==", "<>", ">",
                       "<", ">=", "<=", "lparen",
                       "rparen", "if", "fi", "do", "od", "check", "$$"};

//ad hoc first sets
vector<token> P = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
vector<token> SL {t_id, t_read, t_write, t_if, t_do, t_check};
vector<token> S {t_id, t_read, t_write, t_if, t_do, t_check};
vector<token> R {t_lparen, t_id, t_literal};
vector<token> E {t_lparen, t_id, t_literal};
vector<token> T {t_lparen, t_id, t_literal};
vector<token> F {t_lparen, t_id, t_literal};
vector<token> ET {t_equals, t_notequals, t_greater, t_lessthan, t_greaterequals, t_lessthanequals};
vector<token> TT {t_add, t_sub};
vector<token> FT {t_mul, t_div};
vector<token> ro {t_equals, t_notequals, t_greater, t_lessthan, t_greaterequals, t_lessthanequals};
vector<token> ao {t_add, t_sub};
vector<token> mo {t_mul, t_div};

//ad hoc follow sets
vector<token> Pflw {};
vector<token> SLflw {t_fi, t_od, t_eof};
vector<token> Sflw {t_id, t_read, t_write, t_if, t_do, t_check,
        t_fi, t_od, t_eof};
vector<token> Rflw {t_rparen, t_id, t_read, t_write, t_if, t_do,
        t_check, t_fi, t_od, t_eof};
vector<token> Eflw {t_equals, t_notequals, t_greater, t_lessthan, t_greaterequals,
        t_lessthanequals, t_rparen, t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> Tflw {t_add, t_sub, t_equals, t_notequals, t_greater, t_lessthan,
        t_greaterequals, t_lessthanequals, t_rparen, t_id, t_read, t_write, t_if, t_do,
        t_check, t_fi, t_od, t_eof};
vector<token> Fflw {t_mul, t_div, t_add, t_sub, t_equals, t_notequals, t_greater,
        t_lessthan, t_greaterequals, t_lessthanequals, t_rparen, t_id, t_read, t_write,
        t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> ETflw {t_rparen, t_id, t_read, t_write, t_if, t_do, t_check,
        t_fi, t_od, t_eof};
vector<token> TTflw {t_literal, t_equals, t_notequals, t_greater, t_lessthan, t_greaterequals,
        t_lessthanequals, t_rparen, t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> FTflw {t_add, t_sub, t_equals, t_notequals, t_greater, t_lessthan,
        t_greaterequals, t_lessthanequals, t_rparen, t_id, t_read, t_write, t_if,
        t_do, t_check, t_fi, t_od, t_eof};
vector<token> roflw {t_lparen, t_id, t_literal};
vector<token> aoflw {t_lparen, t_id, t_literal};
vector<token> moflw {t_lparen, t_id, t_literal};

string syntaxTree = "";
static token upcoming_token;

void error (token t) {
    cerr << "Syntax error: " << names[t] << endl;
    return;
}

void match (token expected) {
    if (upcoming_token == expected) {
        cout << "Matched " << names[upcoming_token];
        if (upcoming_token == t_id || upcoming_token == t_literal){
            cout << ": " << token_image;
			 syntaxTree += token_image;
            syntaxTree += " ";
        }
        else{
            syntaxTree += names[expected] + " ";
        }
        cout << endl;
        upcoming_token = scan ();
    }
    else {
        cout << "expected: " << names[expected] << ", saw: " << names[upcoming_token] << endl;
    }
}

bool contains(vector<token> v, token check){
        for(int i = 0; i<v.size(); i++){
                if(upcoming_token == v[i]){
                        return true;
                }
        }
        return false;
}

void check_for_error(vector<token> first, vector<token> follow, bool epsilon){
        if(!(contains(first, upcoming_token) || epsilon)){
                error(upcoming_token);
                do{
                        upcoming_token = scan();
                } while(!(contains(first, upcoming_token) || contains(follow, upcoming_token)
                                        || upcoming_token == t_eof));
        }
}

void program ();
void stmt_list ();
void stmt ();
void relat ();
void expr ();
void expr_tail ();
void term ();
void term_tail ();
void factor ();
void factor_tail ();
void rel_op ();
void add_op ();
void mul_op ();

void program () {
    check_for_error(P, Pflw, false);
    syntaxTree += "(p";
    switch (upcoming_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict program --> stmt_list eof\n";
            stmt_list ();
            match (t_eof);
            break;
        default:
            cout << "program";
            return;
    }
    syntaxTree += ")";
    regex pattern("\\(\\)");
    syntaxTree = regex_replace(syntaxTree, pattern, "");
    cout << syntaxTree << endl;
}

void stmt_list () {
    check_for_error(SL, SLflw, true);
    switch (upcoming_token) {
        case t_id:
        case t_read:
		 case t_write:
        case t_if:
        case t_do:
        case t_check:
            cout << "predict stmt_list --> stmt stmt_list\n";
            syntaxTree += "(sl";
            stmt ();
            stmt_list ();
            syntaxTree += ")";
            break;
        case t_eof:
        case t_fi:
        case t_od:
            cout << ("predict stmt_list --> epsilon\n");
            break;          /* epsilon production */
        default:
            cout << "stmt_list";
            return;
    }
}

void stmt () {
    check_for_error(S, Sflw, false);
    syntaxTree += "(s";
    switch (upcoming_token) {
        case t_id:
            cout << ("predict stmt --> id gets rela\n");
            match (t_id);
            match (t_gets);
            relat();
            break;
        case t_read:
            cout << ("predict stmt --> read id\n");
            match (t_read);
            match (t_id);
            break;
        case t_write:
		   cout << ("predict stmt --> write rela\n");
            match (t_write);
            relat();
            break;
        case t_if:
            cout << ("predict stmt --> if rela stmt_list fi\n");
            match(t_if);
            relat();
            stmt_list();
            match(t_fi);
            break;
        case t_do:
            cout << ("predict stmt --> do stmt_list od\n");
            match(t_do);
            stmt_list();
            match(t_od);
            break;
        case t_check:
            cout << ("predict stmt --> check rela\n");
            match(t_check);
            relat();
            break;
        default:
            cout << "stmt";
            return;
    }
    syntaxTree += ")";
}

void relat() {
    check_for_error(R, Rflw, false);
    syntaxTree += "(r";
    switch(upcoming_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                    cout << ("predict relat --> expr expr_tail\n");
					 expr ();
                    expr_tail ();
                    break;
            default:
                    cout << "relat";
                    return;
    }
    syntaxTree += ")";
}

void expr () {
    check_for_error(E, Eflw, false);
    syntaxTree += "(";
    switch (upcoming_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << ("predict expr --> term term_tail\n");
            term ();
            term_tail ();
            break;
       default:
            cout << "expr";
            return;
    }
    syntaxTree += ")";
}

void expr_tail() {
    check_for_error(ET, ETflw, true);
    switch(upcoming_token) {
        case t_equals:
        case t_notequals:
        case t_greater:
        case t_lessthan:
        case t_greaterequals:
        case t_lessthanequals:
		syntaxTree += "(";
                rel_op();
                expr();
                syntaxTree += ")";
                break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        case t_if:
        case t_fi:
        case t_do:
        case t_od:
        case t_check:
                cout << ("predict expr_tail --> epsilon\n"); //epsilon
                break;
        default:
                cout << "expr_tail";
                return;
    }
}

void term () {
    check_for_error(T, Tflw, false);
    syntaxTree += "(";
    switch (upcoming_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << ("predict term --> factor factor_tail\n");
            factor ();
            factor_tail ();
            break;
        default:
            cout << "term";
            return;
	}
    syntaxTree += ")";
}

void term_tail () {
    check_for_error(TT, TTflw, true);
    switch (upcoming_token) {
        case t_add:
        case t_sub:
            cout << ("predict term_tail --> add_op term term_tail\n");
            syntaxTree += "(";
            add_op ();
            term ();
            term_tail ();
            syntaxTree += ")";
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        case t_if:
        case t_fi:
        case t_do:
        case t_od:
        case t_check:
        case t_equals:
        case t_notequals:
        case t_greater:
        case t_lessthan:
        case t_greaterequals:
        case t_lessthanequals:
           cout << ("predict term_tail --> epsilon\n");
           break;          /* epsilon production */
        default:
           cout << "term_tail";
           return;
		}
}

void factor () {
    check_for_error(F, Fflw, false);
    syntaxTree += "(";
    switch (upcoming_token) {
        case t_literal:
            cout << ("predict factor --> literal\n");
            match (t_literal);
            break;
        case t_id :
            cout << ("predict factor --> id\n");
            match (t_id);
            break;
        case t_lparen:
            cout << ("predict factor --> lparen expr rparen\n");
            match (t_lparen);
            relat ();
            match (t_rparen);
            break;
        default:
            cout << "factor";
            return;
    }
    syntaxTree += ")";
}

void factor_tail () {
    check_for_error(FT, FTflw, true);
    switch (upcoming_token) {
        case t_mul:
        case t_div:
            cout << ("predict factor_tail --> mul_op factor factor_tail\n");
            syntaxTree += "(";
            mul_op ();
            factor ();
			factor_tail ();
            syntaxTree += ")";
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        case t_if:
        case t_fi:
        case t_do:
        case t_od:
        case t_check:
        case t_equals:
        case t_notequals:
        case t_greater:
        case t_lessthan:
        case t_greaterequals:
        case t_lessthanequals:
            cout << ("predict factor_tail --> epsilon\n");
            break;          /* epsilon production */
        default:
            cout << "factor_tail";
            return;
    }
}

void rel_op () {
    check_for_error(ro, roflw, false);
    syntaxTree += "(";
    switch(upcoming_token) {
            case t_equals:
                    cout << ("predict rel_op --> equals\n");
                    match(t_equals);
                    break;
			case t_notequals:
                    cout << ("predict rel_op --> notequals\n");
                    match(t_notequals);
                    break;
            case t_greater:
                    cout << ("predict rel_op --> greater\n");
                    match(t_greater);
                    break;
            case t_lessthan:
                    cout << ("predict rel_op --> lessthan\n");
                    match(t_lessthan);
                    break;
            case t_greaterequals:
                    cout << ("predict rel_op --> add\n");
                    match(t_greaterequals);
                    break;
            case t_lessthanequals:
                    cout << ("predict rel_op --> add\n");
                    match(t_lessthanequals);
                    break;
            default:
                    cout << "rel_op";
                    return;
        }
    syntaxTree += ")";
}

void add_op () {
    check_for_error(ao, aoflw, false);
    syntaxTree += "(";
    switch (upcoming_token) {
        case t_add:
           cout << ("predict add_op --> add\n");
           match (t_add);
            break;
        case t_sub:
            cout << ("predict add_op --> sub\n");
			 match (t_sub);
            break;
        default:
            cout << "add_op";
            return;
    }
    syntaxTree += ")";
}

void mul_op () {
    check_for_error(mo, moflw, false);
    syntaxTree += "(";
    switch (upcoming_token) {
        case t_mul:
            cout << ("predict mul_op --> mul\n");
            match (t_mul);
            break;
        case t_div:
            cout << ("predict mul_op --> div\n");
            match (t_div);
            break;
        default:
            cout << "mul_op";
            return;
    }
    syntaxTree += ")";
}

int main () {
    upcoming_token = scan ();
    program ();
    return 0;
}
                                                                
                                           
                                                                                                         
                                                     
												
                                   
