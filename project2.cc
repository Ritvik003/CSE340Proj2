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
bool syntaxError = false;

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
    syntaxError = true;
    cout << "SYNTAX ERROR !!!\n";
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
//TASK 0
void ReadGrammar()
{
    Token curr_token = lexer.GetToken();
    while(curr_token.token_type!=HASH&&curr_token.token_type!=END_OF_FILE){
        ruleSet *rule = new ruleSet; //check if this should be here
        if (curr_token.token_type == ID) {
            //add non terminal to the data structure
            rule->left = curr_token.lexeme;
            rule->generating=false;
            rule->reachable = false;
            //if the grammar isn't a syntax error, then store the id in a variable.
            curr_token = lexer.GetToken();
            if (curr_token.token_type == ARROW) {
                //error prone
                if(parse_right(rule)){
                    syntax_error();
                    return;
                }
                curr_token = lexer.GetToken();
                if (curr_token.token_type == STAR) {
                    //this verifies the grammar is correct, so add it to the set
                    //set.insert(maybe_add_id);
                    grammar.push_back(*rule);
                    //curr_token = lexer.GetToken(); //go to the next line
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
    
}

bool isTerminal(string s) {
     for (int i =0;i<print_t.size();i++){
        if(print_t[i].compare(s)==0){
            return true;
        }
     }
     return false;
}
bool isLast(string s, vector<string> vec, int index) {
    if (index== vec.size()-1) {
        return true;
    }
    return false;
}

bool hasEpsilon(string key) {
    return first_sets.find(key)->second.find("#")!=first_sets.find(key)->second.end();
}

bool isSubset(set<string> set1, set<string> set2) {
    for( string curr  : set2){
        if(set1.count(curr)==0){
            return false;
        }
    }
    return true;
}

set<string> unify(set<string> set1, set<string> set2) {
    set<string> result = set1;
    result.insert(set2.begin(), set2.end());
    return result;
}

void printFirstSets(map<string,set<string> > currMap){
    for(string curr_nt : print_nt){
        if(currMap.find(curr_nt)!=currMap.end()){
            cout<<"FIRST"<<"("<<curr_nt<<") = { ";
            bool isFirst = true;
            set<string> currSet= currMap[curr_nt];
            if(currSet.find("#")!=currSet.end()){
                cout<<"#";
                isFirst = false;
            }
            for(string curr_t : print_t){
                if(currSet.find(curr_t)!=currSet.end()){
                    if(isFirst){
                        isFirst=false;
                        cout<<curr_t;
                    }
                    else{
                    cout<<", "<<curr_t;
                    }
                }
            }
            cout<<" }";

        }
        cout<<endl;
    }
}
void getFirstSets()
{

    
    findTerminals_nonTerminals();
    //create the new ruleset
    createNewRuleSet();
    //first initialize empty sets
    for (newruleSet n : definition) {
        set<string> empty_set = {};
        first_sets.insert(pair<string, set<string> >(n.left, empty_set));
      
    }
    
    bool changed = true;
    
    //make a boolean array with size set to number of non terminals
    //put changed in here
    while (changed) {
        changed =false;
        for (newruleSet rule : definition) {
            for (auto ors : rule.right) {
                //here we're considering epsilon
                //
                if (ors.size() == 0) {
                    if (first_sets[rule.left].count("#") == 0) {
                        changed = true;
                        first_sets[rule.left].insert("#");
                    }
                    
                    continue;
                    }
                //this is the same as (for string s : ors)
                for (int i = 0; i < ors.size(); i++) {
                        //cout<<" current thingy : "<<ors[i]<<endl;
                        if (isTerminal(ors[i])) {
                          //  cout<<"TERMINAL ENCOUNTERED !!!"<<endl;
                            if (first_sets[rule.left].count(ors[i]) == 0) {
                                first_sets[rule.left].insert(ors[i]);
                                changed = true;
                            }
                            break;
                        }

                    else {
                        //add everything from the terminal minus epsilon to the first set
                        set<string> old_set = first_sets[rule.left];
                        set<string> newset = first_sets[ors[i]];
                        newset.erase("#");
                        if(!isSubset(old_set, newset)){
                            changed = true;
                            set<string> tempfirst;
                            tempfirst = unify(newset, first_sets[rule.left]);
                           // set_union(newset.begin(), newset.end(), first_sets[rule.left].begin(), first_sets[rule.left].end(), inserter(tempfirst, tempfirst.begin()));
                            first_sets[rule.left] = tempfirst;
                        }

                        if (isLast(ors[i], ors, i)) {
                            if (hasEpsilon(ors[i])&&first_sets[rule.left].count("#") == 0) {
                                    first_sets[rule.left].insert("#");
                                    changed = true;
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
        
    }
    
    //print em out
   //printFirstSets();
   //cout<<"\nFINAL OUTPUT : \n";
   
   // cout << "3\n";
   
}
void CalculateFirstSets(){
    getFirstSets();
    printFirstSets(first_sets);
}


// Task 4
void printFollowSets(map<string,set<string> > currMap){
    for(string curr_nt : print_nt){
        if(currMap.find(curr_nt)!=currMap.end()){
            cout<<"FOLLOW"<<"("<<curr_nt<<") = { ";
            bool isFirst = true;
            set<string> currSet= currMap[curr_nt];
            if(currSet.find("$")!=currSet.end()){
                cout<<"$";
                isFirst = false;
            }
            for(string curr_t : print_t){
                if(currSet.find(curr_t)!=currSet.end()){
                    if(isFirst){
                        isFirst=false;
                        cout<<curr_t;
                    }
                    else{
                    cout<<", "<<curr_t;
                    }
                }
            }
            cout<<" } \n ";

        }
    }
    //cout<<endl;
}
map <string,set<string> > follow_sets;
void getFollowSets(){
    getFirstSets();
    // correct : printFirstSets("FIRST",first_sets,"#");
    for (newruleSet n : definition) {
        set<string> empty_set;
        follow_sets.insert(pair<string, set<string> >(n.left, empty_set));
    }
    //add $ to follow(S)
    follow_sets[print_nt[0]].insert("$");

    //for each non-terminal get the basic composition of the follow set (only 1 pass required)
    cout<<endl;
    for(newruleSet currSet: definition){
        //inside the ruleSet
       // cout<<"here2\n";
       // cout<<currSet.left<<" : ";
        for(int j=0;j<currSet.right.size();j++){
            //inside the right-hand sides
            if(currSet.right[j].size()>0){
            for(int k=0;k<currSet.right[j].size()-1;k++){
                //inside the particular rule
                string curr_nt =currSet.right[j][k] ;
                //cout<<curr_nt;
                
                if(inLeft(curr_nt,grammar)){
                        for(int t = k+1;t<currSet.right[j].size();t++){
                            string check_nt =currSet.right[j][t];
                            if(inLeft(currSet.right[j][t],grammar)){
                                //add first of t - epsilon in follow of k
                                bool hasEpsilon = first_sets.find(check_nt)->second.find("#")!=first_sets.find(check_nt)->second.end();
                                set<string> newset = first_sets[check_nt];
                                newset.erase("#");
                                set<string> tempfirst;
                                //set_union(newset.begin(), newset.end(), follow_sets[curr_nt].begin(), follow_sets[curr_nt].end(), inserter(tempfirst, tempfirst.begin()));
                                tempfirst = unify(newset,follow_sets[curr_nt]);
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
            //cout<<" , ";
            
        }
        //cout<<endl;
    }
    //printFollowSets(follow_sets);
    
    bool changed = true;
    //while change is true : apply rules 2 and 3 for every rule
    while(changed == true){
        changed = false;
        for(newruleSet currSet : definition){
            //for each lhs
            string left_nt = currSet.left;
            for (int j=0;j<currSet.right.size();j++){
                //for each rule
                //cout<< left_nt <<" : ";
                if(currSet.right[j].size()>0){
                for(int k = currSet.right[j].size()-1;k>=0;k--){
                    //for each _t or _nt in the rule (reverse order)
                    string curr_nt = currSet.right[j][k];
                    //cout<<curr_nt<< "(";
                    // if the component is _nt
                    if(inLeft(curr_nt,grammar)){
                    //combine the two follow sets and insert to follow(b) if follow(a) is not a subset of follow(b)
                
                    if(!isSubset(follow_sets[curr_nt],follow_sets[left_nt])){
                    set<string> newset = follow_sets[left_nt];
                    set<string> tempfollow;
                    //set_union(newset.begin(), newset.end(), follow_sets[curr_nt].begin(), follow_sets[curr_nt].end(), inserter(tempfollow, tempfollow.begin()));
                    tempfollow = unify(newset,follow_sets[curr_nt]);
                    follow_sets[curr_nt] = tempfollow;
                    changed = true;
                    }
                    //stop iterating if the first of _nt doesn't have epsilon
                    bool hasEpsilon = first_sets[curr_nt].count("#")==1;
                    //cout<<first_sets[curr_nt].count("#")<<") ";
                    if(!hasEpsilon){
                        break;
                    }
                    }
                    else{
                        //stop iterating through if the curr_nt is terminal
                        break;
                }
                    
                }
                //cout<<endl;
            }
            }
        }
       
    }
    
}
bool intersectionIsNull(set<string> set1, set<string> set2){
    for(string curr : set1 ){
        if(set2.count(curr)==1){
            return false;
        }
    }
    return true;
}

void CalculateFollowSets()

{
  //  cout<<"OUR OUTPUT"<<endl;
    getFollowSets();
    
    printFollowSets(follow_sets);
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    getFollowSets();
    getFirstSets();
    
    for(newruleSet currRule :definition){
        set<string> major_comp; //idList1
        if(currRule.right.size()>=1){
        int it = 0;
        for(int i = 0;i<currRule.right.size()-1;i++){
            vector<string> rule = currRule.right[i];
            it++;
            set<string> currStrings;
            if(rule.size()==0){
                currStrings.insert("#");
                //cout<< "current STRING : "<<"#"<<", "<< currRule.left<<endl; 
            }
            else{
            for(int j = 0;j<rule.size();j++){
                string curr = rule[j];
                //cout<< "current STRING : "<<curr<<", "<< currRule.left<<endl; 
                if(inLeft(curr,grammar)){
                     set<string> newset = first_sets[curr];
                    if(j!=rule.size()-1&&newset.count("#")==1){
                        newset.erase("#");
                    }
                    currStrings = unify(currStrings,newset);
                    if(first_sets[curr].count("#")==0){
                        break;
                    }

                }
                else{
                    currStrings.insert(curr);
                    break;
                }
            }
        }
            if(!intersectionIsNull(currStrings,major_comp)){
                //cout<<currRule.left<<endl;
                //cout<<"iteration number : "<<it<<endl;
                for(string t : currStrings){
                    //cout<< t;
                }
                //cout<<endl;
                for(string t : major_comp){
                    //cout<< t;
                }        
                //cout<<endl;        
                cout<<"NO";
               
                return;
        
            }
            else{
                major_comp = unify(major_comp,currStrings);
            }
        }
    }
    } 
        for(int i = 0; i<print_nt.size();i++){
            if(first_sets[print_nt[i]].count("#")==1){
                if(!intersectionIsNull(first_sets[print_nt[i]],follow_sets[print_nt[i]])){
                //cout<<print_nt[i]<<endl;
                cout<<"N0"<<endl;
                return;
                }
            }
        }
    
    cout<<"YES"<<endl;
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
    if(syntaxError==false){

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
    }
    return 0;

}