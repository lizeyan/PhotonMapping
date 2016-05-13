#ifndef OBJECTPARSER_H
#define OBJECTPARSER_H
#include "ray.h"
#include <vector>
#include <string>
class Condutor;
class Material;
class ObjectParser
{
public:
    ObjectParser(Condutor* condutor);
    void parse (const std::string& content);
protected:
    void analyseFile (const std::string& fileName, Material* material);
    void clean ();
private:
    Condutor* _condutor;
    Vec3 _center;
    Vec3 _rotate;
    double _size;
    std::vector<Vec3> _vertices;
    std::vector<Vec3> _normals;
};

#endif // OBJECTPARSER_H
