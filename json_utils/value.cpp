#include <cstring>
#include "value.h"
namespace utils
{
    namespace json
    {
        //*********************************************************************
        // Constructor && Deconstructor
        //*********************************************************************
        Value::Value(Token *t) : type(t->type)
        {
            switch (type)
            {
            case token_int:
            {
                char *p_end = &(t->str[t->len - 1]);
                _i64 = std::strtoll(t->str, &(p_end), 10);
                break;
            }
            case token_float:
            {
                char *p_end = &(t->str[t->len - 1]);
                _f64 = std::strtod(t->str, &(p_end));
                break;
            }
            case token_bool:
                _bool = (t->to_string() == "true");
                break;
            case token_null:
                _null = Null{};
                break;
            case token_string:
                _string = new FRString(t->str, t->len);
                break;
            default:
                break;
            }
        }

        Value::Value(TokenType typ)
        {
            switch (typ)
            {
            // map
            case token_obj:
                type = token_obj;
                _obj = new ValueObject();
                break;
            // array
            case token_arr:
                type = token_arr;
                _arr = new ValueArray();
                break;
            default:
                break;
            }
        }

        Value::Value(Value &&t)
        {
            type = t.type;
            switch (type)
            {
            case token_int:
                _i64 = t._i64;
                break;
            case token_float:
                _f64 = t._f64;
                break;
            case token_bool:
                _bool = t._bool;
                break;
            case token_null:
                _null = Null{};
                break;
            case token_string:
                _string = t._string;
                t._string = nullptr;
                break;
            // map
            case token_obj:
                _obj = t._obj;
                t._obj = nullptr;
                break;
            // array
            case token_arr:
                _arr = t._arr;
                t._arr = nullptr;
                break;
            default:
                break;
            }
        }

        void Value::shrink()
        {
            switch (type)
            {
            // map
            case token_obj:
                // shrink_v(*_obj);
                break;
            // array
            case token_arr:
                _arr->shrink_to_fit();
                break;
            default:
                break;
            }
        }

        Value::~Value()
        {
            switch (type)
            {
            case token_string:
                if (_string != nullptr)
                    delete _string;
                break;
            case token_arr:
                if (_arr != nullptr)
                {
                    for (auto ele : (*_arr))
                        delete ele;
                    delete _arr;
                }
                break;
            case token_obj:
                if (_obj != nullptr)
                {
                    for (auto item : (*_obj))
                        delete item.second;
                    delete _obj;
                }
                break;
            default:
                break;
            }
        }

        //*********************************************************************
        // Get: to get value
        //*********************************************************************
        // to get FRString from value
        FRString Value::get_string() const
        {
            if (type == token_string)
                return *_string;
            return "";
        }

        // to get int from value
        int64_t Value::get_int() const
        {
            if (type == token_int)
                return _i64;
            return int64_t(0);
        }

        // to get double from value
        double Value::get_double()
        {
            if (type == token_float)
                return _f64;
            else if (type == token_int)
            {
                type = token_float;
                double x = _i64;
                _f64 = x;
                return _f64;
            }
            return 0.0;
        }

        // to get bool from value
        bool Value::get_bool() const
        {
            if (type == token_bool)
                return _bool;
            return false;
        }

        // to get array from value
        ValueArray &Value::get_array() const
        {
            if (type == token_arr)
                return *_arr;
        }

        // to get object from value
        ValueObject &Value::get_object() const
        {
            if (type == token_obj)
                return *_obj;
        }

        //*********************************************************************
        // Access:
        //*********************************************************************
        int Value::size() const
        {
            if (type == token_arr && _arr != nullptr)
                return _arr->size();
            if (type == token_obj && _obj != nullptr)
                return _obj->size();
            return 0;
        }

        Value &Value::operator[](const int index) const
        {
            if (type == token_arr)
                return *((*_arr)[index]);
        }

        Value &Value::operator[](const FRString &key) const
        {
            if (type == token_obj)
                return *((*_obj)[key]);
        }

        bool Value::has_member(const FRString &key) const
        {
            return _obj->has((void *)(&key));
        }

        // for object
        // not change the order of key
        void Value::replace_item(const FRString &key, Value *item)
        {
            _obj->set((void *)(&key), (void *)(item));
        }

        // for object
        // change the order of key
        void Value::delete_item(const FRString &key)
        {
            _obj->remove((void *)(&key));
        }
    }
}