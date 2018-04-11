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

#include "DoubleBuffer.h"
#include <tclap/CmdLine.h>

int main(int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Perform double buffering on linestrings in input shapefile", ' ', "none", false);
        TCLAP::ValueArg<double> distanceArg("d","distance","Buffering distance",true,1,"float", cmd);
        TCLAP::ValueArg<std::string> attributeArg("a","attribute","Shapefile attribute that stores the real-valued elevation. Defaults to 'depth'",false,"depth","string", cmd);
        TCLAP::SwitchArg bilSwitch("s","alternative","Apply GEOS BufferInputLine simplification instead of double buffering", cmd, false);
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to shapefile", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to shapefile dir", true, "", "output file", cmd);
        cmd.parse(argc,argv);
        
        simplificationMethod m = DOUBLEBUFF;
        if (bilSwitch.getValue()) m = GEOSBIL;
        
        DoubleBuffer dBuffer( inputArg.getValue().c_str(), outputArg.getValue().c_str(), attributeArg.getValue().c_str() );
        dBuffer.perform(m, distanceArg.getValue());
        
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
    
    return 0;
}

