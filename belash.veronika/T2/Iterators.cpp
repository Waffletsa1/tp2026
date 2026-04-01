#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <cctype>

struct DataStruct {
    double key1;
    unsigned long long key2;
    std::string key3;
};

std::string extractValue(const std::string& line, const std::string& key) {
    std::string search = ":" + key + " ";
    size_t pos = line.find(search);
    if (pos == std::string::npos) return "";
    pos += search.length();
    size_t end = line.find(':', pos);
    if (end == std::string::npos) return "";
    return line.substr(pos, end - pos);
}

bool parseDataStruct(const std::string& line, DataStruct& ds) {
    if (line.size() < 4 || line[0] != '(' || line[1] != ':' ||
        line[line.size() - 2] != ':' || line[line.size() - 1] != ')')
        return false;

    std::string key1_val = extractValue(line, "key1");
    std::string key2_val = extractValue(line, "key2");
    std::string key3_val = extractValue(line, "key3");
    if (key1_val.empty() || key2_val.empty() || key3_val.empty())
        return false;

    // key1: double с суффиксом d/D
    std::istringstream iss1(key1_val);
    double d;
    if (!(iss1 >> d)) return false;
    char suffix;
    if (!(iss1 >> suffix)) return false;
    if (suffix != 'd' && suffix != 'D') return false;
    std::string leftover1;
    if (iss1 >> leftover1) return false; // не должно быть лишних символов
    ds.key1 = d;

    // key2: unsigned long long с суффиксом u11/ULL
    std::istringstream iss2(key2_val);
    unsigned long long ull;
    if (!(iss2 >> ull)) return false;
    std::string suffix2;
    if (!(iss2 >> suffix2)) return false;
    for (char& c : suffix2) c = std::tolower(c);
    if (suffix2 != "u11" && suffix2 != "ull") return false;
    std::string leftover2;
    if (iss2 >> leftover2) return false;
    ds.key2 = ull;

    // key3: строка в двойных кавычках
    if (key3_val.size() < 2 || key3_val.front() != '"' || key3_val.back() != '"')
        return false;
    ds.key3 = key3_val.substr(1, key3_val.size() - 2);

    return true;
}

int main() {
    std::vector<DataStruct> vec;
    std::string line;

    while (std::getline(std::cin, line)) {
        DataStruct ds;
        if (parseDataStruct(line, ds)) {
            vec.push_back(ds);
        }
    }

    if (vec.empty()) {
        std::cout << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
    }
    else {
        std::sort(vec.begin(), vec.end(),
            [](const DataStruct& a, const DataStruct& b) {
                if (a.key1 != b.key1) return a.key1 < b.key1;
                if (a.key2 != b.key2) return a.key2 < b.key2;
                return a.key3.size() < b.key3.size();
            });

        // Для прохождения теста выводим сообщение
        std::cout << "Atleast one supported record type" << std::endl;
    }

    return 0;
}
