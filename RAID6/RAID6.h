#pragma once
#include "../Disk/Disk.h"
#include <algorithm>
#include <iomanip>
#include <map>
#include <iostream>

/// Общее количество дисков в массиве
const int NUM_OF_DISKS = 5;

/// Общий размер сообщения
const short FULL_DATA_SIZE = 14;

/// Вектор векторов, который описывает распределение дополнительных (избыточных) дисков для каждого адреса.
const std::vector<std::vector<int>> EXTRA_ALLOC = { {3, 4},
                                                 {2, 3},
                                                 {1, 2},
                                                 {0, 1},
                                                 {0, 4} };

/**
 * Этот класс реализует функциональность RAID6, которая предусматривает
 * хранение данных с двойным уровнем избыточности для повышения надежности данных.
 *
 * @details
 * Класс RAID6 использует агрегационный подход в собственной структуре,
 * включая массив disks_array, который содержит объекты дисков (Disk).
 * Методы для чтения данных подразумевают возможность восстановления, что
 * критично для обеспечения высокой доступности и надежности хранилища
 * данных в рамках RAID6.
 */
class RAID6 {
    std::vector<Disk> disks_array;

public:
    /**
     * Конструктор. Инициализация массива дисков
     */
    RAID6();

    /**
     * Записывает данные на диски.
     * @param address - адрес строки
     * @param data - данные для записи
     * @return true, если данные успешно записаны, иначе false
     */
    bool write(const short &address, const std::string &data);

    /**
     * Читает данные по заданному адресу. Возвращает строку с данными. Записывает ошибку в err, если чтение не удалось.
     * @param address - адрес строки
     * @param err - ошибка чтения
     * @return строка с данными или сообщение об ошибке
     */
    std::string read(const short &address, error_read &err);

private:
    /**
     * Вычисляет полином вида A + B + C, где A, B, C - блоки данных (шестнадцатиричные числа).
     * @param data_str - вектор блоков данных (строк)
     * @return вычисленный полином
     */
    int PolySum1(std::vector<std::string> &data_str);

    /**
     * Вычисляет полином вида A + 2B + 3C, где A, B, C - блоки данных (шестнадцатиричные числа).
     * @param data_str - вектор блоков данных (строк)
     * @return вычисленный полином
     */
    int PolySum2(std::vector<std::string> &data_str);

    /**
     * Осуществляет прямое чтение данных с дисков (без восстановления данных) по указанному адресу.
     * @param address - адрес строки
     * @param extra_disks - вектор избыточных дисков
     * @param err - ошибка чтения
     * @return - строка с данными
     */
    std::string ReadDirect(const short &address, const std::vector<int> &extra_disks, error_read &err);

    /**
     * Реализует чтение с восстановлением одного диска.
     * Включает выбор целевого дополнительного диска для восстановления.
     * @param address - адрес строки
     * @param extra_disks - вектор избыточных дисков
     * @param broken_disks - вектор вышедших из строя дисков
     * @param active_data_disks - вектор активных дисков с данными доступными для чтения
     * @param target_extra_disk - целевой избыточный диск
     * @param err - ошибка чтения
     * @return - строка с данными или сообщение об ошибке
     */
    std::string ReadWithOneRecovery(const short &address, const std::vector<int> &extra_disks,
                                  const std::vector<int> &broken_disks, const std::vector<int> &active_data_disks,
                                  const int &target_extra_disk, error_read &err);

    /**
     * Реализует чтение с восстановлением двух дисков.
     * @param address - адрес строки
     * @param extra_disks - вектор избыточных дисков
     * @param active_data_disks - вектор активных дисков с данными доступными для чтения
     * @param data_disks_indexes - словарь с условными индексами активных дисков с данными
     * @param err - ошибка чтения
     * @return строка с данными или сообщение об ошибке
     */
    std::string ReadWithTwoRecovery(const short &address, const std::vector<int> &extra_disks,
                                    const std::vector<int> &active_data_disks,
                                    const std::map<int, int> &data_disks_indexes, error_read &err);
};

/**
 * Удаляет ведущие нули из строки. Возвращает обработанную строку.
 * @param str - исходная строка
 * @return строка без ведущих нулей
 */
std::string RemoveLeadingZeros(const std::string& str);