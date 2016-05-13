#include "base.h"
#include "condutor.h"
#include "raytracer.h"
#include <string>
#include <regex>
#include <utility>
#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <algorithm>
#ifdef DEBUG
std::ofstream Log("rayTrace.log");
std::mutex logMutex;
#endif
std::mt19937 rd(time(0));
Condutor::Condutor(std::ifstream& _input): input(_input), _parser (new ObjectParser(this))
{
    init ();
    readScene ();
#ifdef DEBUG
    Log << "read scene" << std::endl;
#endif
    _kdTree.reset (new KdTree(this));
#ifdef DEBUG

    Log << *_camera << std::endl;
    for (const auto& light: _lights)
        Log << *light << std::endl;
    for (const auto& object: _objects)
        Log << *object << std::endl;
    for (const auto& material: _materials)
        Log << *material << std::endl;
#endif
}

Condutor::~Condutor ()
{

}

void Condutor::run ()
{
    std::cout << "running" << std::endl;
//    ray tracing
//    Vec3 o = camera ()->center ();
//    Vec3 o = Vec3(std::array<double,3>{{10.0001, 6.83336, 100.833}});
//    Vec3 link = Vec3(std::array<double,3>{{38, 21, 10}}) - o;
//    Vec3 link = Vec3 (std::array<double, 3>{{-0.768221, -0.640184, 0}});
//    RayTracer rt (std::make_pair(o, link), this);
//    std::cout << rt.run () << std::endl;
//    singleThread ();
    fixedNumTheads ();

    std::cout << "finished" << std::endl;
}

bool Condutor::save (const std::string &outputFileName)
{
    std::string output = outputFileName;
    if (output.empty ())
        output = outputName;
    return _image->save (output);
}

void Condutor::init ()
{
    _lights.clear ();
    _objects.clear ();
}

void Condutor::readScene ()
{
    std::string line;
    std::smatch matchRes;
    std::string name;
    std::string content;
    while (!input.eof())
    {
        std::getline (input, line);
        if (std::regex_match(line, commentReg))
        {
            //comment
        }
        else if (std::regex_match(line, spaceReg))
        {
            //spaces
        }
        else if (std::regex_match(line, matchRes, startReg))//start line
        {
            if (!name.empty ())
            {
                std::cout << "warning: unexpected start line, name is " << name << std::endl;
            }
            name = matchRes[elementNameRank].str();
        }//end line
        else if (std::regex_match(line, endReg))
        {
            addElement (name, content);
            name.clear ();
            content.clear ();
        }
        else//content line
        {
            if (!name.empty ())
            {
                if (!content.empty ())
                    content += '\n';
                content += line;
            }
        }
    }
}

void Condutor::addElement (const std::string &name, const std::string &content)
{
    if (name == std::string("camera"))
    {
        if (_camera)
        {
            std::cout << "warning: redefine a camera, new content:" << std::endl << content << std::endl;
        }
        _camera.reset (new Camera(content));
        _image.reset (new Image(_camera->width() * _camera->dpi (), _camera->height() * _camera->dpi ()));
    }
    else if (name == std::string("light"))
    {
        std::unique_ptr<Light> ptr(Light::produce(content, this));
        _lights.push_back (std::move(ptr));
    }
    else if (name == std::string("object"))
    {
        std::unique_ptr<Object> ptr(Object::produce(content, this));
        _objects.push_back (std::move(ptr));
    }
    else if (name == std::string("material"))
    {
        std::unique_ptr<Material> ptr (Material::produce (content));
        _materials.push_back (std::move(ptr));
    }
    else if (name == std::string ("model"))
    {
        _parser->parse (content);
    }
    else
    {
        std::cout << "warning: unknown name, name = " << name << ", content is" << std::endl << content << std::endl;
    }
}

void Condutor::singleThread ()
{
    handleSegments (0, camera ()->width ());
}
void Condutor::oneThreadPerRay ()
{
    double dx = 1.0f / camera ()->dpi ();
    double dy = 1.0f / camera ()->dpi ();
    int imageX = camera ()->width () * camera ()->dpi ();
    int imageY = camera ()->height () * camera ()->dpi ();
    std::vector<std::unique_ptr<RayTracer> > lineTracers;
    std::vector<std::unique_ptr<std::thread> > lineThreads;
    for (int x = 0; x < imageX; ++x)
    {
        lineTracers.clear ();
        lineThreads.clear ();
        for (int y = 0; y < imageY; ++y)
        {
            lineTracers.push_back (std::move (std::unique_ptr<RayTracer>(new RayTracer(_camera->emitRay(x * dx, y * dy), this))));
            lineThreads.push_back (std::move (std::unique_ptr<std::thread>(new std::thread(&RayTracer::run , lineTracers.back ().get()))));
        }
        for (int y = 0; y < imageY; ++y)
        {
            lineThreads[y]->join ();
            _image->setPixel (x, y, lineTracers[y]->color ());
        }
    }
}

void Condutor::fixedNumTheads ()
{
    int segment = camera ()->width () / maxThreadNum;
    std::array<std::unique_ptr<std::thread>, maxThreadNum> threads;
    for (int i = 0; i < maxThreadNum - 1; ++i)
    {
        threads[i] = std::move (std::unique_ptr<std::thread>(new std::thread(&Condutor::handleSegments, this, i * segment, (i + 1) * segment)));
    }
    threads[maxThreadNum - 1] = std::move (std::unique_ptr<std::thread>(new std::thread(&Condutor::handleSegments, this, (maxThreadNum - 1) * segment, camera ()->width ())));
    for (int i = 0; i < maxThreadNum; ++i)
        threads[i]->join ();
}

void Condutor::handleSegments (int x1, int x2)
{
    if (x1 < 0 || x2 > camera ()->width () || x1 > x2)
        throw std::logic_error ("wrong arguments, in Condutor::handleSegments");
    double dx = 1.0f / camera ()->dpi ();
    double dy = 1.0f / camera ()->dpi ();
    int imageX1 = x1 * camera ()->dpi ();
    int imageX2 = x2 * camera ()->dpi ();
    int imageY = camera ()->height () * camera ()->dpi ();
    for (int x = imageX1; x < imageX2; ++x)
    {
        for (int y = 0; y < imageY; ++y)
        {
            Ray ray = _camera->emitRay(x * dx, y * dy);
            RayTracer rayTracer (ray, this);
#ifdef DEBUG
            logMutex.lock ();
            Log << "emitted ray, source:" << ray.first << " direction:" << ray.second << std::endl;
            Log << "x:" << x << ", y:" << y << std::endl;
            logMutex.unlock ();
#endif
            Color c = rayTracer.run ();
#ifdef DEBUG
            logMutex.lock ();
            Log << "result:" << c << std::endl;
            logMutex.unlock ();
#endif
            _image->setPixel (x, y, c);
        }
    }
}
