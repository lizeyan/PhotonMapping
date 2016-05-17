#ifndef BASE_H
#define BASE_H
#include <regex>
#include <fstream>
#include <random>
//#define DEBUG
//#define LOG
const double EPS = 1e-10;
const double PI = 3.141592654;
const double PI_Double = 6.283185307;
const double PI_Half = 1.5707963267948966192313216916398;
const double Bound = 1e35;
const std::regex entryReg("^\\s*([\\w\\d-_.](\\s*[\\w\\d-_.])*)\\s*:\\s*([\\w\\d-_./](\\s*[\\w\\d-_./])*)\\s*(#.*)?");
const std::regex objEntryReg("^\\s*(\\w+)(\\s+)([\\w\\d-_./](\\s*[\\w\\d-_./])*)\\s*(#.*)?");
const std::regex simpleFaceReg("\\s*[\\d.]+\\s+[\\d.]+\\s+[\\d.]+\\s*");
const std::regex fullFaceReg("\\s*([\\d.]+\\\\[\\d.]+\\\\[\\d.]+)\\s+([\\d.]+\\\\[\\d.]+\\\\[\\d.]+)\\s+([\\d.]+\\\\[\\d.]+\\\\[\\d.]+)\\s*");
const std::regex noMaterialFaceReg ("\\s*([\\d.]+\\\\[\\d.]+)\\s+([\\d.]+\\\\[\\d.]+)\\s+([\\d.]+\\\\[\\d.]+)\\s*");
const std::regex noNormalFaceReg ("\\s*([\\d.]+\\\\\\\\[\\d.]+)\\s+([\\d.]+\\\\\\\\[\\d.]+)\\s+([\\d.]+\\\\\\\\[\\d.]+)\\s*");
const int keyRank = 1;
const int valueRank = 3;
const int commentRank = 5;
const std::regex commentReg("^#.*$");
const std::regex spaceReg("^\\s*$");
const std::regex startReg("^start\\s+([\\w\\d-_.](\\s*[\\w\\d-_.])*)\\s*(#.*)?$");
const int elementNameRank = 1;
const std::regex endReg("^end\\s*(#.*)?$");

#endif // BASE_H
