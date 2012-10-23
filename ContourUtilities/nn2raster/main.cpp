
#include <CgalProcessor.h>
#include <tclap/CmdLine.h>

using namespace std;

int main(int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Performs Natural Neighbour interpolation from input points to a raster output. ", ' ', "none", false);
        
        TCLAP::ValueArg<int> sArg("s","smooth","Number of times to smooth",false,0,"int", cmd);
        TCLAP::ValueArg<int> dArg("d","densify","Number of times to densify",false,0,"int", cmd);

        TCLAP::SwitchArg uSwitch("u","unsafe","Smooth without attempting to respect bathymetric safety constraint", cmd, false);
        
        
        vector<string> allowed;
		allowed.push_back("NN");
		allowed.push_back("LP");
		allowed.push_back("LIN");
		TCLAP::ValuesConstraint<string> allowedVals( allowed );
		TCLAP::ValueArg<string> methodArg("m","method","interpolation method to use (Natural Neighbour, LaPlace interpolant or LINear)",false,"LP",&allowedVals, cmd);
        
        
//        TCLAP::MultiArg<double> levelArg("l", "level", "add a level to contour", true, "double", cmd );
        TCLAP::ValueArg<double> csArg("c","cellsize","Cellsize to be used for output raster, units as in source",true,10,"double", cmd);
        
        
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .tiff file", true, "", "output file", cmd);
        cmd.parse(argc,argv);
        
        smoothAlg alg;
        if (methodArg.getValue() == "NN")
            alg = NN;
        else if (methodArg.getValue() == "LP")
            alg = LP;
        else if (methodArg.getValue() == "LIN")
            alg = LIN;
        
        CgalProcessor cp(inputArg.getValue().c_str());
        for (int i = 0; i < sArg.getValue(); ++i)   cp.smooth(alg, !uSwitch.getValue());
        for (int i = 0; i < dArg.getValue(); ++i)   cp.densify(alg);

//        std::cout << "set:" << alg <<", LIN: " <<LIN<<std::endl;
        cp.toRaster(outputArg.getValue().c_str(), csArg.getValue(), alg, cp.MinX(), cp.MaxX(), cp.MinY(), cp.MaxY());
        
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
    
    return 0;
}

