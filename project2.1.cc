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
#include <unordered_set>
#include <map>

using namespace std;
LexicalAnalyzer lexer = LexicalAnalyzer();


struct ruleSet {
    string left;
    vector<string> right;
    vector<string> terminal;
    vector<string> nont;
};

struct newruleSet {
    string left;
    vector<vector<string>> right;
};

vector<ruleSet> grammar;
vector<string> print_t;
vector<string> print_nt;
std::set<string> setforsomething;
vector<newruleSet> definition;

map<string, set<string>> first_sets;


bool inLeft (string lexeme,vector<ruleSet> grammarRule){
    for(int i=0;i<grammarRule.size();i++){
            if(lexeme.compare(grammarRule[i].left)==0){
                return true;
        }
    }
    return false;
}
void addToTerminals(string lexeme){
    for(int i =0; i<print_t.size();i++){
        if(lexeme.compare(print_t[i])==0){
            return;
        }
    }
    print_t.push_back(lexeme);
}
void addToNonTerminals(string lexeme){
    for(int i =0; i<print_nt.size();i++){
        if(lexeme.compare(print_nt[i])==0){
            return;
        }
    }

    print_nt.push_back(lexeme);
}
void syntax_error() {
    cout << "syntax error";
}
void findTerminals_nonTerminals(){
        for (int i = 0; i < grammar.size(); i++) {
        addToNonTerminals(grammar[i].left);
        for (int j = 0; j < grammar[i].right.size(); j++) {
            if(inLeft(grammar[i].right[j],grammar)){
                addToNonTerminals(grammar[i].right[j]);
            }
            else{
                addToTerminals(grammar[i].right[j]);
            }
        }
    }
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
            rule->left = curr_token.lexeme;
            //if the grammar isn't a syntax error, then store the id in a variable.
            curr_token = lexer.GetToken();
            if (curr_token.token_type == ARROW) {
                parse_right(rule);
                curr_token = lexer.GetToken();
                if (curr_token.token_type == STAR) {
                    //this verifies the grammar is correct, so add it to the set
                    //set.insert(maybe_add_id);
                    grammar.push_back(*rule);
                    //curr_token = lexer.GetToken(); //go to the next line
                }
                else {
                    delete rule;
                    cout<<"here1\n";
                    syntax_error();
                    return;
                }
            }
            else {
                delete rule;
                cout<<"here2\n";
                syntax_error();
                return;
            }
        }
        else {
            syntax_error();
            return;
        }
        curr_token = lexer.GetToken();
       // cout<<"checking exiting token\n";
        //curr_token.Print();
    }
    //cout<<"checking token\n";
    //curr_token.Print();
    if (curr_token.token_type == END_OF_FILE) {
        syntax_error();
        cout<<"\nhere3\n";
        return;
    }
    if(curr_token.token_type==HASH){
    curr_token = lexer.GetToken();
    if(curr_token.token_type!=END_OF_FILE){
        syntax_error();
    }
    }

}

// Task 1
void printTerminalsAndNoneTerminals()
{
    //ReadGrammar();
    // parses throght all lefts and rights to determine the terminals and non terminals
    findTerminals_nonTerminals();
    for(int i =0; i<print_t.size();i++){
        cout<<print_t[i]<<" ";
    }
    for(int i =0; i<print_nt.size();i++){
        cout<<print_nt[i]<<" ";
    } 
    cout<<endl;
    
}
vector<ruleSet> condensedGrammar;
// Task 2
void RemoveUselessSymbols()
{
    findTerminals_nonTerminals();

}

void createNewRuleSet() {
    unordered_set<string> lefts;
    for (int i = 0; i < grammar.size(); i++) {
        if (lefts.count(grammar[i].left) == 0) {
            newruleSet nrs;
            nrs.left = grammar[i].left;
            nrs.right.push_back(grammar[i].right);
            definition.push_back(nrs);
            lefts.insert(grammar[i].left);
        }
        else {
            for (int j = 0; j < definition.size(); j++) {
                if (definition[j].left == grammar[i].left) {
                    definition[j].right.push_back(grammar[i].right);
                    break;
                }
            }
        }
    }
}

bool terminates(newruleSet rule, int index) {
    if (find(print_t.begin(), print_t.end(), rule.right[0]) != print_t.end()) {
        first_sets[rule.left].insert(rule.right[index][0]);
    }

    return true;
}

bool isTerminal(string s) {
     if (find(print_t.begin(), print_t.end(), s) == print_t.end()) {
        return false;
     }
     return true;
}

bool isLast(string s, vector<string> vec, int index) {
    if (vec[index] == vec[vec.size()-1]) {
        return true;
    }
    return false;
}

bool hasEpsilon(string key) {
    return first_sets.find(key)->second.find("#")!=first_sets.find(key)->second.end();
}

bool isSubset(set<string> set1, set<string> set2) {
    return includes(set1.begin(), set1.end(), set2.begin(), set2.end());
}

// Task 3
void CalculateFirstSets()
{
    map<string, int> id;
    int counter = 0;
    //create the new ruleset
    createNewRuleSet();

    //first initialize empty sets
    for (auto n : definition) {
        set<string> empty_set;
        first_sets.insert({n.left, empty_set});
        id.insert({n.left, counter});
        counter++;
    }

    bool changed = true;
    

    //make a boolean array with size set to number of non terminals
    bool haschanged[definition.size()];

    for (int i = 0; i < definition.size(); i++) {
        haschanged[i] = true;
    }

    while (changed) {
        for (newruleSet rule : definition) {
            for (auto ors : rule.right) {
                //here we're considering epsilon
                //
                if (ors.size() == 0) {
                        first_sets[rule.left].insert("#");
                        continue;
                    }
                //this is the same as (for string s : ors)
                for (int i = 0; i < ors.size(); i++) {
                        if (isTerminal(ors[i])) {
                            first_sets[rule.left].insert(ors[i]);
                            break;
                        }
                    else {
                        //add everything from the terminal minus epsilon to the first set
                        set<string> old_set = first_sets[rule.left];
                        set<string> newset = first_sets[ors[i]];
                        haschanged[id[rule.left]] = isSubset(old_set, newset);
                        newset.erase("#");
                        set<string> tempfirst;
                        set_union(newset.begin(), newset.end(), first_sets[rule.left].begin(), first_sets[rule.left].end(), inserter(tempfirst, tempfirst.begin()));
                        first_sets[rule.left] = tempfirst;
                        if (isLast(ors[i], ors, i)) {
                            if (hasEpsilon(ors[i])) {
                            first_sets[rule.left].insert("#");
                            }
                            else {
                                break;
                            }
                        }
                    }
                }
            }
        }
        int i = 0;
        bool all_false;
        for (bool b : haschanged) {
            if (b == true) all_false = false;
        }
        if (all_false) changed = false;
    }
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