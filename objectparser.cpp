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
            throw std::logic_error ("unexcepted key type in scene, object model" + key);
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
    _faces.clear ();
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
	bool firstFace = true;
	Vec3 vecSum;
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
			vecSum += vec;
            _vertices.push_back (vec);
            _normals.push_back (std::make_pair (Vec3 (), 0));
        }
        else if (key == std::string ("f"))
        {
			if (firstFace)
			{
				firstFace = false;
				vecSum *= 1.0 / static_cast<double> (_vertices.size ());
//                std::cout << "rotate:" << _rotate << std::endl;
				for (auto& point: _vertices)
                    //std::cout << "point:" << point << std::endl;
                    //std::cout << "after move:" << point << std::endl;
                    point = _size * rotate (point - vecSum, _rotate) + _center;
                    //std::cout << "after rotate:" << point << std::endl;
                    //point = point * _size + _center;
                    //std::cout << "after move again:" << point << std::endl;
			}
            if (std::regex_match(value, simpleFaceReg))
            {
                int ranks[3];
                for (int i = 0; i < 3; ++i)
                    valueStream >> ranks[i];
//                _condutor->objects ().push_back (std::move(std::unique_ptr<Triangle>(new Triangle(_vertices[ranks[0] - 1], _vertices[ranks[1] - 1], _vertices[ranks[2] - 1], material, _condutor, true))));
                _faces.push_back (std::make_tuple (ranks[0] - 1, ranks[1] - 1, ranks[2] - 1));
                Vec3 normal = standardize (cross (_vertices[ranks[0] - 1] - _vertices[ranks[1] - 1], _vertices[ranks[0] - 1] - _vertices[ranks[2] - 1]));
                for (std::size_t i = 0; i < 3; ++i)
                {
                    _normals[ranks[i] - 1].first += normal;
                    _normals[ranks[i] - 1].second += 1;
                }
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
    for (const auto& entry: _faces)
    {
        _condutor->objects ().push_back (std::move (std::unique_ptr<Triangle> (new Triangle (_vertices[std::get<0>(entry)], _vertices[std::get<1> (entry)], _vertices[std::get<2> (entry)], material, _condutor, _normals[std::get<0>(entry)].first * (1.0 / double( _normals[std::get<0>(entry)].second)), _normals[std::get<1>(entry)].first * (1.0 / double( _normals[std::get<1>(entry)].second)), _normals[std::get<2>(entry)].first * (1.0 / double( _normals[std::get<2>(entry)].second))))));
    }
}
