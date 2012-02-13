//
//  main.cpp
//  filter2Raster
//
//  Created by Ravi Peters on 2/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <string>

#include <tclap/CmdLine.h>

#include "Raster.h"

using namespace std;

int main (int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Input points are rasterized with specified method, no interpolation is done.", ' ', "none", false);
        TCLAP::ValueArg<double> csArg("c","cellsize","Cellsize to be used for raster, units as in source",true,10,"float", cmd);

        vector<string> allowed;
		allowed.push_back("min");
		allowed.push_back("max");
		allowed.push_back("avg");
		TCLAP::ValuesConstraint<string> allowedVals( allowed );
        
		TCLAP::ValueArg<string> methodArg("m","method","method to use (min, max or avg)",true,"avg",&allowedVals, cmd);
        TCLAP::ValueArg<string> crsArg("e", "epsg", "reference to EPSG", true, "EPSG:4326", "EPSG:____", cmd);

        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .tiff file", true, "", "output file", cmd);
        cmd.parse(argc,argv);
    
        Raster r(inputArg.getValue().c_str(), crsArg.getValue().c_str(), csArg.getValue());
        
        cout << r.maxx << " " << r.maxy << " " << r.minx << " " << r.miny << " " << r.lineCount <<endl;
        cout << r.dimx*r.dimy << endl;
        
        int16_t *cnt; cnt = new int16_t[r.dimx*r.dimy];
        float *val; val = new float[r.dimx*r.dimy];
        
        if (methodArg.getValue()=="min") {
            r.compute(val, cnt, MIN);
        } else if (methodArg.getValue()=="max") {
            r.compute(val, cnt, MAX);
        } else if (methodArg.getValue()=="avg") {
            r.compute(val, cnt, AVG);
        }
        
        r.write(val, outputArg.getValue().c_str());
        
        delete cnt;
        delete val;        
        
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
    
    return 0;
}

