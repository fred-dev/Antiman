#include "ofApp.h"
#import "AVFoundationVideoPlayer.h"

//--------------------------------------------------------------
void ofApp::setup(){	
    ofBackground(ofColor::black);
    setupFromJSON();
    setupOSC();
    setupAck();
    setupVideos();
    setupStatus();
    firstRun = true;
    ofSetOrientation(OF_ORIENTATION_90_RIGHT);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(firstRun){
        sendStatus();
        firstRun = false;
    }
    if(returnAck){
        oscSender.sendMessage(ackMessage);
        returnAck = false;
        ofLogNotice("Antiman::Mask update") << "Sent osc ack to Antiman: ";
    }
    
    while( oscReceiver.hasWaitingMessages() ){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        if( m.getAddress() == "/ack" ){
            returnAck = true;
            ofLogNotice("Antiman::Mask osc receive") << "Received osc ack from Antiman: ";
            
        }
        
        if( m.getAddress() == "/maskVideoTrigger" ){
            ofLogNotice("Antiman::Mask osc receive") << "Received " + m.getAddress() + " from Antiman Loading video: " + ofToString(outgoingRPIStatus.currentVideo) ;
            outgoingRPIStatus.currentVideo = m.getArgAsInt(0);
            loadMovie(outgoingRPIStatus.currentVideo);
            ofLogVerbose(__func__) << "doing reload";
            
            
        }
        
       
        
        if (m.getAddress() == "/maskResetScreen") {
            outgoingRPIStatus.isPlaying = false;
            outgoingRPIStatus.isDebugging = false;
            ofLogNotice("Antiman::Mask osc receive") << "Received osc reset screen from Antiman";
            sendStatus();
            
        }
        
        if (m.getAddress() == "/maskDebug") {
            if (outgoingRPIStatus.isDebugging == true) {
                outgoingRPIStatus.isDebugging = false;
            }
            else{
                outgoingRPIStatus.isDebugging = true;
            }
            ofLogNotice("Antiman::Mask osc receive") << "Received osc toggle debug from Antiman";
            sendStatus();
        }
    }

    if (!doLoadMovie && outgoingRPIStatus.isPlaying == true) {
        videoPlayer.update();
        if(videoPlayer.getIsMovieDone()){
            outgoingRPIStatus.isPlaying = false;
            sendStatus();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(outgoingRPIStatus.isPlaying && outgoingRPIStatus.isVideo && !doLoadMovie && videoPlayer.isLoaded()){
        videoPlayer.getTexturePtr()->draw(0, 0,ofGetWidth(), ofGetHeight());
    }
    

    
    if(outgoingRPIStatus.isDebugging){
        stringstream info;
        info << "Current movie path: " << videoFiles[outgoingRPIStatus.currentVideo].path() << endl;
        info << "Debugging status: " << outgoingRPIStatus.isDebugging <<endl;
        info << "Playing status: " << outgoingRPIStatus.isPlaying <<endl;
        info << "Is video: " << outgoingRPIStatus.isVideo <<endl;
        info << "Current video: " << outgoingRPIStatus.currentVideo <<endl;
        info << "Total videos: " << outgoingRPIStatus.totalVideos <<endl;
        info << "Current text: " << outgoingRPIStatus.currentText <<endl;
        info << "Total text strings: " << outgoingRPIStatus.totalTextFiles <<endl;
        info << "Mask Pos X: " << outgoingRPIStatus.maskXPos <<endl;
        info << "Mask Pos Y: " << outgoingRPIStatus.maskYPos <<endl;
        info << "Press n to load next movie"<< endl;
        info << "Press 1 - 0 to test text files"<< endl;
        info << "Press ! - $ to test mask positions"<< endl;
        info << "Press d to toggle debug"<< endl;
        ofDrawBitmapStringHighlight(info.str(), 10, 10, ofColor(ofColor::black, 90), ofColor::yellow);
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
//    if(outgoingRPIStatus.currentVideo +1 < videoFiles.size())
//    {
//        outgoingRPIStatus.currentVideo++;
//    }else
//    {
//        outgoingRPIStatus.currentVideo = 0;
//    }
//
//    loadMovie(outgoingRPIStatus.currentVideo);
//    sendStatus();
    
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    outgoingRPIStatus.isDebugging = !outgoingRPIStatus.isDebugging;
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}

void ofApp::setupFromJSON(){
    ofFile file("antiman_settings_IOS.json");
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
        
        antiManIPAddress = antimanSettings[0]["antiManIPAddress"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "antiManIPAddress is set to " + ofToString(antiManIPAddress)<< endl;
        
        iPhoneOSCReceivePort = antimanSettings[0]["iPhoneOSCReceivePort"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "iPhoneOSCReceivePort is set to " + ofToString(iPhoneOSCReceivePort)<< endl;
        
        antimanOSCReceivePort = antimanSettings[0]["antimanOSCReceivePort"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "antimanOSCReceivePort is set to " + ofToString(antimanOSCReceivePort)<< endl;
        
        
        displayFontSmallName = antimanSettings[0]["displayFontSmallName"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "displayFontSmallName is set to " + ofToString(displayFontSmallName)<< endl;
        
        displayFontLargeName = antimanSettings[0]["displayFontLargeName"].get<std::string>();
        ofLogNotice("Antiman::Read settings file") << "displayFontLargeName is set to " + ofToString(displayFontLargeName)<< endl;
        
        
        displayFontSmallSize = antimanSettings[0]["displayFontSmallSize"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "displayFontSmallSize is set to " + ofToString(displayFontSmallSize)<< endl;
        
        displayFontLargeSize = antimanSettings[0]["displayFontLargeSize"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "displayFontLargeSize is set to " + ofToString(displayFontLargeSize)<< endl;
        
        antimanColour1 = ofColor(antimanSettings[0]["antimanColour1R"].get<int>(), antimanSettings[0]["antimanColour1G"].get<int>(), antimanSettings[0]["antimanColour1B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour1 " + ofToString(antimanColour1) << endl;
        
        antimanColour2 = ofColor(antimanSettings[0]["antimanColour2R"].get<int>(), antimanSettings[0]["antimanColour2G"].get<int>(), antimanSettings[0]["antimanColour2B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour2 " + ofToString(antimanColour2) << endl;
        
        antimanColour3 = ofColor(antimanSettings[0]["antimanColour3R"].get<int>(), antimanSettings[0]["antimanColour3G"].get<int>(), antimanSettings[0]["antimanColour3B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour3 " + ofToString(antimanColour3) << endl;
        
        antimanColour4 = ofColor(antimanSettings[0]["antimanColour4R"].get<int>(), antimanSettings[0]["antimanColour4G"].get<int>(), antimanSettings[0]["antimanColour4B"].get<int>());
        ofLogNotice("Antiman::Read settings file") << "antimanColour4 " + ofToString(antimanColour4) << endl;
        
        for (int i = 0; i < 10; i++)
        {
            textStrings[i] = antimanSettings[0]["textStrings" + ofToString(i)].get<std::string>();
            ofLogNotice("Antiman::Read settings file") << "textStrings " + ofToString(i) + ": " + textStrings[i]  << endl;
        }
    }
}

void ofApp::setupOSC(){
    if(oscReceiver.setup(iPhoneOSCReceivePort)){
        ofLogNotice("Antiman::Setup OSC") << "OSC receiver listening on port: " + ofToString(iPhoneOSCReceivePort)<< endl;
        
    }
    else{
        ofLogNotice("Antiman::Setup OSC") << "OSC receiver could not be setup"<< endl;
        
    }
    if(oscSender.setup(antiManIPAddress, antimanOSCReceivePort)){
        ofLogNotice("Antiman::Setup OSC") << "OSC sender sending to: " + antiManIPAddress + " on port: " + ofToString(antimanOSCReceivePort)<< endl;
        
    }
    
}




void ofApp::loadMovie(int videoIndex){
    doLoadMovie = true;
    outgoingRPIStatus.isPlaying= false;
    if(videoIndex > -1 && videoIndex  <videoFiles.size())
    {
        outgoingRPIStatus.currentVideo= videoIndex;
        ofLogNotice("Antiman::Load movie") << "Loading movie" << videoFiles[outgoingRPIStatus.currentVideo].path();
        videoPlayer.load(videoFiles[videoIndex].path());
        videoPlayer.firstFrame();
        videoPlayer.play();
        outgoingRPIStatus.isPlaying = true;
        outgoingRPIStatus.isVideo = true;
        sendStatus();
        
        
    }
    else{
        ofLogNotice("Antiman::Load movie") << "Requested movie index " + ofToString(videoIndex) + " is out of range of loaded movies: must be between 0 and " + ofToString(videoFiles.size() -1);
        
    }
    doLoadMovie = false;
}

void ofApp::setupStatus(){
    outgoingRPIStatus.isDebugging = false;
}

void ofApp::sendStatus(){
    statusMessage.clear();
    statusMessage.setAddress("/status");
    statusMessage.addBoolArg(outgoingRPIStatus.isDebugging);
    statusMessage.addBoolArg(outgoingRPIStatus.isPlaying);
    statusMessage.addBoolArg(outgoingRPIStatus.isVideo);
    statusMessage.addIntArg(outgoingRPIStatus.currentVideo);
    statusMessage.addIntArg(outgoingRPIStatus.totalVideos);
    statusMessage.addIntArg(outgoingRPIStatus.currentText);
    statusMessage.addIntArg(outgoingRPIStatus.totalTextFiles);
    statusMessage.addIntArg(outgoingRPIStatus.maskXPos);
    statusMessage.addIntArg(outgoingRPIStatus.maskYPos);
    oscSender.sendMessage(statusMessage);
    ofLogNotice("Antiman::Send status") << "Sending status message" << endl;
}

void ofApp::setupAck(){
    ackMessage.setAddress("/ack");
}



void ofApp::setupVideos(){
    ofLogNotice("Antiman::setupVideos") << "scanning directory for videos" << endl;
    string directoryPath = "videos";
    ofDirectory currentVideoDirectory(directoryPath);
    if (currentVideoDirectory.exists())
    {
        currentVideoDirectory.listDir();
        currentVideoDirectory.sort();
        for(int i = 0; i < currentVideoDirectory.getFiles().size(); i++){
            if (currentVideoDirectory.getFiles()[i].getExtension() == "mov" || currentVideoDirectory.getFiles()[i].getExtension() == "mp4" || currentVideoDirectory.getFiles()[i].getExtension() == "m4v") {
                videoFiles.push_back(currentVideoDirectory.getFiles()[i]);
                ofLogNotice("Antiman::setupVideos") << "Found file: " + videoFiles[i].path() << endl;
            }
        }
        
        
        if (videoFiles.size()>0)
        {
            outgoingRPIStatus.currentVideo = 0;
            outgoingRPIStatus.totalVideos = videoFiles.size();
            outgoingRPIStatus.isVideo = true;
            outgoingRPIStatus.isPlaying = false;
        }
    }else
    {
        ofLogNotice("Antiman::setupVideos") << "No videos found in directory" << endl;
    }
}
