#include "tasp/db/pg/connection.hpp"

#include "connection_impl.hpp"

using std::any;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string_view;
using std::unique_ptr;
using std::vector;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    Connection
------------------------------------------------------------------------------*/
Connection::Connection(string_view name) noexcept
: impl_(make_shared<ConnectionImpl>(name))
{
}

//------------------------------------------------------------------------------
Connection::Connection(shared_ptr<ConnectionImpl> impl) noexcept
: impl_(std::move(impl))
{
}

//------------------------------------------------------------------------------
Connection::~Connection() noexcept = default;

//------------------------------------------------------------------------------
unique_ptr<Result> Connection::Exec(string_view query,
                                    const vector<any> &params) const noexcept
{
    return make_unique<Result>(impl_->Exec(query, params));
}

//------------------------------------------------------------------------------
bool Connection::Status() const noexcept
{
    return impl_->Status();
}

//------------------------------------------------------------------------------
unique_ptr<Transaction> Connection::BeginTransaction() const noexcept
{
    return make_unique<Transaction>(impl_->BeginTransaction());
}

}  // namespace tasp::db::pg
