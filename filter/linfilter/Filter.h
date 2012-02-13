//
//  Filter.h
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef filter_Filter_h
#define filter_Filter_h

#include <fstream>

class Filter
{
public:
    Filter(const char*, const char*);
    void copy();
    void copy_nth(int);
    void copy_percentage(int);
    size_t getLineCount();
private:
    std::ifstream ifs;
    std::ofstream ofs;
    size_t lineCount;
};


#endif
