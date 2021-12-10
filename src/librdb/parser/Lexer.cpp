#include <Lexer.hpp>
#include <Token.hpp>
#include <cassert>
#include <cctype>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace rdb::parser {

Token Lexer ::get() {
    if (eof()) {
        return Token(Token::Kind::Eof, "<EOF>", location_);
    }

    if (next_token_) {
        Token token(*next_token_);
        next_token_.reset();
        return token;
    }

    skip_spaces();

    if (isalpha(peek_char()) != 0) {
        return get_id_or_kw();
    }
    if ((peek_char() == '-') || (peek_char() == '+') ||
        (isdigit(peek_char()) != 0)) {
        return get_number();
    }

    Location begin(location_);

    get_char();
    return Token(Token::Kind::Err, input_.substr(begin.offset_, 1), begin);
}

Token Lexer::peek() {
    if (next_token_) {
        return *next_token_;
    }
    next_token_ = get();
    return *next_token_;
}

bool Lexer::eof() const {
    return location_.offset_ == input_.size();
}

char Lexer::peek_char() const {
    assert(!eof());
    return input_[location_.offset_];
}

char Lexer::get_char() {
    assert(!eof());
    if (input_[location_.offset_] == '\n') {
        location_.cols_ = 0;
        ++location_.rows_;
    } else {
        ++location_.cols_;
    }
    return input_[location_.offset_++];
}

void Lexer::skip_spaces() {
    while ((!eof()) && (isspace(peek_char()) != 0)) {
        get_char();
    }
}

Token Lexer::get_id_or_kw() {
    const Location begin(location_);

    while ((!eof()) && (isalnum(peek_char()) != 0)) {
        get_char();
    }
    const Location end(location_);
    const std::string_view text(
        input_.substr(begin.offset_, end.offset_ - begin.offset_));

    static const std::unordered_map<std::string_view, Token::Kind>
        text_to_kind = {
            {"SELECT", Token::Kind::KwSelect},
            {"FROM", Token::Kind::KwFrom},
        };

    auto it = text_to_kind.find(text);

    if (it != text_to_kind.end()) {
        return Token(it->second, text, begin);
    }
    return Token(Token::Kind::Id, text, begin);
}

Token Lexer::get_number() {
    const Location begin(location_);

    if ((peek_char() == '-') || (peek_char() == '+')) {
        get_char();
        if ((eof()) || (isdigit(peek_char()) == 0)) {
            return Token(
                Token::Kind::Err, input_.substr(begin.offset_, 1), begin);
        }
    }

    if (peek_char() == '0') {
        get_char();
    } else {
        while ((!eof()) && (isdigit(peek_char()) != 0)) {
            get_char();
        }
    }

    const Location end(location_);

    const std::string_view text(
        input_.substr(begin.offset_, end.offset_ - begin.offset_));
    return Token(Token::Kind::Int, text, begin);
}

}  // namespace rdb::parser
