#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <iomanip>
#include <complex>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cstdlib>

struct DataStruct {
    long long key1;
    std::complex<double> key2;
    std::string key3;
};

struct DelimiterIO {
    char exp;
};

struct SllIO {
    long long& ref;
};

struct CmpIO {
    std::complex<double>& ref;
};

struct StrIO {
    std::string& ref;
};

class iofmtguard {
public:
    explicit iofmtguard(std::basic_ios<char>& s);
    ~iofmtguard();
private:
    std::basic_ios<char>& s_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, SllIO&& dest);
std::istream& operator>>(std::istream& in, CmpIO&& dest);
std::istream& operator>>(std::istream& in, StrIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);

bool compareDataStruct(const DataStruct& a, const DataStruct& b);

int main() {
    std::vector<DataStruct> data;

    while (true) {
        std::copy(std::istream_iterator<DataStruct>(std::cin),
                  std::istream_iterator<DataStruct>(),
                  std::back_inserter(data));

        if (std::cin.eof()) {
            break;
        }
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(data.begin(), data.end(),
              std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return EXIT_SUCCESS;
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in, false);
    if (!sentry) return in;

    char c = in.get();
    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, SllIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    long long val;
    if (!(in >> val)) {
        return in;
    }

    char c1, c2;
    if (in.get(c1)) {
        if (in.get(c2)) {
            if ((c1 == 'l' || c1 == 'L') && (c2 == 'l' || c2 == 'L')) {
            } else {
                in.unget();
                in.unget();
            }
        } else {
            in.unget();
        }
    }
    dest.ref = val;
    return in;
}

std::istream& operator>>(std::istream& in, CmpIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    double re, im;
    in >> DelimiterIO{ '#' } >> DelimiterIO{ 'c' } >> DelimiterIO{ '(' } >> re >> im >> DelimiterIO{ ')' };
    if (in) {
        dest.ref = std::complex<double>(re, im);
    }
    return in;
}

std::istream& operator>>(std::istream& in, StrIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    in >> DelimiterIO{ '"' };
    std::getline(in, dest.ref, '"');
    if (!in) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct tmp;
    bool f1 = false, f2 = false, f3 = false;

    in >> std::ws;
    if (in.peek() != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }
    in.get(); // '('

    while (in && in.peek() != ')') {
        in >> std::ws;
        if (in.peek() != ':') {
            in.setstate(std::ios::failbit);
            return in;
        }
        in.get(); // ':'

        std::string key;
        char c;
        while (in.get(c) && std::isalpha(c)) {
            key += c;
        }
        if (key != "key1" && key != "key2" && key != "key3") {
            in.setstate(std::ios::failbit);
            return in;
        }
        if (c != ' ') {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (key == "key1" && !f1) {
            in >> SllIO{ tmp.key1 };
            f1 = true;
        } else if (key == "key2" && !f2) {
            in >> CmpIO{ tmp.key2 };
            f2 = true;
        } else if (key == "key3" && !f3) {
            in >> StrIO{ tmp.key3 };
            f3 = true;
        } else {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (!in) return in;

        in >> std::ws;
        if (in.peek() == ':') {
            continue;
        } else if (in.peek() == ')') {
            break;
        } else {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    if (in.peek() == ')') {
        in.get(); // ')'
    } else {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (f1 && f2 && f3) {
        dest = std::move(tmp);
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    iofmtguard guard(out);
    out << "(:key1 " << src.key1
        << ":key2 #c(" << src.key2.real() << " " << src.key2.imag() << ")"
        << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    double ma = std::abs(a.key2);
    double mb = std::abs(b.key2);
    const double eps = 1e-9;
    if (std::abs(ma - mb) > eps) return ma < mb;
    return a.key3.size() < b.key3.size();
}

iofmtguard::iofmtguard(std::basic_ios<char>& s)
    : s_(s), fill_(s.fill()), precision_(s.precision()), fmt_(s.flags()) {}

iofmtguard::~iofmtguard() {
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}

