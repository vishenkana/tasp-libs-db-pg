#include "connection_impl.hpp"

#include <experimental/filesystem>
#include <sstream>

#include <tasp/logging.hpp>

#include "authentication.hpp"

using std::any;
using std::any_cast;
using std::make_unique;
using std::string;
using std::string_view;
using std::stringstream;
using std::to_string;
using std::type_index;
using std::unique_ptr;
using std::vector;

namespace fs = std::experimental::filesystem;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    ConnectionImpl
------------------------------------------------------------------------------*/
ConnectionImpl::ConnectionImpl(string_view name) noexcept
: uri_(auth::Manager::Instance().Uri(name))
, conn_(PQconnectdb(uri_.c_str()), PQfinish)
{
    Logging::Debug("Подключение к БД: {}", uri_);
    if (!Status())
    {
        Logging::Error("Ошибка при подключении к БД: {}",
                       PQerrorMessage(conn_.get()));
    }
}

//------------------------------------------------------------------------------
ConnectionImpl::~ConnectionImpl() noexcept
{
    if (Status())
    {
        Logging::Debug("Отключение от БД");
    }
}

//------------------------------------------------------------------------------
bool ConnectionImpl::Status() const noexcept
{
    return PQstatus(conn_.get()) == CONNECTION_OK;
}

//------------------------------------------------------------------------------
unique_ptr<ResultImpl> ConnectionImpl::Exec(
    string_view query,
    const vector<any> &params) const noexcept
{
    if (!Status())
    {
        Logging::Error("Нет подключения к БД, нельзя выполнить запрос. "
                       "Выполняется попытка переподключения к БД.");
        if (!Reconnect())
        {
            return make_unique<ResultImpl>(nullptr);
        }
    }

    const string format_pattern{"{}"};
    string sql{query};

    for (const auto &value : params)
    {
        const auto visitor{any_visitor_.find(type_index(value.type()))};
        if (visitor == any_visitor_.cend())
        {
            Logging::Error("Неизвестный тип данных: {}", value.type().name());
            return make_unique<ResultImpl>(nullptr);
        }

        auto pos = sql.find(format_pattern);
        if (pos == string::npos)
        {
            break;
        }

        sql.replace(pos, format_pattern.length(), visitor->second(value));
    }

    Logging::Debug("Выполняется запрос к БД: {}", sql);
    return make_unique<ResultImpl>(PQexec(conn_.get(), sql.c_str()));
}

//------------------------------------------------------------------------------
unique_ptr<TransactionImpl> ConnectionImpl::BeginTransaction() const noexcept
{
    return make_unique<TransactionImpl>(shared_from_this());
}

//------------------------------------------------------------------------------
bool ConnectionImpl::Reconnect() const noexcept
{
    PQreset(conn_.get());
    if (!Status())
    {
        Logging::Error("Ошибка переподключения к БД: {}",
                       PQerrorMessage(conn_.get()));

        return false;
    }

    return true;
}

/*------------------------------------------------------------------------------
    VisitorList
------------------------------------------------------------------------------*/
template<class Type, class Func>
static inline VisitorList::value_type ToAnyVisitor(const Func &func) noexcept
{
    return {type_index{typeid(Type)},
            [func](const any &value) -> string
            {
                return func(any_cast<const Type &>(value));
            }};
}

//------------------------------------------------------------------------------
template<class Type>
static inline string ConvertToString(const Type &value) noexcept
{
    return string{value};
}

//------------------------------------------------------------------------------
template<class Type>
static inline string ConvertByStream(const Type &value) noexcept
{
    stringstream stream{};
    stream.flush();
    stream << value;
    return stream.str();
}

//------------------------------------------------------------------------------
static inline string ConvertByBool(const bool &value) noexcept
{
    return value ? "t" : "f";
}

//------------------------------------------------------------------------------
static inline string ConvertByJsonValue(const Json::Value &value) noexcept
{
    return value.asString();
}

//------------------------------------------------------------------------------
static inline VisitorList VisitorInitialization() noexcept
{
    VisitorList list = {
        ToAnyVisitor<int>(static_cast<string (&)(int)>(to_string)),
        ToAnyVisitor<unsigned>(static_cast<string (&)(unsigned)>(to_string)),
        ToAnyVisitor<float>(static_cast<string (&)(float)>(to_string)),
        ToAnyVisitor<double>(static_cast<string (&)(double)>(to_string)),
        ToAnyVisitor<size_t>(static_cast<string (&)(size_t)>(to_string)),
        ToAnyVisitor<uint16_t>(ConvertByStream<uint16_t>),
        ToAnyVisitor<int64_t>(ConvertByStream<int64_t>),
        ToAnyVisitor<char *>(ConvertToString<char *>),
        ToAnyVisitor<char const *>(ConvertToString<char const *>),
        ToAnyVisitor<string>(ConvertToString<string>),
        ToAnyVisitor<string_view>(ConvertToString<string_view>),
        ToAnyVisitor<fs::path>(ConvertToString<fs::path>),
        ToAnyVisitor<bool>(ConvertByBool),
        ToAnyVisitor<Json::Value>(ConvertByJsonValue),
    };
    return list;
}

//------------------------------------------------------------------------------
const VisitorList ConnectionImpl::any_visitor_{VisitorInitialization()};

}  // namespace tasp::db::pg
