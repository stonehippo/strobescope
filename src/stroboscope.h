void lcdClear();
void lcdSetCursor(byte position);
void pulse(int duration);
int strobePulseWidth(int hertz);
int strobePulseDuration(int pulseWidth);
void setStrobeParams();
void setStrobeHz(int hertz);
void updateEncoder();