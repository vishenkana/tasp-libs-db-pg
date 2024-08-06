#include "transaction_impl.hpp"

#include <tasp/logging.hpp>

#include "connection_impl.hpp"

using std::shared_ptr;
using std::string_view;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    TransactionImpl
------------------------------------------------------------------------------*/
TransactionImpl::TransactionImpl(
    shared_ptr<const ConnectionImpl> connection) noexcept
: connection_(std::move(connection))
{
    Exec("BEGIN", Status::Begin, "Старт транзакции");
}

//------------------------------------------------------------------------------
TransactionImpl::~TransactionImpl() noexcept
{
    if (status_ < Status::Commit)
    {
        Commit();
    }
}

//------------------------------------------------------------------------------
void TransactionImpl::Commit() noexcept
{
    Exec("COMMIT", Status::Commit, "Фиксация транзакции");
}

//------------------------------------------------------------------------------
void TransactionImpl::Rollback() noexcept
{
    Exec("ROLLBACK", Status::Rollback, "Откат транзакции");
}

//------------------------------------------------------------------------------
void TransactionImpl::Exec(string_view command,
                           Status status,
                           string_view message) noexcept
{
    Logging::Debug("{}", message);
    auto res = connection_->Exec(command);
    if (res->Status())
    {
        status_ = status;
    }
}

}  // namespace tasp::db::pg
