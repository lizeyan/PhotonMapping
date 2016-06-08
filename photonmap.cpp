#include "photonmap.h"
#include "condutor.h"
#include <utility>
#include <stack>
#include <queue>
#include <iterator>
PhotonMap::PhotonMap(Condutor* condutor):_condutor (condutor)
{

}
void PhotonMap::build ()
{
    _root.reset (createKdTree (std::begin(_photons), std::end (_photons)));
}

PhotonBox* PhotonMap::createKdTree (PhotonVec::iterator begin, PhotonVec::iterator end, int depth)
{
    static auto xCmp = [] (const std::unique_ptr<Photon>& a, const std::unique_ptr<Photon>& b)->bool {return a->point.arg (0) < b->point.arg(0);};
    static auto yCmp = [] (const std::unique_ptr<Photon>& a, const std::unique_ptr<Photon>& b)->bool {return a->point.arg (1) < b->point.arg(1);};
    static auto zCmp = [] (const std::unique_ptr<Photon>& a, const std::unique_ptr<Photon>& b)->bool {return a->point.arg (2) < b->point.arg(2);};
    unsigned length = distance (begin, end);
    if (!length)
        return nullptr;
    unsigned halfLength = length >> 1;
    auto medianIt = begin + halfLength;
    int dimension = depth % 3;
    switch (dimension)
    {
    case 0:
        std::nth_element (begin, medianIt, end, xCmp);
        break;
    case 1:
        std::nth_element (begin, medianIt, end, yCmp);
        break;
    case 2:
        std::nth_element (begin, medianIt, end, zCmp);
        break;
    default:
        break;
    }
    return new PhotonBox (createKdTree (begin, medianIt, depth + 1), createKdTree (medianIt + 1, end, depth), medianIt->get (), dimension, (*medianIt)->point.arg (dimension));
}

std::vector<std::pair<Photon*, double> > PhotonMap::search (const Vec3 &point) const
{
    typedef std::pair<Photon*, double> PhotonPair;
    auto cmp = [] (const PhotonPair& a, const PhotonPair& b) -> bool {return a.second < b.second;};
    std::priority_queue<PhotonPair, std::vector<PhotonPair>, decltype (cmp)> knn (cmp);
    std::stack<PhotonBox*> s;
    s.push (_root.get ());
    double d = Bound;
    while (!s.empty ())
    {
        PhotonBox* top = s.top ();
        if (top == nullptr)
            continue;
        s.pop ();
        double dis = point.arg (top->dimension ()) - top->split ();
        if (dis < 0)
        {
            s.push (top->lc ());
            if (d + dis > 0)
                s.push (top->rc ());
        }
        else
        {
            s.push (top->rc ());
            if (dis < d)
                s.push (top->lc ());
        }
        if (fabs (dis) < d)
        {
            knn.push (std::make_pair (top->photon (), distance (point, top->photon ()->point)));
            if (knn.size () > _condutor->camera ()->K ())
                knn.pop ();
            d = knn.top ().second;
        }
    }
	std::vector<PhotonPair> res;
	while (!knn.empty ())
	{
		res.push_back (knn.top());
		knn.pop ();
	}
	return std::move (res);
}

void PhotonMap::store (const Photon &photon)
{
    Vec3 lb = _condutor->lb (), rt = _condutor->rt (), p = photon.point;
    if (p.arg (0) <= rt.arg (0) && p.arg (0) >= lb.arg (0) && p.arg (1) <= rt.arg (1) && p.arg (1) >= lb.arg (1) && p.arg (2) <= rt.arg (2) && p.arg (2) >= lb.arg (2))
        _photons.push_back (std::move (std::unique_ptr<Photon> (new Photon(photon))));
}
