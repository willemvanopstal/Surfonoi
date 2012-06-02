//
//  Point.h
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef filter_Point_h
#define filter_Point_h

class Point
{
public:
    Point();
    Point(double, double, double);
    double x() const;
    double y() const;
    double z() const;
    void set(double, double, double);
    bool isSet() const;
private:
    double px, py, pz;
    bool isset;
};

#endif
