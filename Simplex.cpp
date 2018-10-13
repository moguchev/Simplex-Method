#include "Simplex.hpp"

// проверка совместности системы ограничений
bool Simplex::is_compatibility()
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
bool Simplex::is_limitation()
{
    for (size_t j = 1; j < _Number_of_columns; ++j)
    {
        if (_Table[_Number_of_rows - 1][j] > 0)
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
bool Simplex::is_permissible()
{
    for (size_t i = 0; i < _Number_of_rows - 1; ++i)
    {
        // базисное решение содержит отрицательную компоненту
        if (_Table[i][0] < 0)
            return false;
    }
    return true;  // базисное решение не содержит отрицательных компонент
}

// проверка оптимальности
bool Simplex::is_optimal()
{
    for (size_t j = 1; j < _Number_of_columns; ++j)
    {
        if (_Table[_Number_of_rows - 1][j] >= 0)
            return false;
    }
    // в строке целевой функции не должно быть отрицательных(положительных) элементов
    return true;
}

// проверка на альтернативность решения
bool Simplex::has_alternative()
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
size_t Simplex::find_resolving_column()
{
    size_t resolving_column = 0;
    for (size_t j = 1; j < _Number_of_columns; ++j)
    {
        if (_Table[_Number_of_rows - 1][j] >= 0)
        {
            resolving_column = j;
            break;
        }
    }
    return resolving_column;
}

// поиск пазрешающей строки
size_t Simplex::find_resolving_row(size_t resolving_column)
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

void Simplex::init_basis()
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

// преобразование таблицы
void Simplex::update_table()
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


    // удаление старой таблицы
    for (size_t i = 0; i < _Number_of_rows; ++i)
    {
        delete[] new_table[i];
    }
    delete[] new_table;
}

// ввод с клавиатуры
void Simplex::init()
{
    std::cout << "Задача вида АХ<=B при сX->min" << std::endl;
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
            _Table[_Number_of_rows - 1][j] = _Table[_Number_of_rows - 1][j];
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

void Simplex::init_my_variant()
{
    _Number_of_columns = COLUMN;
    _Number_of_rows = ROW;

    _Table = new double*[ROW];
    for (size_t i = 0; i < 4; ++i)
    {
        _Table[i] = new double[COLUMN];
    }

    double Table[ROW][COLUMN] = {
        {5, 2, 1, 1},
        {3, 1, 2, 0},
        {8, 0, 0.5, 1},
        {0, 5, 6, 1}
    };

    for (size_t i = 0; i < ROW; ++i)
        for (size_t j = 0; j < COLUMN; ++j)
            _Table[i][j] = Table[i][j];

    this->init_basis();
}

void Simplex::print()
{
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << std::setw(5) << ' ';
    for (size_t j = 0; j < _Number_of_columns; ++j)
    {
        if (j == 0)
            std::cout << std::setw(7) << 'S';
        else
            std::cout << std::setw(10) << _Basis[std::make_pair(0, j)];
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
            std::cout << std::setw(10) << std::fixed << std::setprecision(4) << _Table[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------------" << std::endl;
}

Simplex::~Simplex()
{
    if (_Table != nullptr)
    {
        for (size_t i = 0; i < _Number_of_rows; ++i)
            delete[] _Table[i];
        delete[] _Table;
    }
}

void Simplex::calculate()
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

void Simplex::result()
{
    std::cout << "F = " << -_Table[_Number_of_rows - 1][0] << std::endl;
}
