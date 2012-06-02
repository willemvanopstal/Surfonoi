//
//  main.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <vector>
#include <tclap/CmdLine.h>
#include "Filter.h"

int main (int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Keep every nth point or keep a specific percentage randomly.", ' ', "none", false);

        TCLAP::ValueArg<int> percentageArg("e","percentage","Keep percentage of the points randmonly",false,10,"int");
        TCLAP::ValueArg<int> nthArg("n","nth","Keep every nth point",false,10,"int");
        TCLAP::SwitchArg copyallSwitch("c","copy","Copy all points", false);
        
        std::vector<TCLAP::Arg*> copymodes;
        copymodes.push_back(&percentageArg);
        copymodes.push_back(&nthArg);
        copymodes.push_back(&copyallSwitch);
        
        cmd.xorAdd(copymodes);


        TCLAP::SwitchArg projectSwitch("p","project","Project x y values fron WGS84 to auto utm-zone", cmd, false);
        TCLAP::SwitchArg flipSignOnZSwitch("f","flip","flip the sign of z-values in output", cmd, false);
        TCLAP::SwitchArg flipXYSwitch("t","flipxy","Use if order in file is y-x-z or lat-long-z", cmd, false);

        
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .xyz file", true, "", "output file", cmd);
        
        cmd.parse(argc,argv);
        
        Filter f(inputArg.getValue().c_str(),outputArg.getValue().c_str(), projectSwitch.getValue(), flipSignOnZSwitch.getValue(), flipXYSwitch.getValue());

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

