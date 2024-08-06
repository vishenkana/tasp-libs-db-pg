/**
 * @file
 * @brief Интерфейсы для работы с результатом запроса к СУБД PostgreSQL.
 */
#ifndef TASP_DB_PG_RESULT_HPP_
#define TASP_DB_PG_RESULT_HPP_

#include <jsoncpp/json/json.h>

#include <memory>
#include <string>
#include <string_view>

namespace tasp::db::pg
{

class ResultImpl;
class ResultIteratorImpl;

/**
 * @brief Интерфейс для работы с результатом запроса к СУБД PostgreSQL.
 *
 * Класс скрывает от пользователя реализацию с помощью идиомы PIMPL
 * (Pointer to Implementation – указатель на реализацию).
 */
class [[gnu::visibility("default")]] Result final
{
public:
    class Iterator;

    /**
     * @brief Конструктор.
     *
     * @param impl Указатель на реализацию
     */
    explicit Result(std::unique_ptr<ResultImpl> impl) noexcept;

    /**
     * @brief Деструктор.
     */
    ~Result() noexcept;

    /**
     * @brief Статус выполнения запроса к СУБД.
     *
     * @return Статус
     */
    [[nodiscard]] bool Status() const noexcept;

    /**
     * @brief Запрос значения по имени столбца.
     *
     * @param name Название столбца.
     *
     * @return Значение. Пустую строку если значение отсутствует.
     */
    [[nodiscard]] std::string Value(std::string_view name) const noexcept;

    /**
     * @brief Запрос данных запроса в формате JSON.
     *
     * Формат JSON:
     * {
     *   count = 0
     *   data
     *   [
     *     {
     *        field1 = "value",
     *        field2 = "value",
     *     },
     *     {
     *        field1 = "value",
     *        field2 = "value",
     *     }
     *   ]
     * }
     *
     * @return JSON с данными.
     */
    [[nodiscard]] Json::Value JsonValue() const noexcept;

    // Выключается проверка стиля наименований для этого участка, т.к. это
    // методы для использования в стандартной библиотеке c++.
    // NOLINTBEGIN(readability-identifier-naming)
    /**
     * @brief Итератор на первую строку SQL-запроса.
     *
     * @return Итератор на первую строку.
     */
    [[nodiscard]] Result::Iterator begin() const;

    /**
     * @brief Итератор конца строк SQL-запроса.
     *
     * @return Итератор конца.
     */
    [[nodiscard]] Result::Iterator end() const;
    // NOLINTEND(readability-identifier-naming)

    Result(const Result &) = delete;
    Result(Result &&) = delete;
    Result &operator=(const Result &) = delete;
    Result &operator=(Result &&) = delete;

private:
    /**
     * @brief Указатель на реализацию.
     */
    std::unique_ptr<ResultImpl> impl_;
};

/**
 * @brief Итератор для перебора строк результата SQL-команды.
 *
 * Класс скрывает от пользователя реализацию с помощью идиомы PIMPL
 * (Pointer to Implementation – указатель на реализацию).
 */
class Result::Iterator
{
public:
    /**
     * @brief Конструктор.
     *
     * @param impl Указатель на реализацию
     */
    explicit Iterator(std::unique_ptr<ResultIteratorImpl> impl) noexcept;

    /**
     * @brief Деструктор.
     */
    ~Iterator();

    /**
     * @brief Запрос значения по имени столбца.
     *
     * @param name Название столбца.
     *
     * @return Значение. Пустую строку если значение отсутствует.
     */
    [[nodiscard]] std::string Value(std::string_view name) const noexcept;

    /**
     * @brief Переход на следующую строку.
     *
     * @return Ссылка на самого себя.
     */
    Iterator &operator++() noexcept;

    /**
     * @brief Сравнение текущего итератора с итератором переданным в параметрах.
     *
     * @param rhs Итератор для сравнения
     *
     * @return Результат сравнения
     */
    bool operator!=(const Iterator &rhs) noexcept;

    /**
     * @brief Получение значения на которое указывает итератор.
     *
     * @return Ссылка на самого себя
     */
    const Iterator &operator*() const noexcept;

    Iterator(const Iterator &) = delete;
    Iterator(Iterator &&ref) noexcept = delete;
    Iterator &operator=(const Iterator &) = delete;
    Iterator &operator=(Iterator &&) = delete;

private:
    /**
     * @brief Указатель на реализацию
     */
    std::unique_ptr<ResultIteratorImpl> impl_;
};

}  // namespace tasp::db::pg

#endif  // TASP_DB_PG_RESULT_HPP_
