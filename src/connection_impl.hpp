/**
 * @file
 * @brief Реализация интерфейсов для подключения к СУБД PostgreSQL.
 */
#ifndef TASP_CONNECTION_IMPL_HPP_
#define TASP_CONNECTION_IMPL_HPP_

#include <postgresql/libpq-fe.h>

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "result_impl.hpp"
#include "transaction_impl.hpp"

namespace tasp::db::pg
{

/**
 * @brief Тип данных для списка типов данных поддерживаемых для формирования
 * запроса с функциями преобразования их в текстовое представление
 */
using VisitorList =
    std::unordered_map<std::type_index,
                       std::function<std::string(const std::any &)>>;

/**
 * @brief Реализация интерфейса подключения к СУБД PostgreSQL.
 */
class ConnectionImpl final : public std::enable_shared_from_this<ConnectionImpl>
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
    explicit ConnectionImpl(std::string_view name = {}) noexcept;

    /**
     * @brief Деструктор.
     */
    ~ConnectionImpl() noexcept;

    /**
     * @brief Статус подключения к СУБД.
     *
     * @return Статус
     */
    [[nodiscard]] bool Status() const noexcept;

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
    [[nodiscard]] std::unique_ptr<ResultImpl> Exec(
        std::string_view query,
        const std::vector<std::any> &params = {}) const noexcept;

    /**
     * @brief Старт транзакции.
     *
     * @return Указатель на транзакцию
     */
    [[nodiscard]] std::unique_ptr<TransactionImpl> BeginTransaction()
        const noexcept;

    ConnectionImpl(const ConnectionImpl &) = delete;
    ConnectionImpl(ConnectionImpl &&) = delete;
    ConnectionImpl &operator=(const ConnectionImpl &) = delete;
    ConnectionImpl &operator=(ConnectionImpl &&) = delete;

private:
    /**
     * @brief Переподключение к БД.
     *
     * @return Результат переподключения
     */
    [[nodiscard]] bool Reconnect() const noexcept;

    /**
     * @brief Строка подключения к БД в формате PostgreSQL URI.
     */
    std::string uri_;

    /**
     * @brief Указатель на подключения к СУБД библиотеки libpq.
     */
    std::unique_ptr<PGconn, decltype(&PQfinish)> conn_;

    /**
     * @brief Список типов данных поддерживаемых для формирования запроса с
     * функциями преобразования их в текстовое представление.
     */
    static const VisitorList any_visitor_;
};

}  // namespace tasp::db::pg

#endif  // TASP_CONNECTION_IMPL_HPP_
