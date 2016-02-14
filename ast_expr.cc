/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
 #include <string>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
 #include "errors.h"
 #include "ast.h"

void AssignExpr::Check() {
    printf("AssignExpr Checking...\n");
//    //Get last added scope in vector...and add decl to that one.
//    map<Node *, Node *> scope = vector->back();
//    scope[decl->getIdentifier()] = decl;
}

Type* ArithmeticExpr::GetType(){
  // check if unary or binary
  // TODO: Should we check this, left, and right?
  if(left){
    if(left->GetType()->Compare(right->GetType())){
      return left->GetType();
    }
  }
  else{ // if unary
    return right->GetType();
  }
}

void ArithmeticExpr::Check(){

    // Check left and right
    left->Check();
    right->Check();

    // check if unary or binary
    if(left){
      // if types dont match or values not scalar, report error
      if(!(left->GetType()->Compare(right->GetType()))){
        ReportError::IncompatibleOperands(op, left->GetType(),
           right->GetType());
        // update symbol table for cascading errors
        // if((left->GetType()->Compare(Type::floatType) && 
        //     (right->GetType()->Compare(Type::intType))) ||
        //     (right->GetType()->Compare(Type::floatType) && 
        //     (left->GetType()->Compare(Type::intType)))){
        //     
        // }
      }
      else{ // if not scalar
        if((left->GetType()->Compare(Type::intType) || 
            left->GetType()->Compare(Type::floatType)) != true){
          ReportError::IncompatibleOperands(op,left->GetType(),
              right->GetType());       
        }
      }
    }
    else{
      // check if unary is scalar
      if((right->GetType()->Compare(Type::intType) || 
            right->GetType()->Compare(Type::floatType)) != true)
        ReportError::IncompatibleOperand(op, right->GetType());
    }

}

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::PrintChildren(int indentLevel) { 
    printf("%d", value);
}

FloatConstant::FloatConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void FloatConstant::PrintChildren(int indentLevel) { 
    printf("%g", value);
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::PrintChildren(int indentLevel) { 
    printf("%s", value ? "true" : "false");
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    Assert(ident != NULL);
    this->id = ident;
}

void VarExpr::PrintChildren(int indentLevel) {
    id->Print(indentLevel+1);
}

void VarExpr::Check(){
  // make sure identifier exists
  bool found = false;
    
    for(int i = symbolTableVector->NumElements()-1; i >= 0; i--){ 
        char *symbol = this->id->getName();
        printf("symbol, %s\n", symbol);
        printf("symbolTableVector->contains() %d\n", 
            symbolTableVector->Nth(i)->contains(symbol) == 1 );
         if(symbolTableVector->Nth(i)->contains(symbol) == 1){

          found = true;
          break;
         } 
    }

    if(!found){
      ReportError::IdentifierNotDeclared(this->id, LookingForVariable);
    }
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o) 
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}

void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}
   
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

void ArrayAccess::PrintChildren(int indentLevel) {
    base->Print(indentLevel+1);
    subscript->Print(indentLevel+1, "(subscript) ");
  }
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}


  void FieldAccess::PrintChildren(int indentLevel) {
    if (base) base->Print(indentLevel+1);
    field->Print(indentLevel+1);
  }

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

 void Call::PrintChildren(int indentLevel) {
    if (base) base->Print(indentLevel+1);
    if (field) field->Print(indentLevel+1);
    if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
  }
 
