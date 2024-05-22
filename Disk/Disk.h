#pragma once
#include <string>
#include <vector>
#include <fstream>


/// Размер диска
const short DISK_CAPACITY = 64;

/// Длина блока данных
const short DATA_SIZE = 5;

/// Перечисление статуса диска
enum state {
    Success,
    Failure
};

/// Перечисление с ошибками при чтении данных с диска
enum error_read {
    NoError, /// Нет ошибок
    InvalidAddress, /// Недопустимый адрес
    DataEmpty, /// Строка ничего не содержит
    ServiceValue, /// Сервисное значение (избыточный блок)
    DamagedTooManyDisks /// Недостаточное количество дисков для восстановления или чтения/записи
};

/**
 * Структура ByteVec предназначена для управления данными, представленными в виде целых чисел (байтов).
 */
struct ByteVec {
    /// Хранит данные в массиве целых чисел.
    std::vector<int> data;

    /// Конструктор, который принимает строковые данные и преобразует их в вектор целых чисел.
    ByteVec(const std::string &str_data);

    /// Конструктор копирования.
    ByteVec(const ByteVec &other);

    /// Преобразует символ в целочисленное значение.
    int convert(const char &c);

    /// Возвращает строковое представление данных.
    std::string to_string();
};

/**
 * Класс Disk управляет массивом данных ByteVec, представляя функциональность диска.
 */
class Disk {
    friend struct ByteVec;

    /// Название диска.
    std::string disk_name;

    /// Массив данных, представленный объектами ByteVec.
    std::vector<ByteVec> data_array;
public:
    /**
     * Конструктор, назначающий имя диску и инициализирующий данные из файла.
     */
    Disk(const std::string &name);

    /**
     * Записывает данные data по адресу address. Возвращает true, если операция успешна.
     * @param address - адрес в массиве данных.
     * @param data - данные для записи.
     * @return true, если операция успешна.
     */
    bool write(const short &address, const std::string &data);

    /**
     * Читает и возвращает данные по указанному адресу.
     * @param address - адрес для чтения
     * @param error - ошибка при чтении
     * @return данные по указанному адресу
     */
    std::string read(const short &address, error_read &error);

    /**
     * Проверяет состояние диска.
     * @return статус диска
     */
    state check();

private:
    /**
     * Читает данные из файла для инициализации диска.
     *
     * @details Нужен для поддержки актуальных данных.
     */
    void ReadFromFile();

    /**
     * Записывает текущее состояние диска в файл.
     *
     * @details Нужен для поддержки актуальных данных.
     */
    void WriteToFile();
};
