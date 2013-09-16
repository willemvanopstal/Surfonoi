//
//  Point.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Point.h"

Point::Point(): px(0),py(0),pz(0),isset(false)
{}

Point::Point(double x, double y, double z): px(x),py(y),pz(z),isset(true)
{}

double Point::x() const
{
    return px;
}

double Point::y() const
{
    return py;
}

double Point::z() const
{
    return pz;
}

void Point::set(double x, double y, double z)
{
    px = x;
    py = y;
    pz = z;
}

bool Point::isSet() const
{
    return isset;
}