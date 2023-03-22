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
    bool reachable;
    bool generating;
};

vector<ruleSet> grammar;
vector<string> print_t;
vector<string> print_nt;
std::set<string> setforsomething;
bool inLeft (string lexeme,vector<ruleSet> grammarRule){
    for(int i=0;i<grammarRule.size();i++){
            if(lexeme.compare(grammarRule[i].left)==0){
                return true;
        }
    }
    return false;
}

void syntax_error() {
    cout << "SYNTAX ERROR !!!\n";
}


bool parse_right(ruleSet *curr_rule) {

    while (lexer.peek(1).token_type != STAR) {
        Token test = lexer.GetToken();
        if (test.token_type != ID) {
            return true;;
        }
        else{
            curr_rule->right.push_back(test.lexeme);
        }
    }
    return false;
}


// make sure to add each ruleSet
// read grammar
void ReadGrammar()
{
    Token curr_token = lexer.GetToken();    
    while(curr_token.token_type!=HASH&&curr_token.token_type!=END_OF_FILE){
        ruleSet *rule = new ruleSet; //check if this should be here
        if (curr_token.token_type == ID) {
            rule->left = curr_token.lexeme;  //add non terminal to the data structure
            rule->generating=false;
            rule->reachable = false;
            curr_token = lexer.GetToken();
            if (curr_token.token_type == ARROW) {
                if(parse_right(rule)){
                    syntax_error();
                    return;
                }
                curr_token = lexer.GetToken();
                if (curr_token.token_type == STAR) {
                    //this verifies the grammar is correct, so add it to the vector
                    grammar.push_back(*rule);
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
       // cout<<"checking exiting token\n";
        //curr_token.Print();
    }
    //cout<<"checking token\n";
    //curr_token.Print();
    if (curr_token.token_type == END_OF_FILE) {
        syntax_error();
        return;
    }
    if(curr_token.token_type==HASH){
    curr_token = lexer.GetToken();
    if(curr_token.token_type!=END_OF_FILE){
        syntax_error();
        return;
    }
    }

}

// Task 1
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


// Task 2
vector<ruleSet> condensedGrammar;
int num_generating = 0;
int num_reachable = 0;

void turnReachable(string lexeme){
    for (int i=0;i<grammar.size();i++){
        if(grammar[i].left.compare(lexeme)==0&&!grammar[i].reachable){
            grammar[i].reachable=true;
            num_reachable++;
        }
    }
}
bool isGenerating(string lexeme){
       for (int i=0;i<grammar.size();i++){
        if(grammar[i].left.compare(lexeme)==0&&grammar[i].generating==true){
           return true;
        }
    } 
    return false;
}
bool findGenerating(int num){
    ruleSet mainRule = grammar[num];
    for(int i =0; i<mainRule.right.size();i++){
        if(inLeft(mainRule.right[i],grammar)&&!isGenerating(mainRule.right[i])){
            return false;
        }

    }
    return true;
}
void getCondensedGrammar(){
    turnReachable(grammar[0].left);//base grammar is always reachable
    int prev_reachable = 0;
    int prev_generating = 0;
    while(prev_generating!=num_generating||prev_reachable!=num_reachable){
        //cout<<prev_generating<<" , "<<num_generating<<endl;
        //cout<<prev_reachable<<" , "<<num_reachable<<endl;
        prev_generating=num_generating;
        prev_reachable=num_reachable;
        
        for(int i =0; i<grammar.size();i++){
            if(grammar[i].generating ==false && findGenerating(i)){
                grammar[i].generating = true;
                num_generating++;
            }
        }    
        for(int i =0; i<grammar.size();i++){
            if(grammar[i].reachable ==true){
                bool status = true;
                for(int j=0; j<grammar[i].right.size();j++){
                    if(inLeft(grammar[i].right[j],grammar)&&!isGenerating(grammar[i].right[j])){
                        status = false;
                        break;
                    }
                }
                if(status == true){
                    if(!grammar[i].generating){
                    grammar[i].generating=true;
                    num_generating++;
                    }
                    for(int j=0; j<grammar[i].right.size();j++){
                        if(inLeft(grammar[i].right[j],grammar)&&isGenerating(grammar[i].right[j])){
                            turnReachable(grammar[i].right[j]);
                        }
                    }
             
                }
            }
       }
       
    }
    //adds to condensed grammar
    
    for(int i=0;i<grammar.size();i++){
        if(grammar[i].generating==true&&grammar[i].reachable==true){
            condensedGrammar.push_back(grammar[i]);
        }
    }
}
void RemoveUselessSymbols()
{   
    //cout<<"2"<<endl;
    findTerminals_nonTerminals();
    getCondensedGrammar();
    
    for(int i =0; i<condensedGrammar.size();i++){
        cout<<condensedGrammar[i].left<<" -> ";
        if(condensedGrammar[i].right.size()==0){
            cout<<"#";
        }
        else{
            cout<<condensedGrammar[i].right[0];
        }
        for(int j=1;j<condensedGrammar[i].right.size();j++){
            cout<<" "<<condensedGrammar[i].right[j];
        }
        cout<<endl;
    }
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