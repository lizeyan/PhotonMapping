#ifndef BASE_H
#define BASE_H
#include <regex>
#include <fstream>
const float EPS = 1e-2;
const std::regex entryReg("^\\s*([\\w\\d-_.](\\s*[\\w\\d-_.])*)\\s*:\\s*([\\w\\d-_.](\\s*[\\w\\d-_.])*)\\s*(#.*)?");
const int keyRank = 1;
const int valueRank = 3;
const int commentRank = 5;
const std::regex commentReg("^#.*$");
const std::regex spaceReg("^\\s*$");
const std::regex startReg("^start\\s+([\\w\\d-_.](\\s*[\\w\\d-_.])*)\\s*(#.*)?$");
const int elementNameRank = 1;
const std::regex endReg("^end\\s*(#.*)?$");

#endif // BASE_H
