#include <librdb/parser/Statements.hpp>

#include <sstream>
#include <string>
#include <string_view>

namespace rdb::parser {

Statement::~Statement() = default;

static std::string value_to_str(const Value& value) {
    if (const int32_t* pval = std::get_if<int32_t>(&value)) {
        return std::to_string(*pval);
    }
    if (const float* pval = std::get_if<float>(&value)) {
        return std::to_string(*pval);
    }
    return std::string(*std::get_if<std::string_view>(&value));
}

static std::string operand_to_str(const Expression::Operand& operand) {
    if (const std::string_view* pval =
            std::get_if<std::string_view>(&operand)) {
        return std::string(*pval);
    }
    return value_to_str(*std::get_if<Value>(&operand));
}

static std::string operation_to_str(const Expression::Operation& operation) {
    switch (operation) {
        case Expression::Operation::Lte:
            return "<=";
        case Expression::Operation::Rte:
            return ">=";
        case Expression::Operation::Neq:
            return "!=";
        case Expression::Operation::Lt:
            return "<";
        case Expression::Operation::Rt:
            return ">";
        case Expression::Operation::Eq:
            return "=";
    }
    return "Unexpected";
}

static std::string expression_to_str(const Expression& expression) {
    return operand_to_str(expression.left_) + " " +
           operation_to_str(expression.operation_) + " " +
           operand_to_str(expression.right_);
}

static std::string column_def_type_to_str(const ColumnDef::Type type) {
    switch (type) {
        case ColumnDef::Type::Int:
            return "INT";
        case ColumnDef::Type::Real:
            return "REAL";
        case ColumnDef::Type::Text:
            return "TEXT";
    }
    return "Unexpected";
}

static std::string column_def_to_str(const ColumnDef& column_def) {
    return std::string(column_def.column_name_) + " " +
           column_def_type_to_str(column_def.type_);
}

std::string CreateTableStatement::to_string() const {
    std::stringstream out;
    auto column_def = column_defs().begin();
    out << "CREATE TABLE " << table_name() << " ("
        << column_def_to_str(*column_def);
    for (column_def++; column_def != column_defs().end(); column_def++) {
        out << ", " << column_def_to_str(*column_def);
    }
    out << ");";

    return out.str();
}

std::string SelectStatement::to_string() const {
    std::stringstream out;
    out << "SELECT ";
    for (auto column_name : column_names()) {
        out << column_name << " ";
    }
    out << "FROM " << table_name();
    if (expression()) {
        out << " WHERE " << expression_to_str(*expression());
    }
    out << ";";

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

std::string DeleteStatement::to_string() const {
    std::stringstream out;
    out << "DELETE FROM " << table_name();
    if (expression()) {
        out << " WHERE " << expression_to_str(*expression());
    }
    out << ";";

    return out.str();
}

std::string DropTableStatement::to_string() const {
    std::stringstream out;
    out << "DROP TABLE " << table_name() << ";";
    return out.str();
}

}  // namespace rdb::parser
