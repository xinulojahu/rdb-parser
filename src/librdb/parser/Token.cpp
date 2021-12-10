#include <Token.hpp>

namespace rdb::parser {

Token::Kind Token::type() const {
    return type_;
}

std::string_view Token::lexema() const {
    return lexema_;
}

Location Token::location() const {
    return location_;
}

static std::string_view kind_to_str(Token::Kind token_kind) {
    switch (token_kind) {
        case Token::Kind::Id: {
            return "Id";
        }
        case Token::Kind::KwSelect: {
            return "KwSelect";
        }
        case Token::Kind::KwFrom: {
            return "KwFrom";
        }
        case Token::Kind::Int: {
            return "Int";
        }
        case Token::Kind::Err: {
            return "Err";
        }
        case Token::Kind::Eof: {
            return "Eof";
        }
    }
    return "Unexpected";
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << kind_to_str(token.type()) << " '" << token.lexema() << "' "
       << "Loc=" << token.location().cols_ << ":" << token.location().rows_;
    return os;
}

}  // namespace rdb::parser
