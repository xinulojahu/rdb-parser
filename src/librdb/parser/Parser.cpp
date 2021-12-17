#include <librdb/parser/Lexer.hpp>
#include <librdb/parser/Parser.hpp>
#include <librdb/parser/Statements.hpp>

#include <cassert>
#include <charconv>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rdb::parser {

namespace {
class SyntaxError : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
};
}  // namespace

static std::string make_error_msg(std::string_view expected, Token got) {
    return "Expected " + std::string(expected) + ", got " +
           std::string(kind_to_str(got.type())) + " '" +
           std::string(got.lexema()) + "' " +
           std::to_string(got.location().rows_) + ":" +
           std::to_string(got.location().cols_);
}

Parser::Result Parser::parse_sql_script() {
    Parser::Result result;
    while (true) {
        Token next_token = lexer_.peek();
        if (next_token.type() == Token::Kind::Eof) {
            break;
        }
        try {
            result.script.statements_.push_back(parse_sql_statement());
        } catch (const SyntaxError& e) {
            result.errors_.emplace_back(e.what());
            panic();
        }
    }

    return result;
}

void Parser::panic() {
    while (true) {
        const Token preview = lexer_.get();
        if ((preview.type() == Token::Kind::Eof) ||
            (preview.type() == Token::Kind::Semicolon)) {
            break;
        }
    }
}

StatementPtr Parser::parse_sql_statement() {
    const Token token = lexer_.peek();
    switch (token.type()) {
        case Token::Kind::KwInsert:
            return parse_insert_table_statement();
        case Token::Kind::KwDelete:
            return parse_delete_statement();
        case Token::Kind::KwDrop:
            return parse_drop_table_statement();
        default:
            break;
    }
    throw SyntaxError(
        make_error_msg("CREATE, SELECT, INSERT, DELETE or DROP", token));
}

Token Parser::fetch_token(Token::Kind expected_kind) {
    Token token = lexer_.peek();
    if (token.type() != expected_kind) {
        throw SyntaxError(make_error_msg(kind_to_str(expected_kind), token));
    }
    return lexer_.get();
}

Value Parser::parse_value() {
    const Token token = lexer_.peek();
    const int base = 10;
    if (token.type() == Token::Kind::Int) {
        lexer_.get();
        Value val = int32_t(std::strtol(token.lexema().data(), nullptr, base));
        return val;
    }
    if (token.type() == Token::Kind::Real) {
        lexer_.get();
        Value val = std::strtof(token.lexema().data(), nullptr);
        return val;
    }
    if (token.type() == Token::Kind::String) {
        lexer_.get();
        Value val = token.lexema();
        return val;
    }

    throw SyntaxError(make_error_msg("value", token));
}

Operand Parser::parse_operand() {
    const Token token = lexer_.peek();
    if (token.type() == Token::Kind::Id) {
        lexer_.get();
        Operand oper = token.lexema();
        return oper;
    }
    if (token.type() == Token::Kind::Int || token.type() == Token::Kind::Real ||
        token.type() == Token::Kind::String) {
        Operand oper = parse_value();
        return oper;
    }

    throw SyntaxError(make_error_msg("operand", token));
}

Operation Parser::parse_operation() {
    const Token token = lexer_.get();
    switch (token.type()) {
        case Token::Kind::Lte:
            return Operation::Lte;
        case Token::Kind::Rte:
            return Operation::Rte;
        case Token::Kind::Neq:
            return Operation::Neq;
        case Token::Kind::Lt:
            return Operation::Lt;
        case Token::Kind::Rt:
            return Operation::Rt;
        case Token::Kind::Eq:
            return Operation::Eq;
        default:
            throw SyntaxError(make_error_msg("opration", token));
    }
}

Expression Parser::parse_expression() {
    Expression expression;
    expression.left_ = parse_operand();
    expression.operation_ = parse_operation();
    expression.right_ = parse_operand();
    return expression;
}

InsertStatementPtr Parser::parse_insert_table_statement() {
    fetch_token(Token::Kind::KwInsert);
    fetch_token(Token::Kind::KwInto);
    const Token table_name = fetch_token(Token::Kind::Id);

    fetch_token(Token::Kind::LParen);
    std::vector<std::string_view> column_names;
    const Token first_column_name = fetch_token(Token::Kind::Id);
    column_names.push_back(first_column_name.lexema());

    while (lexer_.peek().type() == Token::Kind::Comma) {
        fetch_token(Token::Kind::Comma);
        const Token next_column_name = fetch_token(Token::Kind::Id);
        column_names.push_back(next_column_name.lexema());
    }

    fetch_token(Token::Kind::RParen);

    fetch_token(Token::Kind::KwValues);
    fetch_token(Token::Kind::LParen);
    std::vector<Value> values;
    const Value first_value = parse_value();
    values.push_back(first_value);

    while (lexer_.peek().type() == Token::Kind::Comma) {
        fetch_token(Token::Kind::Comma);
        const Value next_value = parse_value();
        values.push_back(next_value);
    }

    fetch_token(Token::Kind::RParen);
    fetch_token(Token::Kind::Semicolon);

    return std::make_unique<const InsertStatement>(
        table_name.lexema(), column_names, values);
}

DeleteStatementPtr Parser::parse_delete_statement() {
    fetch_token(Token::Kind::KwDelete);
    fetch_token(Token::Kind::KwFrom);
    const Token table_name = fetch_token(Token::Kind::Id);

    if (lexer_.peek().type() == Token::Kind::KwWhere) {
        fetch_token(Token::Kind::KwWhere);
        const Expression expression = parse_expression();
        fetch_token(Token::Kind::Semicolon);
        return std::make_unique<const DeleteStatement>(
            table_name.lexema(), expression);
    }
    fetch_token(Token::Kind::Semicolon);
    return std::make_unique<const DeleteStatement>(table_name.lexema());
}

DropTableStatementPtr Parser::parse_drop_table_statement() {
    fetch_token(Token::Kind::KwDrop);
    fetch_token(Token::Kind::KwTable);
    const Token table_name = fetch_token(Token::Kind::Id);
    fetch_token(Token::Kind::Semicolon);
    return std::make_unique<const DropTableStatement>(table_name.lexema());
}

}  // namespace rdb::parser
