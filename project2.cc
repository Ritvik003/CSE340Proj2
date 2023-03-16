/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "lexer.h"

using namespace std;
LexicalAnalyzer lexer = LexicalAnalyzer();


struct ruleSet {
    string left;
    vector<string> right;
    vector<string> terminal;
    vector<string> nont;
};

vector<ruleSet> grammar;

void syntax_error() {
    cout << "syntax error";
}

void parse_right(ruleSet *curr_rule) {
    vector<string> out;
    while (lexer.peek(1).token_type != STAR) {
        Token test = lexer.GetToken();
        if (test.token_type != ID) {
            syntax_error();
            return;
        }
        else{
            curr_rule->right.push_back(test.lexeme);
        }
    }
}


// read grammar
void ReadGrammar()
{
    bool isid;
    Token curr_token = lexer.GetToken();
    while(curr_token.token_type!=HASH&&curr_token.token_type!=END_OF_FILE){
        if (curr_token.token_type == ID) {
            //add non terminal to the data structure
            curr_token = lexer.GetToken();
            if (curr_token.token_type == ARROW) {
                parse_right();
                if (curr_token.token_type == STAR) {

                }
                else {
                    syntax_error();
                    return;
                }
            }
            else {
                syntax_error();
                return;
            }
        }
        else {
            syntax_error();
            return;
        }
        curr_token = lexer.GetToken();
    }
    if (curr_token.token_type == END_OF_FILE) {
        syntax_error();
        return;
    }
    curr_token = lexer.GetToken();
    if(curr_token.token_type!=END_OF_FILE){
        syntax_error();
    }



    cout << "0\n";
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    
    cout << "1\n";
}

// Task 2
void RemoveUselessSymbols()
{
    cout << "2\n";
}

// Task 3
void CalculateFirstSets()
{
    cout << "3\n";
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    cout << "5\n";
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}


