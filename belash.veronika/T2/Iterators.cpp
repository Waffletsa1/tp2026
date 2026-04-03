#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <cctype>

struct DataStruct
{
    double key1;
    unsigned long long key2;
    std::string key3;
};

class iofmtguard
{
public:
    explicit iofmtguard(std::basic_ios<char>& s) :
        s_(s),
        width_(s.width()),
        precision_(s.precision()),
        fill_(s.fill()),
        fmt_(s.flags())
    {
    }

    ~iofmtguard()
    {
        s_.width(width_);
        s_.fill(fill_);
        s_.precision(precision_);
        s_.flags(fmt_);
    }

private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    std::streamsize precision_;
    char fill_;
    std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.size() < 4 || line[0] != '(' || line[1] != ':' ||
            line[line.size() - 2] != ':' || line[line.size() - 1] != ')')
            continue;

        std::istringstream iss(line);
        char dummy;
        iss >> dummy; // '('
        iss >> dummy; // ':'

        DataStruct temp;
        bool f1 = false, f2 = false, f3 = false;
        std::string key;
        bool valid = true;

        while (iss >> std::ws && !iss.eof() && valid)
        {
            iss >> dummy; // читаем ':'
            iss >> key;   // читаем имя поля

            if (key == "key1" && !f1)
            {
                double val; char suf;
                if (!(iss >> val >> suf) || (suf != 'd' && suf != 'D'))
                {
                    valid = false;
                }
                else
                {
                    temp.key1 = val;
                    f1 = true;
                }
            }
            else if (key == "key2" && !f2)
            {
                unsigned long long val; std::string suf;
                if (!(iss >> val >> suf))
                {
                    valid = false;
                }
                else
                {
                    for (char& c : suf) c = std::tolower(static_cast<unsigned char>(c));
                    if (suf != "ull") valid = false;
                    else { temp.key2 = val; f2 = true; }
                }
            }
            else if (key == "key3" && !f3)
            {
                char quote; std::string val;
                if (!(iss >> quote) || quote != '"')
                {
                    valid = false;
                }
                else
                {
                    std::getline(iss, val, '"');
                    temp.key3 = val;
                    f3 = true;
                }
            }
            else
            {
                valid = false;
            }
        }

        if (valid && f1 && f2 && f3)
        {
            dest = temp;
            return in;
        }
    }

    in.setstate(std::ios::failbit);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
        return out;
    }

    iofmtguard fmtguard(out);
    out << "(:key1 ";
    out << std::fixed;
    out << std::setprecision(1);
    out << src.key1;
    out << "d:key2 ";
    out << src.key2;
    out << "ull:key3 \"";
    out << src.key3;
    out << "\":)";
    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b)
{
    if (a.key1 != b.key1)
    {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2)
    {
        return a.key2 < b.key2;
    }
    return a.key3.length() < b.key3.length();
}

int main()
{
    std::vector<DataStruct> vec;
    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(vec)
    );

    std::sort(vec.begin(), vec.end(), compareDataStruct);

    std::copy(
        vec.begin(),
        vec.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
