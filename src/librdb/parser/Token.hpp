#pragma once

#include <librdb/parser/Location.hpp>

#include <ostream>
#include <string_view>

namespace rdb::parser {

class Token {
   public:
    enum class Kind {
        KwSelect,
        KwFrom,
        KwDrop,
        KwTable,
        KwInsert,
        KwInto,
        KwValues,
        KwDelete,
        KwWhere,
        KwCreate,
        KwInt,
        KwReal,
        KwText,
        Semicolon,
        Comma,
        LParen,
        RParen,
        Lte,
        Rte,
        Neq,
        Lt,
        Rt,
        Eq,
        Id,
        Int,
        Real,
        Text,
        Eof,
        Unknown,
    };
    Token(Kind type, std::string_view lexeme, Location location)
        : type_(type), lexeme_(lexeme), location_(location) {}

    Kind type() const;
    std::string_view lexeme() const;
    Location location() const;

   private:
    Kind type_;
    std::string_view lexeme_;
    Location location_;
};

std::string_view kind_to_str(const Token::Kind& kind);

std::ostream& operator<<(std::ostream& os, const Token& token);

}  // namespace rdb::parser
