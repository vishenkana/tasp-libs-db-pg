#include "tasp/db/pg/result.hpp"

#include "result_impl.hpp"

using std::string;
using std::string_view;
using std::unique_ptr;

namespace tasp::db::pg
{

/*------------------------------------------------------------------------------
    Result::Iterator
------------------------------------------------------------------------------*/
Result::Result(unique_ptr<ResultImpl> impl) noexcept
: impl_(std::move(impl))
{
}

//------------------------------------------------------------------------------
Result::~Result() noexcept = default;

//------------------------------------------------------------------------------
bool Result::Status() const noexcept
{
    return impl_->Status();
}

//------------------------------------------------------------------------------
string Result::Value(string_view name) const noexcept
{
    return impl_->Value(0, name);
}

//------------------------------------------------------------------------------
Json::Value Result::JsonValue() const noexcept
{
    return impl_->JsonValue();
}

//------------------------------------------------------------------------------
Result::Iterator Result::begin() const
{
    return Iterator(impl_->begin());
}

//------------------------------------------------------------------------------
Result::Iterator Result::end() const
{
    return Iterator(impl_->end());
}

/*------------------------------------------------------------------------------
    Result
------------------------------------------------------------------------------*/
Result::Iterator::Iterator(unique_ptr<ResultIteratorImpl> impl) noexcept
: impl_(std::move(impl))
{
}

//------------------------------------------------------------------------------
Result::Iterator::~Iterator()
{
    impl_.reset(nullptr);
}

//------------------------------------------------------------------------------
string Result::Iterator::Value(string_view name) const noexcept
{
    return impl_->Value(name);
}

//------------------------------------------------------------------------------
Result::Iterator &Result::Iterator::operator++() noexcept
{
    impl_->operator++();
    return *this;
}

//------------------------------------------------------------------------------
bool Result::Iterator::operator!=(const Result::Iterator &rhs) noexcept
{
    return impl_->operator!=(*rhs.impl_);
}

//------------------------------------------------------------------------------
const Result::Iterator &Result::Iterator::operator*() const noexcept
{
    return *this;
}

}  // namespace tasp::db::pg
