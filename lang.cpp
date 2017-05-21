#include "lang.h"

const std::array<const char *, 30> cpp::keywords = {
    "private",   "public",   "protected",          //
    "virtual",                                     //
    "volatile",                                    //
    "extern",    "static",                         //
    "const",     "auto",                           //
    "namespace", "class",    "struct",    "union", //
    "template",  "typename",                       //
    "void",                                        //
    "goto",      "break",    "return",             //
    "true",      "false",                          //
    "inline",                                      //
    "if",        "switch",   "case",               //
    "while",     "do",       "for",                //
    "using",     "typedef"                         //
};
