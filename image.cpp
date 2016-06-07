#include "image.h"
#include <QColor>
#include <algorithm>
#include <iostream>
#include <mutex>
std::mutex imageMutex;
Image::Image(int width, int height): _width(width), _height(height)
{
    image.reset (new QImage(width, height, QImage::Format_RGB32));
    _data = new Color*[width];
    for (int i = 0; i < width; ++i)
        _data[i] = new Color[height];
    fill ();
}

void Image::smooth (double l)
{
    static const std::array<int, 8> dx {{}};
    static const std::array<int, 8> dy {{}};
    static auto legal = [this] (int x, int y)->bool {return x >= 0 && y >= 0 && x < _width && y < _height;};
    std::vector<std::vector<Color> > dataTmp;
    for (int i = 0; i < _width; ++i)
    {
        std::vector<Color> column;
        for (int j = 0; j < _height; ++j)
            column.push_back (_data[i][j]);
        dataTmp.push_back (std::move (column));
    }
    for (int i = 0; i < _width; ++i)
        for (int j = 0; j < _height; ++j)
        {
            double dis = 0;
            Color res;
            for (int k = 0; k < 8; ++k)
            {
                int x = i + dx[k], y = j + dy[k];
                if (legal (x, y))
                {
                    res += dataTmp[x][y];
                    dis += distance (dataTmp[i][j], dataTmp[x][y]);
                }
            }
            if (dis > l)
            {
                res += dataTmp[i][j];
                _data[i][j] = res * 0.1111111111111;
            }
        }
}


Image::Image (std::string fileName)
{
    image.reset (new QImage(fileName.c_str ()));
    _width = image->width();
    _height = image->height ();
    _data = new Color*[_width];
    for (int i = 0; i < _width; ++i)
        _data[i] = new Color[_height];
    _width = image->width();
    _height = image->height ();
    for (int i = 0; i < _width; ++i)
        for (int j = 0; j < _height; ++j)
        {
            _data[i][j][0] = qRed(image->pixel(i, j)) / 255.0;
            _data[i][j][1] = qGreen (image->pixel(i, j)) / 255.0;
            _data[i][j][2] = qBlue(image->pixel(i, j)) / 255.0;
        }
}

Image::~Image ()
{
    for (int i = 0; i < _width; ++i)
        delete []_data[i];
    delete []_data;
}

void Image::fill (const Color &color)
{
    for (int i = 0; i < _width; ++i)
        for (int j = 0; j < _width; ++j)
            _data[i][j] = color;
}

void Image::setPixel (int x, int y, const Color &color)
{
    _data[x][y] = color;
}

bool Image::save (const std::string &fileName)
{
    smooth (1.0);
    for (int i = 0; i < _width; ++i)
        for (int j = 0; j < _height; ++j)
            image->setPixel(i, j, toQColor (_data[i][j]).rgb ());
    return image->save (fileName.c_str (), "JPG", 100);
}

QColor Image::toQColor (const Color &c)
{
    return QColor(std::max(std::min(255.0 * c.arg(R), 255.0), 0.0), std::max(std::min(255.0 * c.arg(G), 255.0), 0.0), std::max(std::min(255.0 * c.arg(B), 255.0), 0.0));
}

Color Image::smoothPixel (double x, double y)
{
    x *= _width - 1;
    y *= _height - 1;
    int x1 = x, y1 = y, x2 = x1 + 1, y2 = y1 + 1;
    if (x2 >= _width)
        x2 -= _width;
    if (y2 >= _height)
        y2 -= _height;
    return _data[x1][y1] * (x2 - x) * (y2 - y) + _data[x2][y2] * (x - x1) * (y - y1) + _data[x1][y2] * (x2 - x) * (y - y1) + _data[x2][y1] * (x - x1) * (y2 - y);
}

                                                                    
