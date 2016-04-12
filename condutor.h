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
const int MAX_RAY_TRACING_DEPTH = 30;
class ifstream;
class ofstream;
class Condutor
{
public:
    Condutor(std::ifstream& _input);
    ~Condutor ();
    void run ();
    bool save (const std::string& output = std::string());
    inline void setOutputFileName(const std::string& output) {outputName = output;}
    Image* image () {return _image.get ();}
    Camera* camera () {return _camera.get ();}
    PhotonMap* photonMap () {return _photonMap.get ();}
    std::vector<std::unique_ptr<Light> >& lights () {return _lights;}
    std::vector<std::unique_ptr<Object> >& objects () {return _objects;}
    std::vector<std::unique_ptr<Material> >& materials () {return _materials;}
protected:
    void init ();
    void readScene ();

    void addElement (const std::string& name, const std::string& content);
private:
    std::string outputName;
    std::ifstream& input;//initialized in constructor
    std::unique_ptr<Image> _image;//initialized with camera in readSecne
    std::vector<std::unique_ptr<Light> > _lights;//innitialized in readScene
    std::vector<std::unique_ptr<Object> > _objects;//innitialized in readScene
    std::vector<std::unique_ptr<Material> > _materials;//initialized in readScene.
    std::unique_ptr<Camera> _camera;//innitialized in readScene
    std::unique_ptr<PhotonMap> _photonMap;//initialized in readScene
};

#endif // CONDUTOR_H
