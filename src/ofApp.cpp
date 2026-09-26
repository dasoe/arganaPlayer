#include "ofApp.h"

//This app is a demo of the ability to play multiple files with the Non-Texture Player
//It requires multiple video files to be in /home/pi/videos/current
//There is a bit of glitching while the files switch

//This also demonstrates the ofxOMXPlayerListener pattern available

//If your app extends ofxOMXPlayerListener you will receive an event when the video ends or loops


bool doLoadNextMovie = false;
string weekday[7] = { "so", "mo", "di", "mi", "do", "fr", "sa" };

void ofApp::onVideoEnd(ofxOMXPlayer* player)
{
    ofLog() << "onVideoEnd: " << player->isLoopingEnabled();
    doLoadNextMovie = true;
}

void ofApp::onVideoLoop(ofxOMXPlayer* player)
{
    
}


void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}



unsigned long long skipTimeStart=0;
unsigned long long skipTimeEnd=0;
unsigned long long amountSkipped =0;
unsigned long long totalAmountSkipped =0;
//--------------------------------------------------------------
void ofApp::setup()
{		
	settings.addBoolean("useDay", true);
	settings.addBoolean("debugOnStart", false);
	settings.addInt("switchPin", 10);
	settings.addInt("menuStartHour", 11);
	settings.addInt("menuEndHour", 15 );
	settings.addInt("lastWeekday", 5);

	settings.init("settings.xml", true);
	 
	menuStartHour = settings.getIntValue( "menuStartHour" );
	menuEndHour = settings.getIntValue( "menuEndHour" );

	lastWeekday = settings.getIntValue( "lastWeekday" );
	
	useDay = settings.getBooleanValue( "useDay" );
	debug = settings.getBooleanValue( "debugOnStart" );
	switchPin = settings.getIntValue( "switchPin" );
	    
	ofBackground(ofColor::black);
	consoleListener.setup(this);	
	
	 if (wiringPiSetup() == 1) {
		ofLogError("Probleme beim Initialisieren von wiringPi. Schalter funktioniert nicht!");
	 } else {
		 pinMode( switchPin, INPUT );
		 pullUpDnControl( switchPin, PUD_UP );
	 }
	 menuState = isMenuTime();

	 getFiles();
	 doLoadNextMovie = true;
}

void ofApp::getFiles() 
{
		// define directoryPath (we'll need it in any case, so definition not inside if/else)
		string directoryPath = ofToDataPath( "general", true);	
		if (menuState) { directoryPath = ofToDataPath( "general/menu", true); }
		if (state) {
			// no menu possible. It'S Feiertag
			// overwrite in case switch is switched (duh!)
			directoryPath = ofToDataPath( "feiertag", true);	
		} else {			
			// overwrite in case weekday shall be used and switch is NOT switched
			if (useDay) {	
				int weekdayIndex = ofGetWeekday();
				if (menuState) { 
					directoryPath = ofToDataPath( weekday[weekdayIndex]+"/menu", true);
				} else {
					//this will let us just grab a video without recompiling
					directoryPath = ofToDataPath( weekday[weekdayIndex], true);
				}
			} 
		}
		
		ofDirectory currentVideoDirectory(directoryPath);
		if (currentVideoDirectory.exists()) 
		{
			currentVideoDirectory.allowExt("mp4");
			currentVideoDirectory.allowExt("m4v");
			currentVideoDirectory.allowExt("avi");
			currentVideoDirectory.allowExt("mkv");
			currentVideoDirectory.allowExt("mov");
			currentVideoDirectory.allowExt("ogv");
			currentVideoDirectory.allowExt("ogm");
			currentVideoDirectory.listDir();
			currentVideoDirectory.sort();
			files = currentVideoDirectory.getFiles();
			if (files.size()>0) 
			{
				videoCounter = 0;
				playerSettings.videoPath = files[videoCounter].path();
				playerSettings.useHDMIForAudio = true;	//default true
				playerSettings.enableLooping = false;		//default true
				playerSettings.enableTexture = true;		//default true
				playerSettings.listener = this;			//this app extends ofxOMXPlayerListener so it will receive events ;
				omxPlayer.setup(playerSettings);
			}		
		} else
		{
			if (useDay) {
				ofLogError() << "Folder " << directoryPath << " DOES NOT EXIST. (As you are using videos depending on weekday, you need one folder per day.)";
				ofExit();
			} else {
				ofLogError() << "General Folder (as you are not using videos depending on weekday): " << directoryPath << " DOES NOT EXIST.";
				ofExit();
			}
		}
		
		if (debug) {
			ofShowCursor();
		} else {
			ofHideCursor();
		}   
		

}

void ofApp::loadNextMovie()
{
	if ( digitalRead( switchPin ) == HIGH ) {
		state = true;
	} else {
		state = false;
	}
	menuState = isMenuTime();
	
	bool trigger = false;
	if ( lastState != state ) {
		lastState = state;
		trigger = true;
	}	
	if ( lastMenuState != menuState ) {
		lastMenuState = menuState;
		trigger = true;
	}
	if (trigger) {
		getFiles();
	}
	
	
	if(videoCounter+1<files.size())
	{
		videoCounter++;
	}else
	{
		videoCounter = 0;
	}
	skipTimeStart = ofGetElapsedTimeMillis();
    ofLog() << "LOADING MOVIE" << files[videoCounter].path();
	omxPlayer.loadMovie(files[videoCounter].path());
	skipTimeEnd = ofGetElapsedTimeMillis();
	amountSkipped = skipTimeEnd-skipTimeStart;
	totalAmountSkipped+=amountSkipped;
	doLoadNextMovie = false;
}

//--------------------------------------------------------------
bool ofApp::isWeekday(){
	bool isWeekday = false;
	if (!state) {
		if ( ofGetWeekday() > 0 && ofGetWeekday() <= lastWeekday ) {
			isWeekday = true;
		}		
	}		
	return isWeekday;
} 	

//--------------------------------------------------------------
bool ofApp::isMenuTime(){
	bool isMenuTime = false;
	if (!state && isWeekday()) {
		if ( ofGetHours() >= menuStartHour && ofGetHours() < menuEndHour ) {
			isMenuTime = true;
		}		
	}		
	return isMenuTime;
} 	

//--------------------------------------------------------------
void ofApp::update()
{
	if (doLoadNextMovie) 
	{
		ofLogVerbose(__func__) << "doing reload";
		
		if(omxPlayer.isTextureEnabled())
		{
			//clear the texture if you want
			//omxPlayer.getTextureReference().clear();
		}
		//with the texture based player this must be done here - especially if the videos are different resolutions
		loadNextMovie();
	}
		
	
}


//--------------------------------------------------------------
void ofApp::draw(){
	
	//ofBackgroundGradient(ofColor::red, ofColor::black, OF_GRADIENT_CIRCULAR);
	
	if(!omxPlayer.isTextureEnabled()) return;
	
	omxPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	if (debug) {
	
		//draw a smaller version in the lower right
		int scaledHeight = omxPlayer.getHeight()/4;
		int scaledWidth = omxPlayer.getWidth()/4;
		omxPlayer.draw(ofGetWidth()-scaledWidth, ofGetHeight()-scaledHeight, scaledWidth, scaledHeight);

		stringstream info;
		info << omxPlayer.getInfo() << endl;
		info << "MILLIS SKIPPED: " << amountSkipped << endl;
		info << "TOTAL MILLIS SKIPPED: " << totalAmountSkipped << endl;
		info << "CURRENT MOVIE: " << files[videoCounter].path() << endl;
		info << "PRESS n TO LOAD NEXT MOVIE"<< endl;
		info << "--------------------------"<< endl;
		info << "Menu state: "<< menuState << " (last: " << lastMenuState << ")" << endl;
		info << "Hour: "<< ofGetHours() << endl;
		info << "Day: "<< ofGetWeekday() << endl;
		info << "Is Weekday: "<<  isWeekday() << endl;
		ofDrawBitmapStringHighlight(info.str(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){

	ofLogVerbose(__func__) << "key: " << key;
	switch (key) 
	{
		case 'n':
		{
			doLoadNextMovie = true;
			break;
		}
		case 'd':
		{
			debug = !debug;
			if (debug) {
				ofShowCursor();
			} else {
				ofHideCursor();
			}
			break;
		}		
	}
}


