//
//  Filter.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

//#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Filter.h"

Filter::Filter(const char *inFile, const char *outFile): 
    ifs(inFile), ofs(outFile), lineCount(0)
{
    if(!ifs)
    {
        std::cerr << "Unable to open file" << std::endl;
        exit(1);
    }
}

void Filter::copy()
{
    std::string s;

    while (std::getline(ifs,s)){
        ofs << s;
        lineCount++;
    }
}

void Filter::copy_nth(int n)
{
    std::string s;
    size_t i;

    while (std::getline(ifs,s)){
        if (i++ % n == 0) {
            ofs << s;
            lineCount++;
        }
    }
}

void Filter::copy_percentage(int percentage)
{
    std::string s;
    
    while (std::getline(ifs,s)){
        if (percentage >= rand()%100) {
            ofs << s;
            lineCount++;
        }
    }
}

size_t Filter::getLineCount()
{
    return lineCount;
}