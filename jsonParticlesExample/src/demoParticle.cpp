#include "demoParticle.h"
#include "ofxJsonLoader.h"

//------------------------------------------------------------------
demoParticle::demoParticle(){

  // Read particle parameters from JSON file
  Json::Value json = ofxJsonLoader("parameters.json");  

  // No reference operator & this time as we want to copy 
  particleJson = json["Particle"];
 
	attractPoints = NULL;
}

//------------------------------------------------------------------
void demoParticle::setMode(particleMode newMode){
	mode = newMode;
}

//------------------------------------------------------------------
void demoParticle::setAttractPoints( vector <ofPoint> * attract ){
	attractPoints = attract;
}

//------------------------------------------------------------------
void demoParticle::reset(){

  // Get the Particle parameter sublist
  Json::Value& resetJson = particleJson["Reset"];

	//the unique val allows us to set properties slightly differently for each particle
  Json::Value uvmin = resetJson.get("Unique Value Min", -10000);
  Json::Value uvmax = resetJson.get("Unique Value Max",  10000);

	uniqueVal = ofRandom( uvmin.asInt() , uvmax.asInt() );
	
	pos.x = ofRandomWidth();
	pos.y = ofRandomHeight();
	
  Json::Value velocityBound = resetJson.get("Velocity Bound",3.9);
  float velBnd = velocityBound.asFloat();

	vel.x = ofRandom(-velBnd, velBnd);
	vel.y = ofRandom(-velBnd, velBnd);
	
	frc   = ofPoint(0,0,0);
	
  Json::Value scaleMin = resetJson.get("Minimum Scale",0.5);
  Json::Value scaleMax = resetJson.get("Maximum Scale",1.0);

	scale = ofRandom( scaleMin.asFloat(), scaleMax.asFloat() );
	
	if( mode == PARTICLE_MODE_NOISE ){

    float dragMin = resetJson.get("Drag Min Noise",0.97).asFloat();
    float dragMax = resetJson.get("Drag Max Noise",0.99).asFloat();
		drag  = ofRandom(dragMin, dragMax);

    float velMultiplier = resetJson.get("Velocity Multiplier",3.0).asFloat();
		vel.y = fabs(vel.y) * velMultiplier; //make the particles all be going down

	}else{

    float dragMin = resetJson.get("Drag Min",0.95).asFloat();
    float dragMax = resetJson.get("Drag Max",0.998).asFloat();
  	drag  = ofRandom(dragMin, dragMax);	

	}
}

//------------------------------------------------------------------
void demoParticle::update(){

   Json::Value& updateJson = particleJson["Update"];
   
	//1 - APPLY THE FORCES BASED ON WHICH MODE WE ARE IN 
	
	if( mode == PARTICLE_MODE_ATTRACT ){

    Json::Value& attractJson = updateJson["Attract"];
    
		ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
		frc = attractPt-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
		frc.normalize(); //by normalizing we disregard how close the particle is to the attraction point 
		
		vel *= drag; //apply drag

    float forceMultiplier = attractJson.get("Force Multiplier",0.6).asFloat();

		vel += frc * forceMultiplier; //apply force
	}
	else if( mode == PARTICLE_MODE_REPEL ){
 
    Json::Value& repelJson = updateJson["Repel"];  

		ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
		frc = attractPt-pos; 
		
		//let get the distance and only repel points close to the mouse
		float dist = frc.length();
		frc.normalize(); 
		
		vel *= drag; 

    float distThreshold = repelJson.get("Distance Threshold",150).asFloat();

		if( dist < distThreshold ){
      
      float forceMultiplier = repelJson.get("Near Force Multiplier",0.6).asFloat();

			vel += -frc * forceMultiplier; //notice the frc is negative 
		}else{
			//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
			frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
			frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
      float forceMultiplier = repelJson.get("Far Force Multiplier",0.6).asFloat();
			vel += frc * forceMultiplier;
		}
	}
	else if( mode == PARTICLE_MODE_NOISE ){

    
    Json::Value& noiseJson = updateJson["Noise"];  

		//lets simulate falling snow 
		//the fake wind is meant to add a shift to the particles based on where in x they are
		//we add pos.y as an arg so to prevent obvious vertical banding around x values - try removing the pos.y * 0.006 to see the banding
		float fakeWindX = ofSignedNoise(pos.x * 0.003, pos.y * 0.006, ofGetElapsedTimef() * 0.6);
		
		frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.6;
		frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, ofGetElapsedTimef()*0.2) * 0.09 + 0.18;

		vel *= drag; 

    float forceMultiplier = noiseJson.get("Force Multiplier",0.4).asFloat();

		vel += frc * forceMultiplier;
		
		//we do this so as to skip the bounds check for the bottom and make the particles go back to the top of the screen
		if( pos.y + vel.y > ofGetHeight() ){
			pos.y -= ofGetHeight();
		}
	}
	else if( mode == PARTICLE_MODE_NEAREST_POINTS ){
		
		if( attractPoints ){

      Json::Value& nearestJson = updateJson["Nearest"];

			//1 - find closest attractPoint 
			ofPoint closestPt;
			int closest = -1; 
			float closestDist = 9999999;
			
			for(unsigned int i = 0; i < attractPoints->size(); i++){
				float lenSq = ( attractPoints->at(i)-pos ).lengthSquared();
				if( lenSq < closestDist ){
					closestDist = lenSq;
					closest = i;
				}
			}
			
			//2 - if we have a closest point - lets calcuate the force towards it
			if( closest != -1 ){
				closestPt = attractPoints->at(closest);				
				float dist = sqrt(closestDist);
				
				//in this case we don't normalize as we want to have the force proportional to distance 
				frc = closestPt - pos;
		
				vel *= drag;
 				 
        float minDist = nearestJson.get("Minimum Distance",40).asFloat();
        float maxDist = nearestJson.get("Maximum Distance",300).asFloat();

				//lets also limit our attraction to a certain distance and don't apply if 'f' key is pressed
				if( dist < maxDist && dist > minDist && !ofGetKeyPressed('f') ){
          float forceMultiplier = nearestJson.get("In Range Force Multiplier",0.4).asFloat();
					vel += frc * forceMultiplier;
				}else{
          float forceMultiplier = nearestJson.get("Out of Range Force Multiplier",0.4).asFloat();
					//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
					frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
					frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
					vel += frc * forceMultiplier;
				}
				
			}
		
		}
		
	}
	
	
	//2 - UPDATE OUR POSITION
	
	pos += vel; 
	
	
	//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
	//we could also pass in bounds to check - or alternatively do this at the ofApp level
	if( pos.x > ofGetWidth() ){
		pos.x = ofGetWidth();
		vel.x *= -1.0;
	}else if( pos.x < 0 ){
		pos.x = 0;
		vel.x *= -1.0;
	}
	if( pos.y > ofGetHeight() ){
		pos.y = ofGetHeight();
		vel.y *= -1.0;
	}
	else if( pos.y < 0 ){
		pos.y = 0;
		vel.y *= -1.0;
	}	
		
}

//------------------------------------------------------------------
void demoParticle::draw(){

	if( mode == PARTICLE_MODE_ATTRACT ){
		ofSetColor(255, 63, 180);
	}
	else if( mode == PARTICLE_MODE_REPEL ){
		ofSetColor(208, 255, 63);
	}
	else if( mode == PARTICLE_MODE_NOISE ){
		ofSetColor(99, 63, 255);
	}
	else if( mode == PARTICLE_MODE_NEAREST_POINTS ){
		ofSetColor(103, 160, 237);
	}
			
	ofDrawCircle(pos.x, pos.y, scale * 4.0);
}

