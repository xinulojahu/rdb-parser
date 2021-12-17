#include <librdb/parser/Statements.hpp>

#include <sstream>
#include <string>
#include <string_view>

namespace rdb::parser {

Statement::~Statement() = default;

static std::string value_to_str(const Value& value) {
    std::stringstream out;
    if (const int32_t* pval = std::get_if<int32_t>(&value)) {
        return std::to_string(*pval);
    }
    if (const float* pval = std::get_if<float>(&value)) {
        return std::to_string(*pval);
    }
    return std::string(*std::get_if<std::string_view>(&value));
}

std::string DropTableStatement::to_string() const {
    std::stringstream out;
    out << "DROP TABLE " << table_name() << ";";
    return out.str();
}

std::string InsertStatement::to_string() const {
    std::stringstream out;
    auto column_name = column_names().begin();
    out << "INSERT INTO " << table_name() << " (" << *column_name;
    for (column_name++; column_name != column_names().end(); column_name++) {
        out << ", " << *column_name;
    }
    auto value = values().begin();
    out << ") VALUES (" << value_to_str(*value);
    for (value++; value != values().end(); value++) {
        out << ", " << value_to_str(*value);
    }
    out << ");";

    return out.str();
}

}  // namespace rdb::parser
