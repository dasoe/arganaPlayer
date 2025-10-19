#include "ofMain.h"
#include "ofApp.h"

int main()
{
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);
	ofRunApp( new ofApp());
}
