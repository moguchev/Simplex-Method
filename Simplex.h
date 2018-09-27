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


class Simplex
{
private:
    double** _Table;
    std::map<std::pair<size_t, size_t>, std::string> _Basis;
    size_t _Number_of_rows;
    size_t _Number_of_columns;

    // проверка совместности системы ограничений
    bool is_compatibility()
    {
        // проверка признака несовместности системы ограничений
        for (size_t i = 0; i < _Number_of_rows - 1; ++i)
        {
            if (_Table[i][0] < 0)
            {
                for (size_t j = 1; j < _Number_of_columns; ++j)
                {
                    if (_Table[i][j] < 0)
                        return true;
                }
                return false;  // нет хотя бы одного отрицательного элемента
            }
        }
        return true;  // нет строки с отрицательным свободным числом
    }

    // проверка ограниченности целевой функции
    bool is_limitation()
    {
        for (size_t j = 1; j < _Number_of_columns; ++j)
        {
            if (_Table[_Number_of_rows - 1][j] < 0)
            {
                bool flag = false;
                for (int i = _Number_of_rows - 2; i != -1; i--)
                {
                    if (_Table[i][j] > 0)
                    {
                        flag = true;
                        break;
                    }
                }
                // нет хотя бы одного положительного элемента
                if (flag == false)
                    return false;
            }
        }
        // нет колонки с отрицательным элементом в строке целевой функции
        return true;
    }

    // проверка допустимости найденного базисного решения
    bool is_permissible()
    {
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            // базисное решение содержит отрицательную компоненту
            if (_Table[i][0] < 0)
                return false;
        }
        return true;  // базисное решение не содержит отрицательных компонент
    }

    // проверка оптимальности
    bool is_optimal()
    {
        for (size_t j = 1; j < _Number_of_columns; ++j)
        {
            if (_Table[_Number_of_rows - 1][j] < 0)
                return false;
        }
        // в строке целевой функции не должно быть отрицательных элементов
        return true;
    }

    // проверка на альтернативность решения
    bool has_alternative()
    {
        bool has_alt = false;
        for (size_t j = 1; j < _Number_of_columns; ++j)
        {
            if (_Table[_Number_of_rows - 1][j] == 0)
            {
                has_alt = true;
                break;
            }
        }
        return has_alt;
    }

    // поиск разрешающего столбца
    size_t find_resolving_column()
    {
        double min = 0;
        size_t resolving_column = 0;
        for (size_t j = 1; j < _Number_of_columns; ++j)
        {
            if (_Table[_Number_of_rows - 1][j] < 0 &&
                _Table[_Number_of_rows - 1][j] < min)
            {
                min = _Table[_Number_of_rows - 1][j];
                resolving_column = j;
            }
        }
        return resolving_column;
    }

    // поиск пазрешающей строки
    size_t find_resolving_row(size_t resolving_column)
    {
        std::vector<double> ratios;
        for (size_t i = 0; i < _Number_of_rows - 1; ++i)
        {
            if (_Table[i][resolving_column] != 0)
            {
                if (_Table[i][0] / _Table[i][resolving_column] > 0)
                    ratios.push_back(_Table[i][0] / _Table[i][resolving_column]);
                else
                    ratios.push_back(std::numeric_limits<double>::max());
            }
            else
            {
                ratios.push_back(std::numeric_limits<double>::max());
            }
        }
        auto min = std::min_element(ratios.begin(), ratios.end());
        return std::distance(ratios.begin(), min);
    }

    void init_basis()
    {
        for (int k = 1; k < _Number_of_columns + _Number_of_rows - 2;)
        {
            for (size_t i = 0, j = 1; j < _Number_of_columns; ++j, ++k)
            {
                std::string x = "X";
                x += (k + '0');
                _Basis[std::make_pair(i, j)] = x;
            }
            for (size_t i = 0, j = 0; i < _Number_of_rows - 1; ++i, ++k)
            {
                std::string x = "X";
                x += (k + '0');
                _Basis[std::make_pair(i, j)] = x;
            }
        }
    }

    void update_table()
    {
        // нахождение разреш элемента
        size_t main_col = find_resolving_column();
        size_t main_row = find_resolving_row(main_col);
        double resolving_element = _Table[main_row][main_col];

        // создание новой таблицы
        double** new_table = new double*[_Number_of_rows];
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            new_table[i] = new double[_Number_of_columns];
        }

        // меняем базис
        std::swap(_Basis[std::make_pair(0, main_col)],
            _Basis[std::make_pair(main_row, 0)]);

        // Преобразование разрешающего элемента
        new_table[main_row][main_col] = 1 / resolving_element;

        // Преобразование разрешающей строки
        for (size_t j = 0; j < _Number_of_columns; ++j)
        {
            if (j != main_col)
                new_table[main_row][j] = _Table[main_row][j] / resolving_element;
        }

        // Преобразование разрешающей колонки
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            if (i != main_row)
                new_table[i][main_col] = -_Table[i][main_col] / resolving_element;
        }

        // Преобразование остальных элементов симплекс-таблицы
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            if (i == main_row)
                continue;
            for (size_t j = 0; j < _Number_of_columns; ++j)
            {
                if (j == main_col)
                    continue;
                new_table[i][j] = _Table[i][j] -
                    (_Table[i][main_col] * _Table[main_row][j] / resolving_element);
            }
        }

        // новую таблицу делаем записываем в поле
        std::swap(_Table, new_table);
        this->print();

        // удаление старой таблицы
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            delete[] new_table[i];
        }
        delete[] new_table;
    }

public:
    Simplex() = default;

    void init_my_variant()
    {
        _Number_of_columns = 4;
        _Number_of_rows = 4;

        _Table = new double*[4];
        for (size_t i = 0; i < 4; ++i)
        {
            _Table[i] = new double[4];
        }

        _Table[0][0] = 5; _Table[0][1] = 2; _Table[0][2] = 1; _Table[0][3] = 1;
        _Table[1][0] = 3; _Table[1][1] = 1; _Table[1][2] = 2; _Table[1][3] = 0;
        _Table[2][0] = 8; _Table[2][1] = 0; _Table[2][2] = 0.5; _Table[2][3] = 1;
        _Table[3][0] = 0; _Table[3][1] = -5; _Table[3][2] = -6; _Table[3][3] = -1;

        this->init_basis();
    }

    void print()
    {
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << std::setw(5) << ' ';
        for (size_t j = 0; j < _Number_of_columns; ++j)
        {
            if (j == 0)
                std::cout << std::setw(7) << 'S';
            else
                std::cout << std::setw(7) << _Basis[std::make_pair(0, j)];
        }
        std::cout << std::endl;
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            if (i == _Number_of_rows - 1)
                std::cout << "F " << std::setw(3) << ' ';
            else
                std::cout << _Basis[std::make_pair(i, 0)] << std::setw(3) << ' ';

            for (size_t j = 0; j < _Number_of_columns; ++j)
            {
                std::cout << std::setw(7) << _Table[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << "-----------------------------------------------" << std::endl;
    }

    ~Simplex()
    {
        if (_Table != nullptr)
        {
            for (size_t i = 0; i < _Number_of_rows; ++i)
            {
                delete[] _Table[i];
            }
            delete[] _Table;
        }
    }

    void init()
    {
        std::cout << "Задача вида АХ<=B при сX->max" << std::endl;

        int number_of_variables = 0;
        int number_of_conditions = 0;

        std::cout << "Введите число переменных целевой функции: ";
        std::cin >> number_of_variables;
        std::cout << std::endl;
        _Number_of_columns = number_of_variables + 1;

        std::cout << "Введите число ограничений: ";
        std::cin >> number_of_conditions;
        std::cout << std::endl;
        _Number_of_rows = number_of_conditions + 1;

        _Table = new double*[_Number_of_rows];
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            _Table[i] = new double[_Number_of_columns];
        }
        std::cout << "Введите коэффициенты целевой функции :" << std::endl;
        for (size_t j = 0; j < _Number_of_columns; ++j)
        {
            if (j == 0)
            {
                _Table[_Number_of_rows - 1][j] = 0;
            }
            else
            {
                std::cout << "коэффициент при Х" << j << " = ";
                std::cin >> _Table[_Number_of_rows - 1][j];
                _Table[_Number_of_rows - 1][j] = -_Table[_Number_of_rows - 1][j];
                std::cout << std::endl;
            }
        }

        std::cout << "Введите коэффициенты ограничений : " << std::endl;
        for (size_t i = 0; i < _Number_of_rows - 1; ++i)
        {
            std::cout << "Неравенство " << i + 1 << " : " << std::endl;
            for (size_t j = 0; j < _Number_of_columns; ++j)
            {
                if (j == 0)
                {
                    std::cout << "Введите свободный член S : ";
                    std::cin >> _Table[i][0];
                }
                else
                {
                    std::cout << "коэффициент при Х" << j << " = ";
                    std::cin >> _Table[i][j];
                }
            }
        }
        this->init_basis();
        this->print();
    }

    void calculate()
    {
        this->print();
        while (true)
        {
            if (is_compatibility())  // проверка совместности системы ограничений
            {
                if (is_limitation())  // проверка ограниченности целевой функции
                {
                    // проверка допустимости найденного базисного решения
                    if (is_permissible() && is_optimal())
                    {
                        // проверка альтернативности решения
                        if (has_alternative())
                            std::cout << "Имеет альтернативное решение." << std::endl;
                        break;
                    }
                    else
                    {
                        update_table();
                        this->print();
                    }
                }
                else
                {
                    std::cout << "Целевая функция не ограничена." << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "Система не совместна." << std::endl;
                break;
            }
        }
    }

    void result()
    {
        std::cout << "F = " << _Table[_Number_of_rows - 1][0] << std::endl;
    }
};