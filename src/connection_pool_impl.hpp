/**
 * @file
 * @brief Реализация интерфейсов для работы с пулом подключений к СУБД
 * PostgreSQL.
 */
#ifndef TASP_CONNECTION_POOL_IMPL_HPP_
#define TASP_CONNECTION_POOL_IMPL_HPP_

#include <memory>
#include <mutex>

#include "connection_impl.hpp"

namespace tasp::db::pg
{

/**
 * @brief Реализация интерфейса пула подключений к СУБД PostgreSQL.
 */
class ConnectionPoolImpl final
{
public:
    /**
     * @brief Конструктор.
     */
    ConnectionPoolImpl() noexcept;

    /**
     * @brief Деструктор.
     */
    ~ConnectionPoolImpl() noexcept;

    /**
     * @brief Запрос свободного подключения к СУБД PostgreSQL из пула.
     *
     * @return Указатель на подключение к СУБД PostgreSQL
     */
    [[nodiscard]] std::shared_ptr<ConnectionImpl> GetConnection() noexcept;

    ConnectionPoolImpl(const ConnectionPoolImpl &) = delete;
    ConnectionPoolImpl(ConnectionPoolImpl &&) = delete;
    ConnectionPoolImpl &operator=(const ConnectionPoolImpl &) = delete;
    ConnectionPoolImpl &operator=(ConnectionPoolImpl &&) = delete;

private:
    /**
     * @brief Максимальное количество подключений к СУБД в пуле.
     */
    size_t max_;

    /**
     * @brief Таймаут ожидания свободного подключения к СУБД.
     */
    int timeout_;

    /**
     * @brief Количество попыток ожидания подключения к СУБД.
     */
    int retry_;

    /**
     * @brief Пул подключений к СУБД.
     */
    std::vector<std::shared_ptr<ConnectionImpl>> connections_{};

    /**
     * @brief Мьютекс для синхронизации чтения/записи и перезагрузки логов.
     */
    std::mutex mutex_{};
};

}  // namespace tasp::db::pg

#endif  // TASP_CONNECTION_POOL_IMPL_HPP_
