//
//  main.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "../tclap/CmdLine.h"
#include "Filter.h"

int main (int argc, const char * argv[])
{
    try {
        
        TCLAP::CmdLine cmd("Keep every nth point or keep a specific percentage randomly.", ' ', "none", false);

        TCLAP::ValueArg<int> percentageArg("p","percentage","Keep percentage of the points randmonly",true,10,"int");
        TCLAP::ValueArg<int> nthArg("n","nth","Keep every nth point",true,10,"int");
        cmd.xorAdd(nthArg, percentageArg );

        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .xyz file", true, "", "output file", cmd);
        
        cmd.parse(argc,argv);
        
        Filter f(inputArg.getValue().c_str(),outputArg.getValue().c_str());

        if (percentageArg.isSet())
            f.copy_percentage(percentageArg.getValue());
        else
            f.copy_nth(nthArg.getValue());
        
        std::cout << "Written " << f.getLineCount() << " lines" << std::endl;
        
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    return 0;
}

