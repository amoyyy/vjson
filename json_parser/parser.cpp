// system
#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>

// internal
#include "parser.h"

//*********************************************************************
// static functions
//*********************************************************************
static size_t get_file_size(const char *filename)
{
    if (filename == nullptr)
        return 0;
    struct stat statbuf;
    stat(filename, &statbuf);
    size_t filesize = statbuf.st_size;
    return filesize;
}

static bool skip_bom(const char *c_text, size_t len)
{
    // BOM check for UTF-8 format.
    if (len >= 3)
    {
        if (c_text[0] == char(0xEF) && c_text[1] == char(0xBB) && c_text[2] == char(0xBF))
        {
            return true;
        }
    }
    return false;
}

inline static bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

inline static bool is_hex_digit(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static bool char_array_contains(const char str[], int size, char s)
{
    for (int ii = 0; ii < size; ++ii)
        if (str[ii] == s)
            return true;
    return false;
}

#define IS_NEWLINE(c) char_array_contains(newlines, 3, c)

#define IS_SEPERATOR(c) char_array_contains(seperators, 6, c)

#define IS_ESCAPE(c) char_array_contains(escapable, 5, c)

#define IS_EXP_SIGN(c) char_array_contains(exp_signs, 2, c)

//*********************************************************************
// Scanner
//*********************************************************************
Scanner::Scanner(char *str, size_t size) : text(str), len(size)
{
    if (skip_bom(text, len))
        pos += 3;
}

Scanner::~Scanner()
{
    free(text);
}

void Scanner::move(bool include_space)
{
    pos++;
    if (pos < len)
    {
        if (IS_NEWLINE(text[pos]))
        {
            line++;
            col = 0;
            if (text[pos] == '\r' && pos + 1 < len && text[pos + 1] == '\n')
                pos++;
            while (pos < len && IS_NEWLINE(text[pos]))
                move();
        }
        else if (include_space && text[pos] == ' ')
        {
            pos++;
            col++;
            while (pos < len && text[pos] == ' ')
                move();
        }
    }
    else
        col++;
}

Token *Scanner::error_token(const FRString &msg)
{
    std::cout << "Invalid token. " << msg << std::endl;
    Token *e = new Token(token_error, col, line);
    return e;
}

Token *Scanner::tokenize(unsigned type)
{
    Token *t = new Token(type, col, line);
    return t;
}

Token *Scanner::tokenize(char *lit, size_t len, unsigned type)
{
    Token *t = new Token(lit, len, type, col, line);
    return t;
}

Token *Scanner::scan_text()
{
    bool has_closed = false;
    int pos_start = pos;
    size_t number_len = 0;
    while (1)
    {
        pos++;
        col++;
        if (pos >= len)
            break;
        char ch = text[pos];
        if ((pos - 1 >= 0 && text[pos - 1] != '\\') && ch == '"')
        {
            has_closed = true;
            break;
        }
        else if ((pos - 1 >= 0 && text[pos - 1] != '\\') && IS_ESCAPE(ch))
            return error_token("character must be escaped with a \\");
        else if ((pos == len - 1 && ch == '\\') || ch == uint8_t(0))
            return error_token("invalid \\ escape");
        else if (pos + 1 < len && ch == '\\')
        {
            // For unicode, do nothing;
        }
        number_len++;
    }
    if (!has_closed)
    {
        return error_token("missing right \"");
    }
    move();
    return tokenize(&(text[pos_start + 1]), number_len, token_string);
}

Token *Scanner::scan_number()
{
    bool is_float = false;
    int dot_index = -1;
    int pos_start = pos;
    int number_len = 0;
    if (text[pos] == '-')
    {
        number_len++;
        if (!is_digit(text[pos + 1]))
        {
            return error_token();
        }
        move(false);
    }
    if (text[pos] == '0' && (pos + 1 < len && is_digit(text[pos + 1])))
    {
        return error_token("leading zeroes in a number");
    }
    while (pos < len && (is_digit(text[pos]) || (!is_float && text[pos] == '.')))
    {
        number_len++;
        if (text[pos] == '.')
        {
            is_float = true;
            dot_index = number_len - 1;
        }
        move(false);
    }
    if (dot_index + 1 < len && dot_index + 1 == number_len)
    {
        return error_token("invalid float number");
    }
    if (pos < len && (text[pos] == 'e' || text[pos] == 'E'))
    {
        is_float = true;
        number_len++;
        move(false);
        if (pos < len && IS_EXP_SIGN(text[pos]))
        {
            number_len++;
            move(false);
        }
        int exp_digits_count = 0;
        while (pos < len && is_digit(text[pos]))
        {
            number_len++;
            move(false);
            exp_digits_count++;
        }
        if (exp_digits_count == 0)
        {
            return error_token("invalid exponent number");
        }
    }
    unsigned type = (is_float) ? token_float : token_int;
    return tokenize(&(text[pos_start]), number_len, type);
}

Token *Scanner::scan()
{

#define MULTY_MOVE(n)           \
    for (int i = 0; i < n; ++i) \
        move();

    Token *tok = nullptr;

    if ((pos < len) && (text[pos] == ' ' || IS_NEWLINE(text[pos])))
        move();

    if (pos >= len)
    {
        tok = tokenize(token_eof);
        return tok;
    }
    else if (pos + 3 < len && (text[pos] == 't' || text[pos] == 'n'))
    {
        FRString ident(&(text[pos]), 4);
        if (ident == "true" || ident == "null")
        {
            TokenType type = token_null;
            if (ident == "true")
                type = token_bool;
            tok = tokenize(&(text[pos]), 4, type);
            MULTY_MOVE(4);
            return tok;
        }
        tok = error_token();
        return tok;
    }
    else if (pos + 4 < len && text[pos] == 'f')
    {
        FRString ident(&(text[pos]), 5);
        if (ident == "false")
        {
            tok = tokenize(&(text[pos]), 5, token_bool);
            MULTY_MOVE(5);
            return tok;
        }
        tok = error_token();
        return tok;
    }
    else if (IS_SEPERATOR(text[pos]))
    {
        tok = tokenize(&(text[pos]), 1, (int)(text[pos]));
        move();
        return tok;
    }
    else if (text[pos] == '"')
    {
        tok = scan_text();
        return tok;
    }
    else if (is_digit(text[pos]) || text[pos] == '-')
    {
        tok = scan_number();
        return tok;
    }

    tok = error_token();
    return tok;
}

//*********************************************************************
// Class: Parser
//*********************************************************************
Parser::Parser(const std::string &file_path)
{
    size_t size = get_file_size(file_path.c_str());
    char *buf;
    buf = (char *)malloc(sizeof(char) * size);
    std::ifstream input(file_path);
    input.read(buf, size);
    scanner = new Scanner(buf, size);
    input.close();
}

void Parser::print_errors()
{
    std::cout << "-----------Parsing token ERROR(begin)-----------" << std::endl;
    std::cout << "wrong format exists in following 1,2,3 inputs";
    std::cout << "| 1:" << p_tok->to_string();
    std::cout << "| 2:" << tok->to_string();
    std::cout << "| 3:" << n_tok->to_string() << std::endl;
    std::cout << "-----------Parsing token ERROR(end)-------------" << std::endl;
}

void Parser::next() noexcept
{
    if (p_tok != nullptr)
        delete p_tok;
    p_tok = tok;
    tok = n_tok;
    n_tok = scanner->scan();
}

bool Parser::next_with_check() noexcept
{
    next();
    if (tok->type == token_error)
    {
        print_errors();
        return true;
    }
    return false;
}

#define RETURN_EMPTY_VALUE \
    return new Value();

#define RETURN_EMPTY_IF_NEXT_CHECK_FAILS \
    if (next_with_check())               \
        RETURN_EMPTY_VALUE;

Value *Parser::decode() noexcept
{
    next();

    RETURN_EMPTY_IF_NEXT_CHECK_FAILS

    auto &&res = decode_value();

    if (tok->type != token_eof)
        RETURN_EMPTY_VALUE;

    return res;
}

Value *Parser::decode_value() noexcept
{
    switch (tok->type)
    {
    case token_lsbr:
    {
        return decode_array();
    }
    case token_lcbr:
    {
        return decode_object();
    }
    case token_int:
    case token_float:
    case token_bool:
    case token_null:
    case token_string:
    {
        RETURN_EMPTY_IF_NEXT_CHECK_FAILS
        return new Value(p_tok);
    }
    default:
    {
        print_errors();
        RETURN_EMPTY_VALUE
    }
    }
}

Value *Parser::decode_array() noexcept
{
    RETURN_EMPTY_IF_NEXT_CHECK_FAILS
    n_level++;

    auto &&arr = new Value(token_arr);

    while (tok->type != token_rsbr)
    {
        arr->push_back(decode_value());

        if (tok->type == token_comma)
        {
            RETURN_EMPTY_IF_NEXT_CHECK_FAILS
            if (tok->type == token_rsbr)
                RETURN_EMPTY_VALUE
        }
        else if (tok->type != token_rsbr)
            RETURN_EMPTY_VALUE
    }
    RETURN_EMPTY_IF_NEXT_CHECK_FAILS
    n_level--;

    arr->shrink();
    return arr;
}

Value *Parser::decode_object() noexcept
{
    RETURN_EMPTY_IF_NEXT_CHECK_FAILS
    n_level++;

    Value *fields = new Value(token_obj);

    while (tok->type != token_rcbr)
    {
        if (tok->type != token_string)
            RETURN_EMPTY_VALUE;

        FRString key(tok->str, tok->len);

        RETURN_EMPTY_IF_NEXT_CHECK_FAILS

        if (tok->type != token_colon)
            RETURN_EMPTY_VALUE;

        RETURN_EMPTY_IF_NEXT_CHECK_FAILS

        fields->push_back(decode_value(), key);

        if (tok->type != token_comma && tok->type != token_rcbr)
            RETURN_EMPTY_VALUE
        else if (tok->type == token_comma)
            RETURN_EMPTY_IF_NEXT_CHECK_FAILS
    }

    RETURN_EMPTY_IF_NEXT_CHECK_FAILS;
    n_level--;

    fields->shrink();
    return fields;
}