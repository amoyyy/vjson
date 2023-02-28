#pragma once
#include <map>
#include "../json_utils/value.h"

namespace utils
{
    namespace json
    {
        //*********************************************************************
        // Const
        //*********************************************************************
        const char seperators[6] = {'{', '}', '[', ']', ',', ':'};
        const char newlines[3] = {'\r', '\n', '\t'};
        const char exp_signs[2] = {u'-', u'+'};
        const char escapable[5] = {'\b', '\f', '\n', '\r', '\t'};

        //*********************************************************************
        // Class: Scanner
        //*********************************************************************
        class Scanner
        {
        public:
            char *text;
            size_t len;

        public:
            int pos = 0;
            int line = 0;
            int col = 0;

        private:
            Token *error_token(const FRString &msg = "");

            Token *tokenize(unsigned typ);

            Token *tokenize(char *lit, size_t len, unsigned typ);

        public:
            void move(bool include_space = true);

            Token *scan();

            Token *scan_text();

            Token *scan_number();

        public:
            Scanner(char *str, size_t size);
            ~Scanner();

        public:
            Scanner() = default;
            Scanner(const Scanner &s) = default;
            Scanner(Scanner &&s) = default;
            Scanner &operator=(const Scanner &s) = default;
            Scanner &operator=(Scanner &&s) = default;
        };

        //*********************************************************************
        // Class: Parser
        //*********************************************************************
        class Parser
        {
        private:
            Scanner *scanner = nullptr;
            Token *p_tok = nullptr;
            Token *tok = nullptr;
            Token *n_tok = nullptr;
            int n_level = 0;

        private:
            void print_errors();

            void next() noexcept;

            bool next_with_check() noexcept;

            Value *decode_value() noexcept;

            Value *decode_array() noexcept;

            Value *decode_object() noexcept;

        public:
            Value *decode() noexcept;

        public:
            Parser(const std::string &file_path);

            ~Parser()
            {
                if (scanner != nullptr)
                    delete scanner;
                if (p_tok != nullptr)
                    delete p_tok;
                if (tok != nullptr)
                    delete tok;
                if (n_tok != nullptr)
                    delete n_tok;
            }

        public:
            Parser() = default;
            Parser(const Parser &p) = default;
            Parser(Parser &&p) = default;
            Parser &operator=(const Parser &p) = default;
            Parser &operator=(Parser &&p) = default;
        };
    }
}