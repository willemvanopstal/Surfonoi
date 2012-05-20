
#include "CgalProcessor.h"
#include <tclap/CmdLine.h>

using namespace std;

int main(int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Simplify a pointcloud based on the drop-heuristic algorithm", ' ', "none", false);

        TCLAP::ValueArg<double> tArg("t","threshold","Simplification threshold",true,0.2,"float", cmd);
        TCLAP::SwitchArg uSwitch("u","unsafe","Simplify without attempting to respect bathymetric safety constraint", cmd, false);
        
        vector<string> allowed;
		allowed.push_back("LIN");
		allowed.push_back("NN");
		allowed.push_back("LP");
		TCLAP::ValuesConstraint<string> allowedVals( allowed );
		TCLAP::ValueArg<string> methodArg("m","method","interpolation method to use (LINear, Natural Neighbour or LaPlace interpolant)",false,"LIN",&allowedVals, cmd);
        
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .xyz file", true, "", "output file", cmd);
        cmd.parse(argc,argv);
        
        smoothAlg alg;
        if (methodArg.getValue() == "LIN")
            alg = LIN;
        else if (methodArg.getValue() == "NN")
            alg = NN;
        else if (methodArg.getValue() == "LP")
            alg = LP;
        
        CgalProcessor cp(inputArg.getValue().c_str());
        cp.simplify(alg, tArg.getValue(), uSwitch.getValue(), false);
        cp.dumpXYZ(outputArg.getValue().c_str());

    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
    
    return 0;
}

