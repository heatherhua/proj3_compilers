/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ast_scope.h
 * Author: heatherhua
 *
 * Created on February 11, 2016, 8:21 PM
 */

#ifndef _H_ast_scope
#define _H_ast_scope

#include "list.h"
#include <map>
#include <vector>

using namespace std;

class SymbolTable
{
    protected:
        map<char *, Node *> table;
       
    public:
        SymbolTable() {}
        
        void insert(char* symbol, Node* info) {
            table.insert(std::pair<char*, Node*>(symbol, info));
        }
        Node* lookup(char* id){
            return table.at(id);
        }
        
        void update(char* symbol, Node* info) {
            std::map<char*, Node*>::iterator it = table.find(symbol); 
            if (it != table.end())
                it->second = info;
        }
        
        int contains(char* id){
            return table.count(id);
        }
};

#endif 

