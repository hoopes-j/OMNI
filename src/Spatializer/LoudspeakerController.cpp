//
//  LoudspeakerController.cpp
//  granular
//
//  Created by Jason Hoopes on 4/25/24.
//

#include "LoudspeakerController.hpp"

LoudspeakerController::~LoudspeakerController() {
    this->cleanup();
}

bool LoudspeakerController::setup(int numProcessors, int numOutputs) {
    _numProcessors = numProcessors;
    _numOutputs = numOutputs;
    for (int i = 0; i < _numProcessors; i++) {
        _loudspeakerProcessors.push_back(new LoudspeakerSpatializer());
        _loudspeakerProcessors[i]->setup(numOutputs);
    }
    
    return true;
}


void LoudspeakerController::processAndStore(int processorNum, float input) {
    _loudspeakerProcessors[processorNum]->processAndStore(input);
}
void LoudspeakerController::clear(int processorNum) {
    _loudspeakerProcessors[processorNum]->clearOutput();
}
void LoudspeakerController::clearAll() {
    for (LoudspeakerSpatializer * sp : _loudspeakerProcessors) {
        sp->clearOutput();
    }
}

void LoudspeakerController::readOutput(float *output) {
    for (int i = 0; i < _numOutputs; i++) {
        float sum = 0;
        for (int j = 0; j < _numProcessors; j++) {
            sum += _loudspeakerProcessors[j]->getSampleAtChannel(i);
        }
        output[i] = sum;
    }
}


void LoudspeakerController::updatePosition(int index, int angle) {
    _loudspeakerProcessors[index]->updateAngle(angle);
}

void LoudspeakerController::cleanup() {
    for (LoudspeakerSpatializer * ptr : _loudspeakerProcessors) {
        delete ptr;
    }
}
