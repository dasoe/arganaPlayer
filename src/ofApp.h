#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "TerminalListener.h"
#include "ofxXmlBasedProjectSettings.h"
#include "wiringPi.h"

class ofApp : public ofBaseApp, public ofxOMXPlayerListener, public KeyListener{

	public:

		void setup();
		void getFiles();
		void update();
		void draw();
		bool isWeekday();
		bool isMenuTime();
	
		void keyPressed(int key);
		ofxOMXPlayer omxPlayer;
	
		void onVideoEnd(ofxOMXPlayer* player);
        void onVideoLoop(ofxOMXPlayer* player);

		
		vector<ofFile> files;
		int videoCounter;
	
		void onCharacterReceived(KeyListenerEventData& e);
		TerminalListener consoleListener;
		ofxOMXPlayerSettings playerSettings;
		
		int hour, menuStartHour, menuEndHour, lastWeekday;
	
		void loadNextMovie();
		bool debug, useDay, state, lastState, menuState, lastMenuState;
		int switchPin;
		
		ofxXmlBasedProjectSettings settings;
	
};

