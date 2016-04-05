#include "image.h"
#include <QColor>
Image::Image(int width, int height): _width(width), _height(height)
{
    image.reset (new QImage(width, height, QImage::Format_RGB32));
    fill ();
}

void Image::fill (const Color &color)
{
    image->fill (toQColor (color));
}

void Image::setPixel (int x, int y, const Color &color)
{
    image->setPixel (x, y, toQColor (color).rgba ());
}

bool Image::save (const std::string &fileName)
{
    return image->save (fileName.c_str (), "BMP", 100);
}

QColor Image::toQColor (const Color &c)
{
    return QColor(255 * c[R], 255 * c[G], 255 * c[B]);
}
