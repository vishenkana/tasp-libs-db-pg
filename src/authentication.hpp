/**
 * @file
 * @brief Классы с информацией по подключениям к БД.
 */
#ifndef TASP_AUTHENTICATION_HPP_
#define TASP_AUTHENTICATION_HPP_

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace tasp::db::pg::auth
{

class BaseConnection;

/**
 * @brief Менеджер подключений к БД.
 */
class Manager final
{
public:
    /**
     * @brief Запрос ссылки на менеджер подключений к БД.
     *
     * При первом вызове читает конфигурационный файл и формирует строки
     * подключения к БД.
     *
     * @return Ссылка на менеджер подключения к БД
     */
    static Manager &Instance() noexcept;

    /**
     * @brief Запрос строки подключения к БД в формате PostgreSQL URI.
     *
     * В случае отсутствия подключения с таким именем возвращает пустую строку.
     *
     * @param name Название подключения к БД
     *
     * @return Строка подключения к БД
     */
    [[nodiscard]] const std::string &Uri(std::string_view name) const noexcept;

    /**
     * @brief Функция обновления информации и подключениях к БД.
     */
    void Reload() noexcept;

    Manager(const Manager &) = delete;
    Manager(Manager &&) = delete;
    Manager &operator=(const Manager &) = delete;
    Manager &operator=(Manager &&) = delete;

private:
    /**
     * @brief Конструктор.
     *
     * Загружает информацию о всех подключениях к БД и формирует для них строки
     * uri для подключения
     */
    Manager() noexcept;

    /**
     * @brief Деструктор.
     */
    ~Manager() noexcept;

    /**
     * @brief Список подключений к БД из конфигурационного файла.
     */
    std::unordered_map<std::string, std::unique_ptr<BaseConnection>>
        connections_{};

    /**
     * @brief Список типов аутентификации с лямбдами создания объекта нужного
     * типа.
     */
    std::unordered_map<
        std::string,
        std::function<std::unique_ptr<BaseConnection>(std::string_view name)>>
        auth_creator_{};
};

/**
 * @brief Базовый тип подключения к БД.
 */
class BaseConnection
{
public:
    /**
     * @brief Конструктор.
     *
     * @param path Путь к параметрам в конфигурационном файле
     */
    explicit BaseConnection(std::string_view path) noexcept;

    /**
     * @brief Деструктор.
     */
    virtual ~BaseConnection() noexcept;

    /**
     * @brief Запрос строки подключения к БД в формате PostgreSQL URI.
     *
     * @return Строка подключения к БД
     */
    [[nodiscard]] virtual const std::string &ConnectionString() const noexcept;

    BaseConnection(const BaseConnection &) = delete;
    BaseConnection(BaseConnection &&) = delete;
    BaseConnection &operator=(const BaseConnection &) = delete;
    BaseConnection &operator=(BaseConnection &&) = delete;

protected:
    /**
     * @brief Замена первого вхождения символов {} в строке подключения к БД на
     * параметр из конфигурационного файла.
     *
     * @param param Название параметра из конфигурационного файла
     * @param default_value Значение по умолчанию, при отсутствии параметра в
     * конфигурационном файле
     */
    void ReplaceParam(std::string_view param,
                      std::string_view default_value = {}) noexcept;

    /**
     * @brief Добавление дополнительный параметров подключения в конец строки.
     *
     * @param params Дополнительные параметры
     */
    void AddParams(std::string_view params) noexcept;

private:
    /**
     * @brief Путь к к данным подключения в конфигурационном файле.
     */
    std::string config_{};

    /**
     * @brief Строка подключения к БД в формате PostgreSQL URI.
     */
    std::string uri_{"postgresql://{}:{}/{}?application_name="};
};

/**
 * @brief Подключение к БД с помощью задания логина и пароля.
 */
class Md5 final : public BaseConnection
{
public:
    /**
     * @brief Конструктор.
     *
     * @param path Путь к параметрам в конфигурационном файле
     */
    explicit Md5(std::string_view path) noexcept;

    /**
     * @brief Деструктор.
     */
    ~Md5() noexcept override;

    Md5(const Md5 &) = delete;
    Md5(Md5 &&) = delete;
    Md5 &operator=(const Md5 &) = delete;
    Md5 &operator=(Md5 &&) = delete;

private:
};

/**
 * @brief Подключение к БД с сквозной аутентификации.
 */
class Gss final : public BaseConnection
{
public:
    /**
     * @brief Конструктор.
     *
     * @param path Путь к параметрам в конфигурационном файле
     */
    explicit Gss(std::string_view path) noexcept;

    /**
     * @brief Деструктор.
     */
    ~Gss() noexcept override;

    Gss(const Gss &) = delete;
    Gss(Gss &&) = delete;
    Gss &operator=(const Gss &) = delete;
    Gss &operator=(Gss &&) = delete;

private:
};

/**
 * @brief Подключение к БД с помощью строки в формате PostgreSQL URI.
 */
class Uri final : public BaseConnection
{
public:
    /**
     * @brief Конструктор.
     *
     * @param path Путь к параметрам в конфигурационном файле
     */
    explicit Uri(std::string_view path) noexcept;

    /**
     * @brief Деструктор.
     */
    ~Uri() noexcept override;

    /**
     * @brief Запрос строки подключения к БД в формате PostgreSQL URI.
     *
     * @return Строка подключения к БД
     */
    [[nodiscard]] const std::string &ConnectionString() const noexcept override;

    Uri(const Uri &) = delete;
    Uri(Uri &&) = delete;
    Uri &operator=(const Uri &) = delete;
    Uri &operator=(Uri &&) = delete;

private:
    /**
     * @brief Строка подключения к БД в формате PostgreSQL URI.
     */
    std::string uri_{"postgresql://127.0.0.1:5432/"
                     "ta?application_name=ta&user=ta&password=12345678"};
};

}  // namespace tasp::db::pg::auth

#endif  // TASP_AUTHENTICATION_HPP_
