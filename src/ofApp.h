#pragma once

#include "ofMain.h"
#include "Granular.hpp"
#include "ofxGui.h"
#include "MonoFilePlayer.h"
#include "Utils.h"
#include "ofxDropdown.h"
#include "Processor.hpp"
#include "gui/GrainSliders.hpp"
#include "EnvelopeFollower.hpp"
#include "AudioEngine.hpp"
#include "ofxOsc.h"
#include "WaveformDisplay.hpp"
#include "OmniConfig.h"
#include "yaml_all.hpp"
#include "SpatialSliders.hpp"
#include "SpatializerDisplay.hpp"
#include "BufferDisplay.hpp"

#define OSC_PORT 5000

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;
    
        void audioOut(ofSoundBuffer &outBuffer);
        void audioIn(ofSoundBuffer & input);
        bool audioReady;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
    
    
        // audio helper
        int msToSamps(float ms);
        float sampsToMs(int samps);
    
        // ======= Config =======
        void loadConfig();
        Omni::OfxConfig omniCfg;
    
    
        // ====== GUI =========
        ofPolyline drawWaveform(ofSoundBuffer * buffer, ofVec2f pos);
        ofPolyline drawBuffer(const float *buffer, int length, ofVec2f pos);
        void drawBuffer(ofSoundBuffer *buffer, ofVec2f pos);
    
        void drawGrainWindow(float * data, int length, std::string & type, int x, int y, int height);
        JWindow currentWindow;
    
    
        void changeAudioDevice();
    
        // ====== END GUI
    
    
        double wavePhase;
        double pulsePhase;

        std::mutex audioMutex;
        ofSoundStream soundStream;
        ofSoundBuffer lastBuffer;
        ofSoundBuffer lastInputBuffer;
        ofPolyline waveform;
        ofPolyline dryWaveform;
        float rms;
    
        float * inputBuffer;
    
        // ** Granular
    
        Granular gran;
        GranularConfig granConfig;
        Processor processor;
        PitchManager _pitchManager;
    
        // Test File
        MonoFilePlayer mockInput;
        bool useMockInput;
    
    
        std::vector<int> warpPoints;
    
    
        //  =================GUI
        ofxPanel spatialPanel;
        SpatialSliders spatialSliders;   // Spatial Controls
    
    
        ofxPanel gui;
        ofParameterGroup delayGroup;
    
        ofParameter<bool> delayModeOn;
        ofParameter<int> delayTime;
        ofParameter<float> feedback;
        ofxFloatSlider wetDryKnob;
    
        // Grain controls
        ofParameter<bool> freeze;
        ofxPanel grainPanel;
        ofParameter<float> bufferIdx;
        ofParameter<int> loopLength;
        ofxToggle follow;
        ofParameter<int> loopStart;
    
    
        // Randomness
        ofxPanel randness;
        ofParameter<float> startRandomness;
        ofParameter<float> lengthRandomness;
    
        // IO
        ofxPanel ioPanel;

        ofParameter<string> options;//, options2;
        ofParameter<int> intOptions;
        ofParameter<bool> mockInputToggle;
        ofParameter<bool> mute;
        ofParameter<float> globalAmplitude;
    
        // Pitch;
        std::vector<ofxFloatSlider> grainSpeeds;

        unique_ptr<ofxDropdown> outputDropdown;
        void ofOutputChanged(string & output);
        std::string currentOutput;
        unique_ptr<ofxDropdown> inputDropdown;
        void ofInputChanged(string & input);
        std::string currentInput;

        std::vector<GranularState> lastGrainStates;
    
        ofVec2f bufferDisplayPos;
        int bufferHeight = 150;
        int bufferWidth = 1000;
        int bufferX = 0;
        int bufferY = 800;
        float * bufferMirror;
    
        JWindow grainWindow;
    
    
    
    
        ofParameterGroup mainGroup;
        GrainSliders gSliders;
    
    
        AudioEngine audioEngine;
    
    
    
    

        bool stopAudio;
    
        ofxOscReceiver oscClient;
        template <class T>
        void setParamRef(ofParameter<T> * ref, float value) {
            float max = ref->getMax();
            float min = ref->getMin();
            float r = max-min;
            float v = value*(r/127)+min;
            std::cout << value << std::endl;
            ref[0] = (T) v;
        }
        WaveformDisplay waveformDisplay;
        BinauralDisplay binauralDisplay;
        BufferDisplay buffDisplay;
        ofEasyCam cam;
    
    
        

};
