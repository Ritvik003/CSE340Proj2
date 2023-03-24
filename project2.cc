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
#include <map>
#include <unordered_set>

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
void findTerminals_nonTerminals(vector<ruleSet> grammarRule){
        for (int i = 0; i < grammarRule.size(); i++) {
        addToNonTerminals(grammarRule[i].left);
        for (int j = 0; j < grammarRule[i].right.size(); j++) {
            if(inLeft(grammarRule[i].right[j],grammarRule)){
                addToNonTerminals(grammarRule[i].right[j]);
            }
            else{
                addToTerminals(grammarRule[i].right[j]);
            }
        }
    }
}

void printTerminalsAndNoneTerminals()
{
    //ReadGrammar();
    // parses throght all lefts and rights to determine the terminals and non terminals
    findTerminals_nonTerminals(grammar);
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
    findTerminals_nonTerminals(grammar);
    getCondensedGrammar();
    findTerminals_nonTerminals(condensedGrammar);
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
struct newruleSet {
    string left;
    vector< vector<string> > right;
};
vector<newruleSet> definition;
map < string,set<string> > first_sets;
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
    /*
    for (newruleSet nrs : definition) {
        cout << nrs.left << ": ";

        for (auto const i : nrs.right) {
            for (auto const j : i) {
                cout << j;
            }
            cout << " or ";
        }
        cout<<endl;
    }
    */
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
void printFirstSets(){
    for (auto const pair : first_sets) {
        cout << "FIRST(" << pair.first << ") = {";

        for (string const s : pair.second) {
            cout << s << ", ";
        }
        cout << "}" << endl;
    }
} 



void CalculateFirstSets()
{
    cout<< "\nOUR OUTPUT"<<endl;
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
    int loopVariable =10;
    while (loopVariable !=10) {
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
                        else {
                            if (!(hasEpsilon(ors[i]))) {
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
        loopVariable++;
    }
    
    //print em out
   printFirstSets();
    //printFirstSets();
    cout << "3\n";
}

// Task 4

map <string,set<string> > follow_sets;

void getFollowSets(){
    CalculateFirstSets();
    for (auto n : definition) {
        set<string> empty_set;
        follow_sets.insert({n.left, empty_set});
    }
    //add $ to follow(S)
   map <string,set<string> >::iterator FS_it = follow_sets.begin();
   while(FS_it!=follow_sets.end()){
    if(FS_it->first.compare(print_nt[0])==0){
        FS_it->second.insert("$");
    }
    ++FS_it;
   } 
    //for each non-terminal get the basic composition of the follow set (only 1 pass required)

    for(int i=0;i<definition.size();i++){
        //inside the ruleSet
        for(int j=0;j<definition[i].right.size();j++){
            //inside the right-hand sides
            for(int k=0;k<definition[i].right[j].size()-1;k++){
                //inside the particular rule
                string curr_nt =definition[i].right[j][k] ;
                if(inLeft(curr_nt,condensedGrammar)){
                        for(int t = k+1;t<definition[i].right[j].size();t++){
                            string check_nt =definition[i].right[j][t]; 
                            if(inLeft(definition[i].right[j][t],condensedGrammar)){
                                //add first of t - epsilon in follow of k
                                bool hasEpsilon = first_sets.find(check_nt)->second.find("#")!=first_sets.find(check_nt)->second.end();
                                set<string> newset = first_sets[check_nt];
                                newset.erase("#");
                                set<string> tempfirst;
                                set_union(newset.begin(), newset.end(), follow_sets[curr_nt].begin(), follow_sets[curr_nt].end(), inserter(tempfirst, tempfirst.begin()));
                                follow_sets[curr_nt] = tempfirst;
                                if(!hasEpsilon){
                                    break;
                                } 

                            }
                            else{
                                //when terminal
                                follow_sets.find(curr_nt)->second.insert(check_nt);
                                break;
                            }
                        }
                }
            }
        }
    }
    bool changed = true;
    //while change is true : apply rules 2 and 3 for every rule
    while(changed == true){
        changed = false;
        for(int i = 0; i<definition.size();i++){
            //for each lhs
            string left_nt = definition[i].left;
            for (int j=0;j<definition[i].right.size();j++){
                //for each rule
                for(int k = definition[i].right[j].size()-1;k>=0;k++){
                    //for each _t or _nt in the rule (reverse order)
                    string curr_nt = definition[i].right[j][k];
                     
                    // if the component is _nt
                    if(inLeft(curr_nt,condensedGrammar)){
                    //combine the two follow sets and insert to follow(b) if follow(a) is not a subset of follow(b)
                    if(!includes(follow_sets[curr_nt].begin(),follow_sets[curr_nt].end(),follow_sets[left_nt].begin(),follow_sets[left_nt].end())){
                    set<string> newset = follow_sets[left_nt];
                    set<string> tempfollow;
                    set_union(newset.begin(), newset.end(), follow_sets[curr_nt].begin(), follow_sets[curr_nt].end(), inserter(tempfollow, tempfollow.begin()));
                    follow_sets[curr_nt] = tempfollow;
                    changed = true;
                    }
                    //stop iterating if the first of _nt doesn't have epsilon
                    bool hasEpsilon = first_sets.find(curr_nt)->second.find("#")!=first_sets.find(curr_nt)->second.end();
                    if(!hasEpsilon){
                        break;
                    }                        
                    }
                    else{
                        //stop iterating through if the curr_nt is terminal
                        break;
                    }
                    
                }
            }
        }
    }
}

void CalculateFollowSets()

{
    getFollowSets();
    
    //format the output
    map <string,set<string> >::iterator FS_it = follow_sets.begin();
    while(FS_it!=follow_sets.end()){
        cout<<"FOLLOW("<<FS_it->first<<") = {";
        set<string> curr_fs = FS_it->second;
        for(string curr : print_t){
            if(curr_fs.find(curr)!=curr_fs.end()){
                cout<<curr<<", ";
            }
        }
        cout<<endl;
        ++FS_it;
    }
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