#include <librdb/parser/Lexer.hpp>

#include <librdb/parser/Token.hpp>

#include <cassert>
#include <cctype>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace rdb::parser {

Token Lexer::get() {
    if (next_token_) {
        Token token(*next_token_);
        next_token_.reset();
        return token;
    }

    skip_spaces();

    if (eof()) {
        return Token(Token::Kind::Eof, "<EOF>", location_);
    }

    const auto next_char = peek_char();

    static const std::unordered_map<char, Token::Kind> trivial_token_to_kind = {
        {';', Token::Kind::Semicolon},
        {',', Token::Kind::Comma},
        {'(', Token::Kind::LParen},
        {')', Token::Kind::RParen},
    };

    const auto it = trivial_token_to_kind.find(next_char);
    if (it != trivial_token_to_kind.end()) {
        const Location begin(location_);
        get_char();
        return make_token(it->second, begin);
    }

    if (isalpha(next_char) != 0) {
        return get_id_or_kw();
    }

    if (isdigit(next_char) != 0) {
        return get_number();
    }

    switch (next_char) {
        case '-':
        case '+':
            return get_number();
        case '"':
            return get_string();
        case '<':
        case '>':
        case '=':
        case '!':
            return get_operation();
        default:
            break;
    }

    const Location begin(location_);
    get_char();
    return make_token(Token::Kind::Unknown, begin);
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
        location_.cols_ = 1;
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
            {"DROP", Token::Kind::KwDrop},
            {"TABLE", Token::Kind::KwTable},
            {"INSERT", Token::Kind::KwInsert},
            {"INTO", Token::Kind::KwInto},
            {"VALUES", Token::Kind::KwValues},
            {"DELETE", Token::Kind::KwDelete},
            {"WHERE", Token::Kind::KwWhere},
            {"CREATE", Token::Kind::KwCreate},
            {"INT", Token::Kind::KwInt},
            {"REAL", Token::Kind::KwReal},
            {"TEXT", Token::Kind::KwText},
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
            return make_token(Token::Kind::Unknown, begin);
        }
    }

    if (peek_char() == '0') {
        get_char();
    } else {
        while ((!eof()) && (isdigit(peek_char()) != 0)) {
            get_char();
        }
    }

    if (peek_char() == '.') {
        get_char();
        while ((!eof()) && (isdigit(peek_char()) != 0)) {
            get_char();
        }
        return make_token(Token::Kind::Real, begin);
    }

    return make_token(Token::Kind::Int, begin);
}

Token Lexer::get_string() {
    const Location begin(location_);

    get_char();

    while ((!eof()) && (peek_char() != '"') && (peek_char() != '\n')) {
        get_char();
    }

    if (peek_char() == '"') {
        get_char();
        return make_token(Token::Kind::Text, begin);
    }

    return make_token(Token::Kind::Unknown, begin);
}

Token Lexer::get_operation() {
    const Location begin(location_);

    char cur_char = get_char();

    if (cur_char == '=') {
        return make_token(Token::Kind::Eq, begin);
    }

    if (peek_char() == '=') {
        get_char();
        switch (cur_char) {
            case '<':
                return make_token(Token::Kind::Lte, begin);
            case '>':
                return make_token(Token::Kind::Rte, begin);
            case '!':
                return make_token(Token::Kind::Neq, begin);
            default:
                assert(false && "cur_char != < | > | !");
        }
    }
    switch (cur_char) {
        case '<':
            return make_token(Token::Kind::Lt, begin);
        case '>':
            return make_token(Token::Kind::Rt, begin);
        case '!':
            return make_token(Token::Kind::Unknown, begin);
        default:
            assert(false && "cur_char != < | > | !");
    }
}

Token Lexer::make_token(Token::Kind kind, const Location& begin) const {
    const auto length = location_.offset_ - begin.offset_;
    const auto text = input_.substr(begin.offset_, length);
    return Token(kind, text, begin);
}

}  // namespace rdb::parser
