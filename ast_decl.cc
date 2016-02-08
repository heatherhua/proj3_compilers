/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "utility.h"
#include <vector> 
#include <map> 

Decl::Decl() : id(NULL) {
}

        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

Identifier* Decl::getIdentifier() {
    return id;
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
  
void VarDecl::Check(vector< map<Node *, Node *> > * vector){
    //TODO Do not need to create scope. 
    // Needs to check vector scopes for decl conflicts
    
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

void FnDecl::Check(vector< map<Node *, Node *> > * vector){
    printf("Checking function decl. \n");
    std::map<Node *, Node *> functionScope;
    vector->push_back(functionScope);
    
    std::cout << "Number of scopes: " << vector->size() << "\n";
    
    //Add formal parameters to scope
    for(int i = 0; i < formals->NumElements(); i++) {
	VarDecl *curr = formals->Nth(i);
	functionScope[curr->getIdentifier()] = curr;
    }
    
    std::cout << "Number of entries in function scope: " 
	    << functionScope.size() << "\n";
    
    //Check Body
    if(body){	
	body->Check(vector);
	printf("Body is %s \n", body->GetPrintNameForNode());
    }
    
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


