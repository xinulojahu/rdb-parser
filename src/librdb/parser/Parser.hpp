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

    ColumnDef parse_column_def();

    Value parse_value();

    Expression::Operand parse_operand();
    Expression::Operation parse_operation();
    Expression parse_expression();

    CreateTableStatementPtr parse_create_table_statement();
    SelectStatementPtr parse_select_statement();
    InsertStatementPtr parse_insert_statement();
    DeleteStatementPtr parse_delete_statement();
    DropTableStatementPtr parse_drop_table_statement();

    Lexer& lexer_;
};

}  // namespace rdb::parser
