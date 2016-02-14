/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "errors.h"
#include "utility.h"
#include <string.h>
#include <vector> 
#include <map> 
#include <stdio.h>

Decl::Decl() : id(NULL) {
}
       
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

Type* Decl::GetType(){
    //printf("in decl GetType");
    Type * type;
    if(strcmp(this->GetPrintNameForNode(), "VarDecl") == 0){
        type = dynamic_cast<VarDecl*>(this)->GetType();
    } else if (strcmp(this->GetPrintNameForNode(), "FnDecl") == 0) {
        type = dynamic_cast<FnDecl*>(this)->GetType();
    }
    return type;
}

/*void Decl::Check(){    
    std::cout << "Decl checking..." << "\n";
    
    if(strcmp(this->GetPrintNameForNode(), "VarDecl") == 0){
        dynamic_cast<VarDecl*>(this)->Check();
    } else if (strcmp(this->GetPrintNameForNode(), "FnDecl") == 0) {
        dynamic_cast<FnDecl*>(this)->Check();
    }
}*/

Identifier* Decl::getIdentifier() {
    return id;
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}

  
void VarDecl::Check(){
    bool error = false;
    
    for(int i = symbolTableVector->NumElements()-1; i >= 0; i--){ 
        char *symbol = this->getIdentifier()->getName();
        // std::cout << "VarDecl checking..." << symbol << "\n";
         // printf("Symbol, %s\n", symbol);
        if(symbolTableVector->Nth(i)->contains(symbol) == 1){
            Decl *decl = dynamic_cast<Decl*>(symbolTableVector->Nth(i)->lookup(symbol));
            
            ReportError::DeclConflict(this, decl);           
            symbolTableVector->Nth(i)->update(symbol, this);
            error = true;
            break;
        } 
    }
    
    // No error
    symbolTableVector->Last()->insert(this->getIdentifier()->getName(), this);
}

void VarDecl::PrintChildren(int indentLevel) { 
   if (type) type->Print(indentLevel+1);
   if (id) id->Print(indentLevel+1);
}


FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::Check(){
    // printf("Checking function decl... %s\n", this->getIdentifier()->getName());

    if(symbolTableVector->Last()->contains(this->getIdentifier()->getName())){
        Decl *old = dynamic_cast<Decl*>(symbolTableVector->Last()->lookup(this->getIdentifier()->getName()));
        ReportError::DeclConflict(this, old);
    }
    symbolTableVector->Last()->insert(this->getIdentifier()->getName(), this);

    // Push new scope onto stack
    SymbolTable *newScope = new SymbolTable();
    symbolTableVector->Append(newScope);

    // std::cout << "Number of scopes: " << symbolTableVector->NumElements() << "\n";

    // Check and add formals
    for(int i = 0; i < formals->NumElements(); i++) {
        char *symbol = formals->Nth(i)->getIdentifier()->getName();
        // printf("symbol, %s\n", symbol);

        if(symbolTableVector->Last()->contains(symbol) == 1){
            Decl *decl = dynamic_cast<Decl*>(symbolTableVector->Nth(i)->lookup(symbol));
            
            ReportError::DeclConflict(this, decl);
            // TODO: currently not handling cascading errors, do something about this
        }
        else {
            symbolTableVector->Last()->insert(formals->Nth(i)->getIdentifier()->getName(),
                 formals->Nth(i));
        }
    }

    // checking if has return types 
    body->Check();

    StmtBlock *temp = dynamic_cast<StmtBlock*>(body);

    // Is there a return?
    bool found = false;
    for(int i = 0; i < temp->stmts->NumElements(); i++){
        if(strcmp(temp->stmts->Nth(i)->GetPrintNameForNode(), "ReturnStmt") == 0){
            // Found a return
            found = true;

            Stmt* stmt = temp->stmts->Nth(i);

            // Make sure we are returning correct type            
            if(returnType == Type::voidType && 
                dynamic_cast<ReturnStmt*>(stmt)->GetType() != Type::voidType){
                ReportError::ReturnMismatch(dynamic_cast<ReturnStmt*>(stmt), 
                    dynamic_cast<ReturnStmt*>(stmt)->GetType(), Type::voidType);

            }
        }
    }
    if(returnType != Type::voidType && found == false){
        ReportError::ReturnMissing(this);
    }

    //std::cout << "FnDecl: return after body check " << symbolTableVector->NumElements() << "\n";
    symbolTableVector->RemoveAt(symbolTableVector->NumElements()-1);
    //std::cout << "Number of scopes: " << symbolTableVector->NumElements() << "\n";
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}


