/**
 * @file
 * @brief Реализация интерфейсов для работы с результатом запроса к СУБД
 * PostgreSQL.
 */
#ifndef TASP_RESULT_IMPL_HPP_
#define TASP_RESULT_IMPL_HPP_

#include <jsoncpp/json/json.h>
#include <postgresql/libpq-fe.h>

#include <memory>
#include <string_view>

namespace tasp::db::pg
{

class ResultIteratorImpl;

/**
 * @brief Реализация интерфейса для работы с результатом запроса к СУБД
 * PostgreSQL.
 */
class ResultImpl final
{
public:
    /**
     * @brief Конструктор.
     *
     * @param result Результат выполнения запроса к СУБД библиотеки libpq
     */
    explicit ResultImpl(PGresult *result) noexcept;

    /**
     * @brief Деструктор.
     */
    ~ResultImpl() noexcept;

    /**
     * @brief Статус выполнения запроса к СУБД.
     *
     * @return Статус
     */
    [[nodiscard]] bool Status() const noexcept;

    /**
     * @brief Запрос количества строк в результате выполнения SQL-запроса.
     *
     * @return Количество строк
     */
    [[nodiscard]] int Rows() const noexcept;

    /**
     * @brief Запрос количества колонок в результате выполнения SQL-запроса.
     *
     * @return Количество колонок
     */
    [[nodiscard]] int Columns() const noexcept;

    /**
     * @brief Запрос значения ячейки таблицы по номеру столбца.
     *
     * @param row Номер строки
     * @param column Номер столбца
     *
     * @return Значение. Пустую строку если значение отсутствует.
     */
    [[nodiscard]] std::string Value(int row, int column) const noexcept;

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

    /**
     * @brief Запрос значения ячейки таблицы по имени столбца.
     *
     * @param row Номер строки
     * @param name Название столбца
     *
     * @return Значение. Пустую строку если значение отсутствует.
     */
    [[nodiscard]] std::string Value(int row,
                                    std::string_view name) const noexcept;

    /**
     * @brief Конвертирование типов PostgreSQL в тип Json.
     *
     * @param row Номер строки
     * @param column Номер столбца
     *
     * @return JSON значение.
     */
    [[nodiscard]] Json::Value ConvertValue(int row, int column) const noexcept;

    /**
     * @brief Конвертация массива формата PostgreSQL в массив JSON.
     *
     * @param row Номер строки
     * @param column Номер столбца
     *
     * @return JSON-массив.
     */
    [[nodiscard]] Json::Value ValueArray(int row, int column) const noexcept;

    /**
     * @brief Конвертация bool PostgreSQL в JSON.
     *
     * @param row Номер строки
     * @param column Номер столбца
     *
     * @return JSON значение.
     */
    [[nodiscard]] Json::Value ValueBoolean(int row, int column) const noexcept;

    /**
     * @brief Конвертация чисел PostgreSQL в JSON.
     *
     * @param row Номер строки
     * @param column Номер столбца
     *
     * @return JSON значение.
     */
    [[nodiscard]] Json::Value ValueInt(int row, int column) const noexcept;

    // Выключается проверка стиля наименований для этого участка, т.к. это
    // методы для использования в стандартной библиотеке c++.
    // NOLINTBEGIN(readability-identifier-naming)
    /**
     * @brief Итератор на первую строку SQL-запроса.
     *
     * @return Итератор на первую строку.
     */
    [[nodiscard]] std::unique_ptr<ResultIteratorImpl> begin() const;

    /**
     * @brief Итератор конца строк SQL-запроса.
     *
     * @return Итератор конца.
     */
    [[nodiscard]] std::unique_ptr<ResultIteratorImpl> end() const;
    // NOLINTEND(readability-identifier-naming)

    ResultImpl(const ResultImpl &) = delete;
    ResultImpl(ResultImpl &&) = delete;
    ResultImpl &operator=(const ResultImpl &) = delete;
    ResultImpl &operator=(ResultImpl &&) = delete;

private:
    /**
     * @brief Указатель на результат выполнения запроса к СУБД библиотеки libpq.
     */
    std::unique_ptr<PGresult, decltype(&PQclear)> result_;
};

/**
 * @brief Реализация итератора для перебора строк результата SQL-команды.
 */
class ResultIteratorImpl
{
public:
    /**
     * @brief Конструктор.
     *
     * @param result Результат выполнения SQL-запроса
     * @param row Указатель на реализацию
     */
    explicit ResultIteratorImpl(const ResultImpl *result, int row = 0) noexcept;

    /**
     * @brief Деструктор.
     */
    ~ResultIteratorImpl() noexcept = default;

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
    ResultIteratorImpl &operator++() noexcept;

    /**
     * @brief Сравнение текущего итератора с итератором переданным в параметрах.
     *
     * @param rhs Итератор для сравнения
     *
     * @return Результат сравнения
     */
    [[nodiscard]] bool operator!=(const ResultIteratorImpl &rhs) noexcept;

    ResultIteratorImpl(const ResultIteratorImpl &) = delete;
    ResultIteratorImpl(ResultIteratorImpl &&) = delete;
    ResultIteratorImpl &operator=(const ResultIteratorImpl &) = delete;
    ResultIteratorImpl &operator=(ResultIteratorImpl &&) = delete;

private:
    /**
     * @brief Результат выполнения SQL-запроса.
     */
    const ResultImpl *result_;

    /**
     * @brief Номер строки в результате SQL-запроса.
     */
    int row_;
};

}  // namespace tasp::db::pg

#endif  // TASP_RESULT_IMPL_HPP_
