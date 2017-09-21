#include "ofMain.h"
#include "ofApp.h"
#include "ofxJsonLoader.h"

//========================================================================
int main( ) {

  // Object that stores the key-value pairs (can be heirarchical)
  Json::Value obj = ofxJsonLoader("parameters.json");
  
  const Json::Value& window = obj["Window"]; // Window size parameters 

  // Extract the value for "Width" if it exists. Otherwise use the default value
  const Json::Value winWidth = window.get("Width",1024);

  // Extract the value for "Height" if it exists. Otherwise use the default value
  const Json::Value winHeight = window.get("Height",768);

  // setup the GL context
  ofSetupOpenGL(winWidth.asInt(),winHeight.asInt(), OF_WINDOW);	
  
  // this kicks off the running of my app
  // can be OF_WINDOW or OF_FULLSCREEN
  // pass in width and height too:
  ofRunApp( new ofApp());
  
}
