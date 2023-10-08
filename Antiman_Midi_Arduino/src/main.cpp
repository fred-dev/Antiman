#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    //settings.setGLVersion(4, 1);
    settings.setSize(1280, 1024);
    settings.decorated = false;
    settings.windowMode = OF_WINDOW;
    
    ofCreateWindow(settings);
    return ofRunApp(std::make_shared<ofApp>());

}
