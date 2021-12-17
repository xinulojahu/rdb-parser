#include <librdb/parser/Token.hpp>

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

std::string_view kind_to_str(const Token::Kind& token_kind) {
    switch (token_kind) {
        case Token::Kind::KwSelect:
            return "KwSelect";
        case Token::Kind::KwFrom:
            return "KwFrom";
        case Token::Kind::KwDrop:
            return "KwDrop";
        case Token::Kind::KwTable:
            return "KwTable";
        case Token::Kind::KwInsert:
            return "KwInsert";
        case Token::Kind::KwInto:
            return "KwInto";
        case Token::Kind::KwValues:
            return "KwValues";
        case Token::Kind::Semicolon:
            return "Semicolon";
        case Token::Kind::Comma:
            return "Comma";
        case Token::Kind::LParen:
            return "LParen";
        case Token::Kind::RParen:
            return "RParen";
        case Token::Kind::Id:
            return "Id";
        case Token::Kind::Int:
            return "Int";
        case Token::Kind::Real:
            return "Real";
        case Token::Kind::String:
            return "String";
        case Token::Kind::Eof:
            return "Eof";
        case Token::Kind::Unknown:
            return "Unknown";
    }
    return "Unexpected";
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << kind_to_str(token.type()) << " '" << token.lexema() << "' "
       << "Loc=" << token.location().cols_ + 1 << ":"
       << token.location().rows_ + 1;
    return os;
}

}  // namespace rdb::parser
