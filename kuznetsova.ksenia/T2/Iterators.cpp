#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <limits>
#include <complex>
#include <sstream>

namespace nspace {
    const double EPS = 1e-9;

    struct DataStruct {
        unsigned long long key1 = 0;
        std::complex<double> key2{ 0.0, 0.0 };
        std::string key3;
    };

    std::istream& parseNumber(std::istream& in, unsigned long long& dest) {
        std::string token;
        in >> token;

        if (token.empty()) {
            in.setstate(std::ios::failbit);
            return in;
        }

        std::string numStr = token;
        if (token.back() == 'd') {
            numStr = token.substr(0, token.length() - 1);
        }
        else if (token.length() >= 2 && token.substr(token.length() - 2) == "ll") {
            numStr = token.substr(0, token.length() - 2);
        }
        else if (token.length() >= 3 && token.substr(token.length() - 3) == "ull") {
            numStr = token.substr(0, token.length() - 3);
        }

        std::istringstream iss(numStr);
        unsigned long long value;
        if (!(iss >> value)) {
            in.setstate(std::ios::failbit);
            return in;
        }

        dest = value;
        return in;
    }

    std::istream& parseComplex(std::istream& in, std::complex<double>& dest) {
        char hash, c, open;
        in >> hash >> c >> open;

        if (hash != '#' || c != 'c' || open != '(') {
            in.setstate(std::ios::failbit);
            return in;
        }

        double real, imag;
        if (!(in >> real >> imag)) {
            in.setstate(std::ios::failbit);
            return in;
        }

        char close;
        in >> close;
        if (close != ')') {
            in.setstate(std::ios::failbit);
            return in;
        }

        dest = std::complex<double>(real, imag);
        return in;
    }

    std::istream& parseQuotedString(std::istream& in, std::string& dest) {
        char quote;
        in >> quote;

        if (quote != '"') {
            in.setstate(std::ios::failbit);
            return in;
        }

        std::getline(in, dest, '"');
        return in;
    }

    std::istream& operator>>(std::istream& in, DataStruct& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;

        char openParen;
        in >> openParen;
        if (openParen != '(') {
            in.setstate(std::ios::failbit);
            return in;
        }

        DataStruct input;
        bool hasKey1 = false, hasKey2 = false, hasKey3 = false;

        for (int i = 0; i < 3; i++) {
            char colon;
            in >> colon;
            if (colon != ':') {
                in.setstate(std::ios::failbit);
                return in;
            }

            std::string key;
            in >> key;

            if (key == "key1") {
                in >> colon;
                if (colon != ':') {
                    in.setstate(std::ios::failbit);
                    return in;
                }
                parseNumber(in, input.key1);
                hasKey1 = true;
            }
            else if (key == "key2") {
                in >> colon;
                if (colon != ':') {
                    in.setstate(std::ios::failbit);
                    return in;
                }
                char next;
                in >> next;
                in.putback(next);

                if (next == '#') {
                    parseComplex(in, input.key2);
                }
                else {
                    unsigned long long dummy;
                    parseNumber(in, dummy);
                    input.key2 = std::complex<double>(static_cast<double>(dummy), 0.0);
                }
                hasKey2 = true;
            }
            else if (key == "key3") {
                in >> colon;
                if (colon != ':') {
                    in.setstate(std::ios::failbit);
                    return in;
                }
                parseQuotedString(in, input.key3);
                hasKey3 = true;
            }
            else {
                in.setstate(std::ios::failbit);
                return in;
            }
        }

        char closeParen;
        in >> closeParen;
        if (closeParen != ')') {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (hasKey1 && hasKey2 && hasKey3) {
            dest = input;
        }
        else {
            in.setstate(std::ios::failbit);
        }

        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
        out << "(:key1 " << src.key1;

        if (std::abs(src.key2.imag()) < EPS && std::abs(src.key2.real() - static_cast<unsigned long long>(src.key2.real())) < EPS) {
            out << ":key2 " << static_cast<unsigned long long>(src.key2.real()) << "d";
        }
        else {
            out << ":key2 #c(" << std::fixed << std::setprecision(1)
                << src.key2.real() << " " << src.key2.imag() << ")";
        }

        out << ":key3 \"" << src.key3 << "\":)";

        return out;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) {
            return a.key1 < b.key1;
        }

        double modA = std::abs(a.key2);
        double modB = std::abs(b.key2);
        if (std::abs(modA - modB) > EPS) {
            return modA < modB;
        }

        return a.key3.length() < b.key3.length();
    }
}

int main() {
    std::vector<nspace::DataStruct> data;

    while (std::cin) {
        nspace::DataStruct temp;
        if (std::cin >> temp) {
            data.push_back(temp);
        }
        else if (std::cin.eof()) {
            break;
        }
        else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    if (data.empty()) {
        std::cerr << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 1;
    }

    std::sort(data.begin(), data.end(), nspace::compareData);

    for (const auto& item : data) {
        std::cout << item << "\n";
    }

    return 0;
}
