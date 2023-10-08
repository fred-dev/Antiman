#pragma once

#include "ofMain.h"
#include "ofxOsc.h"


#define SERVO_X_PIN 23
#define SERVO_Y_PIN 24

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


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    //settings and debug init
    int debugLevel;
    bool firstRun;
    ofJson antimanSettings;
    
    void setupFromJSON();
    
    
    //OSC
    string antiManIPAddress;
    int antimanOSCReceivePort;
    int rpiOSCReceivePort;
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    
    void setupOSC();
    
    

    
    
    // video stuff
    vector<ofFile> videoFiles;
    ofVideoPlayer videoPlayer;
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
    
    void setupText();
    void playText(int textIndex);
    
    string rootPath;


};
