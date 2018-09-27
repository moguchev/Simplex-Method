#pragma once
#include <algorithm>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <numeric>
#include <string>
#include <iomanip>


class Simplex
{
private:
    double** _Table;
    std::map<std::pair<size_t,size_t>, std::string> _Basis;
    size_t _Number_of_rows;
    size_t _Number_of_columns;

    // ïðîâåðêà ñîâìåñòíîñòè ñèñòåìû îãðàíè÷åíèé
    bool is_compatibility()
    {
        for (size_t i = 0; i < _Number_of_rows - 1; ++i)
        {
            if (_Table[i][0] < 0)
            {
                for (size_t j = 1; j < _Number_of_columns; ++j)
                {
                    if (_Table[i][j] < 0)
                        return true; // ïðîâåðêà ïðèçíàêà íåñîâìåñòíîñòè ñèñòåìû îãðàíè÷åíèé
                }
                return false; // íåò õîòÿ áû îäíîãî îòðèöàòåëüíîãî ýëåìåíòà
            }
        }
        return true; // íåò ñòðîêè ñ îòðèöàòåëüíûì ñâîáîäíûì ÷èñëîì (êðîìå ñòðîêè öåëåâîé ôóíêöèè)
    }

    // ïðîâåðêà îãðàíè÷åííîñòè öåëåâîé ôóíêöèè
    bool is_limitation()
    {
        for (size_t j = 1; j < _Number_of_columns; ++j)
        {
            if (_Table[_Number_of_rows - 1][j] < 0)
            {
                bool flag = false;
                for (int i = _Number_of_rows - 2; i != -1; i--)
                {
                    if (_Table[i][j] > 0)  // ïðîâåðêà ïðèçíàêà íåîãðàíè÷åííîñòè öåëåâîé ôóíêöèè 
                    {
                        flag = true;
                        break;
                    }
                }
                if (flag == false) // íåò õîòÿ áû îäíîãî ïîëîæèòåëüíîãî ýëåìåíòà
                    return false; 
            }
        }
        return true; // íåò êîëîíêè ñ îòðèöàòåëüíûì ýëåìåíòîì â ñòðîêå öåëåâîé ôóíêöèè
    }

    // ïðîâåðêà äîïóñòèìîñòè íàéäåííîãî áàçèñíîãî ðåøåíèÿ
    bool is_permissible()
    {
        for (size_t i = 0; i < _Number_of_rows; ++i) 
        {
            if (_Table[i][0] < 0)
                return false; // áàçèñíîå ðåøåíèå ñîäåðæèò îòðèöàòåëüíóþ êîìïîíåíòó
        }
        return true; // áàçèñíîå ðåøåíèå íå ñîäåðæèò îòðèöàòåëüíûõ êîìïîíåíò
    }

    // ïðîâåðêà îïòèìàëüíîñòè
    bool is_optimal()
    {
        for (size_t j = 1; j < _Number_of_columns; ++j)
        {
            if (_Table[_Number_of_rows - 1][j] < 0)
                return false;
        }
        return true; // â ñòðîêå öåëåâîé ôóíêöèè íå äîëæíî áûòü îòðèöàòåëüíûõ ýëåìåíòîâ
    }

    // ïðîâåðêà íà àëüòåðíàòèâíîñòü ðåøåíèÿ
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

    // ïîèñê ðàçðåøàþùåãî ñòîëáöà
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

    // ïîèñê ïàçðåøàþùåé ñòðîêè
    size_t find_resolving_row(size_t resolving_column)
    {
        std::vector<double> ratios;
        for (size_t i = 0; i < _Number_of_rows - 1; ++i)
        {
            if (_Table[i][resolving_column] != 0)
                if(_Table[i][0] / _Table[i][resolving_column] > 0)
                    ratios.push_back(_Table[i][0] / _Table[i][resolving_column]);
                else
                    ratios.push_back(std::numeric_limits<double>::max());
            else
                ratios.push_back(std::numeric_limits<double>::max());
        }
        auto min = std::min_element(ratios.begin(), ratios.end());
        return std::distance(ratios.begin(), min);
    }

    void init_basis()
    {
        for (size_t k = 1; k < _Number_of_columns + _Number_of_rows - 2;)
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
        // íàõîæäåíèå ðàçðåø ýëåìåíòà
        size_t main_col = find_resolving_column();
        size_t main_row = find_resolving_row(main_col);
        double resolving_element = _Table[main_row][main_col];

        // ñîçäàíèå íîâîé òàáëèöû
        double** new_table = new double*[_Number_of_rows];
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            new_table[i] = new double[_Number_of_columns];
        }

        // ìåíÿåì áàçèñ
        std::swap(_Basis[std::make_pair(0, main_col)], _Basis[std::make_pair(main_row, 0)]);

        // Ïðåîáðàçîâàíèå ðàçðåøàþùåãî ýëåìåíòà
        new_table[main_row][main_col] = 1 / resolving_element;

        // Ïðåîáðàçîâàíèå ðàçðåøàþùåé ñòðîêè
        for (size_t j = 0; j < _Number_of_columns; ++j)
        {
            if (j != main_col)
                new_table[main_row][j] = _Table[main_row][j] / resolving_element;
        }

        // Ïðåîáðàçîâàíèå ðàçðåøàþùåé êîëîíêè
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            if (i != main_row)
                new_table[i][main_col] = -_Table[i][main_col] / resolving_element;
        }

        // Ïðåîáðàçîâàíèå îñòàëüíûõ ýëåìåíòîâ ñèìïëåêñ-òàáëèöû
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            if (i == main_row)
                continue;
            for (size_t j = 0; j < _Number_of_columns; ++j)
            {
                if (j == main_col)
                    continue;
                new_table[i][j] = _Table[i][j] - (_Table[i][main_col] * _Table[main_row][j] / resolving_element);
            }
        }

        // íîâóþ òàáëèöó äåëàåì çàïèñûâàåì â ïîëå
        std::swap(_Table, new_table);
        this->print();

        // óäàëåíèå ñòàðîé òàáëèöû
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            delete[] new_table[i];
        }
        delete[] new_table;
    }

public:
    Simplex() = default;

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
        std::cout << "Äàííàÿ ïðîãðàììà ðåøàåò çàäà÷ó âèäà ÀÕ<=B ïðè ñõ-> max" << std::endl;

        int number_of_variables = 0;
        int number_of_conditions = 0;

        std::cout << "Ââåäèòå ÷èñëî ïåðåìåííûõ öåëåâîé ôóíêöèè: ";
        std::cin >> number_of_variables;
        std::cout << std::endl;
        _Number_of_columns = number_of_variables + 1;

        std::cout << "Ââåäèòå ÷èñëî îãðàíè÷åíèé: ";
        std::cin >> number_of_conditions;
        std::cout << std::endl;
        _Number_of_rows = number_of_conditions + 1;

        _Table = new double*[_Number_of_rows];
        for (size_t i = 0; i < _Number_of_rows; ++i)
        {
            _Table[i] = new double[_Number_of_columns];
        }
        std::cout << "Ââåäèòå êîýôôèöèåíòû öåëåâîé ôóíêöèè :" << std::endl;
        for (size_t j = 0; j < _Number_of_columns; ++j)
        {
            if (j == 0)
            {
                _Table[_Number_of_rows - 1][j] = 0;
            }
            else
            {
                std::cout << "êîýôôèöèåíò ïðè Õ" << j << " = ";
                std::cin >> _Table[_Number_of_rows - 1][j];
                _Table[_Number_of_rows - 1][j] = -_Table[_Number_of_rows - 1][j];
                std::cout << std::endl;
            }
        }

        std::cout << "Ââåäèòå êîýôôèöèåíòû îãðàíè÷åíèé : " << std::endl;
        for (size_t i = 0; i < _Number_of_rows - 1; ++i)
        {
            std::cout << "Íåðàâåíñòâî " << i + 1 << " : " << std::endl;
            for (size_t j = 0; j < _Number_of_columns; ++j)
            {
                if (j == 0)
                {
                    std::cout << "Ââåäèòå ñâîáîäíûé ÷ëåí S : ";
                    std::cin >> _Table[i][0];
                }
                else
                {
                    std::cout << "êîýôôèöèåíò ïðè Õ" << j << " = ";
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
            if (is_compatibility()) // ïðîâåðêà ñîâìåñòíîñòè ñèñòåìû îãðàíè÷åíèé
            {
                if (is_limitation()) // ïðîâåðêà îãðàíè÷åííîñòè öåëåâîé ôóíêöèè
                {
                    if (is_permissible() && is_optimal())  // ïðîâåðêà äîïóñòèìîñòè íàéäåííîãî áàçèñíîãî ðåøåíèÿ
                    {
                        // ïðîâåðêà àëüòåðíàòèâíîñòè ðåøåíèÿ
                        if (has_alternative())
                            std::cout << "Èìååò àëüòåðíàòèâíîå ðåøåíèå." << std::endl;
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
                    std::cout << "Öåëåâàÿ ôóíêöèÿ íå îãðàíè÷åíà." << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "Ñèñòåìà íå ñîâìåñòíà." << std::endl;
                break;
            }
        }
    }

    void result()
    {
        std::cout << "F = " << _Table[_Number_of_rows - 1][0] << std::endl;
    }

};
