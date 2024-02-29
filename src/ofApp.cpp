#include "ofApp.h"
#include <math.h>
#include "Granular.hpp"
#include "MonoFilePlayer.h"
#include <mutex>


#define MOCK_INPUT
#define NUM_GRAINS 16




int ofApp::msToSamps(float ms) {
    return (ms/1000)*this->soundStream.getSampleRate();
}

float ofApp::sampsToMs(int samps) {
    return ((float) samps/this->soundStream.getSampleRate()) * 1000;
}

//--------------------------------------------------------------
void ofApp::setup(){
    

    this->stopAudio = false;
    
    
    
    // Load wave file
    if (!this->mockInput.setup("/Users/jhoopes/code/libraries/of_v0.12.0_osx_release/apps/myApps/granular/src/sample[piano].wav", true, true)) {
        std::cerr << "sample not found" << std::endl;
    }
    this->mockInput.trigger();
    
    
    

    ofSetVerticalSync(true);
    
    
    soundStream.printDeviceList();

    
    // start the sound stream with a sample rate of 44100 Hz, and a buffer
    // size of 512 samples per audioOut() call
    ofSoundStreamSettings settings;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 1;
    settings.sampleRate = 44100;
    settings.bufferSize = 512;
    settings.numBuffers = 4;
    settings.setOutListener(this);
    settings.setInListener(this);
    soundStream.setup(settings);
    
    // Granular
    
    inputBuffer = (float *) malloc(sizeof(float)*settings.bufferSize);
    

    granConfig.sampleRate = soundStream.getSampleRate();
    granConfig.delayTime = msToSamps(1000);
    granConfig.delayBufferSize = msToSamps(10000);
    granConfig.numGrains = NUM_GRAINS;
        
    if (!gran.setup(granConfig)) {
        std::cerr << "Unable to setup granular instance" << std::endl;
//        return false;
    }
    
    if (!processor.setup(settings.bufferSize)) {
        std::cout << "Unable to setup processor instance" << std::endl;
//        return false;
    }
    
    // GUI
    gui.setup("delay");
    gui.add(delayTime.setup("delay time", this->sampsToMs(granConfig.delayTime), 0, this->sampsToMs(granConfig.delayBufferSize)));
    gui.add(feedback.setup("feedback", 0.1, 0.0, 0.8));
    
    globalCtrl.setup("global", "settings.xml", 0, 200);
    gui.add(wetDryKnob.setup("wet/dry", 0.5, 0.0, 1.0));
    globalCtrl.add(loopToggle.setup("loop"));
    globalCtrl.add(loopLength.setup("loop length", 1000, 0, this->sampsToMs(granConfig.delayBufferSize)));
    globalCtrl.add(follow.setup("follow"));
    globalCtrl.add(loopStart.setup("loop start", 0, 0, this->sampsToMs(granConfig.delayBufferSize)));
    globalCtrl.add(playbackSpeed.setup("speed", 1.0, -2.0, 2.0));

    
    randness.setup("Randomness", "settings.xml", 0, 500);
    randness.add(startRandomness.setup("start", 0.0, 0.0, 1.0));
    randness.add(lengthRandomness.setup("length", 0.0, 0.0, 1.0));
    
    
    
    std::vector<ofSoundDevice> devices = soundStream.getDeviceList();

    //IO Panel
    ioPanel.setup("I/O");
    ioPanel.add(mockInputToggle.setup("mock input"));
    outputDropdown =  make_unique<ofxDropdown>("Output Device");
    for (int i = 0; i < devices.size(); i++) {
        ofSoundDevice device = devices[i];
        if (device.outputChannels > 0) {
            outputDropdown->add(device.name);
        }
    }
    ioPanel.add(outputDropdown.get());
    outputDropdown->addListener(this, &ofApp::ofOutputChanged);
    //Input selector
    
    inputDropdown =  make_unique<ofxDropdown>("Input Device");
    for (int i = 0; i < devices.size(); i++) {
        ofSoundDevice device = devices[i];
        if (device.inputChannels > 0) {
            inputDropdown->add(device.name);
        }
    }
    ioPanel.add(inputDropdown.get());
    inputDropdown->addListener(this, &ofApp::ofInputChanged);
    

    this->bufferDisplayPos = ofVec2f(bufferX, bufferY);
    
    this->pointers.resize(NUM_GRAINS);
    this->grainBBs.resize(NUM_GRAINS);
    
    
}

void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    
//    std::lock_guard<std::mutex> guard(granConfigMutex);
    gran.updateConfig(granConfig);
    
    std::vector<float> tmpBuf;
    std::vector<float> tmpBuf2;
    tmpBuf.resize(outBuffer.getNumFrames());
    tmpBuf2.resize(outBuffer.getNumFrames());


    for(size_t i = 0; i < outBuffer.getNumFrames(); i++) {
        
        
        float in = 0;
#ifdef MOCK_INPUT
        in = this->mockInput.process();
#else
//        in = inputBuffer[i];
        in = lastInputBuffer[i];
#endif

        gran.updateBuffer(in);
        float wet = 0;
        if (granConfig.loopOn) {
            wet = gran.processLoop();
        }
        float dry = in;
        
        
        // write the computed sample to the left and right channels
        tmpBuf[i] = wet;
        tmpBuf2[i] = wet;
        outBuffer.getSample(i,0) = wet;
        outBuffer.getSample(i,1) = dry;
//        outBuffer.getSample(i,0) = wet;
//        outBuffer.getSample(i,1) = wet;
    }
    
    std::vector<float> processedBuf = processor.processKiss(tmpBuf2.data());
    
    for (int i = 0; i < 512; i++) {
//        outBuffer.getSample(i,0) = processedBuf[i];
//        outBuffer.getSample(i,1) = processedBuf[i];
//        outBuffer.getSample(i,0) = tmpBuf[i];
//        outBuffer.getSample(i,1) = tmpBuf[i];
    }
    
//    outBuffer.copyFrom(processedBuf.data(), processedBuf.size()*sizeof(float),1,44100);
    
    unique_lock<mutex> lock(audioMutex);
    lastGrainStates = gran.getStates();
    lastBuffer = outBuffer;
}

void ofApp::audioIn(ofSoundBuffer & input){
    
    for (int frame = 0; frame < input.getNumFrames(); frame++) {
        inputBuffer[frame] = input[frame];
//        gran.writeFrame(input[frame]);
    }
    unique_lock<mutex> lock(audioMutex);
    

    lastInputBuffer = input;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    if (stopAudio) {
        soundStream.stop();
        
        ofSoundStream streamNew;
        soundStream = streamNew;
        auto deviceList = soundStream.getDeviceList();
        for (int i = 0; i < deviceList.size(); i++) {
            if (deviceList[i].name == this->currentOutput) {
                const ofSoundDevice outDevice = deviceList[i];
                ofSoundStreamSettings settings;
                settings.numOutputChannels = 2;
                settings.numInputChannels = 1;
                settings.sampleRate = 44100;
                settings.bufferSize = 512;
                settings.numBuffers = 4;
                settings.setOutListener(this);
                settings.setInListener(this);
                settings.setOutDevice(outDevice);
                soundStream.setup(settings);
            }
            if (deviceList[i].name == this->currentInput) {
                const ofSoundDevice inDevice = deviceList[i];
                ofSoundStreamSettings settings;
                settings.numOutputChannels = 2;
                settings.numInputChannels = 1;
                settings.sampleRate = 44100;
                settings.bufferSize = 512;
                settings.numBuffers = 4;
                settings.setOutListener(this);
                settings.setInListener(this);
//                settings.setOutDevice(outDevice);
                settings.setInDevice(inDevice);
                soundStream.setup(settings);
            }
        }
        
        soundStream.start();
        stopAudio = false;
    }
    

    // "lastBuffer" is shared between update() and audioOut(), which are called
    // on two different threads. This lock makes sure we don't use lastBuffer
    // from both threads simultaneously (see the corresponding lock in audioOut())
    unique_lock<mutex> lock(audioMutex);
    

    
    
    granConfig.delayTime = msToSamps(delayTime);
    granConfig.feedback = feedback;
    
    granConfig.loopOn = loopToggle;
    granConfig.loopLength = msToSamps(loopLength);
    granConfig.loopStart = msToSamps(loopStart);
    granConfig.playbackSpeed = playbackSpeed;
    granConfig.follow = follow;
    
    granConfig.startRandomness = this->startRandomness;
    granConfig.lengthRandomness = this->lengthRandomness;
    
    
    gran.updateConfig(granConfig);
        
    waveform.clear();
    int offset = 3;
    
    bufferDisplay = this->drawBuffer(gran.getBuffer(), granConfig.delayBufferSize, this->bufferDisplayPos);
    for (int i = 0; i < NUM_GRAINS; i++) {
        pointers[i] = drawGrainPointer(i, 1.0);
        grainBBs[i] = drawGrainBB(i);
    }
    writePointer = drawWritePointer(lastGrainStates[0].writePointer);
}

ofPolyline ofApp::drawWaveform(ofSoundBuffer *buffer, ofVec2f origin) {
    ofPolyline newWaveform;
    newWaveform.clear();
    
    float thetaDiff = (2 * M_PI) / buffer->getNumFrames();
    float theta = 0;
        
    float firstX = 0;
    float firstY = 0;
    
    for(size_t i = 0; i < buffer->getNumFrames(); i++) {
        float sample = buffer->getSample(i, 0);
        float radius = ofMap(sample, 0, 1, 200, 300);
        float x = sin(theta) * radius + origin.x;
        float y = cos(theta) * radius + origin.y;
        if (i == 0) {
            firstX = x;
            firstY = y;
        }
        theta += thetaDiff;
        newWaveform.addVertex(x, y);
    }
    newWaveform.addVertex(firstX, firstY);
    return newWaveform;
}

ofPolyline ofApp::drawGrainPointer(int idx, float windowAmount) {
    
    int rPos = floor(lastGrainStates[idx].loopPointer);
    int wPos = lastGrainStates[0].writePointer;
    float windowVal = lastGrainStates[idx].windowEnvelope;
    
    float lineHeight = bufferHeight*3-bufferHeight*3*((1-windowVal)*windowAmount);

    float yT = bufferDisplayPos.y-lineHeight/2;
    float yB = bufferDisplayPos.y+lineHeight/2;
    

//    float xT = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
    int pos = (granConfig.delayBufferSize-(wPos-rPos))%granConfig.delayBufferSize;
    float x = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
    
    ofPolyline pointerLine;
    pointerLine.clear();
    pointerLine.addVertex(x,yT);
    pointerLine.addVertex(x, yB);
    
    return pointerLine;
}

ofPolyline ofApp::drawWritePointer(float pos) {
    
    
    float lineHeight = bufferHeight*3;

    float yT = bufferDisplayPos.y-lineHeight/2;
    float yB = bufferDisplayPos.y+lineHeight/2;
    

    float xT = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
    float xB = xT;
    
    ofPolyline pointerLine;
    pointerLine.clear();
    pointerLine.addVertex(xT,yT);
    pointerLine.addVertex(xB, yB);
    
    return pointerLine;
}

ofBoxPrimitive ofApp::drawGrainBB(int idx) {
    float pos = lastGrainStates[idx].loopPointer;
    float windowVal = lastGrainStates[idx].windowEnvelope;
    
    float lineHeight = bufferHeight*3;
    float boxWidth = 50;

    float yT = bufferDisplayPos.y-lineHeight/2;
    float yB = bufferDisplayPos.y+lineHeight/2;
    

    float xT = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
    float xB = xT;
    
    ofBoxPrimitive box;

     
    return box;
}

ofPolyline ofApp::drawBuffer(const float *buffer, int length, ofVec2f origin) {
    
    int writeIdx = lastGrainStates[0].writePointer;
    
    ofPolyline bufferGraphic;
    bufferGraphic.clear();
    int div = 10;
    for(size_t i = 0; i < length/div; i++) {
        int idx = (i*div+writeIdx)%granConfig.delayBufferSize;
        float sample = buffer[idx];
        float y = origin.y-sample*bufferHeight;
        float x = i*((float)div)/(float)length*bufferWidth+origin.x;
        bufferGraphic.addVertex(x, y);
    }
    
        
    return bufferGraphic;
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    ofSetColor(ofColor::white);
    
    // GUI
    gui.draw();
    globalCtrl.draw();
    randness.draw();
    ioPanel.draw();
    
    ofSetLineWidth(1);
    bufferDisplay.draw();
    ofSetLineWidth(3);
    ofSetColor(ofColor(200,0,0, 128));
    for (int i = 0; i < NUM_GRAINS; i++) {
        pointers[i].draw();
//        grainBBs[i].draw();
    }


//    writePointer.draw();
    ofSetColor(ofColor::white);
    ofDrawBitmapString("Window Type: Tukey (tapered cosine)", 500, 700);
    ofDrawBitmapString("Interpolation Method: Cubic Linear", 500, 725);
    
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::ofOutputChanged(string & output) {
    this->stopAudio = true;
    this->currentOutput = output;
}

void ofApp::ofInputChanged(string & input) {
    this->stopAudio = true;
    this->currentInput = input;
}
