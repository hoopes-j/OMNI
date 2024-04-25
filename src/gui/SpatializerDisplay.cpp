//
//  SpatializerDisplay.cpp
//  spatializer
//
//  Created by Jason Hoopes on 4/9/24.
//

#include "SpatializerDisplay.hpp"

bool SpatializerDisplay::setup() {
    
    state.boxPos = ofVec2f(100, 100);
    state.boxSize = ofVec2f(200,200);
    
    box.set(state.boxSize.x);
    
    double fps = 30;
        
        ofSetFrameRate(fps);
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
    
    return true;
}

void SpatializerDisplay::update() {
//    for (int i = 0; i < angles.size(); i++) {
//           angles.at(i).x = ofMap(ofSignedNoise(times.at(i).x), -1, 1, -180, 180);
//           angles.at(i).y = ofMap(ofSignedNoise(times.at(i).y), -1, 1, -180, 180);
//           angles.at(i).z = ofMap(ofSignedNoise(times.at(i).z), -1, 1, -180, 180);
//
//           times.at(i) += 0.01;
//       }
}

void SpatializerDisplay::updatePosition(float azimuth, float elevation) {
    
    this->state.azimuth = azimuth;
    this->state.elevation = elevation;
//    for (int i = 0; i < angles.size(); i++) {
//           angles.at(i).x = ofMap(ofSignedNoise(times.at(i).x), -1, 1, -180, 180);
//           angles.at(i).y = ofMap(ofSignedNoise(times.at(i).y), -1, 1, -180, 180);
//           angles.at(i).z = ofMap(ofSignedNoise(times.at(i).z), -1, 1, -180, 180);
//
//           times.at(i) += 0.01;
//       }
}

void SpatializerDisplay::draw() {
    
   
    
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
    
    float radius = 100;
    float grainX = cosf((float)state.azimuth/180*PI-PI/2)*radius;
    float grainY = sinf((float)state.azimuth/180*PI-PI/2)*radius;
    
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
    ofDrawBox(glm::vec3(grainX, 0, grainY), 20);
    
    ofPushMatrix();

    ofRotateXDeg(x);
    ofRotateYDeg(y);
    ofRotateZDeg(z);
    ofSetColor(48, 53, 149);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawBox(glm::vec3(0, 0, 0), 200);

    cam.end();
    
}

void SpatializerDisplay::cleanup() {
    
}
