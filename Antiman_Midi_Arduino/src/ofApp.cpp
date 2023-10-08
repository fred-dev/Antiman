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
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    readSettingsFile();
    setupOSC();
    setupMidi();
    setupSerial();
    setupGraphics();
    setupInterface(circleRadiusSmall, cirlceSpacingSmall, pwmViewLocationSmall, digitalOutViewLocationSmall, 10, 4);
    setupRPIStatus();
    setupIPhoneStatus();
    firstRun = true;
    ofSetWindowShape(1920,120);
    ofSetWindowPosition(0, 0);
}


//--------------------------------------------------------------
void ofApp::update() {
    if(firstRun){
        
        rpiOscSender.sendMessage(ackMessage);
        rpiAckSendTimer = ofGetElapsedTimeMillis();
        ofLogNotice("Antiman::OSC to mask") << "Sent osc ack to RPI";
        
        iPhoneOscSender.sendMessage(ackMessage);
        iPhonerpiAckSendTimer = ofGetElapsedTimeMillis();
        ofLogNotice("Antiman::OSC to mask") << "Sent osc ack to Iphone";
        
        
        firstRun = false;
    }
    
    if (ofGetElapsedTimeMillis() - displayNameTimer > 2000) {
        drawName = false;
    }
    
    if (ofGetElapsedTimeMillis() - rpiAckReceiveTimer > 4500) {
        rpiOscSender.sendMessage(ackMessage);
        ofLogNotice("Antiman::OSC to mask") << "Sent osc ack to RPI";
        rpiAckSendTimer = ofGetElapsedTimeMillis();
    }
    
    if (ofGetElapsedTimeMillis() - rpiAckSendTimer > 15000) {
        incomingRPIStatus.isConnected = false;
        ofLogNotice("Antiman::OSC to mask") << "RPI not responding to Ack messages";
        incominRPIStatusString = "RPI could not be contacted - check your network connections";
        
    }
    
    if (ofGetElapsedTimeMillis() - iPhonerpiAckReceiveTimer > 4500) {
        iPhoneOscSender.sendMessage(ackMessage);
        ofLogNotice("Antiman::OSC to mask") << "Sent osc ack to Iphone";
        iPhonerpiAckSendTimer = ofGetElapsedTimeMillis();
    }
    
    if (ofGetElapsedTimeMillis() - iPhonerpiAckSendTimer > 15000) {
        incomingIPhoneStatus.isConnected = false;
        ofLogNotice("Antiman::OSC to mask") << "Iphone not responding to Ack messages";
        incominIphoneStatusString = "Iphone could not be contacted - check your network connections";
        
    }
    
    if (ofGetElapsedTimeMillis() - ackArduinoReceiveTimer > 5000) {
        std::string bufferString = "ack";
        ofx::IO::ByteBuffer buffer(bufferString);
        device.send(buffer);
        ackArduinoSendTimer = ofGetElapsedTimeMillis();
        ofLogNotice("Antiman::Arduino") << "Sent serial ack to Arduino";
    }
    if (ofGetElapsedTimeMillis() - ackArduinoSendTimer > 7000) {
        serialIsConnected = false;
        ofLogNotice("Antiman::Arduino") << "Arduino not responding to Ack messages";
        serialDeviceStatusString = "Arduino Not responding to ACK - check usb connections and port settings";
        
    }
    
    while( oscReceive.hasWaitingMessages() ){
        ofxOscMessage m;
        oscReceive.getNextMessage(m);
        
        if( m.getAddress() == "/ack" && m.getRemoteHost() == rpiIPAddress){
            rpiAckReceiveTimer = ofGetElapsedTimeMillis();
            incomingRPIStatus.isConnected = true;
            ofLogNotice("Antiman::OSC to mask") << "Received osc ack from RPI";
            
        }
        
        if( m.getAddress() == "/ack" && m.getRemoteHost() == iPhoneIPAddress){
            iPhonerpiAckReceiveTimer = ofGetElapsedTimeMillis();
            incomingIPhoneStatus.isConnected = true;
            ofLogNotice("Antiman::OSC to mask") << "Received osc ack from Iphone";
            
        }
        
        
        if (m.getAddress() == "/status" && m.getRemoteHost() == rpiIPAddress) {
            ofLogNotice("Antiman::OSC to mask") << "Received osc status from RPI";
            incomingRPIStatus.isDebugging = m.getArgAsBool(0);
            if (incomingRPIStatus.isDebugging) {
                incominRPIStatusString = "RPI is is debugging";
            }
            else{
                incomingRPIStatus.isPlaying = m.getArgAsBool(1);
                
                if (!incomingRPIStatus.isPlaying ) {
                    incominRPIStatusString = "RPI is currently connected and idle ";
                }
                
                if(incomingRPIStatus.isPlaying) {
                    incominRPIStatusString = "RPI is is playing";
                    incomingRPIStatus.isVideo = m.getArgAsBool(2);
                    
                    if (incomingRPIStatus.isVideo) {
                        incomingRPIStatus.currentVideo = m.getArgAsInt(0);
                        incomingRPIStatus.totalVideos = m.getArgAsInt(1);
                        incominRPIStatusString += " video " + ofToString(incomingRPIStatus.currentVideo) + " of " + ofToString(incomingRPIStatus.totalVideos);
                    }
                    else{
                        if (!incomingRPIStatus.isVideo) {
                            incomingRPIStatus.currentText = m.getArgAsInt(2);
                            incomingRPIStatus.totalTextFiles = m.getArgAsInt(3);
                            incominRPIStatusString += " text " + ofToString(incomingRPIStatus.currentText) + " of " + ofToString(incomingRPIStatus.totalTextFiles);
                        }
                    }
                }
            }
            
            incomingRPIStatus.maskXPos = m.getArgAsInt(4);
            incomingRPIStatus.maskYPos = m.getArgAsInt(5);
            
            incominRPIStatusString += " with mask x: " + ofToString(incomingRPIStatus.maskXPos) + " and mask y: " + ofToString(incomingRPIStatus.maskYPos);
            
        }
        
        if (m.getAddress() == "/status" && m.getRemoteHost() == iPhoneIPAddress) {
            ofLogNotice("Antiman::OSC to mask") << "Received osc status from iPhone";
            incomingIPhoneStatus.isDebugging = m.getArgAsBool(0);
            if (incomingIPhoneStatus.isDebugging) {
                incominIphoneStatusString = "Iphone is is debugging";
            }
            else{
                incomingIPhoneStatus.isPlaying = m.getArgAsBool(1);
                
                if (!incomingIPhoneStatus.isPlaying ) {
                    incominIphoneStatusString = "iPhone is currently connected and idle ";
                }
                
                if(incomingIPhoneStatus.isPlaying) {
                    incominIphoneStatusString = "iPhone is is playing";
                    incomingIPhoneStatus.isVideo = m.getArgAsBool(2);
                    
                    if (incomingIPhoneStatus.isVideo) {
                        incomingIPhoneStatus.currentVideo = m.getArgAsInt(0);
                        incomingIPhoneStatus.totalVideos = m.getArgAsInt(1);
                        incominIphoneStatusString += " video " + ofToString(incomingIPhoneStatus.currentVideo) + " of " + ofToString(incomingIPhoneStatus.totalVideos);
                    }
                }
            }
        }
    }
}



//--------------------------------------------------------------
void ofApp::draw() {
    
    ofSetBackgroundColor(0, 0, 0);
    if (!miniVersion) {
        ofPushStyle();
        ofSetColor(antimanColour3);
        displayFontLarge.drawString("Antiman Midi receiver", 30, 45);
        displayFontSmall.drawString("Using midi port: " +  midiIn.getName(), 40, 95);
        
        ofPushStyle();
        if (!incomingRPIStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        displayFontSmall.drawString(incominRPIStatusString, 40, 130);
        ofPopStyle();
        
        ofPushStyle();
        if (!incomingRPIStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        else{
            ofSetColor(0, 255, 0);
        }
        rpiLogo.draw(700, 250,rpiLogo.getWidth()/2, rpiLogo.getHeight()/2 );
        ofPopStyle();
        
        ofPushStyle();
        if (!serialIsConnected) {
            ofSetColor(255, 0, 0);
        }
        displayFontSmall.drawString(serialDeviceStatusString, 40, 163);
        ofPopStyle();
        
        ofPushStyle();
        if (!serialIsConnected) {
            ofSetColor(255, 0, 0);
        }
        else{
            ofSetColor(0, 255, 0);
        }
        arduinoLogo.draw(700, 500, arduinoLogo.getWidth()/2, arduinoLogo.getHeight()/2);
        ofPopStyle();
        
        ofPushStyle();
        if (!incomingIPhoneStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        displayFontSmall.drawString(incominIphoneStatusString, 40, 196);
        ofPopStyle();
        
        ofPushStyle();
        if (!incomingIPhoneStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        else{
            ofSetColor(0, 255, 0);
        }
        iPhoneLogo.draw(670, 750,iPhoneLogo.getWidth()/2.5, iPhoneLogo.getHeight()/2.5 );
        ofPopStyle();
        
        if (drawName) {
            displayFontSmall.drawString(nameMapString, 40, 230);
        }
        
        displayFontSmall.drawString("Digital output previews", digitalOutViewLocation.x - 20, digitalOutViewLocation.y-40);
        displayFontSmall.drawString("PWM output previews", pwmViewLocation.x - 20, pwmViewLocation.y-40);
        ofPopStyle();
        
    }
    if (miniVersion) {
        ofPushStyle();
        ofSetColor(antimanColour3);
        displayFontSmall.drawString("Antiman Midi receiver", 10, 20);
        displayFontMini.drawString("Using midi port: " +  midiIn.getName(), 10, 38);
        
        ofPushStyle();
        if (!incomingRPIStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        displayFontMini.drawString(incominRPIStatusString, 10, 58);
        ofPopStyle();
        
        ofPushStyle();
        if (!incomingRPIStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        else{
            ofSetColor(0, 255, 0);
        }
        rpiLogo.draw(1100, (ofGetHeight() - rpiLogo.getHeight()/5)/2 ,rpiLogo.getWidth()/5, rpiLogo.getHeight()/5);
        ofPopStyle();
        
        ofPushStyle();
        if (!serialIsConnected) {
            ofSetColor(255, 0, 0);
        }
        else{
            ofSetColor(0, 255, 0);
        }
        arduinoLogo.draw(1200, (ofGetHeight() - arduinoLogo.getHeight()/5)/2, arduinoLogo.getWidth()/5, arduinoLogo.getHeight()/5);
        ofPopStyle();
        
        ofPushStyle();
        if (!serialIsConnected) {
            ofSetColor(255, 0, 0);
        }
        displayFontMini.drawString(serialDeviceStatusString, 10, 78);
        ofPopStyle();
        
        ofPushStyle();
        if (!incomingIPhoneStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        displayFontMini.drawString(incominIphoneStatusString, 10, 98);
        ofPopStyle();
        
        ofPushStyle();
        if (!incomingIPhoneStatus.isConnected) {
            ofSetColor(255, 0, 0);
        }
        else{
            ofSetColor(0, 255, 0);
        }
        iPhoneLogo.draw(1300, (ofGetHeight() - iPhoneLogo.getHeight()/7)/2 ,iPhoneLogo.getWidth()/7, iPhoneLogo.getHeight()/7);
        ofPopStyle();
        
        if (drawName) {
            displayFontMini.drawString(nameMapString, 10, 115);
        }
        
        displayFontMini.drawString("Digital output previews", digitalOutViewLocationSmall.x -5, digitalOutViewLocationSmall.y - 10);
        displayFontMini.drawString("PWM output previews", pwmViewLocationSmall.x -5 , pwmViewLocationSmall.y-12);
        ofPopStyle();
        
    }
    
    
    
    for(int i = 0; i < numDigiPins ; i++){
        digitaloutputsViewFilled[i].draw();
        ofPushStyle();
        ofSetColor(antimanColour3);
        if(miniVersion){
            ofSetLineWidth(1);
        }
        else{
            ofSetLineWidth(3);
        }
        
        digitaloutputsViewOutline[i].draw();
        ofPopStyle();
    }
    
    for(int j = 0; j < numPWMPins; j++){
        
        pwmOuputsViewFilled[j].draw();
        ofPushStyle();
        ofSetColor(antimanColour3);
        if(miniVersion){
            ofSetLineWidth(1);
        }
        else{
            ofSetLineWidth(3);
        }
        pwmOuputsViewOutline[j].draw();
        ofPopStyle();
        
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::exit() {
    exitSerial();
    exitMidi();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    midiMessage = msg;
    
    if (msg.channel == midiChannelArduino) {
        if (msg.status == MIDI_NOTE_ON) {
            if (msg.pitch > pwmNoteStart-1 && msg.value < pwmNoteStart + numPWMPins + 1 && msg.pitch < digiNoteStart) {
                sendPWNPinSignal(msg.pitch - pwmNoteStart + pwmPinStart, int(ofMap(msg.velocity, 0, 127, pWMVelocityMapRangeLow, pWMVelocityMapRangeHigh)), msg.pitch - pwmNoteStart);
            }
            
            if (msg.pitch > digiNoteStart-1 &&  msg.value < digiNoteStart + numDigiPins + 1 && msg.velocity != 0 && msg.pitch < digiNoteStart + numDigiPins +1) {
                sendDigitalPinSignal(msg.pitch - digiNoteStart + digiPinStart, 1, msg.pitch - digiNoteStart);
            }
            
            else if(msg.pitch > digiNoteStart-1 &&  msg.value < digiNoteStart + numDigiPins + 1  && msg.velocity == 0 && msg.pitch < digiNoteStart + numDigiPins +1){
                sendDigitalPinSignal(msg.pitch - digiNoteStart + digiPinStart, 0, msg.pitch - digiNoteStart);
            }
        }
        
        if (msg.status == MIDI_NOTE_OFF) {
            if (msg.pitch > pwmNoteStart-1 && msg.value < pwmNoteStart + numPWMPins + 1 && msg.pitch < digiNoteStart) {
                sendPWNPinSignal(msg.pitch - pwmNoteStart + pwmPinStart, 0, msg.pitch - pwmNoteStart);
            }
            
            if(msg.pitch > digiNoteStart-1 &&  msg.value < digiNoteStart + numDigiPins + 1 && msg.pitch < digiNoteStart + numDigiPins +1){
                sendDigitalPinSignal(msg.pitch - digiNoteStart + digiPinStart, 0, msg.pitch - digiNoteStart);
            }
        }
    }
    
    if (msg.channel == midiChannelMask) {
        if (msg.status == MIDI_NOTE_ON) {
//            for (int i = 0; i < 4; i++) {
//                if(msg.pitch == noteMaskPosX[i]){
//                    recycleMessage.clear();
//                    recycleMessage.setAddress("/maskX");
//                    recycleMessage.addIntArg(maskPosX[i]);
//                    rpiOscSender.sendMessage(recycleMessage);
//                    ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
//
//                }
//                if (msg.pitch == noteMaskPosY[i]) {
//                    recycleMessage.clear();
//                    recycleMessage.setAddress("/maskY");
//                    recycleMessage.addIntArg(maskPosY[i]);
//                    rpiOscSender.sendMessage(recycleMessage);
//                    ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
//
//                }
//            }
            
            for (int i = 0; i < 15; i++) {
                if(msg.pitch == startNoteMaskVideoTrigger + i){
                    recycleMessage.clear();
                    recycleMessage.setAddress("/maskVideoTrigger");
                    recycleMessage.addIntArg(i);
                    iPhoneOscSender.sendMessage(recycleMessage);
                    ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
                }
            }
            
            for (int i = 0; i < 15; i++) {
                if(msg.pitch == startNoteMaskTextTrigger + i){
                    recycleMessage.clear();
                    recycleMessage.setAddress("/maskTextTrigger");
                    recycleMessage.addIntArg(i);
                    iPhoneOscSender.sendMessage(recycleMessage);
                    ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
                }
            }
            
            if (msg.pitch == noteMaskResetScreeen ) {
                recycleMessage.clear();
                recycleMessage.setAddress("/maskResetScreen");
                iPhoneOscSender.sendMessage(recycleMessage);
                ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
            }
            if (msg.pitch == noteMaskDebug) {
                recycleMessage.clear();
                recycleMessage.setAddress("/maskDebug");
                iPhoneOscSender.sendMessage(recycleMessage);
                ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
            }
            
        }
        
//        if (msg.status == MIDI_CONTROL_CHANGE) {
//            if (msg.control == controllerNumberMaskX) {
//                recycleMessage.clear();
//                recycleMessage.setAddress("/maskX");
//                recycleMessage.addIntArg(int(ofMap(msg.value, 0, 127, maskMinPosX, maskMaxPosX)));
//                rpiOscSender.sendMessage(recycleMessage);
//                ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
//
//            }
//
//            if (msg.control == controllerNumberMaskY) {
//                recycleMessage.clear();
//                recycleMessage.setAddress("/maskY");
//                recycleMessage.addIntArg(int(ofMap(msg.value, 0, 127, maskMinPosY, maskMaxPosY)));
//                rpiOscSender.sendMessage(recycleMessage);
//                ofLogNotice("Antiman::Midi to mask") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
//
//            }
//        }
        
    }
    if (msg.channel == midiChannelText) {
        if (msg.status == MIDI_NOTE_ON) {
            
            
            for (int i = 0; i < 15; i++) {
                if(msg.pitch == startNoteMaskVideoTrigger + i){
                    recycleMessage.clear();
                    recycleMessage.setAddress("/maskVideoTrigger");
                    recycleMessage.addIntArg(i);
                    rpiOscSender.sendMessage(recycleMessage);
                    ofLogNotice("Antiman::Midi to RPI") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
                }
            }
            
            for (int i = 0; i < 15; i++) {
                if(msg.pitch == startNoteMaskTextTrigger + i){
                    recycleMessage.clear();
                    recycleMessage.setAddress("/maskTextTrigger");
                    recycleMessage.addIntArg(i);
                    rpiOscSender.sendMessage(recycleMessage);
                    ofLogNotice("Antiman::Midi to RPI") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
                }
            }
            
            if (msg.pitch == noteMaskResetScreeen ) {
                recycleMessage.clear();
                recycleMessage.setAddress("/maskResetScreen");
                rpiOscSender.sendMessage(recycleMessage);
                ofLogNotice("Antiman::Midi to RPI") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
            }
            if (msg.pitch == noteMaskDebug) {
                recycleMessage.clear();
                recycleMessage.setAddress("/maskDebug");
                rpiOscSender.sendMessage(recycleMessage);
                ofLogNotice("Antiman::Midi to RPI") << "sent osc command to " + recycleMessage.getAddress() + "with value " + ofToString(recycleMessage.getArgAsInt(0));
            }
            
        }
        
    }
    
    if (msg.channel == midiChannelLogicControl) {
        if (msg.status == MIDI_NOTE_ON) {
            
        }
        if (msg.status == MIDI_NOTE_OFF) {
            
        }
    }
    
    
    
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    
    if(key == 'm'){
        miniVersion = !miniVersion;
        if(miniVersion){
            setupInterface(circleRadiusSmall, cirlceSpacingSmall, pwmViewLocationSmall, digitalOutViewLocationSmall, 10, 4);
            ofSetWindowShape(1920,100);
            ofSetWindowPosition(0, 0);
        }
        else{
            setupInterface(circleRadius, cirlceSpacing, pwmViewLocation, digitalOutViewLocation, 8, 4);
            ofSetWindowShape(1280,1024);
            ofSetWindowPosition((ofGetScreenWidth() - 1280) /2, (ofGetScreenHeight() - 1024)/2);
        }
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    
    if(!isTestingPWM && !isTestingDigital){
        for(int i = 0; i < numDigiPins  ; i++){
            if (digitaloutputsViewOutline[i].inside(x, y) && !isTestingDigital) {
                drawName = true;
                nameMapString = "ID is " +  digitalNameMap[i];
                displayNameTimer = ofGetElapsedTimeMillis();
            }
        }
        
        for(int i = 0; i < numPWMPins; i++){
            if (pwmOuputsViewOutline[i].inside(x, y) && !isTestingPWM) {
                drawName = true;
                nameMapString = "ID is " +  pwmNameMap[i];
                displayNameTimer = ofGetElapsedTimeMillis();
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if(!miniVersion){
        for(int i = 0; i < numDigiPins  ; i++){
            if (digitaloutputsViewOutline[i].inside(x, y) && !isTestingDigital) {
                sendDigitalPinSignal(i+ digiPinStart, 1, i);
                testPinDigital = i;
                isTestingDigital = true;
                drawName = true;
                nameMapString = "Testing " +  digitalNameMap[i];
            }
        }
        
        for(int i = 0; i < numPWMPins; i++){
            if (pwmOuputsViewOutline[i].inside(x, y) && !isTestingPWM) {
                sendPWNPinSignal(i + pwmPinStart, 255, i);
                testPinPWM = i;
                isTestingPWM = true;
                drawName = true;
                nameMapString = "Testing " +  pwmNameMap[i];
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    
    if (isTestingPWM) {
        sendPWNPinSignal(testPinPWM + pwmPinStart, 0, testPinPWM);
        isTestingPWM = false;
        
    }
    
    if (isTestingDigital) {
        sendDigitalPinSignal(testPinDigital + digiPinStart, 0, testPinDigital);
        isTestingDigital = false;
        
    }
}


void ofApp::setupMidi(){
    ofLogNotice("Antiman::setup Midi") << "Creating virtual port";
    midiIn.openVirtualPort(midiPortName); // open a virtual port
    
    ofLogNotice("Antiman::setup Midi") << "Setting up midi parameters";
    midiIn.ignoreTypes(false, false, false);
    
    ofLogNotice("Antiman::setup Midi") << "Adding callback method";
    midiIn.addListener(this);
    
    ofLogNotice("Antiman::setup Midi") << "Completed midi setup";
}

void ofApp::exitMidi(){
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
    device.unregisterAllEvents(this);
    
}

void ofApp::onSerialBuffer(const ofxIO::SerialBufferEventArgs& args){
    
    ofLogNotice("Antiman::Arduino") << "Received serial message from Arduino: " + args.buffer().toString();
    
    if (args.buffer().toString() == "ack") {
        ackArduinoReceiveTimer = ofGetElapsedTimeMillis();
        serialIsConnected = true;
        ofLogNotice("Antiman::Arduino") << "Received serial ack from Arduino";
    }
}
void ofApp::onSerialError(const ofxIO::SerialBufferErrorEventArgs& args){
    
    serialDeviceStatusString = "Arduino received and error: " + args.buffer().toString();
    
}

void ofApp::setupSerial(){
    
    ofLogNotice("Antiman::setup Serial") << "Looking for devices";
    std::vector<ofxIO::SerialDeviceInfo> devicesInfo = ofxIO::SerialDeviceUtils::listDevices();
    
    ofLogNotice("Antiman::setup Serial") << "Connected Devices: ";
    
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("Antiman::setup Serial") << "\t" << devicesInfo[i];
    }
    
    if (!devicesInfo.empty())
    {
        // Connect to the first matching device.
        bool success = device.setup(devicesInfo[serialDeviceID], 115200);
        
        if(success)
        {
            device.registerAllEvents(this);
            ofLogNotice("Antiman::setup Serial") << "Successfully setup " << devicesInfo[serialDeviceID];
            serialDeviceStatusString = "Arduino " + devicesInfo[serialDeviceID].port() + " connected";
            serialIsConnected = true;
        }
        else
        {
            ofLogNotice("Antiman::setup Serial") << "Unable to setup " << devicesInfo[serialDeviceID];
            serialDeviceStatusString = "Arduino " + devicesInfo[serialDeviceID].port() + " present but could not be setup";
            serialIsConnected = false;
        }
    }
    else
    {
        ofLogNotice("Antiman::setup Serial") << "No serial devices connected.";
        serialDeviceStatusString = "Arduino not connected";
        serialIsConnected = false;
    }
    
    ofLogNotice("Antiman::setup Serial") << "Completed serial setup";
}

void ofApp::exitSerial(){
    device.unregisterAllEvents(this);
}

void ofApp::setupOSC(){
    
    oscReceive.setup(antimanOSCReceivePort);
    rpiOscSender.setup(rpiIPAddress, rpiOSCReceivePort);
    iPhoneOscSender.setup(iPhoneIPAddress, iPhoneOSCReceivePort);
    ackMessage.setAddress("/ack");
}

void ofApp::readSettingsFile(){
    ofFile file("antiman_settings.json");
    if(file.exists()){
        file >> antimanSettings;
        
        
        debugLevel = antimanSettings[0]["debugLevel"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "debugLevel is set to " + ofToString(debugLevel)<< endl;
        
        switch(debugLevel){
            case 0:
                ofSetLogLevel(OF_LOG_SILENT);
                break;
            case 1:
                ofSetLogLevel(OF_LOG_NOTICE);
                break;
            case 2:
                ofSetLogLevel(OF_LOG_VERBOSE);
                break;
        }
        
        midiPortName = antimanSettings[0]["midiPortName"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "midiPortName is set to " + ofToString(midiPortName)<< endl;
        
        midiChannelArduino = antimanSettings[0]["midiChannelArduino"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "midiChannelArduino is set to " + ofToString(midiChannelArduino)<< endl;
        
        midiChannelMask = antimanSettings[0]["midiChannelMask"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "midiChannelMask is set to " + ofToString(midiChannelMask)<< endl;
        
        midiChannelText = antimanSettings[0]["midiChannelText"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "midiChannelText is set to " + ofToString(midiChannelText)<< endl;
        
        midiChannelLogicControl = antimanSettings[0]["midiChannelLogicControl"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "midiChannelLogicControl is set to " + ofToString(midiChannelLogicControl)<< endl;
        
        serialDeviceID = antimanSettings[0]["serialDeviceID"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "serialDeviceID is set to " + ofToString(serialDeviceID)<< endl;
        
        rpiIPAddress = antimanSettings[0]["rpiIPAddress"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "rpiIPAddress is set to " + ofToString(rpiIPAddress)<< endl;
        
        rpiOSCReceivePort = antimanSettings[0]["rpiOSCReceivePort"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "rpiOSCReceivePort is set to " + ofToString(rpiOSCReceivePort)<< endl;
        
        iPhoneIPAddress = antimanSettings[0]["iPhoneIPAddress"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "iPhoneIPAddress is set to " + ofToString(iPhoneIPAddress)<< endl;
        
        iPhoneOSCReceivePort = antimanSettings[0]["iPhoneOSCReceivePort"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "iPhoneOSCReceivePort is set to " + ofToString(iPhoneOSCReceivePort)<< endl;
        
        antimanOSCReceivePort = antimanSettings[0]["antimanOSCReceivePort"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "antimanOSCReceivePort is set to " + ofToString(antimanOSCReceivePort)<< endl;
        
        displayFontMiniName = antimanSettings[0]["displayFontMiniName"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "displayFontMiniName is set to " + ofToString(displayFontMiniName)<< endl;
        
        displayFontSmallName = antimanSettings[0]["displayFontSmallName"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "displayFontSmallName is set to " + ofToString(displayFontSmallName)<< endl;
        
        displayFontLargeName = antimanSettings[0]["displayFontLargeName"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "displayFontLargeName is set to " + ofToString(displayFontLargeName)<< endl;
        
        antimanColour1 = ofColor(antimanSettings[0]["antimanColour1R"].get<int>(), antimanSettings[0]["antimanColour1G"].get<int>(), antimanSettings[0]["antimanColour1B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour1 " + ofToString(antimanColour1) << endl;
        
        antimanColour2 = ofColor(antimanSettings[0]["antimanColour2R"].get<int>(), antimanSettings[0]["antimanColour2G"].get<int>(), antimanSettings[0]["antimanColour2B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour2 " + ofToString(antimanColour2) << endl;
        
        antimanColour3 = ofColor(antimanSettings[0]["antimanColour3R"].get<int>(), antimanSettings[0]["antimanColour3G"].get<int>(), antimanSettings[0]["antimanColour3B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour3 " + ofToString(antimanColour3) << endl;
        
        antimanColour4 = ofColor(antimanSettings[0]["antimanColour4R"].get<int>(), antimanSettings[0]["antimanColour4G"].get<int>(), antimanSettings[0]["antimanColour4B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour4 " + ofToString(antimanColour4) << endl;
        
        circleRadius = antimanSettings[0]["circleRadius"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "circleRadius is set to " + ofToString(circleRadius)<< endl;
        
        cirlceSpacing = antimanSettings[0]["cirlceSpacing"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "cirlceSpacing is set to " + ofToString(cirlceSpacing)<< endl;
        
        pwmViewLocation.x = antimanSettings[0]["pwmViewLocationX"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pwmViewLocation.x is set to " + ofToString(pwmViewLocation.x)<< endl;
        
        pwmViewLocation.y = antimanSettings[0]["pwmViewLocationY"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pwmViewLocation.y is set to " + ofToString(pwmViewLocation.y)<< endl;
        
        digitalOutViewLocation.x = antimanSettings[0]["digitalOutViewLocationX"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "digitalOutViewLocation.x is set to " + ofToString(digitalOutViewLocation.x)<< endl;
        
        digitalOutViewLocation.y = antimanSettings[0]["digitalOutViewLocationY"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "digitalOutViewLocation.y is set to " + ofToString(digitalOutViewLocation.y)<< endl;
        
        circleRadiusSmall = antimanSettings[0]["circleRadiusSmall"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "circleRadiusSmall is set to " + ofToString(circleRadiusSmall)<< endl;
        
        cirlceSpacingSmall = antimanSettings[0]["cirlceSpacingSmall"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "cirlceSpacingSmall is set to " + ofToString(cirlceSpacingSmall)<< endl;
        
        pwmViewLocationSmall.x = antimanSettings[0]["pwmViewLocationXSmall"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pwmViewLocationSmall.x is set to " + ofToString(pwmViewLocationSmall.x)<< endl;
        
        pwmViewLocationSmall.y = antimanSettings[0]["pwmViewLocationYSmall"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pwmViewLocationSmall.y is set to " + ofToString(pwmViewLocationSmall.y)<< endl;
        
        digitalOutViewLocationSmall.x = antimanSettings[0]["digitalOutViewLocationXSmall"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "digitalOutViewLocationSmall.x is set to " + ofToString(digitalOutViewLocationSmall.x)<< endl;
        
        digitalOutViewLocationSmall.y = antimanSettings[0]["digitalOutViewLocationYSmall"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "digitalOutViewLocationSmall.y is set to " + ofToString(digitalOutViewLocationSmall.y)<< endl;
        
        pwmPinStart = antimanSettings[0]["pwmPinStart"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pwmPinStart is set to " + ofToString(pwmPinStart)<< endl;
        
        numPWMPins = antimanSettings[0]["numPWMPins"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "numPWMPins is set to " + ofToString(numPWMPins)<< endl;
        
        pwmNoteStart = antimanSettings[0]["pwmNoteStart"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pwmNoteStart is set to " + ofToString(pwmNoteStart)<< endl;
        
        digiPinStart = antimanSettings[0]["digiPinStart"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "digiPinStart is set to " + ofToString(digiPinStart)<< endl;
        
        numDigiPins = antimanSettings[0]["numDigiPins"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "numDigiPins is set to " + ofToString(numDigiPins)<< endl;
        
        digiNoteStart = antimanSettings[0]["digiNoteStart"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "digiNoteStart is set to " + ofToString(digiNoteStart)<< endl;
        
        for(int i =0; i < 4; i++){
            noteMaskPosX[i] = antimanSettings[0]["noteMaskPosX"+ofToString(i)].get<int>();
            ofLogNotice("Antiman::Read settings file") << "noteMaskPosX" + ofToString(i) + " is set to " + ofToString(noteMaskPosX[i])<< endl;
            
            noteMaskPosY[i] = antimanSettings[0]["noteMaskPosY"+ofToString(i)].get<int>();
            ofLogNotice("Antiman::Read settings file") << "noteMaskPosY" + ofToString(i) + " is set to " + ofToString(noteMaskPosY[i])<< endl;
            
            maskPosX[i] = antimanSettings[0]["maskPosX"+ofToString(i)].get<int>();
            ofLogNotice("Antiman::Read settings file") << "maskPosX" + ofToString(i) + " is set to " + ofToString(maskPosX[i])<< endl;
            
            maskPosY[i] = antimanSettings[0]["maskPosY"+ofToString(i)].get<int>();
            ofLogNotice("Antiman::Read settings file") << "maskPosY" + ofToString(i) + " is set to " + ofToString(maskPosY[i])<< endl;
        }
        
        controllerNumberMaskX = antimanSettings[0]["controllerNumberMaskX"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "controllerNumberMaskX is set to " + ofToString(controllerNumberMaskX)<< endl;
        
        controllerNumberMaskY = antimanSettings[0]["controllerNumberMaskY"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "controllerNumberMaskY is set to " + ofToString(controllerNumberMaskY)<< endl;
        
        pWMVelocityMapRangeLow = antimanSettings[0]["pWMVelocityMapRangeLow"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pWMVelocityMapRangeLow is set to " + ofToString(pWMVelocityMapRangeLow)<< endl;
        
        pWMVelocityMapRangeHigh = antimanSettings[0]["pWMVelocityMapRangeHigh"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "pWMVelocityMapRangeHigh is set to " + ofToString(pWMVelocityMapRangeHigh)<< endl;
        
        maskMinPosX = antimanSettings[0]["maskMinPosX"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "maskMinPosX is set to " + ofToString(maskMinPosX)<< endl;
        
        maskMaxPosX = antimanSettings[0]["maskMaxPosX"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "maskMaxPosX is set to " + ofToString(maskMaxPosX)<< endl;
        
        maskMinPosY = antimanSettings[0]["maskMinPosY"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "maskMinPosY is set to " + ofToString(maskMinPosY)<< endl;
        
        maskMaxPosY = antimanSettings[0]["maskMaxPosY"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "maskMaxPosY is set to " + ofToString(maskMaxPosY)<< endl;
        
        
        for(int i = 0; i <NUM_PWM_OUT; i++){
            pwmNameMap[i] = antimanSettings[0]["pwmNameMap"+ofToString(i)].get<std::string>();
            ofLogNotice("Antiman::Read settings file") << "pwmNameMap " + ofToString(i) + " is " + pwmNameMap[i]<< endl;
        }
        
        for(int i = 0; i <NUM_DIGITAL_OUT; i++){
            digitalNameMap[i] = antimanSettings[0]["digitalNameMap"+ofToString(i)].get<std::string>();
            ofLogNotice("Antiman::Read settings file") << "digitalNameMap " + ofToString(i) + " is " + digitalNameMap[i]<< endl;
        }
        
        startNoteMaskVideoTrigger = antimanSettings[0]["startNoteMaskVideoTrigger"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "startNoteMaskVideoTrigger is set to " + ofToString(startNoteMaskVideoTrigger)<< endl;
        
        startNoteMaskTextTrigger = antimanSettings[0]["startNoteMaskTextTrigger"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "startNoteMaskTextTrigger is set to " + ofToString(startNoteMaskTextTrigger)<< endl;
        
        noteMaskResetScreeen = antimanSettings[0]["noteMaskResetScreeen"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "noteMaskResetScreeen is set to " + ofToString(noteMaskResetScreeen)<< endl;
        
        noteMaskDebug = antimanSettings[0]["noteMaskDebug"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "noteMaskDebug is set to " + ofToString(noteMaskDebug)<< endl;
        
        
    }
    
}

void ofApp::setupGraphics(){
    ofLogNotice("Antiman::setup graphics") << "Setting up graphics";
    
    if(displayFontLarge.load("graphics_resources/" + displayFontLargeName, 40, true, true)){
        ofLogNotice("Antiman::setup graphics") << "Loaded large display font";
        
    }
    else{
        ofLogNotice("Antiman::setup graphics") << "Unable to load large display font";
        
    }
    
    if(displayFontSmall.load("graphics_resources/" + displayFontSmallName, 21, true, true)){
        ofLogNotice("Antiman::setup graphics") << "Loaded small display font";
        
    }
    else{
        ofLogNotice("Antiman::setup graphics") << "Unable to load small display font";
        
    }
    
    if(displayFontMini.load("graphics_resources/" + displayFontMiniName, 13, true, true)){
        ofLogNotice("Antiman::setup graphics") << "Loaded mini display font";
        
    }
    else{
        ofLogNotice("Antiman::setup graphics") << "Unable to load mini display font";
        
    }
    
    if(arduinoLogo.load("graphics_resources/arudino_logo.png")){
        ofLogNotice("Antiman::setup graphics") << "Arduino logo loaded";
        
    }
    
    else{
        ofLogNotice("Antiman::setup graphics") << "Unable to load Arduino Logo";
        
    }
    
    if(rpiLogo.load("graphics_resources/rpi_logo.png")){
        ofLogNotice("Antiman::setup graphics") << "RPI logo loaded";
        
    }
    else{
        ofLogNotice("Antiman::setup graphics") << "Unable to load RPI Logo";
        
    }
    if(iPhoneLogo.load("graphics_resources/iphone_logo.png")){
        ofLogNotice("Antiman::setup graphics") << "Iphone logo loaded";
        
    }
    else{
        ofLogNotice("Antiman::setup graphics") << "Unable to load Iphone Logo";
        
    }
    
    miniVersion = true;
    
}
void ofApp::setupInterface(int _circleRadius, int _cirlceSpacing, ofPoint _pwmViewLocation, ofPoint _digitalOutViewLocation, int columnsDigital, int columnsPWM){
    int newRow = 0;
    for(int i = 0; i < numDigiPins  ; i++){
        
        if (i > 0 && i % columnsDigital == 0) {
            newRow ++;
        }
        digitaloutputsViewFilled[i].clear();
        digitaloutputsViewFilled[i].setFilled(true);
        digitaloutputsViewFilled[i].setCircleResolution(32);
        digitaloutputsViewFilled[i].circle(_digitalOutViewLocation.x + ((i%8 ) * _cirlceSpacing), _digitalOutViewLocation.y + (newRow * _cirlceSpacing ), _circleRadius);
        digitaloutputsViewFilled[i].setColor(antimanColour1);
        digitaloutputsViewOutline[i] =  digitaloutputsViewFilled[i].getOutline()[0];
        
    }
    
    newRow = 0;
    for(int i = 0; i < numPWMPins; i++){
        if (i > 0 && i % columnsPWM == 0) {
            newRow ++;
            
        }
        pwmOuputsViewFilled[i].clear();
        pwmOuputsViewFilled[i].setFilled(true);
        pwmOuputsViewFilled[i].setCircleResolution(32);
        pwmOuputsViewFilled[i].circle(_pwmViewLocation.x + ((i%4)* _cirlceSpacing), _pwmViewLocation.y + (newRow * _cirlceSpacing ), _circleRadius);
        pwmOuputsViewFilled[i].setColor(antimanColour1);
        pwmOuputsViewOutline[i]= pwmOuputsViewFilled[i].getOutline()[0];
    }
}

void ofApp::exitGraphics(){
    
}

void ofApp::sendDigitalPinSignal(int pin, int value, int displayValue){
    std::string bufferString = "DIG~" + ofToString(pin) + "/" + ofToString(value);
    ofx::IO::ByteBuffer buffer(bufferString);
    device.send(buffer);
    ofLogNotice("Antiman::Midi to arduino") << "sent buffer " + bufferString;
    if (value == 0) {
        digitaloutputsViewFilled[displayValue].setColor(antimanColour1);
        
    }
    if (value == 1) {
        digitaloutputsViewFilled[displayValue].setColor(antimanColour2);
        
    }
}
void ofApp::sendPWNPinSignal(int pin, int value, int displayValue){
    std::string bufferString = "PWM~" + ofToString(pin) + "/" + ofToString(value);
    ofx::IO::ByteBuffer buffer(bufferString);
    device.send(buffer);
    ofLogNotice("Antiman::Midi to arduino") << "sent buffer " + bufferString;
    if (value==0) {
        pwmOuputsViewFilled[displayValue].setColor(antimanColour1);
        
    }
    else {
        pwmOuputsViewFilled[displayValue].setColor(ofColor(antimanColour2.r, antimanColour2.g, antimanColour2.b,value));
        
    }
}
void ofApp::setupRPIStatus(){
    incominRPIStatusString = "RPI has not published any status events";
    rpiAckSendTimer =  ofGetElapsedTimeMillis();
    rpiAckReceiveTimer =  ofGetElapsedTimeMillis();
}

void ofApp::setupIPhoneStatus(){
    incominIphoneStatusString = "Iphone has not published any status events";
    iPhonerpiAckSendTimer =  ofGetElapsedTimeMillis();
    iPhonerpiAckReceiveTimer =  ofGetElapsedTimeMillis();
}

void ofApp::updateArduinoStatus(){
    // The serial device can throw exeptions.
    
}

void ofApp::setupArduinoStatus(){
    ackArduinoSendTimer =  ofGetElapsedTimeMillis();
    ackArduinoReceiveTimer =  ofGetElapsedTimeMillis();
}

