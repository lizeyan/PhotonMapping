#include "condutor.h"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <chrono>
using namespace std;
#define CMDLINE
Utility::Timer<> timer;
int main(int argc, char *argv[])
{
//    Vec3 ro (std::array<double, 3> {{90 * Deg2Rad, 0, 90 * Deg2Rad}});
//    Vec3 vec (std::array<double, 3> {{-0.5, 0.5, 0.5}});
//    std::cout << rotate (vec, ro) << std::endl;
//    return 0;
    timer.start ();
#ifdef CMDLINE
    string inputFileName, outputFileName;
    if (argc < 2)
        inputFileName = string("scene.txt");
    else
        inputFileName = string(argv[1]);
    if (argc < 3)
        outputFileName = string ("res.jpg");
    else
        outputFileName = string (argv[2]);
    std::ifstream input(inputFileName.c_str ());
    if (!input.is_open ())
    {
        cout << "can't open input file:" << inputFileName << endl;
        return 1;
    }
    unique_ptr<Condutor> condutor(new Condutor(input));
    condutor->setOutputFileName (outputFileName);
    condutor->run ();
    condutor->save (outputFileName);
    input.close ();
    cout << "total time cost: " << double(timer.report ().count()) / 1000 << " seconds" << endl;
    return 0;
#endif
}
