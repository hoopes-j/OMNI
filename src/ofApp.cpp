#define RYML_SINGLE_HDR_DEFINE_NOW
#include "yaml_all.hpp"

#include "ofApp.h"
#include <math.h>
#include "Granular.hpp"
#include "MonoFilePlayer.h"
#include <mutex>
#include "UnitTests/UnitTests.hpp"



#define RUN_UNIT_TESTS



int ofApp::msToSamps(float ms) {
    return (ms/1000)*this->soundStream.getSampleRate();
}

float ofApp::sampsToMs(int samps) {
    return ((float) samps/this->soundStream.getSampleRate()) * 1000;
}

void ofApp::loadConfig() {
    // Open the text file
    std::ifstream file("/Users/jhoopes/code/libraries/of_v0.12.0_osx_release/apps/myApps/granular/src/config/config.yaml");
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
    }

    // Read the file contents into a string
    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));

    // Close the file
    file.close();

    ryml::Tree tree = ryml::parse(ryml::to_csubstr(content));
    ryml::ConstNodeRef root = tree.rootref();  // a const node reference
        
    omniCfg.parseYaml(tree);

}

//--------------------------------------------------------------
void ofApp::setup(){
    this->audioReady = false;
    
    
    this->stopAudio = false;
    
#ifdef RUN_UNIT_TESTS
    UnitTests tests;
    tests.runAll();
#endif
    
    
    
    
    loadConfig();
    
    
    
    
    // Load wave file
    if (!this->mockInput.setup("/Users/jhoopes/code/libraries/of_v0.12.0_osx_release/apps/myApps/granular/src/sample[voice].wav", true, true)) {
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
    granConfig.numGrains = omniCfg.granular.numVoices;
    
    if (!gran.setup(granConfig)) {
        std::cerr << "Unable to setup granular instance" << std::endl;
        //        return false;
    }
    
    if (!processor.setup(settings.bufferSize)) {
        std::cout << "Unable to setup processor instance" << std::endl;
        //        return false;
    }
    
    // ================== Audio
    if (!audioEngine.setup(
                           settings.sampleRate,
                           settings.bufferSize,
                           settings.numInputChannels,
                           settings.numOutputChannels,
                           granConfig,
                           omniCfg.spatial.type,
                           omniCfg.spatial.numSpatializers
    )) {
        std::cout << "FATAL===========" << std::endl;
        std::cout << "Audio Engine Setup Failed!" << std::endl;
        std::cout << "================" << std::endl;
    }
    
    
    
    
    
    
    
    // Control Panels ============================================
    int nCols = 4;
    int colWidth = 200;
    int pX = 10;
    std::vector<int> cols(nCols);
    for (int i=0;i<nCols;i++) {
        cols[i] = i*(colWidth+pX);
    }
    
    // Panels
    globalCtrl.setup("global", "settings.xml", cols[0], 0);
    randness.setup("Randomness", "settings.xml", cols[1], 0);
    gui.setup("delay", "settings.xml", cols[2], 0);
    ioPanel.setup("I/O", "settings.xml", cols[3], 0);
    spatialPanel.setup("Spatial Panel", "spatialPanel.xml", cols[3], 150);
    // Groups
    gSliders.setup(omniCfg.granular.numVoices);


    
    
    
    gui.add(delayModeOn.set("delaymode[follow]",false));
    gui.add(delayTime.setup("delay time", this->sampsToMs(granConfig.delayTime), 0, this->sampsToMs(granConfig.delayBufferSize)));
    gui.add(feedback.setup("feedback", 0.1, 0.0, 0.9));

    
    globalCtrl.add(loopToggle.setup("loop"));
    globalCtrl.add(loopLength.setup("loop length", 1000, 0, this->sampsToMs(granConfig.delayBufferSize)));
    globalCtrl.add(follow.setup("follow", false));
    globalCtrl.add(loopStart.setup("loop start", 0, 0, this->sampsToMs(granConfig.delayBufferSize)));
    globalCtrl.add(playbackSpeed.setup("speed", 1.0, -2.0, 2.0));
    globalCtrl.add(gSliders.params);
    globalCtrl.add(gSliders.warpGroup);
    globalCtrl.add(gSliders.windowDropdown.get());
    

   
    randness.add(startRandomness.setup("start", 0.0, 0.0, 1.0));
    randness.add(lengthRandomness.setup("length", 0.0, 0.0, 1.0));
    
    std::vector<ofSoundDevice> devices = soundStream.getDeviceList();

    //IO Panel
    ioPanel.add(wetDryKnob.setup("wet/dry", 0.5, 0.0, 1.0));
    ioPanel.add(mute.set("mute", true));
    ioPanel.add(globalAmplitude.set("global amplitude", 0.0, 0.0, 5.0));
    ioPanel.add(mockInputToggle.set("mock input", true));
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
    
    this->pointers.resize(omniCfg.granular.numVoices);
    this->grainBBs.resize(omniCfg.granular.numVoices);
    
    
    if (!_pitchManager.setup()) {
        std::cerr << "[ofApp::setup] Unable to setup Pitch Manager" << std::endl;
        exit();
    }
    
    // Spatial Gui
    spatialSliders.setup(
                         omniCfg.spatial.numSpatializers,
                         omniCfg.spatial.type);
    spatialPanel.add(spatialSliders.params);
    
    // ==========================================================================================
    
    
    // GRAPHICS ============================================
    if (!waveformDisplay.setup(500,50)) {
        std::cerr << "[ofApp::setup] Unable to setup Waveform Display" << std::endl;
        exit();
    }
    if (!spatializerDisplay.setup()) {
        std::cerr << "[ofApp::setup] Unable to setup Spatial Display" << std::endl;
        exit();
    }
    // ==========================================================================================
    
    
  // OSC ===========
    if (!oscClient.setup(5000)) {
        std::cerr << "Unable to setup Osc Client" << std::endl;
        exit();
    }
    
    audioReady = true;
}

void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    
//    std::lock_guard<std::mutex> guard(granConfigMutex);
    gran.updateConfig(granConfig);
    
    audioEngine.updateConfig(granConfig);
    audioEngine.process();
    
    for (uint frame = 0; frame < audioEngine.framesPerBlock(); frame++) {
        float dry = audioEngine.getInput(frame, 0);
        for (uint channel = 0; channel < audioEngine.numOutputChannels(); channel++) {
            float wet = audioEngine.getOutput(frame, channel);
            float out = wet*wetDryKnob+dry*(1-wetDryKnob);
            if (!mute) {
                outBuffer.getSample(frame,channel) = out*globalAmplitude;
            }
        }        
    }
    

    unique_lock<mutex> lock(audioMutex);
//    lastGrainStates = gran.getStates();
    lastGrainStates = audioEngine.getGranularStates();
    lastBuffer = outBuffer;
}
int globalFrame = 0;
void ofApp::audioIn(ofSoundBuffer & input){
    
   
    globalFrame++;
    for (int frame = 0; frame < input.getNumFrames(); frame++) {

        // Assume Mono Input
        float in = 0;
        if (this->useMockInput == 1) {
            in = this->mockInput.process();
        } else {
            in = input[frame];
        }
        audioEngine.setInput(in, frame, 0);
        inputBuffer[frame] = input[frame];
    }
    unique_lock<mutex> lock(audioMutex);
    lastInputBuffer = input;
}


void ofApp::changeAudioDevice() {
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

//--------------------------------------------------------------
void ofApp::update(){
    
    if (stopAudio) {
        changeAudioDevice();
    }
    
    
    while (oscClient.hasWaitingMessages()) {
        ofxOscMessage msg;
        oscClient.getNextMessage(&msg);
        
        std::cout << msg.getAddress() << std::endl;
        
        std::string addr = msg.getAddress();
        std::string prefix = addr.substr(1, 8);
        std::string suffix = addr.substr(10,addr.length()-9);
        
        if (prefix == "granular") {

            
            if (suffix=="global_amplitude") {
                float val = msg.getArgAsInt(0)/(float)127;
                ofParameter<float> * ref = &globalAmplitude;
                ref[0] = val*globalAmplitude.getMax();
            }else if (suffix=="grain_pitch") {
                float grainIdx = msg.getArgAsInt(1);
                float value = msg.getArgAsFloat(0);
                std::cout << grainIdx << "  --- " << value << std::endl;
            }else if (suffix=="global_pitch") {
                float value = msg.getArgAsFloat(0)/(float)127;
                ofParameter<float> * ref = &gSliders.pitchMultiplier;
                ref[0] = value*(ref->getMax()-ref->getMin())+ref->getMin();
            }



        }
        
    }
    

    // "lastBuffer" is shared between update() and audioOut(), which are called
    // on two different threads. This lock makes sure we don't use lastBuffer
    // from both threads simultaneously (see the corresponding lock in audioOut())
    unique_lock<mutex> lock(audioMutex);
    
    
    useMockInput = mockInputToggle;
    

    if (delayModeOn) {
        gSliders.warpAmount=0;
        follow = true;
    } else {
        feedback = 0.f;
        follow = false;
    }
    
    granConfig.delayTime = msToSamps(delayTime);
    granConfig.feedback = feedback;
    granConfig.loopOn = loopToggle;
    granConfig.loopLength = msToSamps(loopLength);
    granConfig.loopStart = msToSamps(loopStart);
    granConfig.playbackSpeed = playbackSpeed;
    granConfig.follow = follow;
    
    // === Warping
    granConfig.warpAmount = gSliders.warpAmount;
    granConfig.numWarpPoints = gSliders.numWarpPoints;
    
    granConfig.startRandomness = this->startRandomness;
    granConfig.lengthRandomness = this->lengthRandomness;
    
    // === Window
    currentWindow = gran.getWindow();
    
    

    // === Pitch
    
    if (audioReady) {
        std::vector<float> pitches(omniCfg.granular.numVoices);
        if (gSliders.manualPitch) {
            for (int i = 0; i<omniCfg.granular.numVoices; i++) {
                pitches[i]=gSliders.pitches[i]*gSliders.pitchMultiplier;
            }
            gran.distributePitch(pitches);
        } else {
            pitches = _pitchManager.generate(granConfig.numGrains, gSliders.pitchSpread);
            gran.distributePitch(pitches);
            gSliders.setPitches(pitches.data());
        }
    }

    // ==========
    
    if (audioReady) {
        if (lastGrainStates.size()>0) {
            float ptr = lastGrainStates[0].loopPointer;
            int grainLength = lastGrainStates[0].loopLength;
            int grainStart = lastGrainStates[0].loopStart;
            waveformDisplay.update(audioEngine.getBuffer(), granConfig.delayBufferSize, ptr, grainLength, grainStart);
        }
    }
    
    
    // Pass Spatial Params ==========
    for (int i = 0; i<omniCfg.spatial.numSpatializers; i++) {
        if (omniCfg.spatial.type == BINAURAL_MODE) {
            int eVal = spatialSliders.elevations[i];
            int aVal = spatialSliders.azimuths[i];
            int elevation = eVal-(eVal%15);
            int azimuth = aVal-(aVal%15);
            spatialSliders.elevations[i] = elevation;
            spatialSliders.azimuths[i] = azimuth;
//            spatializerDisplay.updatePosition(azimuth, elevation);
            audioEngine.setBinauralPosition(i,azimuth, elevation);
        }

    }
    // ==========



    
    warpPoints = gran.getTransients();
    
//    gran.updateConfig(granConfig);
    
        
    waveform.clear();
    int offset = 3;
    
    bufferDisplay = this->drawBuffer(gran.getBuffer(), granConfig.delayBufferSize, this->bufferDisplayPos);
    for (int i = 0; i < omniCfg.granular.numVoices; i++) {
        pointers[i] = drawGrainPointer(i, 1.0);
        grainBBs[i] = drawGrainBB(i);
    }
    
    if (lastGrainStates.size() > 0) {
        writePointer = drawWritePointer(lastGrainStates[0].writePointer);
    }

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
    
    ofPolyline pointerLine;
    pointerLine.clear();
    
    if (lastGrainStates.size() > 0) {
        int rPos = floor(lastGrainStates[idx].loopPointer);
        int wPos = lastGrainStates[0].writePointer;
        float windowVal = lastGrainStates[idx].windowEnvelope;
        
        float lineHeight = bufferHeight*3-bufferHeight*3*((1-windowVal)*windowAmount);

        float yT = bufferDisplayPos.y-lineHeight/2;
        float yB = bufferDisplayPos.y+lineHeight/2;
        

    //    float xT = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
        int pos = (granConfig.delayBufferSize-(wPos-rPos))%granConfig.delayBufferSize;
        float x = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
        

        pointerLine.addVertex(x,yT);
        pointerLine.addVertex(x, yB);
    }
    

    
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
    
    ofBoxPrimitive box;
    
    if (lastGrainStates.size() > 0) {
        float pos = lastGrainStates[idx].loopPointer;
        float windowVal = lastGrainStates[idx].windowEnvelope;
        
        float lineHeight = bufferHeight*3;
        float boxWidth = 50;
        
        float yT = bufferDisplayPos.y-lineHeight/2;
        float yB = bufferDisplayPos.y+lineHeight/2;
        
        
        float xT = pos/(float)granConfig.delayBufferSize*bufferWidth+bufferDisplayPos.x;
        float xB = xT;
    }
    
    return box;
}

ofPolyline ofApp::drawBuffer(const float *buffer, int length, ofVec2f origin) {
    
    ofPolyline bufferGraphic;
    bufferGraphic.clear();
    
    if (lastGrainStates.size() > 0) {
        int writeIdx = lastGrainStates[0].writePointer;
        
        int div = 10;
        for(size_t i = 0; i < length/div; i++) {
            int idx = (i*div+writeIdx)%granConfig.delayBufferSize;
            float sample = buffer[idx];
            float y = origin.y-sample*bufferHeight;
            float x = i*((float)div)/(float)length*bufferWidth+origin.x;
            bufferGraphic.addVertex(x, y);
        }
    }
    
        
    return bufferGraphic;
}



//____    ____    ______  __      __
///\  _`\ /\  _`\ /\  _  \/\ \  __/\ \
//\ \ \/\ \ \ \L\ \ \ \L\ \ \ \/\ \ \ \
//\ \ \ \ \ \ ,  /\ \  __ \ \ \ \ \ \ \
// \ \ \_\ \ \ \\ \\ \ \/\ \ \ \_/ \_\ \
//  \ \____/\ \_\ \_\ \_\ \_\ `\___x___/
//   \/___/  \/_/\/ /\/_/\/_/'\/__//__/
                                      


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofFloatColor(0.9, 0.9, 0.8		));

    // ====== Controls
    gui.draw();
    globalCtrl.draw();
    randness.draw();
    ioPanel.draw();
    spatialPanel.draw();
    // ======

    
    
    // ====== Visualizers
//    cam.begin();
//    waveformDisplay.draw();
//    cam.end();
    
//    spatializerDisplay.draw();
    // ======
    

// ========== WAVEFORM DISPLAY
//
//    ofSetLineWidth(1);
//    bufferDisplay.draw();
//    ofSetLineWidth(3);
//
//    ofSetColor(100,100,255, 150);
//    for (int i = 0; i < omniCfg.granular.numVoices; i++) {
//        pointers[i].draw();
//    }
//
//    ofSetColor(255,0,0,150);
//    for (int i = 0; i < warpPoints.size(); i++) {
//        int writePtr = lastGrainStates[0].writePointer;
//        float idx = (float)warpPoints[i]+(granConfig.delayBufferSize-writePtr)%granConfig.delayBufferSize;
//        float x = (idx/(float)granConfig.delayBufferSize)*bufferWidth+bufferDisplayPos.x;
//        int boxHeight = 100;
//        ofDrawRectangle(x, bufferDisplayPos.y-boxHeight, 5, boxHeight);
//    }

// ========= Window
//
//    ofDrawBitmapString("Window Type: Tukey (tapered cosine)", 500, 700);
//    ofDrawBitmapString?("Interpolation Method: Cubic Linear", 500, 725);

//    drawGrainWindow(currentWindow._buf.data(), currentWindow._resolution, currentWindow.name, 200, 500, 100);
}


void ofApp::drawGrainWindow(float * data, int length, std::string & type, int x, int y, int height) {
    
    ofNoFill();
    ofDrawRectangle(x, y, length, height);
    int textHeight=10;
    ofDrawBitmapString(type, x, y+textHeight);
    
    ofPolyline window;
    for (int i = 0; i < length; i++) {
        window.addVertex(ofPoint(x+i, y+height+data[i]*height));
    }
    window.draw();
    ofFill();
}

//--------------------------------------------------------------
void ofApp::exit(){
    
    audioEngine.cleanup();
    spatializerDisplay.cleanup();

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


