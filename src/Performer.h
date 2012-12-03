//
//  Performer.h
//  openNiSample007
//
//  Created by Ryan B on 10/3/12.
//
//

#include <map>
#include "ofxOpenNI.h"

#ifndef openNiSample007_Performer_h
#define openNiSample007_Performer_h

enum TRACKED_PARTS {HEAD, LEFT_FOOT, RIGHT_FOOT, LEFT_HAND, RIGHT_HAND};

struct gridPosition{
	unsigned char i;
	unsigned char j;
};

class Performer{
	
private:
	std::map<TRACKED_PARTS,XnPoint3D> prev_frame;
	std::map<TRACKED_PARTS,XnPoint3D> curr_frame;
	std::map<TRACKED_PARTS,gridPosition> grid;
	
public:
	double getDistance(TRACKED_PARTS p1, TRACKED_PARTS p2){
		double dx = curr_frame[p1].X - curr_frame[p2].X;
		double dy = curr_frame[p1].Y - curr_frame[p2].Y;
		return sqrt(dx*dx + dy*dy);
	}
	
	double getPrevDistance(TRACKED_PARTS p1, TRACKED_PARTS p2){
		double dx = prev_frame[p1].X - prev_frame[p2].X;
		double dy = prev_frame[p1].Y - prev_frame[p2].Y;
		return sqrt(dx*dx + dy*dy);
	}
	
	double getSpeed(TRACKED_PARTS p1, TRACKED_PARTS p2){
		double d1 = getDistance(p1, p2);
		double d2 = getPrevDistance(p1, p2);
		return fabs(d1-d2);
	}
	
	XnPoint3D getPosition(TRACKED_PARTS p){
		return curr_frame[p];
	}
	
	void update(ofxTrackedUser *user, ofSerial & serial){
		prev_frame = curr_frame;
		
		curr_frame[HEAD] = user->neck.position[0];
		
		curr_frame[LEFT_HAND] = user->left_lower_arm.position[1];
		curr_frame[RIGHT_HAND] = user->right_lower_arm.position[1];
		
		curr_frame[LEFT_FOOT] = user->left_lower_leg.position[1];
		curr_frame[RIGHT_FOOT] = user->right_lower_leg.position[1];
		
		unsigned char ui, uj, uri, urj;
		ui = curr_frame[LEFT_HAND].X/80;
		uj = curr_frame[LEFT_HAND].Y/60;
		
		uri = curr_frame[RIGHT_HAND].X/80;
		urj = curr_frame[RIGHT_HAND].Y/60;
		
		bool leftChanged = ((grid[LEFT_HAND].i != ui) && (grid[LEFT_HAND].j != uj));
		bool rightChanged = ((grid[RIGHT_HAND].i != uri) && (grid[RIGHT_HAND].j != urj));
		//if(leftChanged || rightChanged){
		if(1){
			grid[LEFT_HAND].i = ui;
			grid[LEFT_HAND].j = uj;
			grid[RIGHT_HAND].i = uri;
			grid[RIGHT_HAND].j = urj;
			//sendData(serial);
		}
	}
	
	void sendData(ofSerial & serial){
		unsigned char buf[4];
		buf[0] = grid[LEFT_HAND].i;
		buf[1] = grid[LEFT_HAND].j;
		buf[2] = grid[RIGHT_HAND].i;
		buf[3] = grid[RIGHT_HAND].j;
		serial.writeBytes(&buf[0],4);
	}
	
	void drawDebug(){
		ofSetColor(0, 0, 255);
		ofCircle(curr_frame[HEAD].X, curr_frame[HEAD].Y, 10);
		
		ofSetColor(255,0,255);
		ofCircle(curr_frame[LEFT_HAND].X,curr_frame[LEFT_HAND].Y,10.0);
		ofCircle(curr_frame[RIGHT_HAND].X,curr_frame[RIGHT_HAND].Y,10.0);
		ofLine(curr_frame[LEFT_HAND].X, curr_frame[LEFT_HAND].Y, curr_frame[RIGHT_HAND].X, curr_frame[RIGHT_HAND].Y);
		
		ofSetColor(255,255,0);
		ofCircle(curr_frame[LEFT_FOOT].X,curr_frame[LEFT_FOOT].Y,10.0);
		ofCircle(curr_frame[RIGHT_FOOT].X,curr_frame[RIGHT_FOOT].Y,10.0);
		ofLine(curr_frame[LEFT_FOOT].X, curr_frame[LEFT_FOOT].Y, curr_frame[RIGHT_FOOT].X, curr_frame[RIGHT_FOOT].Y);
		
		ofSetColor(255,0,0);
		ofLine(curr_frame[LEFT_HAND].X,curr_frame[LEFT_HAND].Y,curr_frame[LEFT_FOOT].X,curr_frame[LEFT_FOOT].Y);
		ofLine(curr_frame[RIGHT_HAND].X,curr_frame[RIGHT_HAND].Y,curr_frame[RIGHT_FOOT].X,curr_frame[RIGHT_FOOT].Y);
		
		ofSetColor(0, 0, 0);

		stringstream msg;
		msg << getDistance(LEFT_HAND,RIGHT_HAND);
		ofDrawBitmapString(msg.str(),(curr_frame[LEFT_HAND].X+curr_frame[RIGHT_HAND].X)/2,(curr_frame[LEFT_HAND].Y+curr_frame[RIGHT_HAND].Y)/2);
		
		msg.clear();
		msg  << getDistance(LEFT_FOOT, RIGHT_FOOT);
		ofDrawBitmapString(msg.str(),(curr_frame[LEFT_FOOT].X+curr_frame[RIGHT_FOOT].X)/2,(curr_frame[LEFT_FOOT].Y+curr_frame[RIGHT_FOOT].Y)/2);
		
		msg.clear();
		msg << getDistance(LEFT_HAND,LEFT_FOOT);
		ofDrawBitmapString(msg.str(),(curr_frame[LEFT_HAND].X+curr_frame[LEFT_FOOT].X)/2, (curr_frame[LEFT_FOOT].Y+curr_frame[LEFT_HAND].Y)/2);

		msg.clear();
		msg << getDistance(RIGHT_HAND,RIGHT_FOOT);
		ofDrawBitmapString(msg.str(),(curr_frame[RIGHT_HAND].X+curr_frame[RIGHT_FOOT].X)/2, (curr_frame[RIGHT_FOOT].Y+curr_frame[RIGHT_HAND].Y)/2);
		
		//draw the marionette lines
		ofLine(curr_frame[LEFT_HAND].X,curr_frame[LEFT_HAND].Y, 320, -1000);

	}

};

#endif
