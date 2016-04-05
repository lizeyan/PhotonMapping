#ifndef CONDUTOR_H
#define CONDUTOR_H
#include "light.h"
#include "color.h"
#include "object.h"
#include "camera.h"
#include "image.h"
#include "photonmap.h"
#include <memory>
#include <vector>
#include <string>
class ifstream;
class ofstream;
class Condutor
{
public:
    Condutor(std::ifstream& _input);
    ~Condutor ();
    void run ();
    bool save (const std::string& output);
protected:
    void init ();
    void readScene ();
    Color rayTracing (const Ray& ray);
    void photonTracing (const Ray& ray);

    void addElement (const std::string& name, const std::string& content);
private:
    std::ifstream& input;//initialized in constructor
    std::unique_ptr<Image> image;//initialized with camera in readSecne
    std::vector<std::unique_ptr<Light> > lights;//innitialized in readScene
    std::vector<std::unique_ptr<Object> > objects;//innitialized in readScene
    std::unique_ptr<Camera> camera;//innitialized in readScene
    std::unique_ptr<PhotonMap> photonMap;//initialized in readScene
};

#endif // CONDUTOR_H
