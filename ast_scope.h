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
#include "ast.h"
#include <map>
#include <vector>
using namespace std;


//struct info {
//    Type *type;
//};

class SymbolTable
{
    protected:
        map<Identifier*, Node *> table;
       
    public:
        SymbolTable() {}
        
        void insert(Identifier* symbol, Node* info) {
            table.insert(std::pair<Identifier*, Node*>(symbol, info));
        }

};

#endif 

