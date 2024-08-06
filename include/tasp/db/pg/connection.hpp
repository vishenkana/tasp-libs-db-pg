/**
 * @file
 * @brief Интерфейсы для работы с подключением к СУБД PostgreSQL.
 */
#ifndef TASP_DB_PG_CONNECTION_HPP_
#define TASP_DB_PG_CONNECTION_HPP_

#include <any>
#include <memory>
#include <string_view>
#include <vector>

#include <tasp/db/pg/result.hpp>
#include <tasp/db/pg/transaction.hpp>

namespace tasp::db::pg
{

class ConnectionImpl;

/**
 * @brief Интерфейс подключения к СУБД PostgreSQL.
 *
 * Класс скрывает от пользователя реализацию с помощью идиомы PIMPL
 * (Pointer to Implementation – указатель на реализацию).
 */
class [[gnu::visibility("default")]] Connection final
{
public:
    /**
     * @brief Конструктор.
     *
     * Если необходимо подключать по дефолтному подключению из конфигурационного
     * файла, передавать в конструкторе имя подключения не нужно.
     *
     * @param name Имя подключения к БД из конф. файла
     */
    explicit Connection(std::string_view name = {}) noexcept;

    /**
     * @brief Конструктор.
     *
     * @param impl Указатель на реализацию
     */
    explicit Connection(std::shared_ptr<ConnectionImpl> impl) noexcept;

    /**
     * @brief Деструктор.
     */
    ~Connection() noexcept;

    /**
     * @brief Статус подключения к СУБД.
     *
     * @return Статус
     */
    [[nodiscard]] bool Status() const noexcept;

    /**
     * @brief Выполнение запроса у СУБД с переменным количеством параметров.
     *
     * В запроса можно указать {}. Вместо этого будет подставлено значение из
     * параметров.
     *
     * @param query SQL-запрос
     * @param params Параметры запроса
     *
     * @return Результат выполнения запроса
     */
    template<typename... Args>
    [[nodiscard]] std::unique_ptr<Result> Exec(std::string_view query,
                                               Args && ...params) const noexcept
    {
        return Exec(query, {std::any(std::forward<Args>(params))...});
    }

    /**
     * @brief Выполнение запроса у СУБД.
     *
     * В запроса можно указать {}. Вместо этого будет подставлено значение из
     * параметров.
     *
     * @param query SQL-запрос
     * @param params Параметры запроса
     *
     * @return Результат выполнения запроса
     */
    [[nodiscard]] std::unique_ptr<Result> Exec(
        std::string_view query, const std::vector<std::any> &params)
        const noexcept;

    /**
     * @brief Старт транзакции.
     *
     * @return Указатель на транзакцию
     */
    [[nodiscard]] std::unique_ptr<Transaction> BeginTransaction()
        const noexcept;

    Connection(const Connection &) = delete;
    Connection(Connection &&) = delete;
    Connection &operator=(const Connection &) = delete;
    Connection &operator=(Connection &&) = delete;

private:
    /**
     * @brief Указатель на реализацию.
     */
    std::shared_ptr<ConnectionImpl> impl_;
};

}  // namespace tasp::db::pg

#endif  // TASP_DB_PG_CONNECTION_HPP_
