#include <gtest/gtest.h>
#include <Lexer.hpp>
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
        "Id 'a' Loc=0:0\n"
        "Id 'a' Loc=0:0\n"
        "Id 'a' Loc=0:0\n"
        "Id 'a' Loc=0:0\n"
        "Id 'b' Loc=2:0\n"
        "Id 'b' Loc=2:0\n"
        "Id 'c' Loc=4:0\n"
        "Id 'c' Loc=4:0\n"
        "Id 'c' Loc=4:0\n"
        "Id 'd' Loc=1:1\n"
        "Id 'e' Loc=1:3\n";

    EXPECT_EQ(expcted_token, tokens.str());
}

TEST(LexerSuite, IdTest) {
    auto tokens = get_tokens("a b2 aselect $");

    const std::string expcted_token =
        "Id 'a' Loc=0:0\n"
        "Id 'b2' Loc=2:0\n"
        "Id 'aselect' Loc=5:0\n"
        "Err '$' Loc=13:0\n"
        "Eof '<EOF>' Loc=14:0\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, KwTest) {
    auto tokens = get_tokens("SELECT value FROM table");

    const std::string expcted_token =
        "KwSelect 'SELECT' Loc=0:0\n"
        "Id 'value' Loc=7:0\n"
        "KwFrom 'FROM' Loc=13:0\n"
        "Id 'table' Loc=18:0\n"
        "Eof '<EOF>' Loc=23:0\n";

    EXPECT_EQ(expcted_token, tokens);
}

TEST(LexerSuite, IntTest) {
    auto tokens = get_tokens("123 -312 +0123 -k");

    const std::string expcted_token =
        "Int '123' Loc=0:0\n"
        "Int '-312' Loc=4:0\n"
        "Int '+0' Loc=9:0\n"
        "Int '123' Loc=11:0\n"
        "Err '-' Loc=15:0\n"
        "Id 'k' Loc=16:0\n"
        "Eof '<EOF>' Loc=17:0\n";

    EXPECT_EQ(expcted_token, tokens);
}
