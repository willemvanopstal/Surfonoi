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
#include <tclap/CmdLine.h>
#include "Grid.h"

void f_max(Point& p, double& x, double& y, double& z);
void f_min(Point& p, double& x, double& y, double& z);

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
        TCLAP::ValueArg<double> csArg("c","cellsize","Cellsize to be used",true,50,"float", cmd);
        TCLAP::SwitchArg projectSwitch("p","project","Project x y values fron WGS84 to auto utm-zone", cmd, false);
        TCLAP::SwitchArg reverseSwitch("r","reverse","Keep minima instead of maxima", cmd, false);
        TCLAP::SwitchArg flipSignOnZSwitch("f","flip","flip the sign of z-values in output", cmd, false);
        TCLAP::SwitchArg flipXYSwitch("t","flipxy","Use if order in file is y-x-z or lat-long-z", cmd, false);
        TCLAP::UnlabeledValueArg<std::string>  inputArg( "input", "path to .xyz file", true, "", "input file", cmd);
        TCLAP::UnlabeledValueArg<std::string>  outputArg( "ouput", "path to .xyz file", true, "", "output file", cmd);
        cmd.parse(argc,argv);
        
        Grid g(inputArg.getValue().c_str(), csArg.getValue(), projectSwitch.getValue(), flipSignOnZSwitch.getValue(), flipXYSwitch.getValue());
        
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

