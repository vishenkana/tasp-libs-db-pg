/**
 * @file
 * @brief Интерфейсы для работы с транзакциями СУБД PostgreSQL.
 */
#ifndef TASP_DB_PG_TRANSACTION_HPP_
#define TASP_DB_PG_TRANSACTION_HPP_

#include <memory>

namespace tasp::db::pg
{

class TransactionImpl;

/**
 * @brief Интерфейс работы с транзакциями СУБД PostgreSQL.
 *
 * В деструкторе автоматически вызывается Commit, если не был вызван да этого
 * Rollback.
 *
 * Класс скрывает от пользователя реализацию с помощью идиомы PIMPL
 * (Pointer to Implementation – указатель на реализацию).
 */
class [[gnu::visibility("default")]] Transaction final
{
public:
    /**
     * @brief Конструктор.
     *
     * @param impl Указатель на реализацию
     */
    explicit Transaction(std::unique_ptr<TransactionImpl> impl) noexcept;

    /**
     * @brief Деструктор.
     *
     * Автоматически вызывается Commit, если не был вызван да этого Rollback.
     */
    ~Transaction() noexcept;

    /**
     * @brief Фиксация изменений в транзакции.
     */
    void Commit() const noexcept;

    /**
     * @brief Откат изменений в транзакции.
     */
    void Rollback() const noexcept;

    Transaction(const Transaction &) = delete;
    Transaction(Transaction &&) = delete;
    Transaction &operator=(const Transaction &) = delete;
    Transaction &operator=(Transaction &&) = delete;

private:
    /**
     * @brief Указатель на реализацию.
     */
    std::unique_ptr<TransactionImpl> impl_;
};

}  // namespace tasp::db::pg

#endif  // TASP_DB_PG_TRANSACTION_HPP_
