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
        case Token::Kind::KwDrop:
            return parse_drop_table_statement();
        case Token::Kind::KwInsert:
            return parse_insert_table_statement();
        default:
            break;
    }
    throw SyntaxError("Expected Drop or Insert");
}

Token Parser::fetch_token(Token::Kind expected_kind) {
    Token token = lexer_.peek();
    if (token.type() != expected_kind) {
        throw SyntaxError(
            "Expeted " + std::string(kind_to_str(expected_kind)) + ", got " +
            std::string(kind_to_str(token.type())));
    }
    return lexer_.get();
}

DropTableStatementPtr Parser::parse_drop_table_statement() {
    fetch_token(Token::Kind::KwDrop);
    fetch_token(Token::Kind::KwTable);
    const Token table_name = fetch_token(Token::Kind::Id);
    fetch_token(Token::Kind::Semicolon);
    return std::make_unique<const DropTableStatement>(table_name.lexema());
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

Value Parser::parse_value() {
    const Token token = lexer_.peek();
    char* end = nullptr;
    const int base = 10;
    Value val;
    if (token.type() == Token::Kind::Int) {
        lexer_.get();
        val = int32_t(std::strtol(token.lexema().data(), &end, base));
        assert(token.lexema().data() + token.lexema().size() == end);
        return val;
    }
    if (token.type() == Token::Kind::Real) {
        lexer_.get();
        val = std::strtof(lexer_.get().lexema().data(), &end);
        assert(token.lexema().data() + token.lexema().size() == end);
        return val;
    }
    if (token.type() == Token::Kind::String) {
        lexer_.get();
        val = lexer_.get().lexema();
        return val;
    }

    throw SyntaxError(
        "Expeted int, float or string, got " +
        std::string(kind_to_str(token.type())));
}

}  // namespace rdb::parser
