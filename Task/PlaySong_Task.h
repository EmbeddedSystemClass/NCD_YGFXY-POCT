#ifndef __PLAYSONG_T_H__
#define __PLAYSONG_T_H__

#include	"Define.h"
#include	"DateTime.h"

char StartvPlaySongTask(void);
unsigned char AddNumOfSongToList(unsigned char num, unsigned char mode);
void stopPlay(void);
unsigned char getPlayStatus(void);
#endif




