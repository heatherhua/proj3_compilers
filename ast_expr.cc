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


Type* ArithmeticExpr::GetType(){
  // printf("ArithmeticExpr GetType Checking...\n");
  // check if unary or binary
  // TODO: Should we check this, left, and right?
  if(left){
    if(left->GetType()->Compare(right->GetType())){
      return left->GetType();
    }
  }
  else{// if unary
    return right->GetType();
  }
  return Type::errorType;
}

Type* VarExpr::GetType(){
  // printf("VarExpr GetType Checking...\n");
  char *symbol = id->getName();
  // printf("Search the symbol...%s\n", symbol);
  
  for(int i = symbolTableVector->NumElements()-1; i >= 0; i--){ 
    SymbolTable *table = symbolTableVector->Nth(i);
    if(table->contains(symbol) == 1){
      
      VarDecl *p = dynamic_cast<VarDecl*>(table->lookup(symbol));
      DeclStmt *d = dynamic_cast<DeclStmt*>(table->lookup(symbol));
      if(d){
        return d->GetType();
      }
       else if(p){
         return p->GetType();
      }
    }
  }
  return Type::errorType;
}

void AssignExpr::Check() {
    // Expr *left, *right;
  left->Check();
  right->Check();

  //TODO Check types... error recovery.

  if(!(left->GetType()->Compare(right->GetType())) &&
    (left->GetType() != Type::errorType) &&
    (right->GetType() != Type::errorType)
    ){

  //TODO Look up left in Symbol tables and get the type of THAT
    ReportError::IncompatibleOperands(op, left->GetType(),
      right->GetType());

  }
}

void ArithmeticExpr::Check(){
  // printf("ArithmeticExpr Checking...\n");

    // Check left and right
    if(left) left->Check();
    right->Check();

    // check if unary or binary
    if(left){
      // if types dont match or values not scalar, report error
      if(!(left->GetType()->Compare(right->GetType()))){

        //TODO Look up left in Symbol tables and get the type of THAT
        ReportError::IncompatibleOperands(op, left->GetType(),
           right->GetType());

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
            right->GetType()->Compare(Type::floatType)) != true) {
        ReportError::IncompatibleOperand(op, right->GetType());
      }
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
  char *symbol = this->id->getName();
  // printf("Checking VarExpr...%s\n", symbol);
  for(int i = symbolTableVector->NumElements()-1; i >= 0; i--){ 
        // printf("symbol, %s\n", symbol);
        // printf("symbolTableVector->contains() %d\n", 
            // symbolTableVector->Nth(i)->contains(symbol) == 1 );
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

  void FieldAccess::Check(){
    // printf("Checking Field Access...%s\n\n", field->getName());
    // Check if VarExpr is vector type
    if(base){
      base->Check();
      Type * type = base->GetType();
      if((
        (type->Compare(Type::vec2Type)) ||   
        (type->Compare(Type::vec3Type)) ||   
        (type->Compare(Type::vec4Type)) ) == false
        ) {
        ReportError::InaccessibleSwizzle(field, base);
        //TODO Do something to stop cascading errors
        // Make base into a vec4 type
      }
      // Check size...no more than 4
      if(strlen(field->getName()) > 4){
        ReportError::OversizedVector(field, base);
      }
      // vec2 v2; vec2.xyxy is LEGAL --> evaluates to a vec4 
      // Check if all letters in field are subset of [xyzw]

      char* letters = field->getName();
      //Check if swizzle is valid in general
      for(int i = 0; i < strlen(letters); i++){
        if(letters[i] != 'x' &&
          letters[i] != 'y' && 
          letters[i] != 'z' && 
          letters[i] != 'w') { 
          ReportError::InvalidSwizzle(field, base);
          break;
        }
      }

      //Check for vector boundaries
      // vec2 -> cannot have z or w
      if(type->Compare(Type::vec2Type)){
        for(int i = 0; i < strlen(letters); i++){
          if(letters[i] == 'z' || letters[i] == 'w'){
            ReportError::SwizzleOutOfBound(field, base);
            break;
          }
        }
      }
      // vec3 -> cannot have w
      else if(type->Compare(Type::vec3Type)){
        for(int i = 0; i < strlen(letters); i++){
          if(letters[i] == 'w'){
            ReportError::SwizzleOutOfBound(field, base);
            break;
          }
        }
      }
    }
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
 
