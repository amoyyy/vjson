#pragma once
#include <algorithm>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include "../json_utils/value.h"
#include "dtoa.h"

namespace utils
{
    namespace json
    {
        class Encoder
        {
        public:
            std::stringstream buffer;
            char dbuf[256];

        private:
            void encode(Value *data)
            {
                switch (data->type)
                {
                case (TokenType::token_int):
                    buffer << data->get_int();
                    break;
                case (TokenType::token_float):
                    milo::dtoa(data->get_double(), dbuf);
                    buffer << dbuf;
                    break;
                case (TokenType::token_string):
                    buffer << "\"" << data->get_string() << "\"";
                    break;
                case (TokenType::token_arr):
                {
                    auto &&m = data->get_array();
                    buffer << "[";
                    int len = m.size();
                    for (int ii = 0; ii < len - 1; ++ii)
                    {
                        encode(m[ii]);
                        buffer << ",";
                    }
                    for (int ii = len - 1; ii >= 0 && ii < len; ++ii)
                        encode(m[ii]);
                    buffer << "]";
                    break;
                }
                case (TokenType::token_obj):
                {
                    auto &&m = data->get_object();
                    int size = m.size();
                    int ii = 0;
                    buffer << "{";
                    for (auto iter : m)
                    {
                        ++ii;
                        buffer << "\"" << iter.first << "\":";
                        encode(iter.second);
                        if ( ii < size) buffer << ",";
                    }
                    buffer << "}";
                    break;
                }
                case (TokenType::token_null):
                    buffer << "null";
                    break;
                default:
                    break;
                }
            }

        public:
            Encoder(Value *data)
            {
                this->buffer << std::setprecision(15);
                encode(data);
            }
        };
    }
}