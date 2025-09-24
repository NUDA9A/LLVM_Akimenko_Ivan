#ifndef SIM_H
#define SIM_H

#define SIM_X_SIZE 500
#define SIM_Y_SIZE 500

#ifndef __sim__
void simInit();
void app();
void simExit();
void simFlush();
void simPutPixel(int x, int y, int argb);
int simRand(void);
#endif // __sim__

#endif // SIM_H
