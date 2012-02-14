//
//  main.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <tclap/CmdLine.h>
#include "Grid.h"

void f_min(Point& p, double& x, double& y, double& z)
{
    if (p.z()>z)
        p.set(x,y,z);
}

void f_max(Point& p, double& x, double& y, double& z)
{
    if (p.z()<z)
        p.set(x,y,z);
}

int main (int argc, const char ** argv)
{
    try {
        
        TCLAP::CmdLine cmd("A virtual grid with configurable cellsize is overlaid on the input points. For every grid cell the extreme in z (min or max) point is kept. Default is the max z value", ' ', "none", false);
        TCLAP::ValueArg<double> csArg("c","cellsize","Cellsize to be used",true,10,"float", cmd);
        TCLAP::SwitchArg reverseSwitch("r","reverse","Keep minima instead of maxima", cmd, false);
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .xyz file", true, "", "output file", cmd);
        cmd.parse(argc,argv);
        
        Grid g(inputArg.getValue().c_str(), csArg.getValue());
        
        if (!reverseSwitch.getValue())
            g.calcGrid(&f_max);
        else
            g.calcGrid(&f_min);
        
        g.write(outputArg.getValue().c_str());
        //g.write(outputArg.getValue().c_str(), ',');

    
    } catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    return 0;
}

