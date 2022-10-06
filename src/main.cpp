#include <iostream>
#include "time.h"
#include "base.h"
#include "globalplacer.h"

using namespace std;


int main(int argc,char* argv[])
{
    srand(231);
    time_t start = time(0);
    //GlobalPlacer placer(argv[1], argv[2], "random", 0, "test");
     GlobalPlacer* placer;
    placer = new GlobalPlacer(argv[1], argv[2], "random", 0, "test", rand(), rand()); 
    placer->do_partial_placement();
    placer->write_output(argv[3], argv[4], argv[5]);
    //placer.do_partial_placement();
    /*
    placer.OverallPlacer->set_bstar_parameter(0.3,0.4,0.3);
    cout << "Set bstar parameter successfully" << endl;
    placer.OverallPlacer->set_random_seed(22);
    cout << "Set random seed successfully" << endl;
    placer.OverallPlacer->set_boundary_parameter(2000,2000);
    cout << "Set boundary constraints successfully" << endl;
    placer.OverallPlacer->set_profile_parameter(1, 0.9, 0.1);
    cout << "Set cost parameter successfully" << endl;
    cout << "Overall Placement Starts" << endl;
    placer.do_overall_placement();
    */
    /*
    cout << "Overall Placement Ends" << endl;
    cout << "time: "<< difftime(time(0), start) << endl;
    */
    return 0;
}

