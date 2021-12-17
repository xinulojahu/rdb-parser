#include <librdb/parser/Statements.hpp>

#include <sstream>

namespace rdb::parser {

Statement::~Statement() = default;

std::string DropTableStatement::to_string() const {
    std::stringstream out;
    out << "DROP TABLE " << table_name();
    return out.str();
}

std::string InsertStatement::to_string() const {
    std::stringstream out;
    out << "nothin'";
    return out.str();
}

}  // namespace rdb::parser
