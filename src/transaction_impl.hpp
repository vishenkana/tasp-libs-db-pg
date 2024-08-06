/**
 * @file
 * @brief Реализация интерфейсов для работы с транзакциями СУБД PostgreSQL.
 */
#ifndef TASP_TRANSACTION_IMPL_HPP_
#define TASP_TRANSACTION_IMPL_HPP_

#include <memory>
#include <string_view>

namespace tasp::db::pg
{

class ConnectionImpl;

/**
 * @brief Реализация интерфейса работы с транзакциями СУБД PostgreSQL.
 *
 * В деструкторе автоматически вызывается Commit, если не был вызван да этого
 * Rollback.
 */
class TransactionImpl final
{
public:
    /**
     * @brief Конструктор.
     *
     * @param connection Подключение к БД
     */
    explicit TransactionImpl(
        std::shared_ptr<const ConnectionImpl> connection) noexcept;

    /**
     * @brief Деструктор.
     *
     * Автоматически вызывается Commit, если не был вызван да этого Rollback.
     */
    ~TransactionImpl() noexcept;

    /**
     * @brief Фиксация изменений в транзакции.
     */
    void Commit() noexcept;

    /**
     * @brief Откат изменений в транзакции.
     */
    void Rollback() noexcept;

    TransactionImpl(const TransactionImpl &) = delete;
    TransactionImpl(TransactionImpl &&) = delete;
    TransactionImpl &operator=(const TransactionImpl &) = delete;
    TransactionImpl &operator=(TransactionImpl &&) = delete;

private:
    /**
     * @brief Статусы транзакции.
     */
    enum class Status
    {
        None = 0,     /*!< Транзакция не начата */
        Begin = 1,    /*!< Транзакция начата */
        Commit = 2,   /*!< Транзакция зафиксирована */
        Rollback = 3, /*!< Транзакция отменена */
    };

    /**
     * @brief Исполнение SQL-команды и установка статуса транзакции.
     *
     * Статус изменяется только при успешном выполнении запроса.
     *
     * @param command SQL-команда
     * @param status Новый статус транзакции
     * @param message Сообщении для вывода в лог
     */
    void Exec(std::string_view command,
              Status status,
              std::string_view message) noexcept;

    /**
     * @brief Текущий статус транзакции.
     */
    Status status_{Status::None};

    /**
     * @brief Подключение к БД.
     */
    std::shared_ptr<const ConnectionImpl> connection_;
};

}  // namespace tasp::db::pg

#endif  // TASP_TRANSACTION_IMPL_HPP_
