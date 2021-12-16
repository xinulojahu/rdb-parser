#include <gtest/gtest.h>
#include <librdb/parser/Lexer.hpp>
#include <sstream>
#include <string>
#include <string_view>

TEST(LexerSuite, LexerTest) {
    EXPECT_EQ(40 + 2, 42);
}

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
        "Id 'b' Loc=3:1\n"
        "Id 'b' Loc=3:1\n"
        "Id 'c' Loc=5:1\n"
        "Id 'c' Loc=5:1\n"
        "Id 'c' Loc=5:1\n"
        "Id 'd' Loc=2:2\n"
        "Id 'e' Loc=2:4\n";

    EXPECT_EQ(expcted_token, tokens.str());
}

TEST(LexerSuite, IdTest) {
    auto tokens = get_tokens("a b2 aselect $");

    const std::string expcted_token =
        "Id 'a' Loc=1:1\n"
        "Id 'b2' Loc=3:1\n"
        "Id 'aselect' Loc=6:1\n"
        "Unknown '$' Loc=14:1\n"
        "Eof '<EOF>' Loc=15:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, KwTest) {
    auto tokens = get_tokens("SELECT value FROM table");

    const std::string expcted_token =
        "KwSelect 'SELECT' Loc=1:1\n"
        "Id 'value' Loc=8:1\n"
        "KwFrom 'FROM' Loc=14:1\n"
        "Id 'table' Loc=19:1\n"
        "Eof '<EOF>' Loc=24:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, IntTest) {
    auto tokens = get_tokens("123 -312 +0123 -k");

    const std::string expcted_token =
        "Int '123' Loc=1:1\n"
        "Int '-312' Loc=5:1\n"
        "Int '+0' Loc=10:1\n"
        "Int '123' Loc=12:1\n"
        "Unknown '-' Loc=16:1\n"
        "Id 'k' Loc=17:1\n"
        "Eof '<EOF>' Loc=18:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, RealTest) {
    auto tokens = get_tokens("1. -312.123l2.1 +0.0123 3.5.31.3");

    const std::string expcted_token =
        "Real '1.' Loc=1:1\n"
        "Real '-312.123' Loc=4:1\n"
        "Id 'l2' Loc=12:1\n"
        "Unknown '.' Loc=14:1\n"
        "Int '1' Loc=15:1\n"
        "Real '+0.0123' Loc=17:1\n"
        "Real '3.5' Loc=25:1\n"
        "Unknown '.' Loc=28:1\n"
        "Real '31.3' Loc=29:1\n"
        "Eof '<EOF>' Loc=33:1\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, StringTest) {
    auto tokens = get_tokens(
        "\"str1\"\n"
        "\"str2\n"
        "str3\"\n"
        "\"str4 1231 INSERT 2.1\" 1231 ISERT 2.1"

    );

    const std::string expcted_token =
        "String '\"str1\"' Loc=1:1\n"
        "Unknown '\"str2' Loc=1:2\n"
        "Id 'str3' Loc=1:3\n"
        "Unknown '\"' Loc=5:3\n"
        "String '\"str4 1231 INSERT 2.1\"' Loc=1:4\n"
        "Int '1231' Loc=24:4\n"
        "Id 'ISERT' Loc=29:4\n"
        "Real '2.1' Loc=35:4\n"
        "Eof '<EOF>' Loc=38:4\n";

    EXPECT_EQ(expcted_token, tokens);
}

