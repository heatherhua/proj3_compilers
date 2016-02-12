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
#include <string>
#include <string.h>

typedef std::map<string, Node*> Map;

class SymbolTable
{
    protected:
        
        Map table;
       
    public:
        SymbolTable() {}
        
        void insert(char* symbol, Node* info) {
            std::string sym(symbol);
            table.insert(std::pair<string, Node*>(symbol, info));
        }
        
        Node* lookup(char* id){
            std::string s(id);
            return table.at(s);
        }
        
        void update(char* symbol, Node* info) {
            std::string s(symbol);
            Map::iterator it = table.find(s); 
            if (it != table.end())
                it->second = info;
        }
        
        int contains(char* id){
            std::string s(id);
            return table.count(s);
        }
};

#endif 

