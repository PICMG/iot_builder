//*******************************************************************
//    builder.cpp
//
//    This file implements code that builds build files (config.c, 
//    config.h) from a PICMG IoT.1 config file.  The files are emitted 
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
#include <algorithm>    // std:max
#include "builder.h"
#include "JsonFactory.h"
#include "JsonObject.h"
#include "JsonArray.h"
#include "CSpline.hpp"
#include "pldm.h"

#define COMMON_HEADER_SIZE 10
using namespace std;

#define BASE_RESOLUTION    (1.0/65536.0)
#define OFFSET_VALUE 0.0

//*******************************************************************
// loadJsonFile()
//
// Given the filename of a Json File, load the dictionary from the 
// file.
//
// parameters:
//    filename - the name of the json file to load
// returns:
//    a pointer to json structure that was loaded, otherwise NULL
static JsonAbstractValue* loadJsonFile(string filename) {
    JsonFactory jf;

    ifstream jsonfile(filename, std::ifstream::binary);
    if (!jsonfile.is_open()) {
        cerr << "error opening file" << endl;
        return NULL;
    }

    // get the length of the file 
    jsonfile.seekg(0, jsonfile.end);
    streamoff length = jsonfile.tellg();
    jsonfile.seekg(0, jsonfile.beg);

    // create a buffer that is large enough to hold the file
    char* buffer = new char[(unsigned int)length + 1];

    // read the file into the buffer
    jsonfile.read(buffer, length);

    // add the terminating null character
    buffer[length] = 0;

    // construct the json objects from the file structure
    JsonAbstractValue* json = jf.build(buffer);

    delete[] buffer;
    
    return json;
}

//*******************************************************************
// Builder()
//
// Constructor - create an object instance and initialize all the 
// variables.
Builder::Builder() : 
    pdrjson(0), 
    pdrByteCount(0), 
    bytesOnLine(0), 
    pdrRecordCount(0), 
    largestPdrRecordSize(0), 
    totalPdrSize(0),
    fruRecordCount(0), 
    largestFruRecordSize(0), 
    totalFruSize(0),
    maxAllowedFruSize(0)
{

}

//*******************************************************************
// ~Builder()
//
// Destructor - free memory associated with the object and close any
// open filestreams.
Builder::~Builder()
{
    if (cOutputFile.is_open()) cOutputFile.close();
    if (hOutputFile.is_open()) cOutputFile.close();
    if (pdrjson) delete pdrjson;
}

//*******************************************************************
// emitStructUint8()
//
// emit a single unsigned 8-bit integer in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructUint8(unsigned char byte, bool isFru)
{
    unsigned long byteCount = pdrByteCount;
    if (isFru) byteCount = totalFruSize;
    if (bytesOnLine) cOutputFile<<", ";
    if (((bytesOnLine%16) == 0) && (byteCount)) {
        bytesOnLine = 0;
        cOutputFile<<"\n";
    }
    if (!bytesOnLine) cOutputFile<<"   ";
    cOutputFile<<"0x"<<hex<<setw(2)<<setfill('0')<<((unsigned int)byte);
    bytesOnLine++;
    if (!isFru) {
        pdrByteCount++;
    } else {
        totalFruSize++;
    }
}

//*******************************************************************
// emitStructNewline()
//
// emit a line break in the  pdr section of config.c.  This function 
// takes care of commas between bytes.
//
void Builder::emitStructNewline(bool isFru)
{
    if (bytesOnLine) {
        cOutputFile<<", ";
        bytesOnLine = 0;
        cOutputFile<<"\n";
    }
}

//*******************************************************************
// emitStructSint8()
//
// emit a single signed 8-bit integer in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructSint8(signed char byte, bool isFru)
{
    emitStructUint8((unsigned char)byte, isFru);
}

//*******************************************************************
// emitStructUint16()
//
// emit a single unsigned 16-bit integer in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructUint16(unsigned int word, bool isFru)
{
    // emit the bytes in little endian order
    emitStructUint8(word&0xff, isFru);
    emitStructUint8(word>>8, isFru);
}

//*******************************************************************
// emitStructSint16()
//
// emit a single signed 16-bit integer in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructSint16(signed int word, bool isFru)
{
    // emit the bytes in little endian order
    emitStructUint8(((unsigned int)word)&0xff, isFru);
    emitStructUint8(((unsigned int)word)>>8, isFru);
}

//*******************************************************************
// emitStructUint32()
//
// emit a single unsigned 32-bit integer in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructUint32(unsigned long dword, bool isFru)
{
    // emit the bytes in little endian order
    emitStructUint16(dword&0xffff, isFru);
    emitStructUint16(dword>>16, isFru);
}

//*******************************************************************
// emitStructSint32()
//
// emit a single signed 32-bit integer in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructSint32(signed long dword, bool isFru)
{
    // emit the bytes in little endian order
    emitStructUint16(((unsigned long)dword)&0xffff, isFru);
    emitStructUint16(((unsigned long)dword)>>16, isFru);
}

//*******************************************************************
// emitStructReal32()
//
// emit a single 32-bit floating point in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructReal32(float real, bool isFru)
{
    emitStructUint32(*((unsigned long *)&real), isFru);
}

//*******************************************************************
// emitStructReal64()
//
// emit a single 64-bit floating point in hexadecimal format to the 
// pdr section of config.c.  This function also counts the total
// bytes that have been emitted to the pdr data and takes care of
// line breaks and commas between bytes.
//
void Builder::emitStructReal64(double real, bool isFru)
{
    emitStructUint32(*((unsigned long *)&real), isFru);
    emitStructUint32(*(((unsigned long *)&real)+1), isFru);
}

//*******************************************************************
// emitStructStrAscii()
//
// emit C++ string as an ascii null-terminated string to the pdr section of config.c.  
// This function also counts the total bytes that have been emitted to 
// the pdr data and takes care of line breaks and commas between bytes.
//
void Builder::emitStructStrAscii(string str, bool isFru)
{
    // emit each character in the string
    for (int i=0;i<str.size();i++) {
        emitStructUint8(str.c_str()[i], isFru);
    }
    // emit the null terminator for PDRs, but not FRU
    if (!isFru) emitStructUint8(0x00, isFru);
}

//*******************************************************************
// emitStructStrUtf16be()
//
// emit a c++ string as a UTF-16BE null-terminated string to the pdr 
// section of config.c. This function also counts the total bytes that 
// have been emitted to the pdr data and takes care of line breaks 
// and commas between bytes.
void Builder::emitStructStrUtf16be(string str, bool isFru)
{
    // convert ascii input string to utf16 big endian format
    for (int chnm = 0; chnm < str.size(); chnm++) {
        unsigned char ch = str.c_str()[chnm];
        if (ch < 0x80) { 
            emitStructUint8(0, isFru);
            emitStructUint8(ch, isFru);
        } 
        else {
            emitStructUint8(0xC0 + (ch>>6), isFru);
            emitStructUint8(0x80 + (ch & 0x3F), isFru);
        }
    }
    // null terminator
    if (!isFru) {
        emitStructUint8(0, isFru);
        emitStructUint8(0, isFru);
    }
}

//*******************************************************************
// emitPdrSize()
//
// emit the pdr record size and keep track of the total bytes and 
// largest record in the PDR repository.
void Builder::emitPdrSize(unsigned int size)
{
    totalPdrSize += size;
    largestPdrRecordSize = max(size, largestPdrRecordSize);
    emitStructUint16(size);
}

//*******************************************************************
// emitCIntro()
//
// emit the file comment block and includes to config.c.
//
void Builder::emitCIntro()
{
    cOutputFile<<"//*****************************************************************"<<endl;
    cOutputFile<<"// config.c"<<endl;
    cOutputFile<<"//"<<endl;
    cOutputFile<<"// This file was autogenerated by the PICMG firmware builder"<<endl;
    cOutputFile<<"// utility.  It includes data structures used to build a custom"<<endl;
    cOutputFile<<"// configured firmware image."<<endl;
    cOutputFile<<"#include \"config.h\""<<endl;
    cOutputFile<<endl;
}

//*******************************************************************
// startPdr()
//
// emit the code to start the PDR structure in config.c.
//
void Builder::startPdr() 
{
    cOutputFile<<"PDR_BYTE_TYPE __pdr_data[] PDR_DATA_ATTRIBUTES = { "<<endl;
}

//*******************************************************************
// endPdr()
//
// emit the code to end the PDR structure in config.c.
//
void Builder::endPdr()
{
    cOutputFile<<endl<<"};"<<endl;
    cOutputFile<<endl;
}

//*******************************************************************
// emitTerminusLocatorPdr()
//
// emit the Terminus Locator PDR to config.c.  This structure is
// always the same, regardless of the rest of the PDR repository.
//
void Builder::emitTerminusLocatorPdr()
{
    emitStructNewline();
    cOutputFile<<"   // Terminus Locator PDR "<<endl;
    bytesOnLine = 0;
    pdrRecordCount++;

    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_TERMINUS_LOCATOR); 
    emitStructUint16(0x0001);          // Record Change Number
    emitPdrSize(0x0009);            // data length
    emitStructUint16(0x0001);          // pldm terminus handle
    emitStructUint8(0x01);             // validity - valid              
    emitStructUint8(0x01);             // TID - unassigned
    emitStructUint16(0x0001);          // Container ID
    emitStructUint8(0x01);             // Terminus locator type - MCTP_EID
    emitStructUint8(0x01);             // terminus locator value size
    emitStructUint8(0x01);             // EID (unassigned);
}

//*******************************************************************
// emitFruRecordSetPdr()
//
// emit the Fru Record Set PDR to config.c.  This structure is
// always the same, regardless of the rest of the PDR repository.
//
void Builder::emitFruRecordSetPdr()
{
    emitStructNewline();
    cOutputFile<<"   // FRU Record Set ";
    bytesOnLine = 0;
    pdrRecordCount++;

    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_FRU_RECORD_SET); 
    emitStructUint16(0x0001);          // Record Change Number
    emitPdrSize(0x000a);               // data length
    emitStructUint16(0x0001);          // pldm terminus handle
    emitStructUint16(1);               // FRU Record Set Identifier              
    emitStructUint16(80);              // Container Entity Type - IO Module
    emitStructUint16(0x0001);          // Entity Instance Number
    emitStructUint16(0x0000);          // Container ID (System)
}

//*******************************************************************
// emitFruRecordSets()
//
// emit the Fru Record Sets to config.c.  
//
void Builder::emitFruRecords()
{    
    // get the fru record list from the config file
    JsonObject* cfgObj = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonArray* fruRecords = (JsonArray*)(cfgObj->find("fruRecords"));

    // return if there are no records to emit;
    if (fruRecords->size()==0) return;

    // loop for each record in the set (all records will be associated 
    // with the same fru record set
    cOutputFile<<"FRU_BYTE_TYPE __fru_data[] FRU_DATA_ATTRIBUTES = {"<<endl; 
    bytesOnLine = 0;
    for (int i = 0; i<fruRecords->size(); i++) {
        // get the record structure
        JsonObject * record = (JsonObject*)fruRecords->getElement(i);

        // increment the number of records in the set
        fruRecordCount++;

        // get the starting position of this record
        unsigned int recordStart = totalFruSize;

        // emit a comment indicating a new fru record
        emitStructNewline(true);
        cOutputFile<<"   // FRU Record "<<fruRecordCount<<endl;

        // emit the fru record set identifier
        emitStructUint16(0x0001, true);   
        
        // emit the fru record type
        if (record->getInteger("vendorIANA")==412) {
            // if the vendor IANA is 412 (DMTF), this is a general FRU record
            emitStructUint8(0x01, true);  
        } else {
            // otherwise, this is an OEM fru type
            emitStructUint8(0xfe, true); 
        }

        // emit the number of fru fields
        JsonArray *fields = (JsonArray*)record->find("fields");
        emitStructUint8(fields->size(),true);

        // encoding = utf8
        emitStructUint8(0x02, true);

        // emit each field according to its type
        for (int j=0; j<fields->size(); j++) {
            JsonObject* field = (JsonObject*)fields->getElement(j);
            int type = field->getInteger("type");
            emitStructUint8(type,true);
            if ((field->getValue("format")=="bytes")||(field->getValue("format")=="timestamp104")) {
                // this format is an array of bytes
                JsonArray *bytes = (JsonArray*)field->find("value");
                // emit the length of the record
                emitStructUint8(bytes->size(),true);  

                // emit the bytes
                for (int byteIndex = 0; byteIndex<bytes->size(); byteIndex++) {
                    JsonObject * byte = (JsonObject*)bytes->getElement(byteIndex);
                    emitStructUint8(byte->getInteger(""),true);
                }                       
            } else if ((record->getInteger("vendorIANA")==412)&&(field->getInteger("type")==15)) {
                // general record, IANA
                emitStructUint8(4,true);  
                emitStructUint32(field->getInteger("value"),true);
            } else if ((record->getInteger("vendorIANA")!=412)&&(field->getInteger("type")==1)) {
                // oem record, IANA
                emitStructUint8(4,true);  
                emitStructUint32(field->getInteger("value"),true);
            } else {
                // all remaining record types treated as strings
                emitStructUint8(field->getValue("value").size(),true);
                emitStructStrAscii(field->getValue("value"),true);
            }
        }
        // calculate the record size and update the largest record size if needed
        unsigned int recordEnd = totalFruSize;
        largestFruRecordSize = max(largestFruRecordSize, recordEnd-recordStart);
    }
    // complete the fru record structure
    cOutputFile<<endl<<"};"<<endl; 
    cOutputFile<<endl;
}

//*******************************************************************
// emitEntityAssociationPdr()
//
// emit the Entity Association PDR to config.c.  The entity hierarchy
// is as follows:
//     System -+
//             IO Module 1 (this node)
//                 Logical Entity
//
void Builder::emitEntityAssociationPdr()
{
    emitStructNewline();
    cOutputFile<<"   // Entity Association ";
    bytesOnLine = 0;
    pdrRecordCount++;
    
    // get the list of entities from the config file
    JsonObject* cfgObj = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonArray* entities = (JsonArray*)(cfgObj->find("logicalEntities"));

    // emit the PDR data that never changes regardless of the device
    // architecture
    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_ENTITY_ASSOCIATION);
    emitStructUint16(0x0001);          // Record Change Number
    emitPdrSize(10+6*entities->size());  // data length
    emitStructUint16(0x0001);          // container ID
    emitStructUint8(0x01);             // associationType - logicalContainment
    emitStructUint16(80);              // container type - IO Module
    emitStructUint16(0x0001);          // container Entity Instance Number
    emitStructUint16(0x0000);          // container Entity Container ID (System)
    emitStructUint8(0x01);             // contained entity count    
    emitStructUint16(0x6000);          // Contained entity type
    emitStructUint16(0x0001);          // contained Entity Instance Number
    emitStructUint16(0x0001);          // container Entity Container ID
}

//*******************************************************************
// emitOEMEntityIdPdr()
//
// emit the OEM entity id PDR to config.c.  
//
void Builder::emitOemEntityIdPdr()
{
    emitStructNewline();
    cOutputFile<<"   // OEM Entity ID ";
    bytesOnLine = 0;
    pdrRecordCount++;
    
    // get the list of entities from the config file
    JsonObject* cfgObj = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonArray* entities = (JsonArray*)(cfgObj->find("logicalEntities"));
    JsonObject * entity = (JsonObject*)(entities->getElement(0));
    string entityname = "unknown";
    switch (entity->getInteger("vendorEntityID")) {
        case 1: // simple
            entityname = "Simple";
            break;
        case 2: // pid
            entityname = "PID";
            break;
        case 3:
            entityname = "Profiled";
            break;
    }

    // emit the PDR data that never changes regardless of the device
    // architecture
    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_OEM_ENTITY_ID);
    emitStructUint16(0x0001);          // Record Change Number
    emitPdrSize(14+2*(entityname.size()+1));  // data length
    emitStructUint16(0x0001);          // terminus handle
    emitStructUint16(0x6000);          // oem entity id handle
    emitStructUint32(12634);           // PICMG vendor IANA
    emitStructUint16(entity->getInteger("vendorEntityID"));
    emitStructUint8(0x01);             // contained string count    
    emitStructStrAscii("en");          // language = english
    emitStructStrUtf16be(entityname);  // entity name
}

//*******************************************************************
// emitOemStateSetPdrs()
//
// emit any OEM state set PDRs to config.c.  
//
void Builder::emitOemStateSetPdrs()
{
    // get the list of state sets from the config file
    JsonArray* sets = (JsonArray*)(((JsonObject*)pdrjson)->find("oemStateSets"));

    // loop for each oem state set in the configuration, building the map as we go.
    // stateSetHandleMap maps the oemIANA/enity ID to the entity handle for the OEM
    // state set PDR.  
    map<unsigned long, unsigned int> typeCounts; 
    for (int i=0;i<sets->size();i++) {        
        // get the specific entity of interest
        JsonObject *set = (JsonObject*)(sets->getElement(i));
        
        // get the values for the fields for vendor entity ID and 
        // entity vendor IANA
        unsigned long vendorStateSetId = set->getInteger("stateSetID");
        unsigned long vendorIana = set->getInteger("vendorIANA");
        uint64_t key = (((uint64_t)vendorIana)<<16) + vendorStateSetId;
        unsigned int nextHandle = 0x8003;
        
        // build the map of oem state set handles
        switch (key) {
            case 0x315a0001: // PICMG Trigger State
                oemStateSetMap[key] = 0x8000;
                break;
            case 0x315a0002: // PICMG PID Controller State
            case 0x315a0004: // PICMG Profiled Motion Controller State
                oemStateSetMap[key] = 0x8001;
                break;
            case 0x315a0003: // PICMG PID Controller Command
            case 0x315a0005: // PICMG Profiled Motion Controller Command
                oemStateSetMap[key] = 0x8002;
                break;
            default: // other oem state sets
                oemStateSetMap[key] = nextHandle;
                nextHandle++;
                break;
        }

        // calculate and output the size
        unsigned int pdrsize = 12;
        JsonArray *oemStateValueRecords = ((JsonArray*)(set->find("oemStateValueRecords")));
        for (unsigned int recnum = 0; recnum<oemStateValueRecords->size();recnum++) {
            // get the specific record
            JsonObject *record = (JsonObject *)(oemStateValueRecords->getElement(recnum));

            // get the language tag and state name arrays
            JsonArray* languageTags = (JsonArray *)record->find("languageTags");
            JsonArray* stateName    = (JsonArray *)record->find("stateName");

            // add 3 btyes for each state value record plus size of the strings
            // including terminators.
            pdrsize += 3;
            for (unsigned int strnum = 0; strnum<languageTags->size(); strnum++) {
                pdrsize += languageTags->getElement(strnum)->getValue("").size() + 1;
                pdrsize += stateName->getElement(strnum)->getValue("").size()*2 + 2;
            }
        }

        emitStructNewline();
        cOutputFile<<"   // OEM State Set ";
        bytesOnLine = 0;
        pdrRecordCount++;

        // emit the PDR common header
        emitStructUint32(pdrRecordCount);  // record handle
        emitStructUint8(0x01);             // Pdr header version
        emitStructUint8(PDR_TYPE_OEM_STATE_SET);
        emitStructUint16(0x0001);          // Record Change Number       
        emitPdrSize(pdrsize);           // data length

        // emit the state set definition information
        emitStructUint16(0x0001);              // Terminus handle
        emitStructUint16(oemStateSetMap[key]); // State Set ID Handle
        emitStructUint32(vendorIana);          // vendorIana for this state set
        emitStructUint16(vendorStateSetId);    // Oem State set ID
        emitStructUint8(0x01);                 // unspecified value hint (treat as error)
        emitStructUint8(oemStateValueRecords->size());  // the number of state value records
        
        // emit the information for each state record
        for (unsigned int recnum = 0; recnum<oemStateValueRecords->size();recnum++) {
            // get the specific record
            JsonObject *record = (JsonObject *)(oemStateValueRecords->getElement(recnum));

            // get the language tag and state name arrays
            JsonArray* languageTags = (JsonArray *)record->find("languageTags");
            JsonArray* stateName    = (JsonArray *)record->find("stateName");

            emitStructUint8(record->getInteger("minStateValue")); // min state value for this state
            emitStructUint8(record->getInteger("maxStateValue")); // max state value for this state
            emitStructUint8(languageTags->size());                // String count
            
            // emit information for each string
            for (unsigned int strnum = 0; strnum<languageTags->size(); strnum++) {
                emitStructStrAscii(languageTags->getElement(strnum)->getValue(""));
                emitStructStrUtf16be(stateName->getElement(strnum)->getValue(""));
            }

        }

    }
}

//*******************************************************************
// emitStateSensorPdr()
//
// emit a single state sensor PDR to config.c.  
//
bool Builder::emitStateSensorPdr(JsonObject *binding, JsonObject *entity)
{
    emitStructNewline();
    cOutputFile<<"   // State Sensor "<<(binding->getValue("name"));
    bytesOnLine = 0;
    pdrRecordCount++;

    // emit the PDR common header
    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_STATE_SENSOR);
    emitStructUint16(0x0001);          // Record Change Number       
    emitPdrSize(17);                // data length

    // emit the sensor information
    emitStructUint16(0x0001);              // Terminus handle
    emitStructUint16(binding->getInteger("sensorID")); // sensor ID for this sensor
    emitStructUint16(0x6000);              // Entity Type
    emitStructUint16(0x0001);              // Entity Instance
    emitStructUint16(0x0001);              // Container ID
    emitStructUint8(0x00);                 // Sensor Init
    emitStructUint8(0x00);                 // Auxilary Names Pdr
    emitStructUint8(0x01);                 // Sensor Count

    unsigned long vendorIANA = binding->getInteger("stateSetVendor");
    if (vendorIANA==412) {
        // DMTF - A standard state set
        emitStructUint16(binding->getInteger("stateSet"));  // State Set ID
    } else {
        // loop up the state set handle and give it as the ID
        uint64_t key = (((uint64_t)vendorIANA)<<16) + binding->getInteger("stateSet");
        emitStructUint16(oemStateSetMap[key]);  // State Set ID
    }
    emitStructUint8(0x01); // Possible state size
    emitStructUint8(binding->getInteger("usedStates"));
    return true;
}

static double scaleResolutionByRateUnit(double res_in, unsigned int rateUnit, double sampleRate) {
    double resolution = res_in;
    switch (rateUnit) {
    case 0: // no rate unit
        break;
    case 1: // per microsecond
        resolution *= sampleRate;
        resolution /= 1000000;  // microseconds per second
        break;
    case 2: // per millisecond
        resolution *= sampleRate;
        resolution /= 1000;  // millseconds per second
        break;
    case 3: // per second
        resolution *= sampleRate;
        break;
    case 4: // per minute
        resolution *= sampleRate;
        resolution *= 60;  // seconds per minute
        break;
    case 5: // per hour
        resolution *= sampleRate;
        resolution *= 60*60;  // seconds per hour
        break;
    case 6: // per day
        resolution *= sampleRate;
        resolution *= 24*60*60;  // seconds per day
        break;
    case 7: // per week
        resolution *= sampleRate;
        resolution *= 7*24*60*60;  // seconds per week
        break;
    case 8: // per month
        resolution *= sampleRate;
        resolution *= 30*24*60*60;  // seconds per month
        break;
    case 9: // per year
        resolution *= sampleRate;
        resolution *= 365*24*60*60;  // seconds per day
        break;
    }
    return resolution;
}

//*******************************************************************
// getFieldSupport()
//
// get the value of the field support bit field based on the sensor
// thresholds that have been set in an IOBinding Object.
//
// parameters:
//    binding - the IOBinding object to evaluate
// returns: the value of the field support bitfield
unsigned char Builder::getFieldSupport(JsonObject * binding)
{
    unsigned char fieldSupport = 0;

    if ((binding->find("lowerThresholdFatal")!=NULL)&&(binding->find("lowerThresholdFatal")->getValue("")!="NULL")) fieldSupport |= 0x40;
    if ((binding->find("upperThresholdFatal")!=NULL)&&(binding->find("upperThresholdFatal")->getValue("")!="NULL")) fieldSupport |= 0x20;
    if ((binding->find("lowerThresholdCritical")!=NULL)&&(binding->find("lowerThresholdCritical")->getValue("")!="NULL")) fieldSupport |= 0x10;
    if ((binding->find("upperThresholdCritical")!=NULL)&&(binding->find("upperThresholdCritical")->getValue("")!="NULL")) fieldSupport |= 0x08;
    if ((binding->find("normalMin")!=NULL)&&(binding->find("normalMin")->getValue("")!="NULL")) fieldSupport |= 0x04;
    if ((binding->find("normalMax")!=NULL)&&(binding->find("normalMax")->getValue("")!="NULL")) fieldSupport |= 0x02;

    return fieldSupport;
}

//*******************************************************************
// emitThresholdToPdr()
//
// emit the sensor threshold to the pdr structure.
//
// parameters:
//    binding - the IOBinding object to evaluate
//    thresholdName - the name of the threshold to output
// returns: nothing
void Builder::emitThresholdToPdr(JsonObject * binding, string thresholdName)
{

    if (binding->find(thresholdName)!=NULL) {
        emitStructSint32(binding->find(thresholdName)->getInteger(""));
    } else {
        emitStructSint32(0);
    }
}

//*******************************************************************
// emitNumericSensorPdr()
//
// emit a single numeric sensor PDR to config.c.  
//
bool Builder::emitNumericSensorPdr(JsonObject *binding, JsonObject *entity)
{
    emitStructNewline();
    cOutputFile<<"   // Numeric Sensor "<<(binding->getValue("name"));
    bytesOnLine = 0;
    pdrRecordCount++;

    // get any required parameter values
    JsonArray * parameters = (JsonArray*)(entity->find("parameters"));
    double sampleRate = 0;
    for (int i=0;i<parameters->size();i++) {
        JsonObject * param = (JsonObject*)(parameters->getElement(i));
        if (param->getValue("name").compare("SampleRate")==0) {
            sampleRate = param->getDouble("value");
            break;
        }
    }

    // emit the PDR common header
    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_NUMERIC_SENSOR);
    emitStructUint16(0x0001);          // Record Change Number       
    emitPdrSize(95);                // data length 

    // emit the sensor information
    emitStructUint16(0x0001);              // Terminus handle
    emitStructUint16(binding->getInteger("sensorID")); // Senspr ID for this sensor
    emitStructUint16(0x6000);              // Entity type
    emitStructUint16(0x0001);              // Entity Instance
    emitStructUint16(0x0001);              // Container ID
    emitStructUint8(0x00);                 // Sensor Init
    emitStructUint8(0x00);                 // Sensor Auxilary Names PDR
    emitStructUint8(binding->getInteger("physicalBaseUnit"));         // base unit
    emitStructSint8(binding->getInteger("phsicalUnitModifier"));      // unitModifier
    emitStructUint8(binding->getInteger("physicalRateUnit"));         // rateUnit
    emitStructUint8(0);                                               // base unit OEM Handle
    emitStructUint8(binding->getInteger("physicalAuxUnit"));          // aux unit
    emitStructSint8(binding->getInteger("physicalAuxUnitModifier"));  // aux unit modifier
    emitStructUint8(binding->getInteger("physicalAuxRateUnit"));      // aux rate unit
    if(binding->getValue("rel").compare("DivideBy")==0) {
        emitStructUint8(0);      // rel - divide by
    } else {
        emitStructUint8(1);      // rel - multiply by
    }
    emitStructUint8(0x00);                 // aux oemUnitHandle
    emitStructUint8(1);                    // isLinear
    emitStructUint8(5);                    // sensor data size (sint 32)

    // calculate the resolution - normally it will be the base value, however
    // if it is a rate value (or hertz) it should be normalized to the sample
    // period
    double resolution = BASE_RESOLUTION;
    if (positionResolution) {
        switch(binding->getInteger("sensorID")) {
            case 4:  // verr
            case 5:  // perr
            case 6:  // velocity
                resolution *= positionResolution;
                break;
            case 7:  // position
                resolution = positionResolution;
        }
    }
    if (binding->getInteger("physicalBaseUnit") == 20) {
        // base units are hertz
        resolution *= sampleRate;
    }
    resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalRateUnit"), sampleRate);
    resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalAuxRateUnit"), sampleRate);
    emitStructReal32(resolution);    // resolution
    emitStructReal32(OFFSET_VALUE);        // offset

    // if the effecter is not virtual - construct in/out curves to calculate the
    // accuracy and tolerance
    if (!binding->getBoolean("isVirtual")) 
    {
        // find the channel in the channel list
        JsonObject *cap = (JsonObject*)(((JsonObject*)pdrjson)->find("capabilities"));
        JsonArray *channels = (JsonArray*)(cap->find("channels"));
        JsonObject* channel = NULL;
        for (int i = 0;i<channels->size();i++) {
            channel = (JsonObject *)channels->getElement(i);
            if (channel->getValue("name")==binding->getValue("boundChannel")==0) break;
        }
        if (!channel) {
            cerr<<"Channel not found for IO Binding "<<binding->getValue("name")<<endl;
            return false;
        }

        // accuracy - this part of the error scales linearly with the reading
        emitStructUint16(channel->getDouble("accuracy")*100);  

        // get the output curve for the output stage
        JsonArray *inputCurve = ((JsonArray*)binding->find("inputCurve"));
        JsonObject *sensor = ((JsonObject*)binding->find("sensor"));
        JsonArray *responseCurve = ((JsonArray*)sensor->find("responseCurve"));

        // create the interpolation curves
        CUCSpline inputSpline(true);
        configureSplineFromPoints(inputCurve, &inputSpline,true);
        CUCSpline responseSpline(true);
        configureSplineFromPoints(responseCurve, &responseSpline,true);

        // get the gearing ratio
        double gearing = binding->getDouble("inputGearingRatio");
        if (gearing == 0) gearing = 1.0;

        // calculate and emit the tolerance based on channel specifics
        double plusTolerance;
        double minusTolerance;
        calcPlusMinusTolerance(&plusTolerance, &minusTolerance, channel, binding, &inputSpline, &responseSpline);
        emitStructUint8(plusTolerance);   // plus tolerance
        emitStructUint8(minusTolerance);  // minus tolerance

        emitStructUint32(0);              // hysteresis
        emitStructUint8(0x3f);            // thresholds supported
        emitStructUint8(0x0f);            // threshold volatility

        if (sampleRate!=0) {
            emitStructReal32(1.0/sampleRate);  // State Transition Interval
            emitStructReal32(1.0/sampleRate);  // Update Interval
        } else { 
            emitStructReal32(0.0);  // State Transition Interval
            emitStructReal32(0.0);  // Update Interval
        }
        
        // get the max and min raw values from the selected channel
        string channelName = binding->getValue("boundChannel");

        // matching channel has been found output channel-specific values
        double max = responseSpline.interpolate(inputSpline.interpolate(channel->getDouble("maxValueAtPin")))/gearing;
        double min = responseSpline.interpolate(inputSpline.interpolate(channel->getDouble("minValueAtPin")))/gearing;
        if (min>max) {
            double temp = min;
            min = max;
            max = temp;
        }
        emitStructSint32((max-OFFSET_VALUE)/resolution);                 // max readable
        emitStructSint32((min-OFFSET_VALUE)/resolution);                 // min readable
        emitStructSint8(5);  // range field format (sint32)
        
        // determine which range values are supported
        unsigned char fieldSupport = getFieldSupport(binding);
        emitStructUint8(fieldSupport);       // range field support
        emitStructSint32(0);                 // Nominal Value (not used)
        emitThresholdToPdr(binding, "normalMax"); 
        emitThresholdToPdr(binding, "normalMin"); 
        emitThresholdToPdr(binding, "upperThresholdWarning"); 
        emitThresholdToPdr(binding, "lowerThresholdWarning"); 
        emitThresholdToPdr(binding, "upperThresholdCritical"); 
        emitThresholdToPdr(binding, "lowerThresholdCritical"); 
        emitThresholdToPdr(binding, "upperThresholdFatal"); 
        emitThresholdToPdr(binding, "lowerThresholdFatal"); 
    } else {
        // Virtual numeric sensor
        // accuracy - this part of the error scales linearly with the reading
        // TODO: make this configurable?
        emitStructUint16(100);    // accuracy
        emitStructUint8(0);       // plus tolerance
        emitStructUint8(0);       // minus tolerance
        emitStructUint32(0);      // hysteresis
        emitStructUint8(0x3f);    // thresholds supported
        emitStructUint8(0x0f);    // threshold volatility
        if (sampleRate!=0) {
            emitStructReal32(1.0/sampleRate);  // State Transition Interval
            emitStructReal32(1.0/sampleRate);  // Update Interval
        } else { 
            emitStructReal32(0.0);  // State Transition Interval
            emitStructReal32(0.0);  // Update Interval
        }
        emitStructSint32(0x7fffffff);       // max readable
        emitStructSint32(0x80000000);       // min readable
        emitStructSint8(5);                 // range field format (sint32)
            
        // determine which range values are supported
        unsigned char fieldSupport = getFieldSupport(binding);
        emitStructUint8(fieldSupport);         // range field support
        emitStructSint32(0);                 // Nominal Value (not used)
        emitThresholdToPdr(binding, "normalMax"); 
        emitThresholdToPdr(binding, "normalMin"); 
        emitThresholdToPdr(binding, "upperThresholdWarning"); 
        emitThresholdToPdr(binding, "lowerThresholdWarning"); 
        emitThresholdToPdr(binding, "upperThresholdCritical"); 
        emitThresholdToPdr(binding, "lowerThresholdCritical"); 
        emitThresholdToPdr(binding, "upperThresholdFatal"); 
        emitThresholdToPdr(binding, "lowerThresholdFatal"); 
    }
    return true;
}

//*******************************************************************
// emitStateEffecterPdr()
//
// emit a single state effecter PDR to config.c.  
//
bool Builder::emitStateEffecterPdr(JsonObject *binding, JsonObject *entity)
{
    emitStructNewline();
    cOutputFile<<"   // State Effecter "<<(binding->getValue("name"));
    bytesOnLine = 0;
    pdrRecordCount++;

    // emit the PDR common header
    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_STATE_EFFECTER);
    emitStructUint16(0x0001);          // Record Change Number       
    emitPdrSize(19);                // data length

    // emit the sensor information
    emitStructUint16(0x0001);              // Terminus handle
    emitStructUint16(binding->getInteger("effecterID")); // effecter ID
    emitStructUint16(0x6000);              // Entity type
    emitStructUint16(0x0001);              // Entity Instance
    emitStructUint16(0x0001);              // Container ID
    emitStructUint16(0x0000);              // Effecter Semantic ID
    emitStructUint8(0x00);                 // Effecter Init
    emitStructUint8(0x00);                 // Effecter Description PDR
    emitStructUint8(0x01);                 // Effecter Count

    unsigned long vendorIANA = binding->getInteger("stateSetVendor");
    if (vendorIANA==412) {
        // DMTF - A standard state set
        emitStructUint16(binding->getInteger("stateSet"));  // State Set
    } else {
        // loop up the state set handle and give it as the ID
        uint64_t key = (((uint64_t)vendorIANA)<<16) + binding->getInteger("stateSet");
        emitStructUint16(oemStateSetMap[key]);  // State Set
    }
    emitStructUint8(0x01); // Possible state size
    emitStructUint8(binding->getInteger("usedStates"));
    return true;
}

//*******************************************************************
// configureSplineFromPoints()
//
// Configure a spline object from an array of configuration of DataPoint
// objects.
//
// Parameters:
//    points - a pointer to a JsonArray object that contains the point objects.
//    spline - a pointer to the spline object to be configured.
//    reverse - true if the spline independent variable are the output points.
//        Otherwise, the spline independent variable consists of the input points.  
//
void Builder::configureSplineFromPoints(JsonArray* points, CUCSpline *spline, bool reverse) {
    // allocate memory for the data points
    CUInterpolator::SUConfigurationTableEntry *spline_points = 
        new CUInterpolator::SUConfigurationTableEntry[points->size()]; 

    // initialize the arrays - make sure the independent values are in increasing order
    JsonObject *p1 = (JsonObject*)(points->getElement(0));
    JsonObject *p2 = (JsonObject*)(points->getElement(1));
    double reverse_order = false;
    if (reverse) {
        if (p2->getDouble("out")<p1->getDouble("out")) reverse_order = true;     
    } else {
        if (p2->getDouble("in")<p1->getDouble("in")) reverse_order = true;     
    }
    for (int i = 0; i<points->size();i++) {
        int idx = i;
        if (reverse_order) idx = points->size()-1-i;
        JsonObject *point = (JsonObject*)(points->getElement(idx));
        if (reverse) {
            spline_points[idx].fIndependentValue = point->getDouble("out"); 
            spline_points[idx].fDependentValue = point->getDouble("in"); 
        } else {
            spline_points[idx].fIndependentValue = point->getDouble("in"); 
            spline_points[idx].fDependentValue = point->getDouble("out"); 
        }
    }
    spline->configure(points->size(),spline_points);
    spline->configureNaturalSpline(true);

    delete[] spline_points;
}

//*******************************************************************
// calcPlusMinusTolerance()
//
// Calculate the positive and negative tolerance values based on the 
// channel type and I/O binding specifics.
//
// Parameters:
//    plusTolerance - this value will be updated with the calculated plus tolerance
//    minusTolerance - this value will be updated with the calculated minus tolerance.
//    channel - a pointer to the associated channel json object
//    binding - a pointer to the associated binding object
//    ioSpline - a pointer to a spline that defines the input or output curve of the 
//        the i/o interface.  The independent variable is on the controller side of 
//        the i/o curve.
//    seSpline - a pointer to a spline that defines the sensor or effecter response
//        curve. The independent variable is associated with the controller's i/o 
//        interface.
//
void Builder::calcPlusMinusTolerance(
    double *plusTolerance, 
    double *minusTolerance, 
    JsonObject*channel, 
    JsonObject*binding,
    CUCSpline *ioSpline,
    CUCSpline *seSpline)
{
    // default values
    plusTolerance = 0;
    minusTolerance = 0;

    // calculate tolerance for analog in, analog out, or pwm
    if ((channel->getValue("type").compare("analog_in")==0)|| 
       (channel->getValue("type").compare("analog_out")==0)||
       (channel->getValue("type").compare("pwm_out")==0)) 
    {
       // get the gearing ratio
        double gearing = binding->getDouble("outputGearingRatio");

        // calculate the plus/minus tolerance at a few points to determine the
        // the worst case differences
        int precision = channel->getDouble("precision");
        double minValueAtPin = channel->getDouble("minValueAtPin");
        double maxValueAtPin = channel->getDouble("maxValueAtPin");
        double maxRaw,minRaw;        
        maxRaw = 1<<abs(precision);
        minRaw = 0;
        if (precision<0) {
            maxRaw /= 2.0;
            minRaw = -maxRaw;
        }
        for (double i = minRaw; i<= maxRaw; i += ((maxRaw-minRaw)/8.0)) {
            double valueAtPin = minValueAtPin + (maxValueAtPin-minValueAtPin)*(i/maxRaw);
            double actual = gearing*seSpline->interpolate(ioSpline->interpolate(valueAtPin));
            
            valueAtPin = minValueAtPin + (maxValueAtPin-minValueAtPin)*((i+.5)/maxRaw);
            double high = gearing*seSpline->interpolate(ioSpline->interpolate(valueAtPin));

            valueAtPin = minValueAtPin + (maxValueAtPin-minValueAtPin)*((i-.5)/maxRaw);
            double low = gearing*seSpline->interpolate(ioSpline->interpolate(valueAtPin));

            high = high-actual;
            high = max(high,0.0);
            high = min(high,255.0);
            low = low - actual;
            low = max(low,0.0);
            low = min(low,255.0);

            *plusTolerance = max(*plusTolerance, high);
            *minusTolerance = max(*minusTolerance, low);           
        } 
        return;
    }
    // calculate tolerance for rate_out
    if ((channel->getValue("type").compare("rate_out")==0)) 
    {
       // get the gearing ratio
        double gearing = binding->getDouble("outputGearingRatio");

        // calculate the plus/minus tolerance at a few points to determine the
        // the worst case differences
        int precision = channel->getDouble("precision");
        double maxValueAtPin = channel->getDouble("maxValueAtPin");
        double maxRaw,minRaw;        
        maxRaw = 1<<abs(precision);
        minRaw = 0;
        if (precision<0) {
            maxRaw /= 2.0;
            minRaw = -maxRaw;
        }
        for (double i = minRaw; i<= maxRaw; i += ((maxRaw-minRaw)/8.0)) {
            double valueAtPin = (maxValueAtPin)/(i/maxRaw);
            double actual = gearing*seSpline->interpolate(ioSpline->interpolate(valueAtPin));
            
            valueAtPin = (maxValueAtPin)/((i+.5)/maxRaw);
            double high = gearing*seSpline->interpolate(ioSpline->interpolate(valueAtPin));

            valueAtPin = (maxValueAtPin)/((i-.5)/maxRaw);
            double low = gearing*seSpline->interpolate(ioSpline->interpolate(valueAtPin));

            high = high-actual;
            high = max(high,0.0);
            high = min(high,255.0);
            low = low - actual;
            low = max(low,0.0);
            low = min(low,255.0);

            *plusTolerance = max(*plusTolerance, high);
            *minusTolerance = max(*minusTolerance, low);           
        } 
        return;
    }
}

//*******************************************************************
// emitNumericEffecterPdr()
//
// emit a single numeric effecter PDR to config.c.  
//
bool Builder::emitNumericEffecterPdr(JsonObject *binding, JsonObject *entity)
{
    emitStructNewline();
    cOutputFile<<"   // Numeric Effecter "<<(binding->getValue("name"));
    bytesOnLine = 0;
    pdrRecordCount++;

    // get any required parameter values
    JsonArray * parameters = (JsonArray*)(entity->find("parameters"));
    double sampleRate = 0;
    for (int i=0;i<parameters->size();i++) {
        JsonObject * param = (JsonObject*)(parameters->getElement(i));
        if (param->getValue("name").compare("SampleRate")==0) {
            sampleRate = param->getDouble("value");
            break;
        }
    }

    // emit the PDR common header
    emitStructUint32(pdrRecordCount);  // record handle
    emitStructUint8(0x01);             // Pdr header version
    emitStructUint8(PDR_TYPE_NUMERIC_EFFECTER);
    emitStructUint16(0x0001);          // Record Change Number       
    emitPdrSize(74);                // data length

    // emit the sensor information
    emitStructUint16(0x0001);              // Terminus handle
    emitStructUint16(binding->getInteger("effecterID")); // effecter ID
    emitStructUint16(0x6000);              // Entity type
    emitStructUint16(0x0001);              // Entity Instance
    emitStructUint16(0x0001);              // Container ID
    emitStructUint16(0x0000);              // Effecter Semantic ID
    emitStructUint8(0x00);                 // Effecter Init
    emitStructUint8(0x00);                 // Effecter AuxilaryNames PDR
    emitStructUint8(binding->getInteger("physicalBaseUnit"));         // base unit
    emitStructSint8(binding->getInteger("phsicalUnitModifier"));      // unitModifier
    emitStructUint8(binding->getInteger("physicalRateUnit"));         // rateUnit
    emitStructUint8(0x00);                                            // base oem unit handle
    emitStructUint8(binding->getInteger("physicalAuxUnit"));          // aux unit
    emitStructSint8(binding->getInteger("physicalAuxUnitModifier"));  // aux unit modifier
    emitStructUint8(binding->getInteger("physicalAuxRateUnit"));      // aux rate unit
    emitStructUint8(0x00);                 // aux oemUnitHandle
    emitStructUint8(1);                    // isLinear
    emitStructUint8(5);                    // Effecter data size (sint 32)

    double resolution = BASE_RESOLUTION;
    if (positionResolution) {
        switch(binding->getInteger("effecterID")) {
            case 4:  // pfinal effecter
                resolution = positionResolution;
                break;
            case 5:  // vprofile effecter
            case 6:  // aprofile effecter
                resolution *= positionResolution;
        }
    }
    if (binding->getInteger("physicalBaseUnit") == 20) {
        // base units are hertz
        resolution *= sampleRate;
    }
    resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalRateUnit"), sampleRate);
    resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalAuxRateUnit"), sampleRate);
    emitStructReal32(resolution);    // resolution
    emitStructReal32(OFFSET_VALUE);        // offset

    // if the effecter is not virtual - construct in/out curves to calculate the
    // accuracy and tolerance
    if (!binding->getBoolean("isVirtual")) 
    {
        // find the channel in the channel list
        JsonObject *cap = (JsonObject*)(((JsonObject*)pdrjson)->find("capabilities"));
        JsonArray *channels = (JsonArray*)(cap->find("channels"));
        JsonObject* channel = NULL;
        for (int i = 0;i<channels->size();i++) {
            channel = (JsonObject *)channels->getElement(i);
            if (channel->getValue("name")==binding->getValue("boundChannel")==0) break;
        }
        if (!channel) {
            cerr<<"Channel not found for IO Binding "<<binding->getValue("name")<<endl;
            return false;
        }

        // accuracy - this part of the error scales linearly with the reading
        emitStructUint16(channel->getDouble("accuracy")*100);  

        // get the output curve for the output stage
        JsonArray *outputCurve = ((JsonArray*)binding->find("outputCurve"));
        JsonObject *effecter = ((JsonObject*)binding->find("effecter"));
        JsonArray *responseCurve = ((JsonArray*)effecter->find("responseCurve"));

        // create the interpolation curves
        CUCSpline outputSpline(true);
        configureSplineFromPoints(outputCurve, &outputSpline,false);
        CUCSpline responseSpline(true);
        configureSplineFromPoints(responseCurve, &responseSpline,false);

        // calculate and emit the tolerance based on channel specifics
        double plusTolerance;
        double minusTolerance;
        calcPlusMinusTolerance(&plusTolerance, &minusTolerance, channel, binding, &outputSpline, &responseSpline);
        emitStructUint8(plusTolerance);   // plus tolerance
        emitStructUint8(minusTolerance);  // minus tolerance

        if (sampleRate!=0) {
            emitStructReal32(1.0/sampleRate);  // State Transition Interval
            emitStructReal32(1.0/sampleRate);  // Transition Interval
        } else { 
            emitStructReal32(0.0);  // State Transition Interval
            emitStructReal32(0.0);  // Transition Interval
        }
        
        // get the max and min raw values from the selected channel
        string channelName = binding->getValue("boundChannel");

        // get the gearing ratio
        double gearing = binding->getDouble("outputGearingRatio");

        // matching channel has been found output channel-specific values
        double max = gearing*responseSpline.interpolate(outputSpline.interpolate(channel->getDouble("maxValueAtPin")));
        double min = gearing*responseSpline.interpolate(outputSpline.interpolate(channel->getDouble("minValueAtPin")));
        if (min>max) {
            double temp = min;
            min = max;
            max = temp;
        }
        emitStructSint32((max-OFFSET_VALUE)/resolution);                 // max settable
        emitStructSint32((min-OFFSET_VALUE)/resolution);                 // min settable
        emitStructSint8(6); // range field format (real32)
        
        // determine which range values are supported
        unsigned char fieldSupport = 0;
        if (effecter->getValue("ratedMax").compare("NULL")!=0) fieldSupport |= 0x08;
        if (effecter->getValue("nominalValue").compare("NULL")!=0) fieldSupport |= 0x01;
        emitStructUint8(fieldSupport);         // range field support
        emitStructReal32(effecter->getDouble("nominalValue")); // Nominal Value
        emitStructReal32(0.0);                 // Normal Max
        emitStructReal32(0.0);                 // Normal Min
        emitStructReal32(effecter->getDouble("ratedMax"));     // Rated Max
        emitStructReal32(0.0);                 // Rated Min
    } else {
        // VIRTUAL EFFECTER

        emitStructUint16(0);  // accuracy   
        emitStructUint8(0);   // plus tolerance
        emitStructUint8(0);   // minus tolerance

        if (sampleRate!=0) {
            emitStructReal32(1.0/sampleRate);  // State Transition Interval
            emitStructReal32(1.0/sampleRate);  // Transition Interval
        } else { 
            emitStructReal32(0.0);  // State Transition Interval
            emitStructReal32(0.0);  // Transition Interval
        }
        
        emitStructSint32(0x7fffffff); // max settable
        emitStructSint32(0x100000000); // min settable
        emitStructSint8(6);           // range field format (real32)
        emitStructUint8(0);           // range field support
        emitStructReal32(0.0);        // Nominal Value
        emitStructReal32(0.0);        // Normal Max
        emitStructReal32(0.0);        // Normal Min
        emitStructReal32(0.0);        // Rated Max
        emitStructReal32(0.0);        // Rated Min
    }
    return true;
}

//*******************************************************************
// getPositionResolution()
//
// if the specified entity is a profiled motion controller, get the position
// resolution from either the position feedback (closed-loop mode)
// or the output effecter resolution (stepper mode)
//
// parameters:
//   entity - a pointer to the json object for the entity to check
//   bindings - a pointer to the I/O bindings for the entity
// returns:
//   the position resolution to use, or 0 if the default resolution
//   calcuation should be used.
double Builder::getPositionResolution(JsonObject* entity, JsonArray* bindings) {
    // determine if the entity is a profiled motion controller
    if ((entity->find("entityVendorIANA")!=NULL) && 
        (entity->find("entityVendorIANA")->getInteger("")==12634) &&
        (entity->find("vendorEntityID")!=NULL) && 
        (entity->find("vendorEntityID")->getInteger("")==3)) {
        // attempt to find the binding for the feedback numeric sensor
        // (sensor ID 7).  If it is not virtual, use it to determine
        // the position resolution for the entity.
        for (int i = 0;i<bindings->size(); i++) {
            JsonObject* binding = (JsonObject*)(bindings->getElement(i));
            if (binding->getValue("bindingType") == "numericSensor") {
                // here if the binding is a numeric sensor - check to see
                // if it is a real (non-virtual) position sensor
                if ((!binding->getBoolean("isVirtual")) && (binding->getInteger("sensorID")==7)) {
                    // use the resolution of the position sensor for the base resolution
                    JsonArray *inputCurve = ((JsonArray*)binding->find("inputCurve"));
                    JsonObject *sensor = ((JsonObject*)binding->find("sensor"));
                    JsonArray *responseCurve = ((JsonArray*)sensor->find("responseCurve"));

                    // create the interpolation curves
                    CUCSpline inputSpline(true);
                    configureSplineFromPoints(inputCurve, &inputSpline,true);
                    CUCSpline responseSpline(true);
                    configureSplineFromPoints(responseCurve, &responseSpline,true);

                    // get the gearing ratio
                    double gearing = binding->getDouble("inputGearingRatio");
                    if (gearing == 0) gearing = 1.0;

                    // return the base resolution
                    return gearing*inputSpline.interpolate(responseSpline.interpolate(1.0));
                }
            } 
        }

        // attempt to find the binding for the output effecter
        // Use it to determine the position resolution for the entity.
        for (int i = 0;i<bindings->size(); i++) {
            JsonObject* binding = (JsonObject*)(bindings->getElement(i));
            if (binding->getValue("name") == "OutputEffecter") {
                // use the resolution of the position sensor for the base resolution
                JsonArray *outputCurve = ((JsonArray*)binding->find("outputCurve"));
                JsonObject *effecter = ((JsonObject*)binding->find("effecter"));
                JsonArray *responseCurve = ((JsonArray*)effecter->find("responseCurve"));

                // create the interpolation curves
                CUCSpline outputSpline(true);
                configureSplineFromPoints(outputCurve, &outputSpline,false);
                CUCSpline responseSpline(true);
                configureSplineFromPoints(responseCurve, &responseSpline,false);

                // get the gearing ratio
                double gearing = binding->getDouble("inputGearingRatio");
                if (gearing == 0) gearing = 1.0;

                // return the base resolution
                return gearing*outputSpline.interpolate(responseSpline.interpolate(1.0));
            } 
        }
    }
    // use the default resolution calculations.
    return 0;
}

//*******************************************************************
// emitSensorEffecterPdrs()
//
// emit any Sensor or Effecter PDRs to config.c.  
//
void Builder::emitSensorEffecterPdrs()
{
    // get the logical Entity from the config file
    JsonObject* cfg = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonArray*  entities = (JsonArray*)(cfg->find("logicalEntities"));

    // loop for each entity
    for (unsigned int i = 0; i<entities->size(); i++) {
        JsonObject* entity = (JsonObject*)(entities->getElement(i));

        // get the I/O Bindings
        JsonArray*  bindings = (JsonArray*)(entity->find("ioBindings"));

        // if this entity is a profiled motion controller, get the position
        // resolution - this will be determined by the position feedback
        // if the controller is closed-loop, otherwise, it will be determined
        // by the output effecter resolution (stepper mode)
        positionResolution = getPositionResolution(entity, bindings);

        // loop for each binding
        for (int j = 0;j<bindings->size(); j++) {
            JsonObject* binding = (JsonObject*)(bindings->getElement(j));

            // skip this binding if it does not get emitted to the PDR
            if (!binding->getBoolean("includeInPdr")) continue;
            // emit the particular PDR type
            if (binding->getValue("bindingType").compare("stateSensor")==0) {
                emitStateSensorPdr(binding,entity);
            } else if (binding->getValue("bindingType").compare("numericSensor")==0) {
                emitNumericSensorPdr(binding,entity);
            } else if (binding->getValue("bindingType").compare("stateEffecter")==0) {
                emitStateEffecterPdr(binding,entity);
            } else if (binding->getValue("bindingType").compare("numericEffecter")==0) {
                emitNumericEffecterPdr(binding,entity);
            } 
        }
    }
}

//*******************************************************************
// emitSensorLinearizationTables()
//
// emit any numeric Sensor or Effecter linearization tables to config.c.  
//
void Builder::emitLinearizationTables()
{
    // get the logical Entity from the config file
    JsonObject* cfg = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonArray*  entities = (JsonArray*)(cfg->find("logicalEntities"));

    // loop for each entity
    for (unsigned int i = 0; i<entities->size(); i++) {
        JsonObject* entity = (JsonObject*)(entities->getElement(i));

        // get the I/O Bindings
        JsonArray*  bindings = (JsonArray*)(entity->find("ioBindings"));

        // loop for each binding
        for (int j = 0;j<bindings->size(); j++) {
            JsonObject* binding = (JsonObject*)(bindings->getElement(j));
            // skip this binding is virtual
            if (binding->getBoolean("isVirtual")) continue;
            // only worry about linearization for state effecters and sensors
            if ((binding->getValue("bindingType")=="numericSensor")||
                (binding->getValue("bindingType")=="numericEffecter")) {
                
                // find the channel in the channel list
                JsonObject *cap = (JsonObject*)(((JsonObject*)pdrjson)->find("capabilities"));
                JsonArray *channels = (JsonArray*)(cap->find("channels"));
                JsonObject* channel = NULL;
                for (int i = 0;i<channels->size();i++) {
                    channel = (JsonObject *)channels->getElement(i);
                    if (channel->getValue("name")==binding->getValue("boundChannel")) break;
                }
                if (!channel) {
                    cerr<<"Channel not found for IO Binding "<<binding->getValue("name")<<endl;
                    return;
                }

                // create the interpolation curves based on binding information
                CUCSpline seSpline(true);
                CUCSpline responseSpline(true);
                double gearing;
                if (binding->getValue("bindingType")=="numericEffecter") {
                    // get the output curve for the output stage
                    JsonArray *outputCurve = ((JsonArray*)binding->find("outputCurve"));
                    JsonObject *effecter = ((JsonObject*)binding->find("effecter"));
                    JsonArray *responseCurve = ((JsonArray*)effecter->find("responseCurve"));

                    // create the interpolation curves
                    configureSplineFromPoints(outputCurve, &seSpline, false);
                    configureSplineFromPoints(responseCurve, &responseSpline,false);

                    // get the gearing ratio
                    gearing = binding->getDouble("outputGearingRatio");

                } else {
                    // get the input curve for the input stage
                    JsonArray *inputCurve = ((JsonArray*)binding->find("inputCurve"));
                    JsonObject *sensor = ((JsonObject*)binding->find("sensor"));
                    JsonArray *responseCurve = ((JsonArray*)sensor->find("responseCurve"));

                    // create the interpolation curves
                    configureSplineFromPoints(inputCurve, &seSpline,true);
                    configureSplineFromPoints(responseCurve, &responseSpline,true);
                
                    // get the gearing ratio
                    gearing = binding->getDouble("inputGearingRatio");
                }

                // get the precision for the channel (in bits)
                int precision = channel->getInteger("precision");
                double maxRaw,minRaw;        
                maxRaw = 1<<abs(precision);
                minRaw = 0;
                if (precision<0) {
                    maxRaw /= 2.0;
                    minRaw = -maxRaw;
                }

                double channelMin = channel->getDouble("minValue");
                double channelMax = channel->getDouble("maxValue");
                if (channelMin== channelMax) {
                    channelMin = 0;
                    channelMax = 2.5;
                }                
                double channelStep = (channelMax-channelMin)/64;

                JsonArray * parameters = (JsonArray*)(entity->find("parameters"));
                double sampleRate = 4000;  // default sample rate
                for (int i=0;i<parameters->size();i++) {
                    JsonObject * param = (JsonObject*)(parameters->getElement(i));
                    if (param->getValue("name").compare("SampleRate")==0) {
                        sampleRate = param->getDouble("value");
                        break;
                    }
                }
                double resolution = BASE_RESOLUTION;
                if (positionResolution) {
                    switch(binding->getInteger("sensorID")) {
                        case 4:  // verr
                        case 5:  // perr
                        case 6:  // velocity
                            resolution *= positionResolution;
                            break;
                        case 7:  // position
                            resolution = positionResolution;
                    }
                }
                if (binding->getInteger("physicalBaseUnit") == 20) {
                    // base units are hertz
                    resolution *= sampleRate;
                }
                resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalRateUnit"), sampleRate);
                resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalAuxRateUnit"), sampleRate);

                // loop for each value in the output table;
                cOutputFile<<"LINTABLE_TYPE __lintable_"<<channel->getValue("name")<<"[] LINTABLE_DATA_ATTRIBUTES = { "<<endl<<"   ";
                unsigned int wordsOnLine = 0;
                for (double x=-2*channelStep+channelMin; x<=channelMax+2*channelStep; x+=channelStep) {
                    // calculate the table value
                    double y = gearing*responseSpline.interpolate(seSpline.interpolate(x));
                    y = y/resolution;
                    // saturate on overflow
                    if (y>0x7FFFFFFFL) y = 0x7FFFFFFF;
                    if (y<-0x7FFFFFFFL) y = -0x7FFFFFFF;

                    // output the table value to the c file
                    cOutputFile<<"0x"<<hex<<setw(8)<<setfill('0')<<(unsigned int)(y+.5);
                    
                    // if this was not the last table value, emit the separating comma
                    if (x<=channelMax+channelStep) cOutputFile<<", ";

                    // if there are now 8 values on this line, output a newline character
                    wordsOnLine++;
                    if (wordsOnLine==8) {
                        cOutputFile<<endl<<"   ";
                        wordsOnLine=0;
                    }
                }
                // output the closing curly brace
                if (wordsOnLine!=0) cOutputFile<<endl;
                cOutputFile<<"};"<<endl;
            } 
        }
    }
}

//*******************************************************************
// emitHIntro()
//
// emit the file comment block for config.h.
//
void Builder::emitHIntro()
{
    hOutputFile<<"//*****************************************************************"<<endl;
    hOutputFile<<"// config.h"<<endl;
    hOutputFile<<"//"<<endl;
    hOutputFile<<"// This file was auto-generated by the PICMG firmware builder"<<endl;
    hOutputFile<<"// utility.  It includes macro definitions and data declarations"<<endl;
    hOutputFile<<"// for a custom firmware build"<<endl;
    hOutputFile<<"#pragma once"<<endl;
    hOutputFile<<"#include \"platform_defs.h\""<<endl;
    
    hOutputFile<<endl;
}

//*******************************************************************
// toUpper()
//
// convert the input string to an all upper-case representation
// and return the result;
static string toUpper(string str) {
    string result = "";
    for (unsigned int i = 0; i<str.size(); i++) {
        if (str[i]<=' ') continue; // ignore whitespace
        result += toupper(str[i]);
    }
    return result;
}

//*******************************************************************
// calcDefaultValue()
//
// calculate the default value for an effecter based on the resolution
// and offset parameters of the binding.
//
double Builder::calcDefaultValue(JsonObject *binding, JsonObject *entity)
{
    // get any required parameter values
    JsonArray * parameters = (JsonArray*)(entity->find("parameters"));
    double sampleRate = 0;
    for (int i=0;i<parameters->size();i++) {
        JsonObject * param = (JsonObject*)(parameters->getElement(i));
        if (param->getValue("name").compare("SampleRate")==0) {
            sampleRate = param->getDouble("value");
            break;
        }
    }

    double resolution = BASE_RESOLUTION;

    if (positionResolution) {
        switch(binding->getInteger("effecterID")) {
            case 4:  // pfinal effecter
                resolution = positionResolution;
                break;
            case 5:  // vprofile effecter
            case 6:  // aprofile effecter
                resolution *= positionResolution;
        }
    }
    if (binding->getInteger("physicalBaseUnit") == 20) {
        // base units are hertz
        resolution *= sampleRate;
    }
    resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalRateUnit"), sampleRate);
    resolution = scaleResolutionByRateUnit(resolution, binding->getInteger("physicalAuxRateUnit"), sampleRate);

    double defaultVal = 0;
    if (binding->find("defaultValue")) {
        defaultVal = binding->getDouble("defaultValue");
        defaultVal = ((defaultVal - OFFSET_VALUE)/resolution);
    }
    return defaultVal;
}

//*******************************************************************
// emitMacros()
//
// emit the macros associated with this build.  Macros emitted are:
// Module-related macros
// FRU related macros
// LogicalEntity related macros
//
void Builder::emitMacros()
{
    JsonObject *cfg     = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonObject *cap     = (JsonObject*)(((JsonObject*)pdrjson)->find("capabilities"));
    JsonArray *entities = (JsonArray*)cfg->find("logicalEntities");

    hOutputFile<<"//===================="<<endl;
    hOutputFile<<"// Module-Related Macros"<<endl;
    if (!cap->getValue("device").empty()) hOutputFile<<"#define "<<toUpper(cap->getValue("device"))<<endl;
    hOutputFile<<endl;

    hOutputFile<<"//===================="<<endl;
    hOutputFile<<"// PDR-Related Macros"<<endl;
    hOutputFile<<"extern PDR_BYTE_TYPE __pdr_data[] PDR_DATA_ATTRIBUTES;"<<endl;
    hOutputFile<<"#define PDR_TOTAL_SIZE "<<totalPdrSize<<endl;
    hOutputFile<<"#define PDR_NUMBER_OF_RECORDS "<<pdrRecordCount<<endl;
    hOutputFile<<"#define PDR_MAX_RECORD_SIZE "<<largestPdrRecordSize<<endl;
    hOutputFile<<endl;

    hOutputFile<<"//===================="<<endl;
    hOutputFile<<"// FRU-Related Macros"<<endl;
    hOutputFile<<"extern FRU_BYTE_TYPE __fru_data[] FRU_DATA_ATTRIBUTES;"<<endl;
    hOutputFile<<"#define FRU_TABLE_MAXIMUM_SIZE "<<maxAllowedFruSize<<endl;
    hOutputFile<<"#define FRU_TOTAL_SIZE "<<totalFruSize<<endl;
    hOutputFile<<"#define FRU_TOTAL_RECORD_SETS 1"<<endl;
    hOutputFile<<"#define FRU_NUMBER_OF_RECORDS "<<fruRecordCount<<endl;
    hOutputFile<<"#define FRU_MAX_RECORD_SIZE "<<largestFruRecordSize<<endl;
    hOutputFile<<endl;

    hOutputFile<<"//===================="<<endl;
    hOutputFile<<"// Channel-Related Macros"<<endl;
    for (int i=0;i<entities->size();i++) {
        JsonObject *entity = (JsonObject*)entities->getElement(i);
        JsonArray *bindings = (JsonArray*)entity->find("ioBindings");
        for (int j=0;j<bindings->size();j++) {
            JsonObject *binding = (JsonObject*)bindings->getElement(j);
            if ((binding->find("boundChannel")!=NULL)&&(binding->getValue("boundChannel")!="NULL")) 
                hOutputFile<<"#define CHANNEL_"<<toUpper(binding->getValue("boundChannel"))<<endl;
        }
    }
    hOutputFile<<endl;

    hOutputFile<<"//===================="<<endl;
    hOutputFile<<"// Logical Entity-Related Macros"<<endl;
    for (int i=0;i<entities->size();i++) {
        JsonObject *entity = (JsonObject*)entities->getElement(i);
        string entityRef = string("ENTITY_")+toUpper(entity->getValue("name"));
        hOutputFile<<"#define "<<entityRef<<endl;

        JsonArray *bindings = (JsonArray*)entity->find("ioBindings");
        positionResolution = getPositionResolution(entity, bindings);
        for (int j=0;j<bindings->size();j++) {
            JsonObject *binding = (JsonObject*)bindings->getElement(j);
            string bindingName = entityRef + "_" + toUpper(binding->getValue("name")); 
            hOutputFile<<"#define "<<bindingName<<endl;
            if ((binding->find("bindingType")!=NULL)&&(binding->getValue("bindingType")!="NULL")) {
                string bindingType = binding->getValue("bindingType");
                hOutputFile<<"#define "<<bindingName+"_BINDINGTYPE_"+toUpper(bindingType)<<endl;
                if (((bindingType == "numericSensor")||(bindingType == "numericEffecter"))&&(binding->getValue("boundChannel")!="NULL")) {
                    // find the channel in the channel list
                    JsonArray *channels = (JsonArray*)(cap->find("channels"));
                    JsonObject* channel = NULL;
                    for (int cnum = 0;cnum<channels->size();cnum++) {
                        channel = (JsonObject *)channels->getElement(cnum);
                        if (channel->getValue("name")==binding->getValue("boundChannel")) break;
                    }
                    if (!channel) {
                        cerr<<"Channel not found for IO Binding "<<binding->getValue("name")<<endl;
                        return;
                    }
                    hOutputFile<<"extern LINTABLE_TYPE __lintable_"<<channel->getValue("name")<<"[] LINTABLE_DATA_ATTRIBUTES;"<<endl;
                    hOutputFile<<"#define "<<bindingName+"_BOUNDCHANNEL_PRECISION "<<toUpper(channel->getValue("precision"))<<endl;
                }
            }
            if ((binding->find("sensorID")!=NULL)&&(binding->getValue("sensorID")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_SENSORID "<<toUpper(binding->getValue("sensorID"))<<endl;
            if ((binding->find("effecterID")!=NULL)&&(binding->getValue("effecterID")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_EFFECTERID "<<toUpper(binding->getValue("effecterID"))<<endl;
            if ((binding->find("boundChannel")!=NULL)&&(binding->getValue("boundChannel")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_BOUNDCHANNEL "<<binding->getValue("boundChannel")<<endl;
            if ((binding->find("usedStates")!=NULL)&&(binding->getValue("usedStates")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_USEDSTATES "<<toUpper(binding->getValue("usedStates"))<<endl;
            if ((binding->find("stateWhenHigh")!=NULL)&&(binding->getValue("stateWhenHigh")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_STATEWHENHIGH "<<toUpper(binding->getValue("stateWhenHigh"))<<endl;
            if ((binding->find("stateWhenLow")!=NULL)&&(binding->getValue("stateWhenLow")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_STATEWHENLOW "<<toUpper(binding->getValue("stateWhenLow"))<<endl;
            if ((binding->find("defaultState")!=NULL)&&(binding->getValue("defaultState")!="NULL")) 
                hOutputFile<<"#define "<<bindingName+"_DEFAULTSTATE "<<toUpper(binding->getValue("defaultState"))<<endl;
            if (binding->getValue("bindingType") == "numericSensor") {
                unsigned char enabledThresholds = 0;                
                if ((binding->find("normalMin")!=NULL)&&(binding->getValue("normalMin")!="NULL")) {
                    hOutputFile<<"#define "<<bindingName+"_NORMALMIN "<<toUpper(binding->getValue("normalMin"))<<endl;
                    enabledThresholds |= 0x4;
                } else hOutputFile<<"#define "<<bindingName+"_NORMALMIN "<<0<<endl;
                if ((binding->find("normalMax")!=NULL)&&(binding->getValue("normalMax")!="NULL")) {
                    hOutputFile<<"#define "<<bindingName+"_NORMALMAX "<<toUpper(binding->getValue("normalMax"))<<endl;
                    enabledThresholds |= 0x2;
                } else hOutputFile<<"#define "<<bindingName+"_NORMALMAX "<<0<<endl;
                if ((binding->find("upperThresholdWarning")!=NULL)&&(binding->getValue("upperThresholdWarning")!="NULL"))
                    hOutputFile<<"#define "<<bindingName+"_UPPERTHRESHOLDWARNING "<<toUpper(binding->getValue("upperThresholdWarning"))<<endl;
                if ((binding->find("upperThresholdCritical")!=NULL)&&(binding->getValue("upperThresholdCritical")!="NULL")) {
                    hOutputFile<<"#define "<<bindingName+"_UPPERTHRESHOLDCRITICAL "<<toUpper(binding->getValue("upperThresholdCritical"))<<endl;
                    enabledThresholds |= 0x8;            
                } else hOutputFile<<"#define "<<bindingName+"_UPPERTHRESHOLDCRITICAL "<<0<<endl;
                if ((binding->find("upperThresholdFatal")!=NULL)&&(binding->getValue("upperThresholdFatal")!="NULL")) { 
                    hOutputFile<<"#define "<<bindingName+"_UPPERTHRESHOLDFATAL "<<toUpper(binding->getValue("upperThresholdFatal"))<<endl;
                    enabledThresholds |= 0x20;
                } else hOutputFile<<"#define "<<bindingName+"_UPPERTHRESHOLDFATAL "<<0<<endl;
                if ((binding->find("lowerThresholdWarning")!=NULL)&&(binding->getValue("lowerThresholdWarning")!="NULL")) 
                    hOutputFile<<"#define "<<bindingName+"_LOWERTHRESHOLDWARNING "<<toUpper(binding->getValue("lowerThresholdWarning"))<<endl;
                if ((binding->find("lowerThresholdCritical")!=NULL)&&(binding->getValue("lowerThresholdCritical")!="NULL")) { 
                    hOutputFile<<"#define "<<bindingName+"_LOWERTHRESHOLDCRITICAL "<<toUpper(binding->getValue("lowerThresholdCritical"))<<endl;
                    enabledThresholds |= 0x10;
                } else hOutputFile<<"#define "<<bindingName+"_LOWERTHRESHOLDCRITICAL "<<0<<endl;
                if ((binding->find("lowerThresholdFatal")!=NULL)&&(binding->getValue("lowerThresholdFatal")!="NULL")) {
                    hOutputFile<<"#define "<<bindingName+"_LOWERTHRESHOLDFATAL "<<toUpper(binding->getValue("lowerThresholdFatal"))<<endl;
                    enabledThresholds |= 0x40;
                } else hOutputFile<<"#define "<<bindingName+"_LOWERTHRESHOLDFATAL "<<0<<endl;
                if (binding->getValue("bindingType") == "numericSensor")
                    hOutputFile<<"#define "<<bindingName+"_ENABLEDTHRESHOLDS "<<(unsigned int)enabledThresholds<<endl;
            }
            if (binding->getValue("bindingType") == "numericEffecter") {
                if (binding->find("defaultValue")) {
                    // convert the default value using the resolution/offset for the effecter
                    hOutputFile<<"#define "<<bindingName+"_DEFAULTVALUE "<<(unsigned long)calcDefaultValue(binding,entity)<<endl;
                }
            }
        }

        JsonArray *parameters = (JsonArray*)entity->find("parameters");
        for (int j=0;j<parameters->size();j++) {
            JsonObject *parameter = (JsonObject*)parameters->getElement(j);
            string paramName = entityRef + "_PARAM_" + toUpper(parameter->getValue("name")); 
            if (parameter->getValue("type").compare("enum")==0) {
                // this is an enumerated typue - just define the macro name
                hOutputFile<<"#define "<<paramName+"_"+toUpper(parameter->getValue("value"))<<endl;
            } else {
                // update the name and the value
                hOutputFile<<"#define "<<paramName<<" "<<toUpper(parameter->getValue("value"))<<endl;
            }
        }
    }
    hOutputFile<<endl;

}

//*******************************************************************
// build()
//
// create the build files from the config file given as the first 
// parameter and place the resulting files in the folder specified 
// by the second parameter.
//
// This program returns true if successful, otherwise false.
//
bool Builder::build(string inputFilename, string outputPath) {
    oemStateSetMap.clear();

    //========================
    // Read the config Json File
    pdrjson = loadJsonFile(inputFilename);
    if ((!pdrjson)||(typeid(*pdrjson) != typeid(JsonObject))) {
        cerr << "Invalid input Json file " <<inputFilename<< endl;
        return false;
    }

    //========================
    // open the output files
    string cfilepath = outputPath;
    cfilepath.append("config.c");
    string hfilepath = outputPath;
    hfilepath.append("config.h");
    cOutputFile.open(cfilepath);
    if (!cOutputFile.is_open()) {
        cerr << "error opening output file " << cfilepath << endl;
        return false;
    }
    hOutputFile.open(hfilepath);
    if (!hOutputFile.is_open()) {
        cerr << "error opening output file " << hfilepath << endl;
        return false;
    }

    //========================
    // Create Common Structures
    emitCIntro();
    startPdr();
    emitTerminusLocatorPdr();
    JsonObject* cfgObj = (JsonObject*)(((JsonObject*)pdrjson)->find("configuration"));
    JsonArray* fruRecords = (JsonArray*)(cfgObj->find("fruRecords"));
    for (int i=1;i<=fruRecords->size();i++) emitFruRecordSetPdr();

    //========================
    // Create Entity Associations
    emitEntityAssociationPdr();
    emitOemEntityIdPdr();

    //========================
    // Create OEM State Set PDRS
    emitOemStateSetPdrs();

    //========================
    // Create Logical Entity Content
    emitSensorEffecterPdrs();
    endPdr();

    //========================
    // Create Fru Record Structures
    emitFruRecords();
    
    //========================
    // Create Linearization Structures
    emitLinearizationTables();

    //========================
    // Emit Macro Definitions
    emitHIntro();
    emitMacros();

    cOutputFile.close();
    hOutputFile.close();
    return true;
}