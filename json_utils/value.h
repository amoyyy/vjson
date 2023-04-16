#pragma once

#include <cinttypes>
#include <iostream>
#include <concepts>

#include "token.h"
#include "../../../primitives/FRString.h"
#include "../../../primitives/FRArray.h"
#include "../../../primitives/FRMap.h"

class Value;
using ValueArray = FRPointerArray<Value>;
using ValueObject = FRMap<FRString, Value *>;

struct Null
{
};

class Value
{
private:
    union
    {
        FRString *_string = nullptr;
        Null _null;
        bool _bool;
        int64_t _i64;
        double _f64;
        ValueArray *_arr;
        ValueObject *_obj;
    };

public:
    int type = -1;

public:
    // read data
    FRString get_string() const;
    int64_t get_int() const;
    double get_double();
    bool get_bool() const;
    ValueArray &get_array() const;
    ValueObject &get_object() const;
    Value *get_value(const FRString &key);

public:
    bool is_string();
    bool is_int();
    bool is_double();
    bool is_bool();
    bool is_array();
    bool is_object();

public:
    void shrink();

public:
    // write data to existed value
    void set_value(const FRString &str)
    {
        if (nullptr != _string)
            delete _string;
        _string = new FRString(str);
        type = token_string;
    }
    void set_value(std::floating_point auto number)
    {
        _f64 = number;
        type = token_float;
    }
    void set_value(std::integral auto number)
    {
        _i64 = number;
        type = token_int;
    }

public:
    // write data to existed value
    Value &operator=(const FRString &str)
    {
        if (nullptr != _string)
            delete _string;
        _string = new FRString(str);
        return *this;
    }
    Value &operator=(std::integral auto number)
    {
        _i64 = number;
        return *this;
    }
    Value &operator=(std::floating_point auto number)
    {
        _f64 = number;
        return *this;
    }

public:
    inline bool is_empty() { return type == -1; }
    // array && object size
    int size() const;
    Value &operator[](const int index) const;
    Value &operator[](const FRString &key) const;
    bool has_member(const FRString &key) const;

    template <typename T>
    void get_to(T &obj)
    {
        from_json(*this, obj);
    }

public:
    // for array & object
    void push_back(Value *item, const FRString &key = "")
    {
        switch (type)
        {
        case (token_arr):
            _arr->emplace_back(item);
            break;
        case (token_obj):
            MAP_INSERT((*_obj), key, item);
            break;
        }
    }
    void push_back(std::floating_point auto n, const FRString &key = "")
    {
        switch (type)
        {
        case (token_arr):
        {
            Value *v = new Value();
            v->set_value(n);
            _arr->emplace_back(v);
            break;
        }
        case (token_obj):
        {
            Value *v = new Value();
            v->set_value(n);
            push_back(v, key);
            break;
        }
        }
    }
    void push_back(std::integral auto n, const FRString &key = "")
    {
        switch (type)
        {
        case (token_arr):
        {
            Value *v = new Value();
            v->set_value(n);
            _arr->emplace_back(v);
            break;
        }
        case (token_obj):
        {
            Value *v = new Value();
            v->set_value(n);
            push_back(v, key);
            break;
        }
        }
    }
    void push_back(const FRString &n, const FRString &key = "")
    {
        switch (type)
        {
        case (token_arr):
        {
            Value *v = new Value();
            v->set_value(n);
            _arr->emplace_back(v);
            break;
        }
        case (token_obj):
        {
            Value *v = new Value();
            v->set_value(n);
            push_back(v, key);
            break;
        }
        }
    }
    void pop_back()
    {
        if (_arr->size() > 0)
        {
            delete (_arr->pop_back());
        }
    }

    // for object
    void replace_item(const FRString &key, Value *item);
    void delete_item(const FRString &key);

public:
    Value(Token *tok);
    Value(TokenType typ);
    Value(Value &&t);
    ~Value();

public:
    Value() = default;
    Value(const Value &t) = delete;
    Value &operator=(const Value &t) = delete;
    Value &operator=(Value &&t) = default;
};

#define LOOP_ON_JSON_OBJECT(doc, key) \
    if (doc.has_member(key))          \
        for (auto iter : doc[key].get_object())

#define LOOP_ON_JSON_OBJECT_VALUE(json) \
    for (auto it : json->get_object())

#define LOOP_ON_JSON_ARRAY(doc, key) \
    if (doc.has_member(key))         \
        for (auto &iter : doc[key].get_array())

#define LOOP_ON_JSON_ARRAY_VALUE(json) \
    for (auto &iter : json->get_array())