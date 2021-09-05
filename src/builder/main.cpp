//*******************************************************************
//    builder.cpp
//
//    This file implements the main program entry point for the builder
///   utility.  Builder creates the build files (config.c, config.h) 
//    from a PICMG IoT.1 config file.  The files are emitted 
//    formatted in so they can be built directly into IoT device firmware.
//
//    Portions of this code are based on the Platform Level Data Model
//    (PLDM) specifications from the Distributed Management Task Force 
//    (DMTF).  More information about PLDM can be found on the DMTF
//    web site (www.dmtf.org).
//
//    More information on the PICMG IoT data model can be found within
//    the PICMG family of IoT specifications.  For more information,
//    please visit the PICMG web site (www.picmg.org)
//
//    Copyright (C) 2021,  PICMG
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "builder.h"
#include "CSpline.hpp"

using namespace std;

//*******************************************************************
// main()
//
// main program entry point.  This program takes two arguments:
// the full path to the json file to convert, and the path to write 
// to the output files to.  
//
// This program returns non-zero if an error is encountered.
//
int main(int argc, char *argv[]) {
    Builder builder;
    if (argc != 3) {
        cerr << "Wrong number of arguments.  Syntax: " << endl;
        cerr << "   builder infile.json outfile.c" << endl;
        return -1;
    }

    return builder.build(argv[1],argv[2])?0:1;
}    
