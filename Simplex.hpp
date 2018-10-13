#pragma once
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <utility>

const size_t ROW = 4;
const size_t COLUMN = 4;

class Simplex
{
private:
    double** _Table;
    std::map<std::pair<size_t, size_t>, std::string> _Basis;
    size_t _Number_of_rows;
    size_t _Number_of_columns;

    // проверка совместности системы ограничений
    bool is_compatibility();

    // проверка ограниченности целевой функции
    bool is_limitation();

    // проверка допустимости найденного базисного решения
    bool is_permissible();

    // проверка оптимальности
    bool is_optimal();

    // проверка на альтернативность решения
    bool has_alternative();

    // поиск разрешающего столбца
    size_t find_resolving_column();

    // поиск пазрешающей строки
    size_t find_resolving_row(size_t resolving_column);

    void init_basis();

    // преобразование таблицы
    void update_table();
public:
    Simplex() = default;

    void init();

    void init_my_variant();

    ~Simplex();

    void calculate();

    void print();

    void result();
};
