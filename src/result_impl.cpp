#include "result_impl.hpp"

#include <string>

#include <tasp/logging.hpp>

using std::make_unique;
using std::string;
using std::string_view;
using std::unique_ptr;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    ResultImpl
------------------------------------------------------------------------------*/
ResultImpl::ResultImpl(PGresult *result) noexcept
: result_(result, PQclear)
{
    if (result == nullptr)
    {
        return;
    }

    if (!Status())
    {
        Logging::Error("Ошибка выполнения запроса: {}",
                       PQresultErrorMessage(result_.get()));
    }
}

//------------------------------------------------------------------------------
ResultImpl::~ResultImpl() noexcept = default;

//------------------------------------------------------------------------------
bool ResultImpl::Status() const noexcept
{
    return (PQresultStatus(result_.get()) == PGRES_TUPLES_OK) ||
           (PQresultStatus(result_.get()) == PGRES_COMMAND_OK);
}

//------------------------------------------------------------------------------
int ResultImpl::Rows() const noexcept
{
    return PQntuples(result_.get());
}

//------------------------------------------------------------------------------
int ResultImpl::Columns() const noexcept
{
    return PQnfields(result_.get());
}

//------------------------------------------------------------------------------
string ResultImpl::Value(int row, int column) const noexcept
{
    if (row >= Rows())
    {
        Logging::Error("Запрашивается строка: {} всего строк: {}", row, Rows());
    }

    return PQgetvalue(result_.get(), row, column);
}

//------------------------------------------------------------------------------
string ResultImpl::Value(int row, string_view name) const noexcept
{
    const int column = PQfnumber(result_.get(), name.data());
    if (column == -1)
    {
        Logging::Error("Отсутствует колонка: {}", name);
        return "";
    }

    return Value(row, column);
}

//------------------------------------------------------------------------------
Json::Value ResultImpl::ValueArray(int row, int column) const noexcept
{
    auto array = Value(row, column);

    Json::Value root;

    size_t current{1};
    size_t sep{0};
    while ((sep = array.find_first_of(",}", current)) != string::npos)
    {
        auto value = array.substr(current, sep - current);
        root.append(value);
        current = sep + 1;
    }

    return root;
}

//------------------------------------------------------------------------------
Json::Value ResultImpl::ValueBoolean(int row, int column) const noexcept
{
    return Value(row, column) == "t";
}

//------------------------------------------------------------------------------
Json::Value ResultImpl::ValueInt(int row, int column) const noexcept
{
    return std::stoi(Value(row, column));
}

//------------------------------------------------------------------------------
Json::Value ResultImpl::ConvertValue(int row, int column) const noexcept
{
    switch (PQftype(result_.get(), column))
    {
        case 16:
            return ValueBoolean(row, column);
        case 21:
            return ValueInt(row, column);
        case 1009:
            return ValueArray(row, column);
        default:
            return Value(row, column);
    }
}

//------------------------------------------------------------------------------
Json::Value ResultImpl::JsonValue() const noexcept
{
    Json::Value root;
    root["count"] = Rows();
    root["data"] = Json::arrayValue;

    for (auto row = 0; row < Rows(); ++row)
    {
        Json::Value tuple;
        for (auto column = 0; column < Columns(); ++column)
        {
            auto *key = PQfname(result_.get(), column);
            tuple[key] = ConvertValue(row, column);
        }
        root["data"].append(tuple);
    }

    return root;
}

//------------------------------------------------------------------------------
unique_ptr<ResultIteratorImpl> ResultImpl::begin() const
{
    return make_unique<ResultIteratorImpl>(this);
}

//------------------------------------------------------------------------------
unique_ptr<ResultIteratorImpl> ResultImpl::end() const
{
    return make_unique<ResultIteratorImpl>(this, Rows());
}

/*------------------------------------------------------------------------------
    ResultIteratorImpl
------------------------------------------------------------------------------*/
ResultIteratorImpl::ResultIteratorImpl(const ResultImpl *result,
                                       int row) noexcept
: result_(result)
, row_(row)
{
}

//------------------------------------------------------------------------------
string ResultIteratorImpl::Value(string_view name) const noexcept
{
    return result_->Value(row_, name);
}

//------------------------------------------------------------------------------
ResultIteratorImpl &ResultIteratorImpl::operator++() noexcept
{
    row_++;
    return *this;
}

//------------------------------------------------------------------------------
bool ResultIteratorImpl::operator!=(const ResultIteratorImpl &rhs) noexcept
{
    return (rhs.row_ != row_) || (rhs.result_ != result_);
}

}  // namespace tasp::db::pg
