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
#include <set>

using namespace std;
LexicalAnalyzer lexer = LexicalAnalyzer();


struct ruleSet {
    string left;
    vector<string> right;
    vector<string> terminal;
    vector<string> nont;
};

vector<ruleSet> grammar;
vector<string> print_t;
vector<string> print_nt;
std::set<string> set;

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


// make sure to add each ruleSet
// read grammar
void ReadGrammar()
{
    bool isid;
    string maybe_add_id;
    Token curr_token = lexer.GetToken();
    while(curr_token.token_type!=HASH&&curr_token.token_type!=END_OF_FILE){
        ruleSet *rule = new ruleSet; //check if this should be here
        if (curr_token.token_type == ID) {
            //add non terminal to the data structure
            rule->left = curr_token.token_type;
            //if the grammar isn't a syntax error, then store the id in a variable.
            maybe_add_id = curr_token.token_type;
            curr_token = lexer.GetToken();
            if (curr_token.token_type == ARROW) {
                parse_right(rule);
                if (curr_token.token_type == STAR) {
                    //this verifies the grammar is correct, so add it to the set
                    set.insert(maybe_add_id);
                    grammar.push_back(*rule);
                    curr_token = lexer.GetToken(); //go to the next line
                }
                else {
                    delete rule;
                    syntax_error();
                    return;
                }
            }
            else {
                delete rule;
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
    ReadGrammar();
    //print non-terminals
    for (int i = 0; i < grammar.size(); i++) {
        cout << grammar[i].left;
    }
    //print terminals
    for (int i = 0; i < grammar.size(); i++) {
        for (int j = 0; j < grammar[i].left.size(); j++) {
            cout << grammar[i].left[j] + "\n";
        }
    }
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


