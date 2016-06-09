#include "objectparser.h"
#include "condutor.h"
#include "object.h"
#include <fstream>
#include <stdexcept>
ObjectParser::ObjectParser(Condutor* condutor): _condutor (condutor)
{

}

void ObjectParser::parse (const std::string &content)
{
    std::stringstream contentStream (content);
    std::string line, modelFileName;
    Material* material= nullptr;
    std::smatch matchRes;
    while (!contentStream.eof ())
    {
        std::getline (contentStream, line);
        if (!std::regex_match (line, matchRes, entryReg))
        {
            continue;
        }
        std::string key = matchRes[keyRank].str ();
        std::string value = matchRes[valueRank].str ();
        std::stringstream valueStream (value);
        if (key == std::string("model"))
        {
            modelFileName = value;
        }
        else if (key == std::string ("material"))
        {
            int rank = 0;
            valueStream >> rank;
            material = _condutor->materials ().at (rank).get ();
        }
        else if (key == std::string ("center"))
        {
            valueStream >> _center[0] >> _center[1] >> _center[2];
        }
        else if (key == std::string ("rotate"))
        {
            valueStream >> _rotate[0] >> _rotate[1] >> _rotate[2];    
            _rotate *= Deg2Rad;
        }
        else if (key == std::string("size"))
        {
            valueStream >> _size;
        }
        else
        {
            throw std::logic_error ("unexcepted key type in scene, object model");
        }
    }
    if (material == nullptr)
        throw std::logic_error ("no material specified for model");
    analyseFile (modelFileName, material);
}

void ObjectParser::clean ()
{
    _vertices.clear ();
    _normals.clear ();
}

void ObjectParser::analyseFile (const std::string &fileName, Material *material)
{
    clean ();
    std::ifstream fin (fileName.c_str ());
    if (!fin.is_open ())
    {
        throw std::logic_error("cannot open obj file");
    }
    std::string line;
    std::smatch matchRes;
    Vec3 vec;
    while (!fin.eof ())
    {
        std::getline (fin, line);
        if (!std::regex_match (line, matchRes, objEntryReg))
            continue;
        std::string key = matchRes[keyRank].str ();
        std::string value = matchRes[valueRank].str ();
        std::stringstream valueStream (value);
        if (key == std::string ("v"))
        {
            valueStream >> vec[0] >> vec[1] >> vec[2];
            vec = _center + rotate (vec, _rotate) * _size;
            _vertices.push_back (vec);
        }
        else if (key == std::string ("f"))
        {
            if (std::regex_match(value, simpleFaceReg))
            {
                int ranks[3];
                for (int i = 0; i < 3; ++i)
                    valueStream >> ranks[i];
                _condutor->objects ().push_back (std::move(std::unique_ptr<Triangle>(new Triangle(_vertices[ranks[0] - 1], _vertices[ranks[1] - 1], _vertices[ranks[2] - 1], material, _condutor, true))));
            }
            else if (std::regex_match(value, fullFaceReg))
            {
                throw std::logic_error ("unfinished: objectParser, face reading");
            }
            else if (std::regex_match(value, noMaterialFaceReg))
            {
                throw std::logic_error ("unfinished: objectParser, face reading");
            }
            else if (std::regex_match(value, noNormalFaceReg))
            {
                throw std::logic_error ("unfinished: objectParser, face reading");
            }
        }
        else
        {
            throw std::logic_error ("unexcepted key type in obj file");
        }
    }
}
