#pragma once

#include "ofMain.h"
#include "ofApp.h"
#include <json/json.h>

/**
   --------------------------------------------------------------------------
   \fn         ofxJsonLoader
   \brief      This function loads a JSON file from the data directory and
               returns a Json object containing the data
   @param[in]  filename  the full name of the json file to open from the data
               directory 
   \return     A Json::Value object containing everything read from the file
   \author     Greg von Winckel
   \date       September 20, 2017
   --------------------------------------------------------------------------
*/

inline Json::Value ofxJsonLoader( const string& filename ) {

  // Look for data/parameters.json and read it into a buffer. The false 
  // means that the file is text not binary
  ofBuffer buf = ofBufferFromFile(filename,false);

  // Number of characters in the buffer
  long size = buf.size();

  // Array of characters in the buffer
  const char* text = buf.getData();

  // Factory that creates a character reader object
  Json::CharReaderBuilder rbuilder;

  // Pointer to object that can extract key-value pairs from the file
  unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());

  // Object that stores the key-value pairs (can be heirarchical)
  Json::Value obj;

  // Collect error messages from parsing
  string errs;

  // Read from the buffer and write to the Value object
  bool success = reader->parse(text,text+size*sizeof(char), &obj, &errs );   

  // If things went pear-shaped say what's wrong with the json file
  if(!success) {
    cout << "Error parsing file " + filename + ": \n";
    cout << errs << std::endl;
  }

  return obj;
}

