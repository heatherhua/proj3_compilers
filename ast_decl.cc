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

Decl::Decl() : id(NULL) {
}
       
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
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
    std::cout << "VarDecl checking..." << "\n";
    bool error = false;
    
    for(int i = symbolTableVector->NumElements()-1; i >= 0; i--){ 
        char *symbol = this->getIdentifier()->getName();
        printf("symbol, %s\n", symbol);
        printf("symbolTableVector->contains() %d\n", 
            symbolTableVector->Nth(i)->contains(symbol) == 1 );
        if(symbolTableVector->Nth(i)->contains(symbol) == 1){
//            printf("ERROR decl already exists \n");
            Decl *decl = dynamic_cast<Decl*>(symbolTableVector->Nth(i)->lookup(symbol));
            
            ReportError::DeclConflict(this, decl);           
            //ReportError::DeclConflict(this, symbolTableVector->Nth(i)->lookup(symbol));
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
    printf("Checking function decl. \n");
    // Push new scope onto stack
    SymbolTable *newScope = new SymbolTable();
    symbolTableVector->Append(newScope);

    std::cout << "Number of scopes: " << symbolTableVector->NumElements() << "\n";

    // Check and add formals
    for(int i = 0; i < formals->NumElements(); i++) {
        VarDecl *curr = formals->Nth(i);
        curr->Check();
    }

    // Check function body
    // there always is a function body
    // body is always a StmtBlock
    printf("Body is %s \n", body->GetPrintNameForNode());
    //dynamic_cast<StmtBlock*>(body)->Check();
    body->Check();

//    std::cout << "Number of entries in function scope: " 
//	    << functionScope.size() << "\n";
    
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


