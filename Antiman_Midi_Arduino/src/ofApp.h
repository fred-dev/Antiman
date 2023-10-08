/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxGui.h"
#include "ofxSerial.h"
#include "ofxOsc.h"

#define NUM_PWM_OUT 12
#define NUM_DIGITAL_OUT 40


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

struct SerialMessage
{
    std::string message;
    std::string exception;
};

class ofApp : public ofBaseApp, public ofxMidiListener {
    
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    
    //settings
    ofJson antimanSettings;
    void readSettingsFile();
    
    //debug
    int debugLevel;
    
    //note and pin mapping
    int pwmPinStart, numPWMPins, pwmNoteStart, digiPinStart, numDigiPins, digiNoteStart;
    int noteMaskPosX[4], noteMaskPosY[4];
    int maskPosX[4], maskPosY[4];
    int maskMinPosX, maskMaxPosX, maskMinPosY, maskMaxPosY;
    int controllerNumberMaskX, controllerNumberMaskY;
    int pWMVelocityMapRangeLow, pWMVelocityMapRangeHigh;
    int noteMaskVideoTrigger[15], startNoteMaskVideoTrigger;
    int noteMaskTextTrigger[15], startNoteMaskTextTrigger;
    int noteMaskResetScreeen, noteMaskDebug;
    
    //midi input
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    void setupMidi();
    void exitMidi();
    
    int midiChannelArduino, midiChannelMask, midiChannelLogicControl, midiChannelText;
    string midiPortName;
    
    //serial connection
    ofxIO::PacketSerialDevice device;
    void onSerialBuffer(const ofxIO::SerialBufferEventArgs& args);
    void onSerialError(const ofxIO::SerialBufferErrorEventArgs& args);
    void setupSerial();
    void exitSerial();
    int serialDeviceID;

    
    //osc
    ofxOscSender rpiOscSender, iPhoneOscSender; // all-important ofxrpiOscSender object
    string rpiIPAddress; // IP address we're sending to
    int rpiOSCReceivePort; // port we're sending to
    string iPhoneIPAddress; // IP address we're sending to
    int iPhoneOSCReceivePort; // port we're sending to
    ofxOscReceiver oscReceive; // OSC receiver
    int antimanOSCReceivePort; // port where we listen for messages
    ofxOscMessage recycleMessage, ackMessage;
    void setupOSC();
   
    
    //graphics
    ofTrueTypeFont displayFontLarge, displayFontSmall, displayFontMini;
    string displayFontSmallName, displayFontLargeName, displayFontMiniName;
    
    ofColor antimanColour1, antimanColour2, antimanColour3, antimanColour4;
    
    int circleRadius, cirlceSpacing, circleRadiusSmall, cirlceSpacingSmall;
    ofPoint pwmViewLocation, digitalOutViewLocation, pwmViewLocationSmall, digitalOutViewLocationSmall;

    
    ofPath digitaloutputsViewFilled[NUM_DIGITAL_OUT];
    ofPath pwmOuputsViewFilled[NUM_PWM_OUT];
    
    ofPolyline digitaloutputsViewOutline[NUM_DIGITAL_OUT];
    ofPolyline pwmOuputsViewOutline[NUM_PWM_OUT];
    
    void setupGraphics();
    void setupInterface(int _circleRadius, int _cirlceSpacing, ofPoint _pwmViewLocation, ofPoint _digitalOutViewLocation, int columnsDigital, int columnsPWM);
    void exitGraphics();
    void drawGraphics();
    
    void sendDigitalPinSignal(int pin, int value, int displayValue);
    void sendPWNPinSignal(int pin, int value, int displayValue);
    
    int testPinDigital, testPinPWM;
    bool isTestingDigital, isTestingPWM;
    
    std::string pwmNameMap[NUM_PWM_OUT];
    std::string digitalNameMap[NUM_DIGITAL_OUT];
    bool drawName;
    std::string nameMapString;
    int displayNameTimer;
    
    //views
    bool miniVersion;
    

    //RPI status
    rpiVideoStatus incomingRPIStatus;
    std::string incominRPIStatusString;
    uint64_t rpiAckSendTimer, rpiAckReceiveTimer;
    bool rpiIsConnected;
    void setupRPIStatus();
    bool firstRun;
    
    rpiVideoStatus incomingIPhoneStatus;
    std::string incominIphoneStatusString;
    uint64_t iPhonerpiAckSendTimer, iPhonerpiAckReceiveTimer;
    bool iPhoneIsConnected;
    void setupIPhoneStatus();
   

    //Arduino status
    void setupArduinoStatus();
    void updateArduinoStatus();
    uint64_t ackArduinoSendTimer, ackArduinoReceiveTimer;
    std::string serialDeviceStatusString;
    bool serialIsConnected;
    
    //logos
    ofImage rpiLogo, arduinoLogo, iPhoneLogo;
};
