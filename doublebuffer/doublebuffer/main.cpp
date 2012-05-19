//
//  main.cpp
//  doublebuffer
//
//  Created by Ravi on 15/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "DoubleBuffer.h"

int main(int argc, const char * argv[])
{

    DoubleBuffer dBuffer(argv[1], argv[2], "depth");
    dBuffer.perform(DOUBLEBUFF, 3.0);
    return 0;
}

