#include "primitive.h"

Primitive::Primitive(Condutor* condutor): _condutor (condutor)
{

}

void Primitive::display (std::ostream &os) const
{
    os << "{";
    os << "_condutor:" << _condutor << ";";
    os << "}";
}

std::ostream& operator<< (std::ostream& os, const Primitive& primitive)
{
    primitive.display (os);
    return os;
}
