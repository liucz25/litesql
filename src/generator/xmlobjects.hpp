#ifndef _litesql_xmlobjects_hpp
#define _litesql_xmlobjects_hpp
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include "litesql-gen.hpp"
#include "litesql/split.hpp"
#include "litesql/string.hpp"
extern int yylineno;
namespace xml {
using namespace std;
using namespace litesql;
string capitalize(const string& s);
string decapitalize(const string& s);
string safe(const char *s);

class Value {
public:
    string name, value;
    Value(string n, string v) : name(n), value(v) {}
};
class IndexField {
public:
    string name;
    IndexField(string n) : name(n) {}
};
class Index {
public:
    vector<IndexField> fields;
    AT_index_unique unique;
    Index(AT_index_unique u) : unique(u) {}

    bool isUnique() const {
        return unique == A_index_unique_true;
    }
};
class Field {
public:
    string name;
    string fieldTypeName;
    AT_field_type type;
    string default_;
    AT_field_indexed indexed;
    AT_field_unique unique;
    vector<Value> values;
    Field(string n, AT_field_type t, string d, AT_field_indexed i, AT_field_unique u) 
        : name(n), fieldTypeName(capitalize(n)), type(t), default_(d), indexed(i), unique(u) {
    }
    void value(const Value& v) {
        values.push_back(v);
    }
    bool isIndexed() const {
        return indexed == A_field_indexed_true;
    }
    bool isUnique() const {
        return unique == A_field_unique_true;
    }
    string getQuotedDefaultValue() const {
        if (getCPPType()=="std::string")
            return "\"" + default_ + "\"";
        if (default_.size() == 0)
            return "0";
        return default_;
    }
    string getSQLType() const {
       switch(type) {
           case A_field_type_integer: return "INTEGER";
           case A_field_type_string: return "TEXT";
           case A_field_type_float: return "FLOAT";
           case A_field_type_boolean: return "INTEGER";
           case A_field_type_date: return "INTEGER";
           case A_field_type_time: return "INTEGER";
           case A_field_type_datetime: return "INTEGER";
           default: return "";
       }
    }
    string getCPPType() const {
       switch(type) {
           case A_field_type_integer: return "int";
           case A_field_type_string: return "std::string";
           case A_field_type_float: return "float";
           case A_field_type_boolean: return "bool";
           case A_field_type_date: return "litesql::Date";
           case A_field_type_time: return "litesql::Time";
           case A_field_type_datetime: return "litesql::DateTime";
           default: return "";
       }
    }
};
class Param {
public:
    string name;
    AT_param_type type;
    Param(string n, AT_param_type t) : name(n), type(t) {}
    
};
class Method {
public:
    string name, returnType;
    vector<Param> params;
    Method(string n, string rt) 
        : name(n), returnType(rt) {}
    void param(const Param& p) {
        params.push_back(p);
    }
};
class Relation;
class Relate;
class Object;
class RelationHandle {
public:
    string name;
    Relation * relation;
    Relate * relate;
    Object * object;
    vector< pair<Object*,Relate*> > destObjects;

    RelationHandle(string n, Relation * r, Relate * rel, Object * o) 
        : name(n), relation(r), relate(rel), object(o) {}
};
class Relate {
public:    
    string objectName;
    string fieldTypeName;
    string getMethodName;
    int paramPos;
    AT_relate_limit limit;
    AT_relate_unique unique;
    string handle;
    Relate(string on, AT_relate_limit l, AT_relate_unique u, string h) 
        : objectName(on), limit(l), unique(u), handle(h) {
        if (hasLimit() && isUnique())
            throw logic_error("both limit and unique specified in relate: line " + 
                              toString(yylineno));
    }
    bool hasLimit() const {
        return limit == A_relate_limit_one;
    }
    bool isUnique() const {
        return unique == A_relate_unique_true;
    }
    bool operator < (const Relate& r) const {
        return objectName < r.objectName;
    }

};
class Relation {
public:
    string id, name;
    AT_relation_unidir unidir;
    vector<Relate> related;
    vector<Field> fields;
    vector<Index> indexes;
    Relation(string i, string n, AT_relation_unidir ud) 
        : id(i), name(n), unidir(ud) {}
    string getName() const {
        if (name.size() == 0) {
            string result;
            for (size_t i = 0; i < related.size(); i++) 
                result += related[i].objectName;
            return result + "Relation" + id;
        }
        return name;
    }
    bool isUnidir() const {
        return unidir == A_relation_unidir_true;
    }
    int sameTypes() const {
        map<string, int> names;
        int max = 0;
        for (size_t i = 0; i < related.size(); i++) {
            if (names.find(related[i].objectName) == names.end()) 
                names[related[i].objectName] = 0;
            int value =	++names[related[i].objectName];
            if (value > max)
                max = value;
        }
        return max;
    }
    int countTypes(string name) const {
        int res = 0;
        for (size_t i = 0; i < related.size(); i++)
            if (related[i].objectName == name)
                res++;
        return res;
    }
    string getTable() const {
        Split res;
        for (size_t i = 0; i < related.size(); i++)
            res.push_back(related[i].objectName);
        res.push_back(id);

        return res.join("_");
    }
};
class Object {
public:
    string name, inherits;
    vector<Field> fields;
    vector<Method> methods;
    vector<Index> indexes;
    vector<RelationHandle> handles;
    map<Relation*, vector<Relate*> > relations;
    Object * parentObject;
    vector<Object*> children;

    Object(string n, string i) : name(n), inherits(i),
        parentObject(NULL) {
        if (i.size() == 0) {
            inherits = "litesql::Persistent";
            fields.push_back(Field("id", A_field_type_integer, "", 
                         A_field_indexed_true, A_field_unique_true));
            fields.push_back(Field("type", A_field_type_string, "", 
                        A_field_indexed_false, A_field_unique_false));
        }
    }
    int getLastFieldOffset() const {
        if (!parentObject)
            return fields.size();
        else return parentObject->getLastFieldOffset() + fields.size();
    }
    void getAllFields(vector<Field>& flds) const {
        if (parentObject)
            parentObject->getAllFields(flds);
        for (size_t i = 0; i < fields.size(); i++)
            flds.push_back(fields[i]);
    }
    void getChildrenNames(Split & names) const {
        for (size_t i = 0; i < children.size(); i++) {
            names.push_back(children[i]->name);
            children[i]->getChildrenNames(names);
        }
    }
    const Object* getBaseObject() const{
        if (!parentObject)
            return this;
        else
            return parentObject->getBaseObject();
    }
    string getTable() const {
        return name + "_";
    }
};
class Database {
public:
    string name, include, version, nspace;
};
void init(Database& db, 
          vector<Object>& objects,
          vector<Relation>& relations);


}

#endif