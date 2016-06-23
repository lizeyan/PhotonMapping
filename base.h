#ifndef BASE_H
#define BASE_H
#include "Utility/timer.hpp"
#include <regex>
#include <fstream>
#include <random>
//#define PHOTON_MAPPING
//#define DEBUG
//#define LOG
const double EPS = 1e-10;
const double PI = 3.141592654;
const double PI_Double = 6.283185307;
const double PI_Half = 1.5707963267948966192313216916398;
const double Deg2Rad = 0.01745329251994329576923690768489;
const double Bound = 1e4;
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
const int MAX_RAY_TRACING_DEPTH = 5;
const int MAX_PHOTON_TRACING_DEPTH = 5;
const double k_wp = 1.1;
const size_t finalGatheringK = 100;
const double causticMaxRadius = 0.1;
const double textureEps = 0.01;
#endif // BASE_H
