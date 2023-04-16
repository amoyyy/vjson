#pragma once
#include "../../../primitives/FRString.h"

//*********************************************************************
// Enums
//*********************************************************************
enum TokenType
{
    token_none = 0,   //
    token_error = 1,  //
    token_string = 2, //
    token_float = 3,  //
    token_int = 4,    //
    token_null = 5,   //
    token_bool = 6,   //
    token_eof = 7,
    token_comma = 44, // 44
    token_colon = 58, // 58
    token_lsbr = 91,  // 91
    token_rsbr = 93,  // 93
    token_lcbr = 123, // 123
    token_rcbr = 125, // 125
    token_obj = 888,
    token_arr = 999,
};

//*********************************************************************
// Class: Token
//*********************************************************************
class Token
{
public:
    char *str;
    size_t len = 0;

public:
    unsigned type = TokenType::token_none;
    int col = 0;
    int line = 0;

public:
    inline FRString to_string()
    {
        return FRString(str, len);
    }

public:
    Token(char *s, int len, unsigned typ, int col, int line) : str(s), len(len), type(typ), col(col), line(line) {}

    Token(unsigned typ, int col, int line) : type(typ), col(col), line(line) {}

public:
    Token() = default;
    Token(const Token &t) = default;
    Token(Token &&t) = default;
    Token &operator=(const Token &t) = default;
    Token &operator=(Token &&t) = default;
    ~Token() = default;
};