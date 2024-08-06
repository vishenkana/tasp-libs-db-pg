#include "connection_pool_impl.hpp"

#include <thread>

#include <tasp/config.hpp>
#include <tasp/logging.hpp>

using std::make_shared;
using std::scoped_lock;
using std::shared_ptr;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    ConnectionPoolImpl
------------------------------------------------------------------------------*/
ConnectionPoolImpl::ConnectionPoolImpl() noexcept
: max_(ConfigGlobal::Instance().Get<size_t>("database.pool.max", 10))
, timeout_(ConfigGlobal::Instance().Get<int>("database.pool.timeout", 2))
, retry_(ConfigGlobal::Instance().Get<int>("database.pool.retry", 3))
{
    connections_.reserve(max_);
    Logging::Debug("Максимальное количество соединений в пуле БД: {}", max_);
}

//------------------------------------------------------------------------------
ConnectionPoolImpl::~ConnectionPoolImpl() noexcept = default;

//------------------------------------------------------------------------------
shared_ptr<ConnectionImpl> ConnectionPoolImpl::GetConnection() noexcept
{
    const scoped_lock lock{mutex_};

    int retry{retry_};
    while ((retry--) != 0)
    {
        int current{0};
        for (auto &&connection : connections_)
        {
            current++;
            if (connection.use_count() == 1)
            {
                Logging::Debug(
                    "Текущее подключение в пуле БД {} из {}", current, max_);
                return connection;
            }
        }

        if (connections_.size() < max_)
        {
            Logging::Debug("Новое подключение в пуле БД {} из {}",
                           connections_.size() + 1,
                           max_);
            return connections_.emplace_back(make_shared<ConnectionImpl>());
        }

        Logging::Warning("Нет свободных подключений к БД, ожидаем {} сек.",
                         timeout_);
        std::this_thread::sleep_for(std::chrono::seconds(timeout_));
    }

    Logging::Error(
        "Нет свободных подключений к БД. Закончился лимит попыток: {}", retry_);

    return {};
}

}  // namespace tasp::db::pg
