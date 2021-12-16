#pragma once

#include <librdb/parser/Lexer.hpp>
#include <librdb/parser/Script.hpp>
#include <librdb/parser/Statements.hpp>

namespace rdb::parser {

class Parser {
   public:
    struct Result {
        Script script;
        std::vector<std::string> errors_;
    };

    explicit Parser(Lexer& lexer) : lexer_(lexer) {}

    Result parse_sql_script();

   private:
    void panic();

    StatementPtr parse_sql_statement();

    Token fetch_token(Token::Kind expected_kind);

    DropTablesStatementPtr parse_drop_table_statement();
    InsertStatementPtr parse_insert_table_statement();

    Value parse_value();

    Lexer& lexer_;
};

}  // namespace rdb::parser
