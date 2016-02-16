/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "utility.h"
#include "ast_expr.h"
#include "errors.h"
#include "ast.h"
#include <stdio.h>
 #include "ast_type.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    // printf("\n");
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */

    SymbolTable *global = new SymbolTable();
    symbolTableVector->Append(global);
    
    for(int i = 0; i < decls->NumElements(); i++){
        decls->Nth(i)->Check();
    }
    
    // printf("Size of vector: %d\n\n", symbolTableVector->NumElements());
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

void StmtBlock::Check() {
    // printf("Checking stmtblock. \n");
    // printf(" ........... \n\n");
    // Remember parser actually only appends to stmt list. 
    // VarDecl is reported as DeclStmt
    for(int i = 0; i < stmts->NumElements(); i++){
        // printf("stmt %d: %s\n", i, stmts->Nth(i)->GetPrintNameForNode());
        if(strcmp(stmts->Nth(i)->GetPrintNameForNode(), "StmtBlock") == 0){
            SymbolTable *newScope = new SymbolTable();
            symbolTableVector->Append(newScope);
            // std::cout << "Making new scope in stmtblock " << symbolTableVector->NumElements() << "\n";
            stmts->Nth(i)->Check();
            symbolTableVector->RemoveAt(symbolTableVector->NumElements()-1);
            // std::cout << "Number of scopes: " << symbolTableVector->NumElements() << "\n";
        } else {
            stmts->Nth(i)->Check();
        }
    }
    // printf(" ........... \n\n");
}

DeclStmt::DeclStmt(Decl *d) {
    Assert(d != NULL);
    (decl=d)->SetParent(this);
}

void DeclStmt::Check() {
    //just checking for errors in this scope
    bool error = false;
    char *symbol = decl->getIdentifier()->getName();
    // printf("DeclStmt checking...%s\n", symbol);
    // printf("symbol, %s\n", symbol);
    Decl *o;

    if(symbolTableVector->Last()->contains(symbol) == 1){
        if(strcmp(symbolTableVector->Last()->lookup(symbol)->GetPrintNameForNode(), "DeclStmt") == 0){
        
            DeclStmt *p = dynamic_cast<DeclStmt*>(symbolTableVector->Last()->lookup(symbol));
            o = p->decl;   
        }
        else{
            o = dynamic_cast<Decl*>(symbolTableVector->Last()->lookup(symbol));
        }
        ReportError::DeclConflict(this->decl, o);           
        symbolTableVector->Last()->update(symbol, this);
        error = true;
    }
    
    // No error
    if(!error){
        symbolTableVector->Last()->insert(decl->getIdentifier()->getName(), this);
    }
}

void DeclStmt::PrintChildren(int indentLevel) {
    decl->Print(indentLevel+1);
}

Type* DeclStmt::GetType(){
    if(decl){
        return decl->GetType();
    }
    return Type::errorType;
}


Type* ReturnStmt::GetType(){
    if(expr){
        return expr->GetType();
    }
    return Type::voidType;
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

void ConditionalStmt::Check(){
    // printf("Checking ConditionalStmt... \n");
    bool found = false;

    if((strcmp(test->GetPrintNameForNode(),"ArithmeticExpr")) == 0 ||
        (strcmp(test->GetPrintNameForNode(),"RelationalExpr")) == 0 ||
        (strcmp(test->GetPrintNameForNode(),"AssignExpr")) == 0 ||
        (strcmp(test->GetPrintNameForNode(),"BoolConstant")) == 0){
        found = true;
    }
    if(found == false){
        ReportError::TestNotBoolean(test);
    }
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && b != NULL);
    (init=i)->SetParent(this);
    step = s;
    if ( s )
      (step=s)->SetParent(this);
}

void ForStmt::Check(){
    // printf("Checking Forstmt... \n");
    //make new symboltable
    SymbolTable *newScope = new SymbolTable();
    symbolTableVector->Append(newScope);
    
    // verify and add in parameters
    // vars from ForStmt
    init->Check();
    step->Check();

    // verify and add in body
    // vars from ConditionalStmt

    ConditionalStmt::Check();

    body->Check();

    // pop
    // std::cout << "\n\nNumber of scopes BEFORE POP: " << symbolTableVector->NumElements() << "\n";
    symbolTableVector->RemoveAt(symbolTableVector->NumElements()-1);
    // std::cout << "Number of scopes AFTER POP: " << symbolTableVector->NumElements() << "\n";
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    if ( step )
      step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}


void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::Check(){
        // printf("Checking WhileStmt... \n");
        //make new symboltable
        SymbolTable *newScope = new SymbolTable();
        symbolTableVector->Append(newScope);

        // vars from ConditionalStmt
        test->Check();
        ConditionalStmt::Check();

        // pop
        symbolTableVector->RemoveAt(symbolTableVector->NumElements()-1);
        // std::cout << "Number of scopes: " << symbolTableVector->NumElements() << "\n";
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}


void IfStmt::Check(){
    //Stmt *elseBody;
    //Expr *test;
    //Stmt *body;

        // printf("Checking Ifstmt... \n");

        ConditionalStmt::Check();
        body->Check();

        if(elseBody) elseBody->Check();
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    expr = e;
    if (e != NULL) expr->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    if ( expr ) 
      expr->Print(indentLevel+1);
}

void ReturnStmt::Check(){
    // check if valid expr
    if(expr){
        expr->Check();
    }
}
  
SwitchLabel::SwitchLabel(Expr *l, Stmt *s) {
    Assert(l != NULL && s != NULL);
    (label=l)->SetParent(this);
    (stmt=s)->SetParent(this);
}

SwitchLabel::SwitchLabel(Stmt *s) {
    Assert(s != NULL);
    label = NULL;
    (stmt=s)->SetParent(this);
}

void SwitchLabel::PrintChildren(int indentLevel) {
    if (label) label->Print(indentLevel+1);
    if (stmt)  stmt->Print(indentLevel+1);
}
void SwitchLabel::Check(){
    //     Expr     *label;
    // Stmt     *stmt;
    
    // printf("Checking SwitchLabel... \n");

    // check label
    // printf("Value of SwitchLabel:label %s\n", label->GetPrintNameForNode());
    label->Check();

    // Push new scope onto stack
    SymbolTable *newScope = new SymbolTable();
    symbolTableVector->Append(newScope);

    // std::cout << "Number of scopes: " << symbolTableVector->NumElements() << "\n";

    // Check and verify statement body
    // printf("Value of SwitchLabel:stmt %s\n", stmt->GetPrintNameForNode());
    stmt->Check();

    // pop 
    symbolTableVector->RemoveAt(symbolTableVector->NumElements()-1);

}

void BreakStmt::Check(){
    // printf("Checking BreakStmt... \n");

    bool found = false;
    Node * it = this;
    while(it->GetParent() != NULL){
        if((strcmp(it->GetParent()->GetPrintNameForNode(), "ForStmt" )) == 0 ||
            (strcmp(it->GetParent()->GetPrintNameForNode(), "WhileStmt" )) == 0 ){
            found = true;
            symbolTableVector->RemoveAt(symbolTableVector->NumElements()-1);
            break;
        }
        else{
            it = it->GetParent();
        }
    }

    if(found == false){
        ReportError::BreakOutsideLoop(this);
    }
}

void ContinueStmt::Check(){
    // printf("Checking ContStmt. \n");

    bool found = false;
    Node * it = this;
    while(it->GetParent() != NULL){
        if((strcmp(it->GetParent()->GetPrintNameForNode(), "ForStmt" )) == 0 ||
            (strcmp(it->GetParent()->GetPrintNameForNode(), "WhileStmt" )) == 0 ){
            found = true;
            break;
        }
        else{
            it = it->GetParent();
        }
    }

    if(found == false){
        ReportError::ContinueOutsideLoop(this);
    }
}

void SwitchStmt::Check(){
    // TODO: FIND OUT SCOPING!!! ***********************/
       // printf("Checking SwitchStmt. \n");
        // verify the expr
        // printf("Value of SwitchStmt:Expr: %s\n", expr->GetPrintNameForNode());
        
        // check if expr is correct type
        // printing type
        // printf("Getting Type expr in switch: %s\n\n", expr->GetType()->GetPrintNameForNode());
        expr->Check();

        // verify all stmts in List<Stmt>
        for(int i = 0; i < cases->NumElements(); i++){
           printf("Value of SwitchStmt:case %d: %s\n", i, cases->Nth(i)->GetPrintNameForNode());
           cases->Nth(i)->Check();
        }
        // verify default
        //if(def) printf("Value of SwitchSTmt:def: %s\n", def->GetPrintNameForNode());
       if(def) def->Check();
}

SwitchStmt::SwitchStmt(Expr *e, List<Stmt *> *c, Default *d) {
    Assert(e != NULL && c != NULL && c->NumElements() != 0 );
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
    def = d;
    if (def) def->SetParent(this);
}

void SwitchStmt::PrintChildren(int indentLevel) {
    if (expr) expr->Print(indentLevel+1);
    if (cases) cases->PrintAll(indentLevel+1);
    if (def) def->Print(indentLevel+1);
}

