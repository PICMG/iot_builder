//*******************************************************************
//    builder.h
//
//    This file contains class declarations for the builder utility.
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
#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include "CSpline.hpp"
#include "JsonFactory.h"

using namespace std;

class Builder {
    private:
        ofstream cOutputFile;
        ofstream hOutputFile;
        JsonAbstractValue *pdrjson;
        double       positionResolution;
        unsigned int bytesOnLine;
        unsigned int pdrByteCount;
        unsigned int pdrRecordCount;
        unsigned int largestPdrRecordSize; 
        unsigned int totalPdrSize;
        unsigned int fruRecordCount; 
        unsigned int largestFruRecordSize;
        unsigned int totalFruSize;
        unsigned int maxAllowedFruSize;        
        map<uint64_t,unsigned int> oemStateSetMap;
            
        void emitStructNewline(bool isFru = false);
        void emitStructUint8(unsigned char byte, bool isFru = false);
        void emitStructSint8(signed char byte, bool isFru = false);
        void emitStructUint16(unsigned int, bool isFru = false);
        void emitStructSint16(signed int, bool isFru = false);
        void emitStructUint32(unsigned long, bool isFru = false);
        void emitStructSint32(signed long, bool isFru = false);
        void emitStructReal32(float, bool isFru = false);
        void emitStructReal64(double, bool isFru = false);
        void emitStructStrAscii(string, bool isFru = false);
        void emitStructStrUtf16be(string, bool isFru = false);
        
        void emitCIntro();
        void startPdr();
        void emitPdrSize(unsigned int size);
        void endPdr();
        void emitTerminusLocatorPdr();
        void emitFruRecordSetPdr();
        void emitEntityAssociationPdr();
        void emitOemEntityIdPdr();
        void emitOemStateSetPdrs();
        void emitSensorEffecterPdrs();
        bool emitStateSensorPdr(JsonObject *binding, JsonObject *entity);
        bool emitNumericSensorPdr(JsonObject *binding, JsonObject *entity);
        bool emitStateEffecterPdr(JsonObject *binding, JsonObject *entity);
        bool emitNumericEffecterPdr(JsonObject *binding, JsonObject *entity);
        double calcDefaultValue(JsonObject *binding, JsonObject *entity);
        void emitLinearizationTables();
        void emitFruRecords();
        void emitThresholdToPdr(JsonObject * binding, string thresholdName);
        unsigned char getFieldSupport(JsonObject * binding);

        void emitHIntro();
        void emitMacros();

        void configureSplineFromPoints(JsonArray* points, CUCSpline *spline, bool reverse);
        void calcPlusMinusTolerance(double*, double* , JsonObject*, JsonObject*, CUCSpline*, CUCSpline*);
        double getPositionResolution(JsonObject* entity, JsonArray* bindings);
    public:
        Builder();
        ~Builder();
        bool build(string inputFilename, string outputPath);


};