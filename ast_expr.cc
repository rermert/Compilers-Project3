/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"

Type * Expr::GetType(){
    return type;
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
    char *name = this->GetIdentifier()->GetName();
    Symbol * symres = Node::symtable->find(name);
    if (symres == NULL){
        ReportError::IdentifierNotDeclared(this->GetIdentifier(),reasonT::LookingForVariable);
        this->type = Type::errorType;
    }else{
        VarDecl * vardecl = dynamic_cast<VarDecl*>(symres->decl)
        if (vardecl){
            this->type = vardecl->GetType();
        }else{
            //the case where the declared variable is not vardecl
            this->type = Type::errorType;
        }
    }

}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

bool Operator::IsOp(const char *op) const {
    return strcmp(tokenString, op) == 0;
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

ConditionalExpr::ConditionalExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void ConditionalExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1, "(cond) ");
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}

void ArithmeticExpr::Check(){
    Type * ltype = NULL;
    Type * rtype = NULL;

    this->right->Check();
    rtype =  this->right->GetType();

    const char * And = "&&";
    const char * Or = "||";

    if (this->left){
        //if left expr * is not NULL
        this->left->Check();
        ltype = this->left->GetType();

        if (!ltype->IsConvertibleTo(rtype) && !rtype->IsConvertibleTo(ltype)){
            ReportError::IncompatibleOperands(this->op,ltype,rtype);
            this->type = Type::errorType;
        }else if (ltype->IsError() || rtype->IsError()){
            this->type = Type::errorType;
        }else if (this->op->IsOp(And) || this->op->IsOp(Or)){
            //checking whether the operator is logical
            if(!ltype->isBool() || !rtype->isBool()){
                ReportError::IncompatibleOperands(this->op,ltype,rtype);
                this->type = Type::errorType;
            }else{
                this->type = Type::BoolConstant;
            }
        }else if (!(ltype->IsNumeric() || ltype->IsVector() || ltype->IsMatrix())){
            ReportError::IncompatibleOperands(this->op,ltype,rtype);
            this->type = Type::errorType;
        }else{
            this->type = ltype;
        }
    }else{
        //if left expr * is NULL, prefix
        if (rtype->IsError()){
            this->type = Type::errorType;
        }else if (!(rtype->IsNumeric() || rtype->IsVector() || rtype->IsMatrix())){
            ReportError::IncompatibleOperand(this->op,rtype);
            this->type = Type::errorType;
        }else{
            this->type = rtype;
        }
    }
}

void RelationalExpr::Check(){
    this->left->Check();
    this->right->Check();

    Type * ltype = this->left->GetType();
    Type * rtype = this->right->GetType();

    if (!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype))){
        ReportError::IncompatibleOperands(this->op,ltype,rtype);
        this->type = Type::errorType;
    }else if (!ltype->IsNumeric() || !rtype->IsNumeric()){
        ReportError::IncompatibleOperands(this->op,ltype,rtype);
        this->type = Type::errorType;
    }else if (ltype->IsError() || rtype->IsError()){
        this->type = Type::errorType;
    }else{
        this->type = Type::BoolConstant;
    }
}

void EqualityExpr::Check(){
    this->left->Check();
    this->right->Check();

    Type * ltype = this->left->GetType();
    Type * rtype = this->right->GetType();

    if (!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype))){
        ReportError::IncompatibleOperands(this->op,ltype,rtype);
        this->type = Type::errorType;
    }else if (ltype->IsError() || rtype->IsError()){
        this->type = Type::errorType;
    }else{
        this->type = Type::BoolConstant;
    }
}

/*void LogicalExpr::Check(){
    //add logical expr check in arithmetic check
}
*/

ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this);
    (subscript=s)->SetParent(this);
}

void ArrayAccess::PrintChildren(int indentLevel) {
    base->Print(indentLevel+1);
    subscript->Print(indentLevel+1, "(subscript) ");
}

void ArrayAccess::Check(){
    this->base->Check();
    Type * baseType = this->base->GetType();
    ArrayType * arrayType = dynamic_cast<ArrayType *>(baseType);
    if(arrayType == NULL){
        VarExpr * varExpr = dynamic_cast<VarExpr *>(this->base);
        if(VarExpr)
            ReportError::NotAnArray(varExpr->GetIdentifier());
        this->type = Type::errorType;
    }else{
        this->type = arrayType->GetElemType();
    }
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
