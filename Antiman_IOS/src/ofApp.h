#pragma once

#include "ofxiOS.h"

#include "ofxOsc.h"

struct  rpiVideoStatus{
    bool isConnected;
    bool isDebugging;
    bool isPlaying;
    bool isVideo;
    int currentVideo;
    int totalVideos;
    int currentText;
    int totalTextFiles;
    int maskXPos;
    int maskYPos;
};



class ofApp : public ofxiOSApp {
	
    public:
        void setup();
        void update();
        void draw();
        void exit();
	
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);

        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);
    
    //settings and debug init
    int debugLevel;
    bool firstRun;
    ofJson antimanSettings;
    
    void setupFromJSON();
    
    
    //OSC
    string antiManIPAddress;
    int antimanOSCReceivePort;
    int iPhoneOSCReceivePort;
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    
    void setupOSC();
    
    vector<ofFile> videoFiles;
    ofxiOSVideoPlayer videoPlayer;
    bool doLoadMovie;
    
    void loadMovie(int videoIndex);
    void setupVideos();
    
    
    //status and ack
    rpiVideoStatus outgoingRPIStatus;
    ofxOscMessage statusMessage;
    ofxOscMessage ackMessage;
    bool returnAck;
    
    void setupAck();
    void sendStatus();
    void setupStatus();
    
    // text
    std::string textStrings[10];
    int drawScrollX;
    ofTrueTypeFont displayFontSmall, displayFontLarge;
    std:: string displayFontSmallName, displayFontLargeName;
    int displayFontSmallSize, displayFontLargeSize;
    ofColor antimanColour1, antimanColour2, antimanColour3, antimanColour4;
    


};


