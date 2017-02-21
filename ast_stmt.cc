/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "errors.h"
#include "symtable.h"

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */

    // sample test - not the actual working code
    // replace it with your own implementation
    if ( decls->NumElements() > 0 ) {
      for ( int i = 0; i < decls->NumElements(); ++i ) {
        Decl *d = decls->Nth(i);
        /* !!! YOUR CODE HERE !!!
         * Basically you have to make sure that each declaration is
         * semantically correct.
         */
         d->Check();
      }
    }
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

void StmtBlock::Check(){
    //Statement block does not need to push a new scope, the statement
    //before it (if, funcdecl, etc) should do the job
    if(decls){
        //if decls is not NULL pointer, check every decl
        int size = decls->NumElements();
        for(int i = 0; i < size; i++){
            VarDecl * element = decls->Nth(i);
            element->Check();
        }
    }
    if(stmts){
        //if statement list is not NULL pointer, check every stmt
        int size = stmts->NumElements();
        for(int i = 0; i < size; i++){
            Stmt * element = stmts->Nth(i);
            element->Check();
        }
    }
}

DeclStmt::DeclStmt(Decl *d) {
    Assert(d != NULL);
    (decl=d)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    decl->Print(indentLevel+1);
}

void DeclStmt::Check(){
    //not sure if we need to dynamic cast decl to vardecl & fndecl
    if(decl)
        decl->Check();
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) {
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this);
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) {
    Assert(i != NULL && t != NULL && b != NULL);
    (init=i)->SetParent(this);
    step = s;
    if ( s )
      (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    if ( step )
      step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void ForStmt::Check(){
    //p3exe will let it pass as long as init, body are valid expr,
    //step has to be boolean
    if(init){
        init->Check();
    }
    if(step){
        step->Check();
    }
    //test has to be a valid pointer enforced by parser
    if(test){
        test->Check();
        Type * testType = test->GetType();
        if(!testType->IsEquivalentTo(Type::boolType)){
            ReportError::TestNotBoolean(test);
        }
    }
    //should push a new scope for the following statement body block
    if(body){
        Node::symtable->push();
        Node::loopNum++;
        body->Check();
        Node::loopNum--;
        Node::symtable->pop();
    }
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

boid WhileStmt::Check(){
    if(test){
        test->Check();
        Type * testType = test->GetType();
        if(!testType->IsEquivalentTo(Type::boolType)){
            ReportError::TestNotBoolean(test);
        }
    }
    //should push a new scope for the following statement body block
    if(body){
        Node::symtable->push();
        Node::loopNum++;
        body->Check();
        Node::loopNum--;
        Node::symtable->pop();
    }
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
    if(test){
        test->Check();
        Type * testType = test->GetType();
        if(!testType->IsEquivalentTo(Type::boolType)){
            ReportError::TestNotBoolean(test);
        }
    }
    //should push a new scope for the following statement body block
    if(body){
        Node::symtable->push();
        body->Check();
        Node::symtable->pop();
    }

    //push another scope for else body
    if(elseBody){
        Node::symtable->push();
        elseBody->Check();
        Node::symtable->pop();
    }
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
    //set hasReturn to true if actually return something
    //p3exe will not report missing return as long as there is a return
    Node::symtable->hasReturn = true;

    //case 1: return nothing but function requres something
    if(expr == NULL && Node::symtable->needReturn == true)
        rReportError::ReturnMismatch(this, Type::voidType, Node::symtalbe->needReturnType);
    
    expr->Check();
    Type * returnType = expr->GetType();
    
    //case 2: return something but function requires nothing
    if(Node::symtable->needReturn == false && !returnType->IsEquivalentTo(Type::voidType)){
        ReportError::ReturnMismatch(this, returnType, Type::voidType);
    }else{
        //case3: return something function requires something else
        if(!returnType->IsEquivalentTo(Node::symtable->needReturnType)){
            ReportError::ReturnMismatch(this, returnType, Node::symtalbe->needReturnType);
        }
    }
}

void BreakStmt::Check(){
    if(Node::symtable->loopNum == 0 && Node::symtable->switchNum == 0)
        ReportError::BreakOutsideLoop(this);
}

void ContinueStmt::Check(){
    if(Node::symtable->loopNum == 0)
        ReportError::ContinueOutsideLoop(this);
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
    //SwitchLabel constructor is never called in parser
}

void SwitchStmt::Check(){
    if(expr){
        expr->Check();
    }
    Node::symtable->push();
    Node::symtable->switchNum++;
    if(def){
        def->Check();
    }
    if(cases){
        int size = cases->NumElements();
        for(int i = 0; i < size; i++){
            Stmt * element = cases->Nth(i);
            element->Check();
        }
    }
    Node::symtable->switchNum--;
    Node::symtable->pop();
}

void Case::Check(){
    if(label)
        label->Check();
    if(stmt)
        stmt->Check();
}

void Default::Check(){
    if(label)
        label->Check();
    if(stmt)
        stmt->Check();
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
