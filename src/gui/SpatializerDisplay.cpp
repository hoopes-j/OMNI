//
//  BinauralDisplay.cpp
//  spatializer
//
//  Created by Jason Hoopes on 4/9/24.
//

#include "SpatializerDisplay.hpp"

void BinauralDisplay::setup(int numSpatializers) {
    
    this->_numSpatializers = numSpatializers;
    states.resize(_numSpatializers);
    for (int i = 0; i < numSpatializers; i++) {
        states[i].boxPos = ofVec2f(0, 0);
        states[i].boxSize = ofVec2f(200,200);
    }
    
    pos = ofVec3f(100,300,100);
    

//
//    box.set(state.boxSize.x);
//
    double fps = 30;
//
//        ofSetFrameRate(fps);
        ofBackground(255);
//        ofSetBackgroundAuto(true);
//        ofSetVerticalSync(true);

//        ofEnableDepthTest();
//        ofEnableSmoothing();

        for (int i = 0; i < 3; i++) {
            ofVec3f t = ofVec3f(ofRandom(1000), ofRandom(1000), ofRandom(1000));
            times.push_back(t);

            ofVec3f a = ofVec3f(0, 0, 0);
            angles.push_back(a);
        }
    
    
    cam.removeAllInteractions();
    cam.addInteraction(ofEasyCam::TransformType::TRANSFORM_ROTATE, 0);
    
    
    
}

void BinauralDisplay::update() {
//    for (int i = 0; i < angles.size(); i++) {
//           angles.at(i).x = ofMap(ofSignedNoise(times.at(i).x), -1, 1, -180, 180);
//           angles.at(i).y = ofMap(ofSignedNoise(times.at(i).y), -1, 1, -180, 180);
//           angles.at(i).z = ofMap(ofSignedNoise(times.at(i).z), -1, 1, -180, 180);
//
//           times.at(i) += 0.01;
//       }
}

void BinauralDisplay::updatePosition(int idx, float azimuth, float elevation) {
    
    this->states[idx].azimuth = azimuth;
    this->states[idx].elevation = elevation;
//    for (int i = 0; i < angles.size(); i++) {
//           angles.at(i).x = ofMap(ofSignedNoise(times.at(i).x), -1, 1, -180, 180);
//           angles.at(i).y = ofMap(ofSignedNoise(times.at(i).y), -1, 1, -180, 180);
//           angles.at(i).z = ofMap(ofSignedNoise(times.at(i).z), -1, 1, -180, 180);
//
//           times.at(i) += 0.01;
//       }
}

void BinauralDisplay::draw() {
    
   
    
//    box.setPosition(100, 100, 0);
//
//    float spinX, spinY;
//    spinX = spinY = 0.5;
//
//    box.rotate(spinX, 1.0, 0.0, 0.0);
//    box.rotate(spinY, 0, 1.0, 0.0);
//
//
//    // get all the faces from the icoSphere, handy when you want to copy
//    // individual vertices or tweak them a little ;)
//    vector<ofMeshFace> triangles = box.getMesh().getUniqueFaces();
//
//    // now draw
//    box.draw();
    
    ofDrawBitmapString("Spatializer", 100, 100);
    
    float radius = 100;

    
    cam.begin();
    
    int i = 2;
    float x = angles.at(i).x;
    float y = angles.at(i).y;
    float z = angles.at(i).z;
            
    ofPushMatrix();

    ofRotateXDeg(x);
    ofRotateYDeg(y);
    ofRotateZDeg(z);
    ofSetColor(230, 28, 139);
    ofFill();
    
    // Render all the spatializers
    for (int i = 0; i < _numSpatializers; i++) {
        float grainX = cosf((float)states[i].azimuth/180*PI-PI/2)*radius;
        float grainZ = sinf((float)states[i].azimuth/180*PI-PI/2)*radius;
        ofDrawBox(glm::vec3(grainX+pos.x, pos.y, grainZ+pos.z), 20);
    }

    

    
    ofPushMatrix();

    ofRotateXDeg(x);
    ofRotateYDeg(y);
    ofRotateZDeg(z);
    ofSetColor(48, 53, 149);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawBox(glm::vec3(pos.x, pos.y, pos.z), 200);

    cam.end();
    
}

void BinauralDisplay::cleanup() {
    
}
