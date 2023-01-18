#include <iostream>
#include <fstream>
#include <vector>

const int SIZE_TABLE_CODE = 255;

using error_code_t = int;

/*
Место имен для работы с файлом

Внутри имеется перечисления и коды возращаемых ошибок
*/
namespace ns_file
{
    enum eFileError
    {
        ERR_UNKNOWN = 1
        ,NO = 0
        ,ERR_OPEN = 2
        ,ERR_CLOSE = 3
        ,FILE_CLEAR = 255
    };
}

/*
Функция которая вычисляет частоту встречи символов в файле
Вектор должен быть ассоцирован с таблицой символов. По умолчанию
предполагается таблица ASCI

На вход:
Строка из которой необходимо получить частоты
Вектор куда сохранять результат

На выход:
Код возрата
Измененные данные входного аргумента массив

*/
error_code_t get_frequency(const std::string& _text, std::vector<int>& _table)
{
    if(_text.empty())
    {
        //Обнуляем вектор так как нету символов
        for(size_t i = 0; i < _table.size(); _table.at(i) = 0, ++i);
        return 0;
    }

    

    return 0;
}

int main()
{
    using namespace std;

    vector<int> v(SIZE_TABLE_CODE, 0);
    v[0] = 5;
    
    get_frequency("", v);
    cout << v[0];
    return 0;
}