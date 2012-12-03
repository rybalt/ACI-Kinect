#include "testApp.h"
#include "Performer.h"

//--------------------------------------------------------------
void testApp::setup() {

	isLive			= true;
	isTracking		= false;
	isTrackingHands	= true;
	isFiltering		= false;
	isRecording		= false;
	isCloud			= false;
	isCPBkgnd		= true;
	isMasking		= true;

	nearThreshold = 500;
	farThreshold  = 1000;

	filterFactor = 0.1f;

	setupRecording();

	ofBackground(255, 255, 255);
	
	//serial.setup("/dev/tty.usbmodem1421", 9600);

}

void testApp::setupRecording(string _filename) {

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_BLINK_GREEN); // turn off the led just for yacks (or for live installation/performances ;-)
#endif

	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);

	recordUser.setup(&recordContext);
	recordUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	recordUser.setUseMaskPixels(isMasking);
	recordUser.setUseCloudPoints(isCloud);
	recordUser.setMaxNumberOfUsers(1);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)

	recordHandTracker.setup(&recordContext, 4);
	recordHandTracker.setSmoothing(filterFactor);		// built in openni hand track smoothing...
	recordHandTracker.setFilterFactors(filterFactor);	// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with

	recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();

	oniRecorder.setup(&recordContext, ONI_STREAMING);
	//oniRecorder.setup(&recordContext, ONI_CYCLIC, 60);
	//read the warning in ofxOpenNIRecorder about memory usage with ONI_CYCLIC recording!!!

}

void testApp::setupPlayback(string _filename) {

	playContext.shutdown();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);

	playUser.setup(&playContext);
	playUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	playUser.setUseMaskPixels(isMasking);
	playUser.setUseCloudPoints(isCloud);

	playHandTracker.setup(&playContext, 4);
	playHandTracker.setSmoothing(filterFactor);			// built in openni hand track smoothing...
	playHandTracker.setFilterFactors(filterFactor);		// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with

	playContext.toggleRegisterViewport();
	playContext.toggleMirror();

}

//--------------------------------------------------------------
void testApp::update(){

#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.update();
#endif

	if (isLive) {

		// update all nodes
		recordContext.update();
		recordDepth.update();
		recordImage.update();

		// demo getting depth pixels directly from depth gen
		depthRangeMask.setFromPixels(recordDepth.getDepthPixels(nearThreshold, farThreshold),
									 recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		if (isTracking) recordUser.update();
		if (isRecording) oniRecorder.update();

		// demo getting pixels from user gen
		if (isTracking && isMasking) {
			allUserMasks.setFromPixels(recordUser.getUserPixels(), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user1Mask.setFromPixels(recordUser.getUserPixels(1), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user2Mask.setFromPixels(recordUser.getUserPixels(2), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
		}

	} else {

		// update all nodes
		playContext.update();
		playDepth.update();
		playImage.update();

		// demo getting depth pixels directly from depth gen
		depthRangeMask.setFromPixels(playDepth.getDepthPixels(nearThreshold, farThreshold),
									 playDepth.getWidth(), playDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		if (isTracking) playUser.update();

		// demo getting pixels from user gen
		if (isTracking && isMasking) {
			allUserMasks.setFromPixels(playUser.getUserPixels(), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user1Mask.setFromPixels(playUser.getUserPixels(1), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user2Mask.setFromPixels(playUser.getUserPixels(2), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	static bool handPlayed = false;
	
	//std::cout<<"NUMBER OF TRACKED USERS: "<<recordUser.getNumberOfTrackedUsers()<<'\n';
	if(recordUser.getNumberOfTrackedUsers() > 0){
		
		ofxTrackedUser *user = recordUser.getTrackedUser(1);
		if(user->skeletonCalibrated && user->skeletonTracking){
			performer.update(user,serial);
			
			XnPoint3D lh = performer.getPosition(LEFT_HAND);
			XnPoint3D rh = performer.getPosition(RIGHT_HAND);
			
			
			double t = performer.getDistance(LEFT_HAND,RIGHT_HAND) / 320.0;
			(t>1)?(t=1):(t);
			
			double s = performer.getDistance(LEFT_FOOT,RIGHT_FOOT) / 310.0;
			(s>1)?(s=1):(s);
			
			ofSetColor(255*t, 255*s, 255*(1-t));
			//ofRect(0, 0, 960, 768);
			ofCircle((lh.X + rh.X)/2.0, (lh.Y+rh.Y)/2, performer.getDistance(LEFT_HAND, RIGHT_HAND)/2);
			
			if((performer.getDistance(LEFT_HAND, RIGHT_HAND) < 50)){
				//std::cout<<"PLAYING: LEFT HAND RIGHT HAND "<<performer.getDistance(LEFT_HAND, RIGHT_HAND)<<"\n";
				velSound.playEffect(performer.getSpeed(LEFT_HAND, RIGHT_HAND), 0, 1);
			}
			else{
				velSound.stopEffect(0, 1);
			}
			

			if((performer.getDistance(LEFT_FOOT, RIGHT_FOOT)<50)){
				//std::cout<<"PLAYING: LEFT FOOT RIGHT FOOT "<<performer.getDistance(LEFT_FOOT, RIGHT_FOOT)<<"\n";
				velSound.playEffect(performer.getSpeed(LEFT_FOOT, RIGHT_FOOT), 2, 3);
			}
			else{
				velSound.stopEffect(2, 3);
			}
	
			//std::cout<<"DISTANCE BETWEEN LEFT FOOT AND LEFT HAND: "<<performer.getDistance(LEFT_FOOT, LEFT_HAND)<<'\n';
			if((performer.getDistance(LEFT_HAND,LEFT_FOOT)<75)){
				std::cout<<"PLAYING: LEFT HAND LEFT FOOT "<<performer.getDistance(LEFT_HAND, LEFT_FOOT)<<"\n";
				velSound.playEffect(performer.getSpeed(LEFT_HAND, LEFT_FOOT), 0, 2);
			}
			else{
				velSound.stopEffect(0, 2);
			}
		
			if((performer.getDistance(RIGHT_HAND, RIGHT_FOOT)<75)){
				std::cout<<"PLAYING: RIGHT HAND RIGHT FOOT " <<performer.getDistance(RIGHT_HAND, RIGHT_FOOT)<<"\n";
				velSound.playEffect(performer.getSpeed(RIGHT_HAND,RIGHT_FOOT), 1, 3);
			}
			else{
				velSound.stopEffect(1, 3);
			}

			
			performer.drawDebug();

		}
		else{
			ofSetColor(0, 255, 0);
			ofRect(0, 0, 960, 768);
		}
		
		
	}
	else{
		ofSetColor(255, 0, 0);
		ofRect(0, 0, 960, 768);
	}

	
	
	
	
	glPushMatrix();
		
	recordDepth.draw(160,360,160,120);
	recordImage.draw(0,360,160,120);
	recordUser.draw();
	
	glPopMatrix();
	
	//performer.sendData(serial);

}

void testApp:: drawMasks() {
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	allUserMasks.draw(640, 0, 640, 480);
	glDisable(GL_BLEND);
    glPopMatrix();
	user1Mask.draw(320, 480, 320, 240);
	user2Mask.draw(640, 480, 320, 240);
	
}

void testApp::drawPointCloud(ofxUserGenerator * user_generator, int userID) {

	glPushMatrix();

	int w = user_generator->getWidth();
	int h = user_generator->getHeight();

	glTranslatef(w, h/2, -500);
	ofRotateY(pointCloudRotationY);

	glBegin(GL_POINTS);

	int step = 1;

	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
			if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
			ofColor color = user_generator->getWorldColorAt(x,y, userID);
			glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
			glVertex3f(pos.x, pos.y, pos.z);
		}
	}

	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

	float smooth;

	static char i = 0;
	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif
		case 's':
		case 'S':
			if (isRecording) {
				oniRecorder.stopRecord();
				isRecording = false;
				break;
			} else {
				oniRecorder.startRecord(generateFileName());
				isRecording = true;
				break;
			}
			break;
		case 'p':
		case 'P':
			if (oniRecorder.getCurrentFileName() != "" && !isRecording && isLive) {
				setupPlayback(oniRecorder.getCurrentFileName());
				isLive = false;
			} else {
				isLive = true;
			}
			break;
		case 't':
		case 'T':
			std::cout << "Tracking toggle to: " << !isTracking << '\n';
			isTracking = !isTracking;
			break;
		case 'h':
		case 'H':
			/*
			isTrackingHands = !isTrackingHands;
			if(isLive) recordHandTracker.toggleTrackHands();
			if(!isLive) playHandTracker.toggleTrackHands();
			 */
			serial.writeByte(1);
			serial.flush();
			
			break;
		case 'j':
			serial.writeByte(2);
			serial.flush();
			break;
		case 'k':
			serial.writeByte(3);
			serial.flush();
			break;
		case 'l':
			serial.writeByte(4);
			serial.flush();
			break;
		case 'f':
		case 'F':
			isFiltering = !isFiltering;
			recordHandTracker.isFiltering = isFiltering;
			playHandTracker.isFiltering = isFiltering;
			break;
		case 'm':
		case 'M':
			isMasking = !isMasking;
			recordUser.setUseMaskPixels(isMasking);
			playUser.setUseMaskPixels(isMasking);
			break;
		case 'c':
		case 'C':
			isCloud = !isCloud;
			recordUser.setUseCloudPoints(isCloud);
			playUser.setUseCloudPoints(isCloud);
			break;
		case 'z':
		case 'Z':
			unsigned char lol[2];
			lol[0] = 255;
			lol[1] = 127;
			serial.writeBytes(&lol[0],2);
			serial.flush();
			i++;
			i = i % 256;
			break;
		case 'u':
			performer.sendData(serial);
			break;
		case 'b':
		case 'B':
			isCPBkgnd = !isCPBkgnd;
			break;
		case '9':
		case '(':
			smooth = recordUser.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				recordUser.setSmoothing(smooth - 0.1f);
				playUser.setSmoothing(smooth - 0.1f);
			}
			break;
		case '0':
		case ')':
			smooth = recordUser.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				recordUser.setSmoothing(smooth + 0.1f);
				playUser.setSmoothing(smooth + 0.1f);
			}
			break;
		case '[':
		//case '{':
			if (filterFactor - 0.1f > 0.0f) {
				filterFactor = filterFactor - 0.1f;
				recordHandTracker.setFilterFactors(filterFactor);
				if (oniRecorder.getCurrentFileName() != "") playHandTracker.setFilterFactors(filterFactor);
			}
			break;
		case ']':
		//case '}':
			if (filterFactor + 0.1f <= 1.0f) {
				filterFactor = filterFactor + 0.1f;
				recordHandTracker.setFilterFactors(filterFactor);
				if (oniRecorder.getCurrentFileName() != "") playHandTracker.setFilterFactors(filterFactor);
			}
			break;
		case ';':
		case ':':
			smooth = recordHandTracker.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				recordHandTracker.setSmoothing(smooth -  0.1f);
				playHandTracker.setSmoothing(smooth -  0.1f);
			}
			break;
		case '\'':
		case '\"':
			smooth = recordHandTracker.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				recordHandTracker.setSmoothing(smooth +  0.1f);
				playHandTracker.setSmoothing(smooth +  0.1f);
			}
			break;
		case '>':
		case '.':
			farThreshold += 50;
			if (farThreshold > recordDepth.getMaxDepth()) farThreshold = recordDepth.getMaxDepth();
			break;
		case '<':
		case ',':
			farThreshold -= 50;
			if (farThreshold < 0) farThreshold = 0;
			break;

		case '+':
		case '=':
			nearThreshold += 50;
			if (nearThreshold > recordDepth.getMaxDepth()) nearThreshold = recordDepth.getMaxDepth();
			break;

		case '-':
		case '_':
			nearThreshold -= 50;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'r':
			recordContext.toggleRegisterViewport();
			break;
		default:
			break;
	}
}

string testApp::generateFileName() {

	string _root = "kinectRecord";

	string _timestamp = ofToString(ofGetDay()) +
	ofToString(ofGetMonth()) +
	ofToString(ofGetYear()) +
	ofToString(ofGetHours()) +
	ofToString(ofGetMinutes()) +
	ofToString(ofGetSeconds());

	string _filename = (_root + _timestamp + ".oni");

	return _filename;

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

	if (isCloud) pointCloudRotationY = x;

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	std::cout<<"RESIZING WINDOW\n";
	glViewport(0, 0, 640, 480);
}

