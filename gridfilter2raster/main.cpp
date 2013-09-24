// Copyright (c) 2012, 2013
// Ravi Peters -- r.y.peters@tudelft.nl
// All rights reserved
// 
// This file is part of Surfonoi.
// 
// Surfonoi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Surfonoi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Surfonoi.  If not, see <http://www.gnu.org/licenses/>.

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
		allowed.push_back("cnt");
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
            r.write(MIN, val, outputArg.getValue().c_str());
        } else if (methodArg.getValue()=="max") {
            r.compute(val, cnt, MAX);
            r.write(MAX, val, outputArg.getValue().c_str());
        } else if (methodArg.getValue()=="avg") {
            r.compute(val, cnt, AVG);
            r.write(AVG, val, outputArg.getValue().c_str());
        } else if (methodArg.getValue()=="cnt") {
            r.compute(val, cnt, CNT);
            r.write(CNT, cnt, outputArg.getValue().c_str());
        }
        
        delete cnt;
        delete val;        
        
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
    
    return 0;
}

