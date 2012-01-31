//
//  main.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
//#include "Filter.h"
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
int main (int argc, const char * argv[])
{

//    Filter f(argv[1],argv[2]);
//    f.copy_percentage(80);
//    std::cout << "Written " << f.getLineCount() << " lines" << std::endl;
    
    Grid g(argv[1],10);
    g.calcGrid(&f_min);
    g.write(argv[2]);
    
    return 0;
}

