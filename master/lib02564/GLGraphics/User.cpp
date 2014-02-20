//
//  User.cpp
//  02564_OSX
//
//  Created by J. Andreas Bærentzen on 26/01/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "User.h"

namespace  GLGraphics {
    const HeightMap* default_height_map()
    {
        static HeightMap h;
        return &h;
    }
}
