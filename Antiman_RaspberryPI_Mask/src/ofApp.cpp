#include "ofApp.h"

//https://github.com/sarfata/pi-blaster for config instructions
// proper logging messages
// debug mode



    // ofLog() << "onVideoEnd: " << player->isLoopingEnabled();
    // outgoingRPIStatus.isPlaying = false;
    // sendStatus();
    










unsigned long long skipTimeStart=0;
unsigned long long skipTimeEnd=0;
unsigned long long amountSkipped =0;
unsigned long long totalAmountSkipped =0;


//--------------------------------------------------------------
void ofApp::setup(){
    ofHideCursor();
    ofBackground(ofColor::black);
    rootPath = "";
    setupFromJSON();
    setupOSC();
    setupAck();
    setupVideos();
    setupText();
    

    firstRun = true;
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
        
        if (m.getAddress() == "/maskTextTrigger") {
            ofLogNotice("Antiman::Mask osc receive") << "Received osc text trigger from Antiman with text index: " +ofToString(m.getArgAsInt(0));
            playText(m.getArgAsInt(0));

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
        
        if (m.getAddress() == "/maskX") {
            outgoingRPIStatus.maskXPos = m.getArgAsInt(0);
            ofLogNotice("Antiman::Mask osc receive") << "Received osc mask X from Antiman";

            //setServoPositionX(m.getArgAsInt(0));
            sendStatus();
        }
        
        if (m.getAddress() == "/maskY") {
            outgoingRPIStatus.maskYPos = m.getArgAsInt(0);
            ofLogNotice("Antiman::Mask osc receive") << "Received osc mask Y from Antiman";
            //setServoPositionY(m.getArgAsInt(0));
            sendStatus();
        }
        
    }

    if (!doLoadMovie) {
        videoPlayer.update();
    }

    

}

//--------------------------------------------------------------
void ofApp::draw(){
    if(outgoingRPIStatus.isPlaying && outgoingRPIStatus.isVideo && !doLoadMovie){
        videoPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
    
    else if(outgoingRPIStatus.isPlaying && !outgoingRPIStatus.isVideo){
        ofPopStyle();
        ofSetColor(255);
        displayFontLarge.drawString(textStrings[outgoingRPIStatus.currentText], drawScrollX, 300);
        drawScrollX -= 5;
        
        if(displayFontLarge.getStringBoundingBox(textStrings[outgoingRPIStatus.currentText],0,0).getWidth() + drawScrollX <1){
            drawScrollX =  ofGetWidth();
        }
        ofPushStyle();
    }
    
  if(outgoingRPIStatus.isDebugging){
    stringstream info;
    //info << videoPlayer.getInfo() << endl;
    info << "Current movie path: " << videoFiles[outgoingRPIStatus.currentVideo].path() << endl;
    info << "Millis skipped this movie: " << amountSkipped << endl;
    info << "Total millis skipped: " << totalAmountSkipped << endl;
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
void ofApp::keyPressed(int key){
    
    switch (key)
    {
        case 'n':
        {
            if(outgoingRPIStatus.currentVideo +1 < videoFiles.size())
            {
                outgoingRPIStatus.currentVideo++;
            }else
            {
                outgoingRPIStatus.currentVideo = 0;
            }
       
            loadMovie(outgoingRPIStatus.currentVideo);
            sendStatus();
            break;
        }
        case '!':
            //setServoPositionDual(15,15);
        break;
        
        case '@':
            //setServoPositionDual(10,10);
        break;
               
        case '1':
            
            playText(0);
        break;
        case '2':
            playText(1);
        break;
        case '3':
            playText(2);
        break;
        case '4':
            playText(3);
        break;
        case '5':
            playText(4);
            break;
        case '6':
            playText(5);
            break;
        case '7':
            playText(6);
            break;
        case '8':
            playText(7);
            break;
        case '9':
            playText(8);
            break;
        case '0':
            playText(9);
            break;
        
        case 'r':
            outgoingRPIStatus.isPlaying = false;
            outgoingRPIStatus.isVideo = false;
            sendStatus();
        break;
            
        case 'd':
            outgoingRPIStatus.isDebugging = !outgoingRPIStatus.isDebugging;
            sendStatus();
            break;
    }
    
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

void ofApp::setupFromJSON(){
    ofFile file(rootPath + "antiman_settings_RPI.json");
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
        
        rpiOSCReceivePort = antimanSettings[0]["rpiOSCReceivePort"].get<int>();
        ofLogNotice("Antiman::Read settings file") << "rpiOSCReceivePort is set to " + ofToString(rpiOSCReceivePort)<< endl;
        
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
    if(oscReceiver.setup(rpiOSCReceivePort)){
        ofLogNotice("Antiman::Setup OSC") << "OSC receiver listening on port: " + ofToString(rpiOSCReceivePort)<< endl;

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
        skipTimeStart = ofGetElapsedTimeMillis();
        ofLogNotice("Antiman::Load movie") << "Loading movie" << videoFiles[outgoingRPIStatus.currentVideo].path();
        videoPlayer.loadMovie(videoFiles[videoIndex].path());
        skipTimeEnd = ofGetElapsedTimeMillis();
        amountSkipped = skipTimeEnd-skipTimeStart;
        totalAmountSkipped+=amountSkipped;
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

void ofApp::setupText(){
    if(displayFontLarge.load(rootPath + displayFontLargeName, displayFontLargeSize, true, false, false)){
        displayFontLarge.setLineHeight(1000);
        ofLogNotice("Antiman::setup text") << "displayFontLarge loaded with: " + displayFontLargeName << endl;
    }
    else{
        ofLogNotice("Antiman::setup text") << "displayFontLarge not loaded" << endl;
    }
    
    if(displayFontSmall.load(rootPath + displayFontSmallName, displayFontSmallSize, true, false, false)){
        ofLogNotice("Antiman::setup text") << "displayFontSmall loaded with: " + displayFontSmallName << endl;
    }
    else{
        ofLogNotice("Antiman::setup text") << "displayFontLarge not loaded" << endl;
    }
    
    drawScrollX =  ofGetWidth();
}

void ofApp::playText(int textIndex){
    if (textIndex > -1 && textIndex < 10) {
        ofLogNotice("Antiman::Play  text") << "playing text from index: " + ofToString(textIndex) << endl;
        outgoingRPIStatus.currentText= textIndex;
        outgoingRPIStatus.isPlaying = true;
        outgoingRPIStatus.isVideo = false;
        drawScrollX =  ofGetWidth();
        sendStatus();
    }
    else{
        ofLogNotice("Antiman::Play  text") << "Requested text index is out of range, must be between 0 and 9" << endl;

    }
}

void ofApp::setupVideos(){
    ofLogNotice("Antiman::setupVideos") << "scanning directory for videos" << endl;
    string directoryPath = rootPath + "videos/";
    ofDirectory currentVideoDirectory(directoryPath);
    if (currentVideoDirectory.exists())
    {
        currentVideoDirectory.listDir();
        currentVideoDirectory.sort();
        for(int i = 0; i < currentVideoDirectory.getFiles().size(); i++){
            if (currentVideoDirectory.getFiles()[i].getExtension() == "mov" || currentVideoDirectory.getFiles()[i].getExtension() == "mp4" ) {
                videoFiles.push_back(currentVideoDirectory.getFiles()[i]);
                ofLogNotice("Antiman::setupVideos") << "Found file: " + videoFiles[i].path() << endl;
            }
        }
  
        
        if (videoFiles.size()>0)
        {
            outgoingRPIStatus.currentVideo = 0;
            outgoingRPIStatus.totalVideos = videoFiles.size();
//            omxPlayerSettings.useHDMIForAudio = false;	//default true
//            omxPlayerSettings.enableLooping = false;		//default true
//            omxPlayerSettings.enableTexture = true;		//default true
//            omxPlayerSettings.listener = this;			//this app extends ofxOMXPlayerListener so it will receive events ;
//            videoPlayer.setup(omxPlayerSettings);
            outgoingRPIStatus.isVideo = true;
            outgoingRPIStatus.isPlaying = false;
        }
    }else
    {
        ofLogNotice("Antiman::setupVideos") << "No videos found in directory" << endl;
    }
}

