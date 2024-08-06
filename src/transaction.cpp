#include "tasp/db/pg/transaction.hpp"

#include "transaction_impl.hpp"

using std::unique_ptr;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    Transaction
------------------------------------------------------------------------------*/
Transaction::Transaction(unique_ptr<TransactionImpl> impl) noexcept
: impl_(std::move(impl))
{
}

//------------------------------------------------------------------------------
Transaction::~Transaction() noexcept = default;

//------------------------------------------------------------------------------
void Transaction::Commit() const noexcept
{
    return impl_->Commit();
}

//------------------------------------------------------------------------------
void Transaction::Rollback() const noexcept
{
    return impl_->Rollback();
}

}  // namespace tasp::db::pg
