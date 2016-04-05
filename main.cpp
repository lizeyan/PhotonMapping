#include "condutor.h"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
using namespace std;
#define CMDLINE
int main(int argc, char *argv[])
{
#ifdef CMDLINE
    string inputFileName, outputFileName;
    if (argc < 2)
        inputFileName = string("scene.txt");
    else
        inputFileName = string(argv[1]);
    if (argc < 3)
        outputFileName = string ("res.bmp");
    else
        outputFileName = string (argv[2]);
    std::ifstream input(inputFileName.c_str ());
    if (!input.is_open ())
    {
        cout << "can't open input file:" << inputFileName << endl;
        return 1;
    }
    unique_ptr<Condutor> condutor(new Condutor(input));
    condutor->run ();
    condutor->save (outputFileName);
    input.close ();
    return 0;
#endif
}
