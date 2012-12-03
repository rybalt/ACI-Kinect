#include "velocitySound.h"

//defines for body parts
#define LEFT_HAND 0
#define RIGHT_HAND 1
#define LEFT_FOOT 2
#define RIGHT_FOOT 3
#define HEAD 4

void velocitySound::playEffect(double vel, int a, int b)
{
	
	if(vel==0)
		return;

	//std::cout<<"INCOMING VELOCITY: "<<vel<<'\n';
	vel /= 30.0;
	static double prev_vel = -1.0;
    if((a==LEFT_HAND || a==RIGHT_HAND) && (b==LEFT_HAND || b==RIGHT_HAND))
    {
		if(handPlaying){
			return;
		}
		std::cout<<"SET VOLUME TO: "<<vel<<"\n";
		hand.setVolume(MIN(vel, 1));
		hand.play();
		handPlaying = true;
    }
    else if((a==LEFT_FOOT || a==RIGHT_FOOT) && (b==LEFT_FOOT || b==RIGHT_FOOT))
    {
		
		if(footPlaying){
			return;
		}

		footPlaying = true;
        foot.setVolume(MIN(vel, 1));
        foot.play();
    }
    else if((a==LEFT_HAND || a==LEFT_FOOT) && (b==LEFT_HAND || b==LEFT_FOOT))
    {
		if(handFootLPlaying)
			return;
		
		handFootLPlaying = true;
        handfoot1.setVolume(MIN(vel, 1));
        handfoot1.play();
    }
    else if((a==RIGHT_HAND || a==RIGHT_FOOT) && (b==RIGHT_HAND || b==RIGHT_FOOT))
    {
		if(handFootRPlaying)
			return;
		
		handFootRPlaying = true;
        handfootr.setVolume(MIN(vel, 1));
        handfootr.play();
    }
    else
    {
        cout << "Those don't match. What are you smoking???" << endl;
    }
}

void velocitySound::stopEffect(int a, int b){	
    if((a==LEFT_HAND || a==RIGHT_HAND) && (b==LEFT_HAND || b==RIGHT_HAND))
    {
		if(!handPlaying)
			return;
		
		std::cout<<"STOPPING PLAYBACK\n";
        hand.stop();
		handPlaying = false;
    }
    else if((a==LEFT_FOOT || a==RIGHT_FOOT) && (b==LEFT_FOOT || b==RIGHT_FOOT))
    {
		if(!footPlaying)
			return;
		
		footPlaying = false;
        foot.stop();
    }
    else if((a==LEFT_HAND || a==LEFT_FOOT) && (b==LEFT_HAND || b==LEFT_FOOT))
    {
		if(!handFootLPlaying)
			return;
		
		handFootLPlaying = false;
        handfoot1.stop();
    }
    else if((a==RIGHT_HAND || a==RIGHT_FOOT) && (b==RIGHT_HAND || b==RIGHT_FOOT))
    {
		
		if(!handFootRPlaying)
			return;
		
		handFootRPlaying = false;
        handfootr.stop();
    }
    else
    {
        cout << "Those don't match. What are you smoking???" << endl;
    }
}

