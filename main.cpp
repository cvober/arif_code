#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>

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

На вход:
Строка из которой необходимо получить частоты
Карта в которой будет результат

На выход:
Конечная частота

*/

double arithmetic_coding_probability(
    const std::string _text, 
    std::unordered_map<char, double>& _probabilities
    ) 
{
    // Создание карты символов и их вероятностей
    
    int input_length = _text.length();
    
    for (size_t i = 0; i < input_length; i++) 
    {
        if (_probabilities.find(_text[i]) == _probabilities.end()) 
        {
            _probabilities[_text[i]] = 1;
        } 
        
        else 
        {
            _probabilities[_text[i]]++;
        }
    }

    for (auto& probability : _probabilities) {
        probability.second /= input_length;
    }

    // Подсчет вероятности арифметического кодирования
    double probability = 1;
    
    for (auto& p : _probabilities) 
    {
        probability *= pow(p.second, p.second);
    }

    return probability;
}


void build_interval_table(
    const std::unordered_map<char, double>& _probabilities, 
    std::unordered_map<char, std::pair<double, double>>& _intervals,
    double _right,
    double _left
    )
{
    double full_stretch = _right - _left;
    
    for (auto& p : _probabilities)
    {
        double percent_stretch = p.second * full_stretch; 

        double left = _right - percent_stretch;
        if(left < 0.0 || left > 1.0)
            left = 0;

        _intervals[p.first] = {left, _right};
        _right -= percent_stretch;
    }
    

}

double get_code(
    std::string _text, 
    std::unordered_map<char, double>& _probabilities,
    std::unordered_map<char, std::pair<double, double>>& _intervals
    ) 
{
    double probability = 1;
    double right = 1.0;
    double left = 0;
    
    for(size_t i = 0; i < _text.length(); ++i)
    {
        char symbol = _text.at(i);

        //Получаем значение для отрезка для текущего символа
        probability = ((right - left) * _probabilities.at(symbol)) + left;
        
        //Высчитываем новые интервалы и обновляем левые, правые границы
        build_interval_table(_probabilities, _intervals, right, left);

        left = _intervals.at(symbol).first;
        right = _intervals.at(symbol).second;
    
        //std::cout << probability << " - ";
        //std::cout << "[" << left << ", " << right << "]" << std::endl;
    }    

    return probability;
}


std::string decode(
    std::unordered_map<char, double>& _probabilities,
    std::unordered_map<char, std::pair<double, double>>& _intervals,
    double _probability
    )
{
    std::string text;

    //double probability = 1;
    double right = 1.0;
    double left = 0;
    
    int flag = 1;
    while(flag)
    {
        build_interval_table(_probabilities, _intervals, right, left);

        //std::cout << probability << " - ";
        //std::cout << "[" << left << ", " << right << "]" << std::endl;

        for (auto &[key, value] : _intervals)
        {
            if(value.first < _probability && _probability <= value.second)
            {
                text.push_back(key);
                left = _intervals.at(key).first;
                right = _intervals.at(key).second;

                if(key == '!')
                    flag = 0;

                break;
            }
        }
    }

    std::cout << text << std::endl;
    return text;
}


/*
std::string decode(
    double _code, 
    std::unordered_map<char, 
    double>& _probability) 
{
    std::string decoded = "";
    double left = 0;
    double right = 1.0;
    std::unordered_map<char, std::pair<double, double>> intervals;

    while (true) 
    {
        bool found = false;

         for (auto i : intervals) 
         {
            if (right >= i.first && left < i.second) 
            {
                decoded += i.first;
                
                left = i.second.first;
                right = i.second.second;
                
                _code = (_code - i.second.first) / (i.second.second - i.second.first);
                
                found = true;
                break;
            }        
        }
        if (!found) break;
    }
        
    
    std::cout << decoded << std::endl;
    return decoded;
}
*/

/*
void build_interval_table(
    const std::unordered_map<char, double>& _probabilities, 
    std::unordered_map<char, std::pair<double, double>>& _intervals,
    std::unordered_map<char, double>& prefix_probabilities,
    char symbol,
    double _right,
    double _left
    )
{
    //Считаем вероятность символа с учетом всех символов, которые предшествуют текущему символу
    double symbol_probability = _probabilities.at(symbol);
    for (auto& p : prefix_probabilities)
    {
        symbol_probability /= p.second;
    }

    double percent_stretch = symbol_probability * (_right - _left); 

    double left = _right - percent_stretch;
    if(left < 0.0 || left > 1.0)
        left = 0;

    _intervals[symbol] = {left, _right};
    prefix_probabilities[symbol] = symbol_probability;
}

double get_code(
    std::string _text, 
    std::unordered_map<char, double>& _probabilities,
    std::unordered_map<char, std::pair<double, double>>& _intervals
    ) 
{
    double probability = 1;
    double right = 1.0;
    double left = 0;

    std::unordered_map<char, double> prefix_probabilities;

    for(size_t i = 0; i < _text.length(); ++i)
    {
        char symbol = _text.at(i);

        //Получаем значение для отрезка для текущего символа
        probability = ((right - left) * _probabilities.at(symbol)) + left;
        
        //Высчитываем новые интервалы и обновляем левые, правые границы
        build_interval_table(_probabilities, _intervals, prefix_probabilities, symbol, right, left);

        left = _intervals.at(symbol).first;
        right = _intervals.at(symbol).second;
    }

    return probability;
}

std::string decode(
    std::unordered_map<char, double>& _probabilities,
    std::unordered_map<char, std::pair<double, double>>& _intervals,
    double _probability
    )
{
    std::string text;

    //double probability = 1;
    double right = 1.0;
    double left = 0;
    
    int flag = 1;
    while(flag)
    {
        build_interval_table(_probabilities, _intervals, right, left);

        //std::cout << probability << " - ";
        std::cout << "[" << left << ", " << right << "]" << std::endl;

        for (auto &[key, value] : _intervals)
        {
            if(value.first <= _probability && _probability <= value.second)
            {
                text.push_back(key);
                left = _intervals.at(key).first;
                right = _intervals.at(key).second;

                if(key == '!')
                    flag = 0;

                break;
            }
        }
    }

    std::cout << text << std::endl;
    return text;
}
*/

int main()
{
    using namespace std;

    string text = "abaabaaca!";
    cout << text << endl;
                 //abaabaacaaaaab!
    unordered_map<char, double> probabilities;
    unordered_map<char, pair<double, double>> intervals;

    arithmetic_coding_probability(text, probabilities);
     
    double res = get_code(text, probabilities, intervals);
    cout << res << endl;
    
    decode(probabilities, intervals, res);

    return 0;
}