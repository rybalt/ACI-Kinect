#include "ofMain.h"

#ifndef VELOCITYSOUND_H
#define VELOCITYSOUND_H


class velocitySound
{
private:
    ofSoundPlayer hand;
    ofSoundPlayer foot;
    ofSoundPlayer handfoot1;
    ofSoundPlayer handfootr;
	
	bool handPlaying;
	bool footPlaying;
	bool handFootLPlaying;
	bool handFootRPlaying;
	
public:
	void playEffect(double vel, int a, int b);
	void stopEffect(int a, int b);
	
public:
	velocitySound(){
		handPlaying = footPlaying = handFootLPlaying = handFootRPlaying = false;
		hand.loadSound("sounds/gong.mp3");
		foot.loadSound("sounds/record_scratch.wav");
		handfoot1.loadSound("sounds/wilhelm_scream.wav");
		handfootr.loadSound("sounds/santa_hohoho.mp3");
	}
};

#endif // VELOCITYSOUND_H
