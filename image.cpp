#include "image.h"
#include <QColor>
#include <algorithm>
#include <mutex>
std::mutex imageMutex;
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
    imageMutex.lock ();
    image->setPixel (x, y, toQColor (color).rgba ());
    imageMutex.unlock ();
}

bool Image::save (const std::string &fileName)
{
    return image->save (fileName.c_str (), "JPG", 100);
}

QColor Image::toQColor (const Color &c)
{
    return QColor(std::max(std::min(255 * c.arg(R), 255.0f), 0.0f), std::max(std::min(255 * c.arg(G), 255.0f), 0.0f), std::max(std::min(255 * c.arg(B), 255.0f), 0.0f));
}

Color Image::smoothPixel (float x, float y)
{
    return Color ();
}

                                                                    
