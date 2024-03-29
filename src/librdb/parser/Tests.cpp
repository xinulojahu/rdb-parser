#include <librdb/parser/Lexer.hpp>
#include <librdb/parser/Parser.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>

#include <gtest/gtest.h>

std::string get_tokens(const std::string_view input) {
    rdb::parser::Lexer lexer(input);
    std::stringstream out;
    rdb::parser::Token token = lexer.peek();
    do {
        token = lexer.get();
        out << token << '\n';
    } while (token.type() != rdb::parser::Token::Kind::Eof);
    return out.str();
}

TEST(LexerSuite, PeekGetTest) {
    rdb::parser::Lexer lexer("a b c\n d\n\n e");
    std::stringstream tokens;

    tokens << lexer.peek() << '\n';
    tokens << lexer.peek() << '\n';
    tokens << lexer.peek() << '\n';
    tokens << lexer.get() << '\n';
    tokens << lexer.peek() << '\n';
    tokens << lexer.get() << '\n';
    tokens << lexer.peek() << '\n';
    tokens << lexer.peek() << '\n';
    tokens << lexer.get() << '\n';
    tokens << lexer.get() << '\n';
    tokens << lexer.get() << '\n';

    const std::string expcted_token =
        "Id 'a' Loc=1:1\n"
        "Id 'a' Loc=1:1\n"
        "Id 'a' Loc=1:1\n"
        "Id 'a' Loc=1:1\n"
        "Id 'b' Loc=1:3\n"
        "Id 'b' Loc=1:3\n"
        "Id 'c' Loc=1:5\n"
        "Id 'c' Loc=1:5\n"
        "Id 'c' Loc=1:5\n"
        "Id 'd' Loc=2:2\n"
        "Id 'e' Loc=4:2\n";

    EXPECT_EQ(expcted_token, tokens.str());
}

TEST(LexerSuite, IdTest) {
    const auto tokens = get_tokens("a b2 aselect $");

    const std::string expcted_token =
        "Id 'a' Loc=1:1\n"
        "Id 'b2' Loc=1:3\n"
        "Id 'aselect' Loc=1:6\n"
        "Unknown '$' Loc=1:14\n"
        "Eof '<EOF>' Loc=1:15\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, KwTest) {
    const auto tokens = get_tokens("SELECT value FROM table");

    const std::string expcted_token =
        "KwSelect 'SELECT' Loc=1:1\n"
        "Id 'value' Loc=1:8\n"
        "KwFrom 'FROM' Loc=1:14\n"
        "Id 'table' Loc=1:19\n"
        "Eof '<EOF>' Loc=1:24\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, IntTest) {
    const auto tokens = get_tokens("123 -312 +0123 -k");

    const std::string expcted_token =
        "Int '123' Loc=1:1\n"
        "Int '-312' Loc=1:5\n"
        "Int '+0' Loc=1:10\n"
        "Int '123' Loc=1:12\n"
        "Unknown '-' Loc=1:16\n"
        "Id 'k' Loc=1:17\n"
        "Eof '<EOF>' Loc=1:18\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, RealTest) {
    const auto tokens = get_tokens("1. -312.123l2.1 +0.0123 3.5.31.3");

    const std::string expcted_token =
        "Real '1.' Loc=1:1\n"
        "Real '-312.123' Loc=1:4\n"
        "Id 'l2' Loc=1:12\n"
        "Unknown '.' Loc=1:14\n"
        "Int '1' Loc=1:15\n"
        "Real '+0.0123' Loc=1:17\n"
        "Real '3.5' Loc=1:25\n"
        "Unknown '.' Loc=1:28\n"
        "Real '31.3' Loc=1:29\n"
        "Eof '<EOF>' Loc=1:33\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, StringTest) {
    const auto tokens = get_tokens(
        "\"str1\"\n"
        "\"str2\n"
        "str3\"\n"
        "\"str4 1231 INSERT 2.1\"1231 INSERT 2.1\n"

    );

    const std::string expcted_token =
        "Text '\"str1\"' Loc=1:1\n"
        "Unknown '\"str2' Loc=2:1\n"
        "Id 'str3' Loc=3:1\n"
        "Unknown '\"' Loc=3:5\n"
        "Text '\"str4 1231 INSERT 2.1\"' Loc=4:1\n"
        "Int '1231' Loc=4:23\n"
        "KwInsert 'INSERT' Loc=4:28\n"
        "Real '2.1' Loc=4:35\n"
        "Eof '<EOF>' Loc=5:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, OperationTest) {
    const auto tokens = get_tokens(
        "<=\n"
        ">=\n"
        "!=\n"
        "<\n"
        ">\n"
        "=\n"
        "!\n"
        "<<<=!=>=!!===>>!>\n");

    const std::string expcted_token =
        "Lte '<=' Loc=1:1\n"
        "Rte '>=' Loc=2:1\n"
        "Neq '!=' Loc=3:1\n"
        "Lt '<' Loc=4:1\n"
        "Rt '>' Loc=5:1\n"
        "Eq '=' Loc=6:1\n"
        "Unknown '!' Loc=7:1\n"
        "Lt '<' Loc=8:1\n"
        "Lt '<' Loc=8:2\n"
        "Lte '<=' Loc=8:3\n"
        "Neq '!=' Loc=8:5\n"
        "Rte '>=' Loc=8:7\n"
        "Unknown '!' Loc=8:9\n"
        "Neq '!=' Loc=8:10\n"
        "Eq '=' Loc=8:12\n"
        "Eq '=' Loc=8:13\n"
        "Rt '>' Loc=8:14\n"
        "Rt '>' Loc=8:15\n"
        "Unknown '!' Loc=8:16\n"
        "Rt '>' Loc=8:17\n"
        "Eof '<EOF>' Loc=9:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, OtherTokensTest) {
    const auto tokens = get_tokens(
        ",(;)((;\n"
        ", ,;,;)\n");

    const std::string expcted_token =
        "Comma ',' Loc=1:1\n"
        "LParen '(' Loc=1:2\n"
        "Semicolon ';' Loc=1:3\n"
        "RParen ')' Loc=1:4\n"
        "LParen '(' Loc=1:5\n"
        "LParen '(' Loc=1:6\n"
        "Semicolon ';' Loc=1:7\n"
        "Comma ',' Loc=2:1\n"
        "Comma ',' Loc=2:3\n"
        "Semicolon ';' Loc=2:4\n"
        "Comma ',' Loc=2:5\n"
        "Semicolon ';' Loc=2:6\n"
        "RParen ')' Loc=2:7\n"
        "Eof '<EOF>' Loc=3:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

std::string get_parser_result(const std::string_view input) {
    rdb::parser::Lexer lexer(input);
    rdb::parser::Parser parser(lexer);
    rdb::parser::Parser::Result result;

    result = parser.parse_sql_script();

    std::stringstream out;

    for (const auto& i : result.script.statements_) {
        out << i->to_string() << '\n';
    }

    for (const auto& i : result.errors_) {
        out << i << '\n';
    }

    return out.str();
}

TEST(ParserSuite, CreateTableStatementTest) {
    const auto parser_result = get_parser_result(
        "CREATE TABLE table (col TEXT, int INT, real REAL);\n"
        "CREATE TABLE table (int INT, real REAL);\n"
        "CREATE TABLE table (real REAL);\n"
        "CREATE TABLE table (real);\n"
        "CREATE TABLE table (REAL);\n"
        "CREATE TABLE table2 table (col TEXT, int INT, real REAL);\n"
        "CREATE TABLE (col TEXT, int INT, real REAL);\n"
        "CREATE TABLE col TEXT (col TEXT, int INT, real REAL);\n"
        "CREATE table table (col TEXT, int INT, real REAL);\n"
        "CRETE TABLE table (col TEXT, int INT, real REAL);\n"
        "CREATE TABLE table ();\n"
        "CREATE TABLE table;\n"
        "CREATE TABLE table\n (col TEXT);\n");

    const std::string expected_result =
        "CREATE TABLE table (col TEXT, int INT, real REAL);\n"
        "CREATE TABLE table (int INT, real REAL);\n"
        "CREATE TABLE table (real REAL);\n"
        "CREATE TABLE table (col TEXT);\n"
        "Expected INT, REAL or TEXT, got RParen ')' 4:25\n"
        "Expected Id, got KwReal 'REAL' 5:21\n"
        "Expected LParen, got Id 'table' 6:21\n"
        "Expected Id, got LParen '(' 7:14\n"
        "Expected LParen, got KwText 'TEXT' 8:18\n"
        "Expected KwTable, got Id 'table' 9:8\n"
        "Expected CREATE, SELECT, INSERT, DELETE or DROP, got Id 'CRETE' 10:1\n"
        "Expected Id, got RParen ')' 11:21\n"
        "Expected LParen, got Semicolon ';' 12:19\n";

    EXPECT_EQ(expected_result, parser_result);
}

TEST(ParserSuite, SelectStatementTest) {
    const auto parser_result = get_parser_result(
        "SELECT c1 c2 c3 FROM t;\n"
        "SELECT c1 c2 FROM t WHERE 123 <= 13;\n"
        "SELECT c1 FROM t WHERE \"asd\" = id;\n"
        "SELECT c1 FROM t WHERE \"asd\" != 123;\n"
        "SELECT c1 FROM t WHERE 123.2 >= 123;\n"
        "SELECT c1 FROM t WHERE \"asd\" < 123;\n"
        "SELECT c1 FROM t WHERE id > id2;\n"
        "SELECT c1 FROM t WHERE;\n"
        "SELECT c1 FROM t WHERE dd <=;\n"
        "SELECT c1 FROM t WHERE <= ddd;\n"
        "SELECT c1 FROM t WHERE t ! ddd;\n"
        "SELECT c1 FROM t WHERE t <> ddd;\n"
        "SELEST c1 c2 FROM t WHERE 123 <= 13;\n"
        "SELECT c1 FROM WHERE t <= ddd;\n"
        "SELECT c1 t WHERE t <= ddd;\n"
        "SELECT c1 FROM t <= ddd;\n"
        "SELECT FROM t WHERE t <= ddd;\n");

    const std::string expected_result =
        "SELECT c1 c2 c3 FROM t;\n"
        "SELECT c1 c2 FROM t WHERE 123 <= 13;\n"
        "SELECT c1 FROM t WHERE \"asd\" = id;\n"
        "SELECT c1 FROM t WHERE \"asd\" != 123;\n"
        "SELECT c1 FROM t WHERE 123.199997 >= 123;\n"
        "SELECT c1 FROM t WHERE \"asd\" < 123;\n"
        "SELECT c1 FROM t WHERE id > id2;\n"
        "Expected operand, got Semicolon ';' 8:23\n"
        "Expected operand, got Semicolon ';' 9:29\n"
        "Expected operand, got Lte '<=' 10:24\n"
        "Expected opration, got Unknown '!' 11:26\n"
        "Expected operand, got Rt '>' 12:27\n"
        "Expected CREATE, SELECT, INSERT, DELETE or DROP, got Id 'SELEST' "
        "13:1\n"
        "Expected Id, got KwWhere 'WHERE' 14:16\n"
        "Expected Id, got KwWhere 'WHERE' 15:13\n"
        "Expected Semicolon, got Lte '<=' 16:18\n"
        "Expected Id, got KwFrom 'FROM' 17:8\n";

    EXPECT_EQ(expected_result, parser_result);
}

TEST(ParserSuite, InsertStatementTest) {
    const auto parser_result = get_parser_result(
        "INSERT INTO t (n1) VALUES (123);\n"
        "INSERT INTO t (n1, n2, n3) VALUES (12, 0.53, \"str\");\n"
        "INSERT INTO t VALUES (12);\n"
        "INSERT INTO (n1) VALUES (12);\n"
        "INSERT INTO t (n1) VALUES;\n"
        "INSERT INTO t (n1) (12);\n");

    const std::string expected_result =
        "INSERT INTO t (n1) VALUES (123);\n"
        "INSERT INTO t (n1, n2, n3) VALUES (12, 0.530000, \"str\");\n"
        "Expected LParen, got KwValues 'VALUES' 3:15\n"
        "Expected Id, got LParen '(' 4:13\n"
        "Expected LParen, got Semicolon ';' 5:26\n"
        "Expected KwValues, got LParen '(' 6:20\n";

    EXPECT_EQ(expected_result, parser_result);
}

TEST(ParserSuite, DeleteStatementTest) {
    const auto parser_result = get_parser_result(
        "DELETE FROM t;\n"
        "DELETE FROM t WHERE \"asd\" = id;\n"
        "DELETE FROM t WHERE \"asd\" != 123;\n"
        "DELETE FROM t WHERE id <= 123;\n"
        "DELETE FROM t WHERE 123.2 >= 123;\n"
        "DELETE FROM t WHERE \"asd\" < 123;\n"
        "DELETE FROM t WHERE id > id2;\n"
        "DELETE FROM t WHERE;\n"
        "DELETE FROM t WHERE dd <=;\n"
        "DELETE FROM t WHERE <= ddd;\n"
        "DELETE FROM WHERE <= ddd;\n");

    const std::string expected_result =
        "DELETE FROM t;\n"
        "DELETE FROM t WHERE \"asd\" = id;\n"
        "DELETE FROM t WHERE \"asd\" != 123;\n"
        "DELETE FROM t WHERE id <= 123;\n"
        "DELETE FROM t WHERE 123.199997 >= 123;\n"
        "DELETE FROM t WHERE \"asd\" < 123;\n"
        "DELETE FROM t WHERE id > id2;\n"
        "Expected operand, got Semicolon ';' 8:20\n"
        "Expected operand, got Semicolon ';' 9:26\n"
        "Expected operand, got Lte '<=' 10:21\n"
        "Expected Id, got KwWhere 'WHERE' 11:13\n";

    EXPECT_EQ(expected_result, parser_result);
}

TEST(ParserSuite, DropTableStatementTest) {
    const auto parser_result = get_parser_result(
        "DROP TABLE table;\n"
        "DROP TABLE table2\n;"
        "DROP TABLE table\n"
        "someword; DROP TABLE table3;\n"
        "word DROP TABLE table4;\n"
        "DROP table TABLE;\n"
        "DROP TABLE 123;\n"
        "DROP;\n"
        "DROP TABLE;\n"
        "TABLE;\n");

    const std::string expected_result =
        "DROP TABLE table;\n"
        "DROP TABLE table2;\n"
        "DROP TABLE table3;\n"
        "Expected Semicolon, got Id 'someword' 4:1\n"
        "Expected CREATE, SELECT, INSERT, DELETE or DROP, got Id 'word' 5:1\n"
        "Expected KwTable, got Id 'table' 6:6\n"
        "Expected Id, got Int '123' 7:12\n"
        "Expected KwTable, got Semicolon ';' 8:5\n"
        "Expected Id, got Semicolon ';' 9:11\n"
        "Expected CREATE, SELECT, INSERT, DELETE or DROP, got KwTable 'TABLE' "
        "10:1\n";
    EXPECT_EQ(expected_result, parser_result);
}
