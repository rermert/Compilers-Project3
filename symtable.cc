
#include "symtable.h"
#include "ast_type.h"

int SymbolTable::loopNum = 0;
int SymbolTable::switchNum = 0;
bool SymbolTable::needReturn = false;
bool SymbolTable::hasReturn = false;
Type * SymbolTable::needReturnType = NULL;

SymbolTable::SymbolTable(){
    SymbolTable::push();
}

//push in a new scope
void SymbolTable::push(){
    SymbolTable::tables.push_back(new ScopedTable());
}

void SymbolTable::pop(){
    SymbolTable::tables.pop_back();
}

void SymbolTable::insert(Symbol &sym){
    //calls insert of Scope table
    if(!SymbolTable::tables.empty())
    	SymbolTable::tables.back()->insert(sym);
}

void SymbolTable::remove(Symbol &sym){
    if (!SymbolTable::tables.empty() /*&& !SymbolTable::tables.back()->symbols.empty()*/)
        SymbolTable::tables.back()->remove(sym);
}

Symbol* SymbolTable::find(const char *name){
    Symbol *res_sym;
    for (std::vector<ScopedTable*>::reverse_iterator it = SymbolTable::tables.rbegin();
        it != SymbolTable::tables.rend(); ++it){
        res_sym = (*it)->find(name);
        if (res_sym != NULL) return res_sym;
    }
    return NULL;
}

Symbol* SymbolTable::findInCurrScope(const char *name){
    if (!SymbolTable::tables.empty()){
        return SymbolTable::tables.back()->find(name);
    }
    return NULL;
}




ScopedTable::ScopedTable(){}

void ScopedTable::insert(Symbol &sym){
    std::pair<SymbolIterator,bool> p;

    p = ScopedTable::symbols.insert(SymMap::value_type(sym.name, sym));
    if (p.second == false){
        p.first->second = sym;
    }
}

void ScopedTable::remove(Symbol &sym){
    ScopedTable::symbols.erase(sym.name);
}

Symbol* ScopedTable::find(const char *name){
    //std::map<char *,Symbol,lessStr>::iterator it;
    SymbolIterator it;
    it = ScopedTable::symbols.find(name);
    if (it != ScopedTable::symbols.end())
        return &(it->second);
    return NULL;
}




bool MyStack::insideLoop(){

}

bool MyStack::insideSwitch(){

}
