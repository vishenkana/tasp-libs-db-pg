#include "authentication.hpp"

#include <vector>

using std::make_unique;
using std::string;
using std::string_view;
using std::vector;

#include <tasp/config.hpp>
#include <tasp/logging.hpp>

using namespace std::literals::string_literals;

namespace tasp::db::pg::auth
{

/*------------------------------------------------------------------------------
    Manager
------------------------------------------------------------------------------*/
Manager &Manager::Instance() noexcept
{
    static Manager instance{};
    return instance;
}

//------------------------------------------------------------------------------
const string &Manager::Uri(string_view name) const noexcept
{
    if (name.empty())
    {
        name = ConfigGlobal::Instance().Get<string>("database.main");
    }

    auto connection = connections_.find(name.data());
    if (connection != connections_.end())
    {
        return connection->second->ConnectionString();
    }

    Logging::Error("Отсутствует данные для подключение к БД: {}.", name);

    static const string empty_string;
    return empty_string;
}

//------------------------------------------------------------------------------
void Manager::Reload() noexcept
{
    auto &conf = ConfigGlobal::Instance();

    const string db_path{"database.connections."};
    auto connections = conf.Get<vector<string>>(db_path, {});
    for (const auto &connection : connections)
    {
        auto path = db_path + connection;
        auto type = conf.Get<string>(path + ".type");

        auto auth = auth_creator_[type];
        if (auth == nullptr)
        {
            Logging::Error("Неизвестный тип подключения к БД: {}.", type);
            continue;
        }

        connections_.try_emplace(connection, auth(path));
    }
}

//------------------------------------------------------------------------------
Manager::Manager() noexcept
{
    auth_creator_["md5"] = [](string_view name)
    {
        return make_unique<auth::Md5>(name);
    };

    auth_creator_["gss"] = [](string_view name)
    {
        return make_unique<auth::Gss>(name);
    };

    auth_creator_["uri"] = [](string_view name)
    {
        return make_unique<auth::Uri>(name);
    };

    Reload();
}

//------------------------------------------------------------------------------
Manager::~Manager() noexcept = default;

/*------------------------------------------------------------------------------
    BaseConnection
------------------------------------------------------------------------------*/
BaseConnection::BaseConnection(string_view path) noexcept
: config_(string(path) + ".")
{
    ReplaceParam("host", "127.0.0.1");
    ReplaceParam("port", "5432");
    ReplaceParam("db", "ta");

    AddParams(ConfigGlobal::Instance().Get<string>("program.name"));
}

//------------------------------------------------------------------------------
BaseConnection::~BaseConnection() noexcept = default;

//------------------------------------------------------------------------------
const string &BaseConnection::ConnectionString() const noexcept
{
    return uri_;
}

//------------------------------------------------------------------------------
void BaseConnection::ReplaceParam(string_view param,
                                  string_view default_value) noexcept
{
    auto &conf = ConfigGlobal::Instance();

    auto value = conf.Get(config_ + param.data(), string(default_value));

    const string format_pattern{"{}"};
    auto pos = uri_.find(format_pattern);
    if (pos != string::npos)
    {
        uri_.replace(pos, format_pattern.length(), value);
    }
}

//------------------------------------------------------------------------------
void BaseConnection::AddParams(string_view params) noexcept
{
    uri_ += params.data();
}

/*------------------------------------------------------------------------------
    Md5
------------------------------------------------------------------------------*/
Md5::Md5(string_view path) noexcept
: BaseConnection(path)
{
    AddParams("&user={}&password={}");
    ReplaceParam("user", "ta");
    ReplaceParam("password", "12345678");
}

//------------------------------------------------------------------------------
Md5::~Md5() noexcept = default;

/*------------------------------------------------------------------------------
    Gss
------------------------------------------------------------------------------*/
Gss::Gss(string_view path) noexcept
: BaseConnection(path)
{
    AddParams("&krbsrvname={}");
    ReplaceParam("krbsrvname", "postgres");

    if (!ConfigGlobal::Instance().Get<string>(string(path) + ".user").empty())
    {
        AddParams("&user={}");
        ReplaceParam("user");
    }
}

//------------------------------------------------------------------------------
Gss::~Gss() noexcept = default;

/*------------------------------------------------------------------------------
    Uri
------------------------------------------------------------------------------*/
Uri::Uri(string_view path) noexcept
: BaseConnection(path)
{
    uri_ = ConfigGlobal::Instance().Get(string(path) + ".uri", uri_);
}

//------------------------------------------------------------------------------
Uri::~Uri() noexcept = default;

//------------------------------------------------------------------------------
const string &Uri::ConnectionString() const noexcept
{
    return uri_;
}

}  // namespace tasp::db::pg::auth
