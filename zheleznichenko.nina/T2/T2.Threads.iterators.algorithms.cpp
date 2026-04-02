#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <complex>
#include <iomanip>

struct DataStruct {
  unsigned long long key1;
  std::complex<double> key2;
  std::string key3;
};

bool isBinaryNumber(const std::string& s) {
  if (s.length() < 3) return false;
  if (s[0] != '0') return false;
  if (s[1] != 'b' && s[1] != 'B') return false;
  for (size_t i = 2; i < s.length(); ++i) {
    if (s[i] != '0' && s[i] != '1') return false;
  }
  return true;
}

unsigned long long binaryToULL(const std::string& s) {
  unsigned long long result = 0;
  for (size_t i = 2; i < s.length(); ++i) {
    result = result * 2 + (s[i] - '0');
  }
  return result;
}

std::complex<double> parseComplex(const std::string& s) {
  size_t openParen = s.find('(');
  size_t closeParen = s.find(')');

  if (openParen == std::string::npos || closeParen == std::string::npos) {
    return std::complex<double>(0, 0);
  }

  std::string content = s.substr(openParen + 1, closeParen - openParen - 1);

  size_t spacePos = content.find(' ');
  if (spacePos == std::string::npos) {
    return std::complex<double>(0, 0);
  }

  std::string realPart = content.substr(0, spacePos);
  std::string imagPart = content.substr(spacePos + 1);

  double real = std::stod(realPart);
  double imag = std::stod(imagPart);

  return std::complex<double>(real, imag);
}

std::istream& operator>>(std::istream& is, DataStruct& data) {
  std::string line;
  if (!std::getline(is, line)) {
    return is;
  }

  size_t openParen = line.find('(');
  size_t closeParen = line.find(')');

  if (openParen == std::string::npos || closeParen == std::string::npos) {
    is.setstate(std::ios::failbit);
    return is;
  }

  std::string content = line.substr(openParen + 1, closeParen - openParen - 1);

  data.key1 = 0;
  data.key2 = std::complex<double>(0, 0);
  data.key3 = "";

  std::stringstream ss(content);
  std::string part;

  while (std::getline(ss, part, ':')) {
    if (part.empty()) continue;

    size_t spacePos = part.find(' ');
    if (spacePos == std::string::npos) continue;

    std::string fieldName = part.substr(0, spacePos);
    std::string fieldValue = part.substr(spacePos + 1);

    fieldValue.erase(0, fieldValue.find_first_not_of(" \t"));
    fieldValue.erase(fieldValue.find_last_not_of(" \t") + 1);

    if (fieldName == "key1") {
      if (isBinaryNumber(fieldValue)) {
        data.key1 = binaryToULL(fieldValue);
      }
      else {
        is.setstate(std::ios::failbit);
        return is;
      }
    }
    else if (fieldName == "key2") {
      if (fieldValue.find("#c") == 0) {
        data.key2 = parseComplex(fieldValue);
      }
      else {
        is.setstate(std::ios::failbit);
        return is;
      }
    }
    else if (fieldName == "key3") {
      if (fieldValue.length() >= 2 &&
        fieldValue.front() == '"' &&
        fieldValue.back() == '"') {
        data.key3 = fieldValue.substr(1, fieldValue.length() - 2);
      }
      else {
        is.setstate(std::ios::failbit);
        return is;
      }
    }
  }

  return is;
}

std::ostream& operator<<(std::ostream& os, const DataStruct& data) {
  os << "(:key1 ";

  os << "0b";
  if (data.key1 == 0) {
    os << "0";
  }
  else {
    unsigned long long temp = data.key1;
    std::string binary;
    while (temp > 0) {
      binary = (temp % 2 == 0 ? "0" : "1") + binary;
      temp /= 2;
    }
    os << binary;
  }

  os << ":key2 ";

  os << "#c(" << std::fixed << std::setprecision(1)
    << data.key2.real() << " " << data.key2.imag() << ")";

  os << ":key3 \"" << data.key3 << "\":)";

  return os;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
  if (a.key1 != b.key1) {
    return a.key1 < b.key1;
  }

  double modA = std::abs(a.key2);
  double modB = std::abs(b.key2);
  if (modA != modB) {
    return modA < modB;
  }

  return a.key3.length() < b.key3.length();
}

int main() {
  std::ifstream inputFile("input.txt");
  if (!inputFile.is_open()) {
    std::cerr << "Ошибка: не удалось открыть файл input.txt" << '\n';
    return 1;
  }

  std::istream_iterator<DataStruct> itBegin(inputFile);
  std::istream_iterator<DataStruct> itEnd;

  std::vector<DataStruct> data;

  std::copy(itBegin, itEnd, std::back_inserter(data));

  if (data.empty()) {
    std::cerr << "Warning: Could not read any records" << '\n';
  }

  std::sort(data.begin(), data.end(), compareDataStruct);

  std::ofstream outputFile("output.txt");
  if (!outputFile.is_open()) {
    std::cerr << "Error: Failed to open output.txt file" << '\n';
    return 1;
  }

  std::ostream_iterator<DataStruct> itOut(outputFile, "\n");
  std::copy(data.begin(), data.end(), itOut);

  std::cout << "The program completed successfully! The result is written to output.txt." << std::endl;

  return 0;
}
