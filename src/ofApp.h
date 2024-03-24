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

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;
    
        void audioOut(ofSoundBuffer &outBuffer);
        void audioIn(ofSoundBuffer & input);

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
    
    
        // graphics helpers
        ofPolyline drawWaveform(ofSoundBuffer * buffer, ofVec2f pos);
        ofPolyline drawBuffer(const float *buffer, int length, ofVec2f pos);
        void drawBuffer(ofSoundBuffer *buffer, ofVec2f pos);
    
    
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
    
        // Test File
        MonoFilePlayer mockInput;
        bool useMockInput;
    
    
        std::vector<int> warpPoints;
    
    
        // GUI
        ofxPanel gui;
        ofxIntSlider delayTime;
        ofxFloatSlider feedback;
        ofxFloatSlider wetDryKnob;
    
        // Global controls
        ofxPanel globalCtrl;
        ofxToggle loopToggle;
        ofxIntSlider loopLength;
        ofxToggle follow;
        ofxIntSlider loopStart;
        ofxFloatSlider playbackSpeed;
        ofxFloatSlider warpingAmount;
//        std::vector<ofParameter<float> grainSpeeds;
    
        // Randomness
        ofxPanel randness;
        ofxFloatSlider startRandomness;
        ofxFloatSlider lengthRandomness;
    
        // IO
        ofxPanel ioPanel;
        ofParameter<string> options;//, options2;
        ofParameter<int> intOptions;
        ofxToggle mockInputToggle;
    
        // Pitch;
        std::vector<ofxFloatSlider> grainSpeeds;

        unique_ptr<ofxDropdown> outputDropdown;
        void ofOutputChanged(string & output);
        std::string currentOutput;
        unique_ptr<ofxDropdown> inputDropdown;
        void ofInputChanged(string & input);
        std::string currentInput;

        std::vector<GranularState> lastGrainStates;
    
        ofPolyline bufferDisplay;
        ofVec2f bufferDisplayPos;
        int bufferHeight = 150;
        int bufferWidth = 1000;
        int bufferX = 0;
        int bufferY = 1000;
        float * bufferMirror;
    
        std::vector<ofPolyline> pointers;
        ofPolyline drawWritePointer(float pos);
        ofPolyline drawGrainPointer(int idx, float windowAmount);
        ofBoxPrimitive drawGrainBB(int idx);
        ofPolyline pointerDisplay;
        std::vector<ofPolyline> pointerViews;
        std::vector<ofBoxPrimitive> grainBBs;
        ofPolyline writePointer;
        JWindow grainWindow;
    
    
        // Video
        ofVideoPlayer videoPlayer;
        ofTexture videoTexture;
    
    
        ofParameterGroup mainGroup;
        GrainSliders gSliders;
    
    
    
    

    
        bool stopAudio;
            

		
};
