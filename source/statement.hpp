#if !defined(FUNCDB_STATEMENT_HPP)
#define FUNCDB_STATEMENT_HPP

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "table.hpp"

namespace funcdb
{

    class Statement
    {
    public:

        enum class PrepareError
        {
            UnrecognizedStatement,
            StringTooLong,
            SyntaxError
        };

        static
        std::variant<PrepareError, Statement>
        prepare(std::string const& input);

    public:

        enum class ExecuteResult
        {
            Success,
            KeyAlreadyExists,
            KeyDoesntExist,
        };

        ExecuteResult
        execute(Table& table) const;

    private:

        enum class Type
        {
            Commit,
            Insert,
            Replace,
            Rollback,
            Select,
            SelectAll
        };

        Statement(Type type, 
            std::variant<uint32_t, 
                        std::pair<uint32_t, std::string>, 
                        std::nullptr_t> data = nullptr);

    private:
        static std::string const kCommandInsert;
        static std::string const kCommandSelect;
        static std::string const kCommandReplace;
        static std::string const kCommandCommit;
        static std::string const kCommandRollback;

        Type m_type;

        std::variant<uint32_t, 
                    std::pair<uint32_t, std::string>, 
                    std::nullptr_t> m_data;
    };

} // namespace funcdb

#endif // FUNCDB_STATEMENT_HPP



