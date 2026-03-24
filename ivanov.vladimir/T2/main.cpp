#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <limits>

struct DataStruct {
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;

    DataStruct() : key1(0), key2(0), key3("") {}
    DataStruct(const DataStruct&) = default;
    DataStruct(DataStruct&&) = default;
    DataStruct& operator=(const DataStruct&) = default;
    DataStruct& operator=(DataStruct&&) = default;
    ~DataStruct() = default;
};

struct DelimiterIO {
    char exp;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c = '0';
    in >> c;
    if (in && (std::tolower(c) != std::tolower(dest.exp))) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct UllLitIO {
    unsigned long long& ref;
};

std::istream& operator>>(std::istream& in, UllLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    unsigned long long val;
    if (!(in >> val)) return in;
    char s1, s2, s3;
    if (!(in >> s1 >> s2 >> s3) ||
        (std::tolower(s1) != 'u' || std::tolower(s2) != 'l' || std::tolower(s3) != 'l')) {
        in.setstate(std::ios::failbit);
    } else {
        dest.ref = val;
    }
    return in;
}

struct UllBinIO {
    unsigned long long& ref;
};

std::istream& operator>>(std::istream& in, UllBinIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char p1, p2;
    if (!(in >> p1 >> p2) || (p1 != '0' || std::tolower(p2) != 'b')) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string bin;
    char c;
    while (in.get(c) && (c == '0' || c == '1')) {
        bin += c;
    }
    if (!bin.empty()) {
        dest.ref = std::stoull(bin, nullptr, 2);
    } else {
        in.setstate(std::ios::failbit);
    }
    if (in && c != ':') in.putback(c);
    return in;
}

struct StringIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    if (!(in >> c) || c != '"') {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::getline(in, dest.ref, '"');
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    DataStruct temp;
    if (!(in >> DelimiterIO{'('})) return in;

    for (int i = 0; i < 3; ++i) {
        if (!(in >> DelimiterIO{':'})) return in;
        std::string label;
        char c;
        while (in.get(c) && c != ' ') label += c;

        if (label == "key1") in >> UllLitIO{temp.key1};
        else if (label == "key2") in >> UllBinIO{temp.key2};
        else if (label == "key3") in >> StringIO{temp.key3};
        else { in.setstate(std::ios::failbit); return in; }
    }

    if (!(in >> DelimiterIO{':'} >> DelimiterIO{')'})) return in;
    if (in) dest = std::move(temp);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;
    out << "(:key1 " << src.key1 << "ull:key2 0b";
    if (src.key2 == 0) out << "0";
    else {
        std::string s;
        unsigned long long temp = src.key2;
        while (temp > 0) {
            s += (temp % 2 ? '1' : '0');
            temp /= 2;
        }
        std::reverse(s.begin(), s.end());
        if (src.key2 == 1 && s == "1") out << "0";
        out << s;
    }
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareDS(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> data;
    while (!std::cin.eof()) {
        std::copy(std::istream_iterator<DataStruct>(std::cin),
                  std::istream_iterator<DataStruct>(),
                  std::back_inserter(data));
        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    std::sort(data.begin(), data.end(), compareDS);
    std::copy(data.begin(), data.end(), std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}

