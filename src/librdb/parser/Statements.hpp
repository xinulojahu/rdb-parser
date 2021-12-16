#pragma once

#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace rdb::parser {

using Value = std::variant<int, float, std::string_view>;

class Statement {
   public:
    virtual ~Statement() = 0;
};

using StatementPtr = std::unique_ptr<const Statement>;

class DropTablesStatement : public Statement {
   public:
    explicit DropTablesStatement(std::string_view table_name)
        : table_name_(table_name) {}
    std::string_view table_name() const { return table_name_; }

   private:
    std::string_view table_name_;
};

using DropTablesStatementPtr = std::unique_ptr<const DropTablesStatement>;

class InsertStatement : public Statement {
   public:
    InsertStatement(
        const std::string_view table_name,
        const std::vector<std::string_view>& column_names,
        const std::vector<Value>& values)
        : table_name_(table_name),
          column_names_(column_names),
          values_(values) {}
    std::string_view table_name() const { return table_name_; }
    std::vector<std::string_view>& column_names() { return column_names_; }
    std::vector<Value>& values() { return values_; }

   private:
    std::string_view table_name_;
    std::vector<std::string_view> column_names_;
    std::vector<Value> values_;
};

using InsertStatementPtr = std::unique_ptr<const InsertStatement>;

}  // namespace rdb::parser
