SymbolTable::SymbolTable(){}

//push in a new scope
void SymbolTable::push(){
    SymbolTable::tables.push_back(new ScopedTable());
}

void SymbolTable::pop(){
    SymbolTable::tables.pop_back();
}

void SymbolTable::insert(Symbol &sym){
    //calls insert of Scope table
    SymbolTable::tables.back().insert(sym);
}

void SymbolTable::remove(Symbol &sym){
    if (!SymbolTable::tables.empty())
        SymbolTable::tables.back().remove(sym);
}

Symbol* SymbolTable::find(const char *name){
    Symbol *res_sym;
    for (std::vector<ScopedTable*>::iterator it = SymbolTable::tables.rbegin();
        it != SymbolTable::tables.rend(); ++it){
        res_sym = (*it).find(name);
        if (res_sym != NULL) return res_sym;
    }
    return NULL;
}





ScopedTable::ScopedTable(){}

void ScopedTable::insert(Symbol &sym){
    ScopedTable::symbols.insert(std::pair<char *,Symbol,lessStr>(sym.name,sym));
}

void ScopedTable::remove(Symbol &sym){
    ScopedTable::symbols.erase(sym.name);
}

Symbol* ScopedTable::find(const char *name){
    std::map<char *,Symbol,lessStr>::iterator it;
    it = ScopedTable::symbols.find(name);
    if (it != ScopedTable::symbols.end())
        return &(it->second);
    else:
        return NULL;
}
