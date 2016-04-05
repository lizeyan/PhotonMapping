#include "condutor.h"
#include <string>
#include <regex>
#include <utility>
#include <iostream>
#include <fstream>
Condutor::Condutor(std::ifstream& _input): input(_input)
{
    init ();
    readScene ();
}

Condutor::~Condutor ()
{

}

void Condutor::run ()
{
    //eimt photons and build photon map
    for (const auto& light: lights)
    {
        photonTracing(light->emitPhoton ());
    }
    //ray tracing
    for (int x = 0; x < camera->width(); ++x)
    {
        for (int y = 0; y < camera->height(); ++y)
        {
            Ray ray = camera->emitRay(x, y);
            Color color = rayTracing (ray);
            image->setPixel (x, y, color);
        }
    }
}

bool Condutor::save (const std::string &output)
{
    return image->save (output);
}

void Condutor::init ()
{
    lights.clear ();
    objects.clear ();
}

void Condutor::readScene ()
{
    std::string line;
    std::regex commentRex("^#.*$");
    std::regex spaceReg("^\\s*$");
    std::regex startReg("^start\\s+(\\w+)$");
    std::regex endReg("^end\\s*$");
    std::smatch matchRes;
    std::string name;
    std::string content;
    while (!input.eof())
    {
        std::getline (input, line);
        if (std::regex_match(line, commentRex))
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
            name = matchRes[1].str();
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
        if (camera)
        {
            std::cout << "warning: redefine a camera, new content:" << std::endl << content << std::endl;
        }
        camera.reset (new Camera(content));
        image.reset (new Image(camera->width(), camera->height()));
    }
    else if (name == std::string("object"))
    {
        std::unique_ptr<Light> ptr(Light::produce(content));
        lights.push_back (std::move(ptr));
    }
    else if (name == std::string("light"))
    {
        std::unique_ptr<Object> ptr(Object::produce(content));
        objects.push_back (std::move(ptr));
    }
    else if (name == std::string("photonMap"))
    {
        if (photonMap)
        {
            std::cout << "warning: redefine a photon map, content" << std::endl << content << std::endl;
        }
        photonMap.reset (new PhotonMap(content));
    }
    else
    {
        std::cout << "warning: unknown name, name = " << name << ", content is" << std::endl << content << std::endl;
    }
}

Color Condutor::rayTracing (const Ray &ray)
{
    return Color ();
}

void Condutor::photonTracing (const Ray &ray)
{

}
