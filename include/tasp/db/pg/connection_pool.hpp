/**
 * @file
 * @brief Интерфейсы для работы с пулом подключений к СУБД PostgreSQL.
 */
#ifndef TASP_DB_PG_CONNECTION_POOL_HPP_
#define TASP_DB_PG_CONNECTION_POOL_HPP_

#include <memory>

#include <tasp/db/pg/connection.hpp>

namespace tasp::db::pg
{

class ConnectionPoolImpl;

/**
 * @brief Интерфейс пулов подключений к СУБД PostgreSQL.
 *
 * Класс скрывает от пользователя реализацию с помощью идиомы PIMPL
 * (Pointer to Implementation – указатель на реализацию).
 */
class [[gnu::visibility("default")]] ConnectionPool final
{
public:
    /**
     * @brief Запрос ссылки на глобальный пул подключения к СУБД PostgreSQL.
     *
     * @return Ссылка на глобальный пул подключения к СУБД PostgreSQL
     */
    static ConnectionPool &Instance() noexcept;

    /**
     * @brief Запрос свободного подключения к СУБД PostgreSQL из пула.
     *
     * @return Указатель на подключение к СУБД PostgreSQL
     */
    [[nodiscard]] std::unique_ptr<Connection> GetConnection() const noexcept;

    ConnectionPool(const ConnectionPool &) = delete;
    ConnectionPool(ConnectionPool &&) = delete;
    ConnectionPool &operator=(const ConnectionPool &) = delete;
    ConnectionPool &operator=(ConnectionPool &&) = delete;

private:
    /**
     * @brief Конструктор.
     */
    ConnectionPool() noexcept;

    /**
     * @brief Деструктор.
     */
    ~ConnectionPool() noexcept;

    /**
     * @brief Указатель на реализацию.
     */
    std::unique_ptr<ConnectionPoolImpl> impl_;
};

}  // namespace tasp::db::pg

#endif  // TASP_DB_PG_CONNECTION_POOL_HPP_
