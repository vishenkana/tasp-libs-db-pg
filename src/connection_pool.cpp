#include "tasp/db/pg/connection_pool.hpp"

#include "connection_pool_impl.hpp"

using std::make_unique;
using std::unique_ptr;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    ConnectionPool
------------------------------------------------------------------------------*/
ConnectionPool &ConnectionPool::Instance() noexcept
{
    static ConnectionPool instance{};
    return instance;
}

//------------------------------------------------------------------------------
unique_ptr<Connection> ConnectionPool::GetConnection() const noexcept
{
    return make_unique<Connection>(impl_->GetConnection());
}

//------------------------------------------------------------------------------
ConnectionPool::ConnectionPool() noexcept
: impl_(make_unique<ConnectionPoolImpl>())
{
}

//------------------------------------------------------------------------------
ConnectionPool::~ConnectionPool() noexcept = default;

}  // namespace tasp::db::pg
