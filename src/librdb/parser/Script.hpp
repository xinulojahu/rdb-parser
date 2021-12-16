#pragma once

#include <librdb/parser/Statements.hpp>

#include <vector>

namespace rdb::parser {

struct Script {
    std::vector<StatementPtr> statements_;
};

}  // namespace rdb::parser
