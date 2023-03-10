// IDENTIFIER = 950181F63D0A883F183EC0A5CC67B19928FE896A



#include <iostream>
#include <getopt.h>
#include "Game.h"
using namespace std;

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    Game g;
    g.get_cl_options(argc, argv);
    if (g.get_routing_scheme() != 'Q' && g.get_routing_scheme() != 'S') {
        cerr << "No routing mode specified";
        exit(1);
    }

    g.fill_map();
    g.search();
    g.backtracing();
    g.output_path();

}



