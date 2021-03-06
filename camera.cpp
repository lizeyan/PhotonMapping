#include "camera.h"
#include "base.h"
#include <utility>
#include <sstream>
#include <cmath>
#include <regex>
#include <stdexcept>
extern std::mt19937 rd;
Camera::Camera(const std::string content)
{
    init ();
    analyseContent (content);
    preHandle ();
    if (!check ())
        throw std::logic_error("incorrect camera argument");
}

void Camera::preHandle ()
{
    _dx = standardize (_dx);
    _dy = standardize (_dy);
    _normal = standardize (_normal);
    _r2 = _radius * _radius;
    _apertureDis = std::uniform_real_distribution<> (-_radius, _radius);
}

void Camera::display (std::ostream &os) const
{
    os << "{";
    os << "_width:" << _width << ";";
    os << "_height:" << _height << ";";
    os << "_dx:" << _dx << ";";
    os << "_dy" << _dy << ";";
    os << "_center:" << _center << ";";
    os << "_normal:" << _normal << ";";
    os << "_focus:" << _focus << ";";
    os << "_dpi:" << _dpi << ";";
    os << "}";
}

void Camera::init ()
{
    _width = 1000;
    _height = 1000;
    _dx = Vec3 (std::array<double, 3>{{1, 0, 0}});
    _dy = Vec3 (std::array<double, 3>{{0, 1, 0}});
    _center = Vec3 ();
    _normal = Vec3 (std::array<double, 3>{{0, 0, 1}});
    _focus = 1;
    _dpi = 1;
    _environment = Color ();
}

bool Camera::check ()
{
    if (raysPerPixel () <= 0 || K () <= 0)
        return false;
    return fabs(dot(_dx, _normal)) < EPS && fabs(dot (_dy, _normal)) < EPS;
}

std::ostream& operator<< (std::ostream& os, const Camera& camera)
{
    camera.display (os);
    return os;
}

Ray Camera::emitRay (double x, double y)
{
    x = (_width >> 1) - x;
    y = (_height >> 1) - y;
    Vec3 a = _focus * _normal;
    Vec3 b = x * _dx + y * _dy;
    double deltaX, deltaY;
    do
    {
        deltaX = _apertureDis (rd);
        deltaY = _apertureDis (rd);
    }
    while (deltaX * deltaX + deltaY * deltaY >= _r2);
    Vec3 c = deltaX * _dx + deltaY * _dy;
    return std::make_pair (_center - c, a + b + c);
}

void Camera::analyseContent (const std::string &content)
{
    std::stringstream contentStream (content);
    std::string line;
    std::smatch matchRes;
    while (!contentStream.eof ())
    {
        std::getline (contentStream, line);
        if (!std::regex_match (line, matchRes, entryReg))
        {
            continue;
        }
        std::string key = matchRes[keyRank].str ();
        std::string value = matchRes[valueRank].str ();
        std::stringstream valueStream (value);
        if (key == std::string ("size"))
        {
            valueStream >> _width >> _height;
        }
        else if (key == std::string ("dx"))
        {
            valueStream >> _dx[0] >> _dx[1] >> _dx[2];
        }
        else if (key == std::string ("dy"))
        {

            valueStream >> _dy[0] >> _dy[1] >> _dy[2];
        }
        else if (key == std::string ("environment"))
        {
            valueStream >> _environment[0] >> _environment[1] >> _environment[2];
        }
        else if (key == std::string ("center"))
        {

            valueStream >> _center[0] >> _center[1] >> _center[2];
        }
        else if (key == std::string ("normal"))
        {

            valueStream >> _normal[0] >> _normal[1] >> _normal[2];
        }
        else if (key == std::string ("focus"))
        {
            valueStream >> _focus;
        }
		else if (key == std::string ("causticScale"))
		{
			valueStream >> _causticScale;	
		}
        else if (key == std::string ("dpi"))
        {
            valueStream >> _dpi;
        }
        else if (key == std::string ("radius"))
        {
            valueStream >> _radius;
        }
        else if (key == std::string ("K"))
            valueStream >> _K;
        else if (key == std::string ("brightnessValue"))
            valueStream >> _brightnessValue;
        else if (key == std::string ("raysPerPixel"))
            valueStream >> _raysPerPixel;
        else
        {
            throw std::logic_error ("unexcepted key type in scene, camera");
        }
    }
}

