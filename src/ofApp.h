#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "TerminalListener.h"
#include "ofxXmlBasedProjectSettings.h"


class ofApp : public ofBaseApp, public ofxOMXPlayerListener, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
	
	
		void keyPressed(int key);
		ofxOMXPlayer omxPlayer;
	
		void onVideoEnd(ofxOMXPlayer* player);
        void onVideoLoop(ofxOMXPlayer* player);

		
		vector<ofFile> files;
		int videoCounter;
	
		void onCharacterReceived(KeyListenerEventData& e);
		TerminalListener consoleListener;
		ofxOMXPlayerSettings playerSettings;
	
		void loadNextMovie();
		bool debug;
		
		ofxXmlBasedProjectSettings settings;
	
};

