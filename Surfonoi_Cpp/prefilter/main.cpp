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
#include <vector>
#include <tclap/CmdLine.h>
#include "Filter.h"

int main (int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Keep every nth point, keep a specific percentage randomly or simply copy all points. Creates a .bounds file with the bounding box values.", ' ', "none", false);

        TCLAP::ValueArg<int> percentageArg("e","percentage","Keep percentage of the points randmonly",false,10,"int");
        TCLAP::ValueArg<int> nthArg("n","nth","Keep every nth point",false,10,"int");
        TCLAP::SwitchArg copyallSwitch("c","copy","Copy all points", false);
        
        std::vector<TCLAP::Arg*> copymodes;
        copymodes.push_back(&percentageArg);
        copymodes.push_back(&nthArg);
        copymodes.push_back(&copyallSwitch);
        
        cmd.xorAdd(copymodes);

        TCLAP::ValueArg<int> precArg("x","precision","Decimal precision in output file",false,2,"int",cmd);

        TCLAP::SwitchArg projectSwitch("p","project","Project x y values fron WGS84 to auto utm-zone", cmd, false);
        TCLAP::SwitchArg flipSignOnZSwitch("f","flip","flip the sign of z-values in output", cmd, false);
        TCLAP::SwitchArg flipXYSwitch("t","flipxy","Use if order in file is y-x-z or lat-long-z", cmd, false);

        
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .xyz file", true, "", "output file", cmd);
        
        cmd.parse(argc,argv);
        
        Filter f(inputArg.getValue().c_str(),outputArg.getValue().c_str(), projectSwitch.getValue(), flipSignOnZSwitch.getValue(), flipXYSwitch.getValue(), precArg.getValue());

        if (percentageArg.isSet())
            f.copy_percentage(percentageArg.getValue());
        else if (nthArg.isSet())
            f.copy_nth(nthArg.getValue());
        else
            f.copy_all();
        
        std::cout << "Written " << f.getLineCount() << " lines" << std::endl;
        
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    return 0;
}

