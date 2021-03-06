#include "base.h"
#include "condutor.h"
#include "raytracer.h"
#include "photontracer.h"
#include "photonmap.h"
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
//#define LOG
#ifdef LOG
std::mutex infoMutex;
std::ofstream info ("rayTrace.info");
#endif
std::mt19937 rd(time(0));
extern Utility::Timer<> timer;
Condutor::Condutor(std::ifstream& _input): input(_input), _parser (new ObjectParser(this)), _lb (std::array<double, 3>{{-Bound, -Bound, -Bound}}), _rt (std::array<double, 3>{{+Bound, +Bound, +Bound}})
{
    init ();
    readScene ();
#ifdef DEBUG
    Log << "read scene" << std::endl;
#endif
    _kdTree.reset (new TriangleTree(this));
    _photonMap.reset (new PhotonMap(this));
    _causticPhotonMap.reset (new PhotonMap(this));
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
void Condutor::globalPhotonEmitting ()
{
    for (const auto& light: _lights)
    {
        size_t photonNum = camera ()->brightnessValue ()* model (light->color ());
        size_t lastSize = photonMap ()->size ();
        double scalePhotonColor = 1.0 / static_cast<double> (photonNum);
        auto emitting = [this, lastSize, photonNum, &light, scalePhotonColor] ()
        {
            while (photonMap ()->size () - lastSize < photonNum)
            {
                Photon photon = light->emitPhoton ();
                photon.color *= scalePhotonColor;
                PhotonTracer photonTracer (photon, this);
                photonTracer.run ();
            }
        };
        std::vector<std::thread> threads (_maxThreadNum);
        for (auto& th: threads)
            th = std::thread (emitting);
        for (auto& th: threads)
            th.join ();
    }
    _photonMap->build ();
}

void Condutor::causticPhotonEmitting ()
{
	size_t exceptedObNum = 0;
	for (const auto& object: _objects)
		if (object->material()->reflection() >= EPS || object->material()->refraction () >= EPS)
			++exceptedObNum;
    for (const auto& light: _lights)
    {
        size_t photonNum = camera ()->brightnessValue ()* model (light->color ()) * camera ()-> causticScale ();
		if (photonNum == 0)
			continue;
        double scalePhotonColor = 1.0 / static_cast<double> (photonNum);
		for (const auto& object: _objects)
        {
			if (object->material()->reflection() < EPS && object->material()->refraction () < EPS)
				continue;
            size_t lastSize = causticPhotonMap ()->size ();
			size_t limit = photonNum / exceptedObNum;
            while (causticPhotonMap ()->size () - lastSize < limit)
            {
                Photon photon = light->emitPhoton (object.get ());
                photon.color *= scalePhotonColor;
                PhotonTracer photonTracer (photon, this, 0, true);
                photonTracer.run ();
            }
        }
    }
}

void Condutor::run ()
{
//    Vec3 o = Vec3(std::array<double,3>{{100, 5, 100}});
//    Vec3 link  = Vec3(std::array<double,3>{{5, 5, 1}}) - o;
//    PhotonTracer photonTrace (std::make_pair(o, link), this, Vec3(std::array<double,3>{{1, 1, 1}}));
//    photonTrace.run ();
//    /*
#ifdef PHOTON_MAPPING
    std::cout << "emitting photon" << std::endl;
    globalPhotonEmitting ();
    std::cout << "global photon map size: " << _photonMap->size () << std::endl;
    causticPhotonEmitting ();
    std::cout << "caustic photon map size:" << _causticPhotonMap->size() << std::endl;
#endif
//    */
    std::cout << "ray tracing" << std::endl;
//    ray tracing
//    Vec3 o = camera ()->center ();
//    Vec3 o = Vec3(std::array<double,3>{{10.0001, 6.83336, 100.833}});
//    Vec3 link = Vec3(std::array<double,3>{{0.2, 0.5, 0.05}}) - o;
//    Vec3 link = Vec3 (std::array<double, 3>{{-0.768221, -0.640184, 0}});
//    RayTracer rt (std::make_pair(o, link), this);
//    RayTracer rt (camera ()->emitRay (2.08, 0.68), this);
//    rt.run ();
//    std::cout << rt.color () << std::endl;
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
	bool ignore = false;
    while (!input.eof())
    {
        std::getline (input, line);
        if (line == std::string("/*"))
		{
			ignore = true;
		}
		else if (line == std::string("*/"))
		{
			ignore = false;
		}
		if (ignore)
			continue;
        if (std::regex_match(line, commentReg))
        {
            //comment
        }
        else if (name.empty () && std::regex_match (line, matchRes, entryReg))
        {
            std::string key = matchRes[keyRank];
            std::stringstream valueStream  (matchRes[valueRank]);
            if (key == std::string ("leftBottom"))
            {
                valueStream >> _lb[0] >> _lb[1] >> _lb[2];
            }
            else if (key == std::string ("rightTop"))
            {
                valueStream >> _rt[0] >> _rt[1] >> _rt[2];
            }
            else if (key == std::string ("maxThreadNum"))
                valueStream >> _maxThreadNum;
            else
                std::cout << "warning: unexcepted entry, key=" << key << std::endl;
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
    for (unsigned i = 0; i < _maxThreadNum; ++i)
        handlePart (i);
}
void Condutor::oneThreadPerRay ()
{
    double dx = 1.0 / camera ()->dpi ();
    double dy = 1.0 / camera ()->dpi ();
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

void Condutor::handlePart (unsigned remainder)
{
    double dx = 1.0 / camera ()->dpi ();
    double dy = 1.0 / camera ()->dpi ();
    unsigned imageX = camera ()->width () * camera ()->dpi ();
    unsigned imageY = camera ()->height () * camera ()->dpi ();
    double scale = 1.0 / double (_camera->raysPerPixel ());
    for (unsigned x = 0; x < imageX; ++x)
    {
        for (unsigned y = 0; y < imageY; ++y)
        {
            if (! ((x + y) % _maxThreadNum == remainder))
                continue;
            Color res;
            for (unsigned i = 0; i < _camera->raysPerPixel (); ++i)
            {
                Ray ray = _camera->emitRay(x * dx, y * dy);
                RayTracer t (ray, this);
                t.run ();
                res += t.color ();
            }
#ifdef PHOTON_MAPPING
//            std::cout << "x, y:" << x << "," << y << " color:" << res << std::endl;
#endif
            _image->setPixel (x, y, res * scale);
#ifdef LOG
            infoMutex.lock ();
            info << x << " " << y << " :" << res * scale << std::endl;
            infoMutex.unlock ();
#endif
        }
    }
}

void Condutor::fixedNumTheads ()
{
    std::vector<std::unique_ptr<std::thread> > threads (_maxThreadNum);
    for (unsigned i = 0; i < _maxThreadNum; ++i)
    {
        threads[i] = std::move (std::unique_ptr<std::thread>(new std::thread(&Condutor::handlePart, this, i)));
    }
    for (unsigned i = 0; i < _maxThreadNum; ++i)
        threads[i]->join ();
}

void Condutor::handleSegments (int x1, int x2)
{
    if (x1 < 0 || x2 > camera ()->width () || x1 > x2)
        throw std::logic_error ("wrong arguments, in Condutor::handleSegments");
    double dx = 1.0 / camera ()->dpi ();
    double dy = 1.0 / camera ()->dpi ();
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
            rayTracer.run ();
            Color c = rayTracer.color ();
#ifdef DEBUG
            logMutex.lock ();
            Log << "result:" << c << std::endl;
            logMutex.unlock ();
#endif
            std::cout << "x, y:" << x << "," << y << " color:" << c << std::endl;
            _image->setPixel (x, y, c);
        }
    }
}
