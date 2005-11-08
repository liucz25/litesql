/* XML application for litesql.dtd.
 * Includes actions from litesql-gen.xml.
 * Generated 2005/11/06 03:05:02.
 *
 * This program was generated with the FleXML XML processor generator.
 * FleXML is Copyright � 1999-2005 Kristoffer Rose.  All rights reserved.
 * (Id: flexml.pl,v 1.44 2005/02/23 23:08:16 mquinson Exp).
 * 
 * There are two, intertwined parts to this program, part A and part B.
 *
 * Part A
 * ------
 * 
 * Some parts, here collectively called "Part A", are found in the 
 * FleXML package.  They are Copyright � 1999-2005 Kristoffer Rose. 
 * All rights reserved.
 *
 * You can redistribute, use, perform, display and/or modify "Part A"
 * provided the following two conditions hold:
 *
 * 1. The program is distributed WITHOUT ANY WARRANTY from the author of
 *    FleXML; without even the implied warranty of MERCHANTABILITY or
 *    FITNESS FOR A PARTICULAR PURPOSE.
 *
 * 2. The program distribution conditions do not in any way affect the
 *    distribution conditions of the FleXML system used to generate this
 *    file or any version of FleXML derived from that system.
 *
 * Notice that these are explicit rights granted to you for files
 * generated by the FleXML system.  For your rights in connection with
 * the FleXML system itself please consult the GNU General Public License.
 * 
 * Part B
 * ------
 * 
 * The other parts, here collectively called "Part B", and which came 
 * from the DTD used by FleXML to generate this program, can be 
 * distributed (or not, as the case may be) under the terms of whoever
 * wrote them, provided these terms respect and obey the two conditions 
 * above under the heading "Part A".
 *
 * The author of and contributors to FleXML specifically disclaim
 * any copyright interest in "Part B", unless "Part B" was written 
 * by the author of or contributors to FleXML.
 * 
 */

#include "litesql-gen.hpp"


#line 3 "litesql-gen.xml"

#include "cppgenerator.hpp"
#include "xmlobjects.hpp"
#include "litesql-gen-cpp.hpp"
#include "litesql-gen.hpp"
using namespace std;
using namespace gen;
FILE *hpp, *cpp;
vector<xml::Object> objects;
vector<xml::Relation> relations;
xml::Object * obj;
xml::Relation * rel;
xml::Field * fld;
xml::Method * mtd;
bool hasNamespace;
string dbName;

void STag_database(void)
{
#line 20 "litesql-gen.xml"

dbName = A_database_name;
string hppName = toLower(A_database_name) + ".hpp";
hpp = fopen(hppName.c_str(), "w");
string cppName = toLower(A_database_name) + ".cpp";
cpp = fopen(cppName.c_str(), "w");
fprintf(hpp, "#include \"litesql.hpp\"\n");
if (A_database_include) 
    fprintf(hpp, "#include \"%s\"\n", A_database_include);
fprintf(cpp, "#include \"%s\"\n", hppName.c_str());
if (A_database_namespace) {
    fprintf(hpp, "namespace %s {\n", A_database_namespace);
    fprintf(cpp, "namespace %s {\n", A_database_namespace);
    hasNamespace = true;
} else
    hasNamespace = false;
fprintf(cpp, "using namespace litesql;\n");

} /* STag_database */

void STag_object(void)
{
#line 38 "litesql-gen.xml"

objects.push_back(xml::Object(A_object_name, xml::safe(A_object_inherits)));
obj = &objects[objects.size()-1];

} /* STag_object */

void ETag_object(void)
{
#line 42 "litesql-gen.xml"

obj = NULL;

} /* ETag_object */

void STag_relation(void)
{
#line 45 "litesql-gen.xml"

relations.push_back(xml::Relation(xml::safe(A_relation_id), xml::safe(A_relation_name),A_relation_unidir));
rel = &relations[relations.size()-1];

} /* STag_relation */

void ETag_relation(void)
{
#line 49 "litesql-gen.xml"

rel = NULL;

} /* ETag_relation */

void STag_field(void)
{
#line 52 "litesql-gen.xml"

if (obj) {
    obj->fields.push_back(xml::Field(A_field_name, 
                                     A_field_type,xml::safe(A_field_default),A_field_indexed));
    fld = &obj->fields[obj->fields.size()-1];
} else if (rel) {
    rel->fields.push_back(xml::Field(A_field_name, 
                         A_field_type,xml::safe(A_field_default),A_field_indexed));
    fld = &rel->fields[rel->fields.size()-1];
}

} /* STag_field */

void STag_value(void)
{
#line 63 "litesql-gen.xml"

if (fld) 
    fld->value(xml::Value(A_value_name, A_value_value));

} /* STag_value */

void ETag_field(void)
{
#line 67 "litesql-gen.xml"

fld = NULL;

} /* ETag_field */

void STag_method(void)
{
#line 70 "litesql-gen.xml"

if (obj) {
    obj->methods.push_back(xml::Method(A_method_name, xml::safe(A_method_returntype)));
    mtd = &obj->methods[obj->methods.size()-1];
}

} /* STag_method */

void ETag_method(void)
{
#line 76 "litesql-gen.xml"

mtd = NULL;

} /* ETag_method */

void STag_param(void)
{
#line 79 "litesql-gen.xml"

if (mtd) 
    mtd->param(xml::Param(A_param_name, A_param_type));

} /* STag_param */

void STag_relate(void)
{
#line 83 "litesql-gen.xml"

if (rel)
    rel->related.push_back(xml::Relate(A_relate_object, A_relate_limit, A_relate_unique, xml::safe(A_relate_handle)));

} /* STag_relate */

void ETag_database(void)
{
#line 87 "litesql-gen.xml"

try {
    writeCPPClasses(hpp, cpp, dbName, objects, relations);
} catch (logic_error& e) {
    cerr << e.what() << endl;       
}
if (hasNamespace) {
    fprintf(hpp, "}\n");
    fprintf(cpp, "}\n");
}
fclose(hpp);
fclose(cpp);

} /* ETag_database */


#line 115 "litesql-gen.xml"

extern FILE * yyin;
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: litesql-gen <my-database.xml>\n");
        return -1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("could not open file: ");
        return -1;
    }
    
    return yylex();
}


/* XML application entry points. */
void STag_type(void) {}
void ETag_type(void) {}
void ETag_value(void) {}
void ETag_param(void) {}
void ETag_relate(void) {}
