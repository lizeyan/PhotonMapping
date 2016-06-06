#ifndef CONDUTOR_H
#define CONDUTOR_H
#include "objectparser.h"
#include "light.h"
#include "color.h"
#include "object.h"
#include "triangleTree.h"
#include "camera.h"
#include "photonmap.h"
#include "image.h"
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
    bool save (const std::string& output = std::string());
    inline void setOutputFileName(const std::string& output) {outputName = output;}
    Image* image () {return _image.get ();}
    Camera* camera () {return _camera.get ();}
    std::vector<std::unique_ptr<Light> >& lights () {return _lights;}
    std::vector<std::unique_ptr<Object> >& objects () {return _objects;}
    std::vector<std::unique_ptr<Material> >& materials () {return _materials;}
    PhotonMap* photonMap () {return _photonMap.get ();}
    TriangleTree* kdTree () {return _kdTree.get ();}
    Vec3 lb () {return _lb;}
    Vec3 rt () {return _rt;}
protected:
    void init ();
    void readScene ();
    void singleThread ();
    void oneThreadPerRay ();
    void fixedNumTheads ();
    void handleSegments (int x1, int x2);
    void handlePart (int remainder);
    void addElement (const std::string& name, const std::string& content);
private:
    std::string outputName;
    std::ifstream& input;//initialized in constructor
    std::unique_ptr<Image> _image;//initialized with camera in readSecne
    std::vector<std::unique_ptr<Light> > _lights;//innitialized in readScene
    std::vector<std::unique_ptr<Object> > _objects;//innitialized in readScene
    std::vector<std::unique_ptr<Material> > _materials;//initialized in readScene.
    std::unique_ptr<Camera> _camera;//innitialized in readScene

    std::unique_ptr<TriangleTree> _kdTree;
    std::unique_ptr<ObjectParser> _parser;
    std::unique_ptr<PhotonMap> _photonMap;

    Vec3 _lb, _rt;
};

#endif // CONDUTOR_H
