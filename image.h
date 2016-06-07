#ifndef IMAGE_H
#define IMAGE_H
#include "color.h"
#include <string>
#include <memory>
#include <QImage>
class Image
{
public:
    Image (std::string fileName);
    Image(int width, int height);
    ~Image ();
    static QColor toQColor (const Color& c);
    void fill (const Color& color = Color ());
    Color pixel (int x, int y);
    Color smoothPixel (double x_radio, double y_radio);
    int width () {return _width;}
    int height () {return _height;}
    void setPixel (int x, int y, const Color& color);
    bool save (const std::string& fileName);
    void load (const std::string& fileName);
protected:
    void smooth (double l);
private:
    std::unique_ptr<QImage> image;
    Color** _data;
    int _width;
    int _height;
};

#endif // IMAGE_H
