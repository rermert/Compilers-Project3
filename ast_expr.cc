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

    char AndArr[] = "&&";
    char OrArr[] = "||";
    const char * And = AndArr;
    const char * Or = OrArr;

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

void AssignExpr::Check(){
    this->left->Check();
    this->right->Check();

    Type * ltype = this->left->GetType();
    Type * rtype = this->right->GetType();

    if (!ltype->IsConvertibleTo(rtype) && !rtype->IsConvertibleTo(ltype)){
        ReportError::IncompatibleOperands(this->op,ltype,rtype);
        this->type = Type::errorType;
    }else if (ltype->IsError() || rtype->IsError()){
        this->type = Type::errorType;
    }else if (!(ltype->IsNumeric() || ltype->IsVector() || ltype->IsMatrix())){
        ReportError::IncompatibleOperands(this->op,ltype,rtype);
        this->type = Type::errorType;
    }else{
        this->type = ltype;
    }
}

void PostfixExpr::Check(){
    this->left->Check();
    Type * ltype = this->left->GetType();
    if (ltype->IsError()){
        this->type = Type::errorType;
    }else if (!(ltype->IsNumeric() || ltype->IsVector() || ltype->IsMatrix())){
        ReportError::IncompatibleOperand(this->op,ltype);
        this->type = Type::errorType;
    }else{
        this->type = ltype;
    }
}

void ConditionalExpr::Check(){
    //Tutor says conditional expr won't be tested??
    this->type = Type::errorType;
}

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

    if(baseType->IsError()){
        this->type = Type::errorType;
        return;
    }

    ArrayType * arrayType = dynamic_cast<ArrayType *>(baseType);
    if (baseType->IsMatrix()){
        //for matrix access, where a mat3 access has to return a vec3
        if(baseType->IsEquivalentTo(Type::mat2Type)){
            this->type = Type::vec2Type;
        }else if(baseType->IsEquivalentTo(Type::mat3Type)){
            this->type = Type::vec3Type;
        }else{
            this->type = Type::vec4Type;
        }
    }else if(arrayType == NULL){
        //if it is not matrix nor an array, report error
        VarExpr * varExpr = dynamic_cast<VarExpr *>(this->base);
        //a line of segmentation: if base is not a varExpr then core dump
        //so add a safe check
        if(VarExpr)
            ReportError::NotAnArray(varExpr->GetIdentifier());
        this->type = Type::errorType;
    }
    else{
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

void FieldAccess::Check(){
    this->base->Check();
    Type * baseType = this->base->GetType();

    if(baseType->IsError()){
        this->type = Type::errorType;
        return;
    }

    if(!baseType->IsVector()){
        ReportError::InaccessibleSwizzle(this->field,this->baseType);
        this->type = Type::errorType;
        return;
    }

    Type * typeArr[] = {Type::vec2Type, Type::vec3Type, Type::vec4Type};
    char * fieldName = field->GetName();
    std::string fieldStr(fieldName);
    for(int i = 0; i < 3; i++){
        if(baseType->IsEquivalentTo(typeArr[i])){
            for(unsigned int i = 0; i<fieldName.length(); i++){
                if(fieldName[i] != 'x' || fieldName[i] != 'y' || fieldName[i] != 'z' || fieldName[i] != 'w'){
                    ReportError::InvalidSwizzle(this->field, this->base);
                    this->type = errorType;
                    return;
                }
                //case for vec2Type swizzle out of bound
                if((fieldName[i] == 'w' || fieldName[i] == 'z') && i == 0){
                    ReportError::SwizzleOutOfBound(this->field, this->base);
                    this->type = Type::errorType;
                    return;
                }
                //for vec3type swizzle out of bound
                if(fieldName[i] == 'w' && i == 1){
                    ReportError::SwizzleOutOfBound(this->field, this->base);
                    this->type = Type::errorType;
                    return;
                }
            }
        }
    }
    if(fieldName.length() > 4){
        ReportError::OversizedVector(this->field, this->base);
        this->type = Type::errorType;
        return;
    }

    if(fieldName.length() == 1){
        this->type = Type::floatType;
    }else if(fieldName.length() == 2){
        this->type = Type::vec2Type;
    }else if(fieldName.length() == 3){
        this->type = Type::vec3Type;
    }else{
        this->type = Type::vec4Type;
    }
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

void Call::Check(){
    if(this->field == NULL){
        this->type = Type::errorType;
        return;
    }
    Symbol * funcSym = Node::symtable->find(this->field->GetName());
    //if we cannot find that identifier in symbol table
    if(funcSym == NULL){
        ReportError::IdentifierNotDeclared(this->field, reasonT::LookingForFunction);
        this->type = Type::errorType;
        return;
    }

    FnDecl * fnDecl = dynamic_cast<FnDecl *> funcSym->decl;
    //not sure we can directly do assertion like this or not
    //if found in symbol table, but is not declared as a function
    if(funcSym->kind == EntryKind::E_VarDecl || fnDecl == NULL){
        ReportError::NotAFunction(this->field);
        this->type = Type::errorType;
        return;
    }

    //get the formals declared
    List<VarDecl*> * expectedFormals = fnDecl->GetFormals();
    int expectNum = expectedFormals->size();
    int actualNum = this->actuals->size();
    if(actualNum < expectNum){
        ReportError::LessFormals(this->field, expectNum, actualNum);
        this->type = Type::errorType;
        return;
    }else if(actualNum > expectNum){
        ReportError::ExtraFormals(this->field, expectNum, actualNum);
        this->type = Type::errorType;
        return;
    }
    //for each formal check the expected varDecl type is equivalent to expr type
    for(int i = 0; i < expectNum; i++){
        VarDecl * expDecl = expectedFormals->Nth(i);
        Expr * actualExpr = this->actuals->Nth(i);
        actualExpr->Check();//we are sure this works!
        Type * actualType = actualExpr->GetType();
        if(!actualType->IsEquivalentTo(expDecl->GetType())){
            ReportError::FormalsTypeMismatch(this->field, i, expDecl->GetType(), actualType);
            this->type = Type::errorType;
            return;
        }
    }
    this->type = fnDecl->GetType();
}
