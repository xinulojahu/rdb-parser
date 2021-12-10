#pragma once

#include <Location.hpp>

#include <ostream>
#include <string_view>

namespace rdb::parser {

class Token {
   public:
    enum class Kind {
        Id,
        KwSelect,
        KwFrom,
        Int,
        Err,
        Eof,
    };
    Token(Kind type, std::string_view lexema, Location location)
        : type_(type), lexema_(lexema), location_(location) {}

    Kind type() const;
    std::string_view lexema() const;
    Location location() const;

   private:
    Kind type_;
    std::string_view lexema_;
    Location location_;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

}  // namespace rdb::parser
