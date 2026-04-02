#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <limits>


struct DataStruct {
    double key1;
    long long key2;
    std::string key3;
};


struct DelimiterIO {
    char expected;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    char c;
    in >> c;
    if (in && c != dest.expected) {
        in.setstate(std::ios::failbit);
    }
    return in;
}


struct KeyIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, KeyIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    dest.ref.clear();
    char c;
    while (in.get(c) && std::isalnum(static_cast<unsigned char>(c))) {
        dest.ref += c;
    }
    return in;
}


struct StringIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    char c;
    if (!(in >> c) || c != '"') {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::getline(in, dest.ref, '"');
    return in;
}


struct DoubleSciIO {
    double& ref;
};

std::istream& operator>>(std::istream& in, DoubleSciIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    in >> std::ws;
    std::string temp;
    char c;

    while (in.get(c) && c != ':') {
        temp += c;
    }
    in.putback(':');

    bool hasExp = temp.find('e') != std::string::npos ||
                  temp.find('E') != std::string::npos;

    if (!hasExp) {
        in.setstate(std::ios::failbit);
        return in;
    }

    try {
        dest.ref = std::stod(temp);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }

    return in;
}


struct LongLongIO {
    long long& ref;
};

std::istream& operator>>(std::istream& in, LongLongIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    in >> std::ws;
    std::string temp;
    char c;

    while (in.get(c) && c != ':') {
        temp += c;
    }
    in.putback(':');

    if (temp.size() >= 2) {
        char last = std::tolower(temp.back());
        char secondLast = std::tolower(temp[temp.size() - 2]);
        if (last == 'l' && secondLast == 'l') {
            temp.pop_back();
            temp.pop_back();
            try {
                dest.ref = std::stoll(temp);
                return in;
            }
            catch (...) {
                in.setstate(std::ios::failbit);
            }
        }
        else {
            in.setstate(std::ios::failbit);
        }
    }
    else {
        in.setstate(std::ios::failbit);
    }

    return in;
}


std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    DataStruct temp;

    if (!(in >> DelimiterIO{'('})) {
        return in;
    }

    char firstColon;
    if (!(in >> firstColon) || firstColon != ':') {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (int i = 0; i < 3; ++i) {
        std::string key;
        in >> KeyIO{key};

        if (!in) {
            return in;
        }

        if (key == "key1") {
            in >> DoubleSciIO{temp.key1};
        }
        else if (key == "key2") {
            in >> LongLongIO{temp.key2};
        }
        else if (key == "key3") {
            in >> StringIO{temp.key3};
        }
        else {
            in.setstate(std::ios::failbit);
            break;
        }

        if (!in) {
            return in;
        }

        char colon;
        if (!(in >> colon) || colon != ':') {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    char closeBracket;
    if (!(in >> closeBracket) || closeBracket != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = temp;
    return in;
}

class iofmtguard {
private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::ios::fmtflags fmt_;
public:
    iofmtguard(std::basic_ios<char>& s) : s_(s) {
        width_ = s.width();
        fill_ = s.fill();
        precision_ = s.precision();
        fmt_ = s.flags();
    }

    ~iofmtguard() {
        s_.width(width_);
        s_.fill(fill_);
        s_.precision(precision_);
        s_.flags(fmt_);
    }
};

std::ostream& operator<<(std::ostream& out, const DataStruct& source) {
    iofmtguard guard(out);

    out << "(:key1 ";
    out << std::scientific << std::setprecision(1) << source.key1;
    out << ":key2 " << source.key2 << "ll";
    out << ":key3 \"" << source.key3 << "\":)";

    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (std::abs(a.key1 - b.key1) > 1e-9) {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2) {
        return a.key2 < b.key2;
    }
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> data;
    while (!std::cin.eof()) {
        std::copy(
            std::istream_iterator<DataStruct>(std::cin),
            std::istream_iterator<DataStruct>(),
            std::back_inserter(data)
        );
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        std::begin(data),
        std::end(data),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );
    return 0;
}
