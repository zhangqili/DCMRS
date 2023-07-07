#include "decompressed.h"



void yuyv_to_rgb(uint8_t *yuyvdata,  uint8_t *rgbdata, int w, int h)
{
	//码流Y0 U0 Y1 V1 Y2 U2 Y3 V3 --》YUYV像素[Y0 U0 V1] [Y1 U0 V1] [Y2 U2 V3] [Y3 U2 V3]--》RGB像素
	int r1, g1, b1; 
	int r2, g2, b2;
	for(int i=0; i<w*h/2; i++)
	{
	    char data[4];
	    memcpy(data, yuyvdata+i*4, 4);
	    unsigned char Y0=data[0];
	    unsigned char U0=data[1];
	    unsigned char Y1=data[2];
	    unsigned char V1=data[3]; 
		//Y0U0Y1V1  -->[Y0 U0 V1] [Y1 U0 V1]
	    r1 = Y0+1.4075*(V1-128); if(r1>255)r1=255; if(r1<0)r1=0;
	    g1 =Y0- 0.3455 * (U0-128) - 0.7169*(V1-128); if(g1>255)g1=255; if(g1<0)g1=0;
	    b1 = Y0 + 1.779 * (U0-128);  if(b1>255)b1=255; if(b1<0)b1=0;
	 
	    r2 = Y1+1.4075*(V1-128);if(r2>255)r2=255; if(r2<0)r2=0;
	    g2 = Y1- 0.3455 * (U0-128) - 0.7169*(V1-128); if(g2>255)g2=255; if(g2<0)g2=0;
	    b2 = Y1 + 1.779 * (U0-128);  if(b2>255)b2=255; if(b2<0)b2=0;
	    
	    // rgbdata[i*6+0]=r1;
	    // rgbdata[i*6+1]=g1;
	    // rgbdata[i*6+2]=b1;
	    // rgbdata[i*6+3]=r2;
	    // rgbdata[i*6+4]=g2;
	    // rgbdata[i*6+5]=b2;
        rgbdata[i*4+0]=( ((g1 & 0x1C) << 3) | ( b1 >> 3) );
	    rgbdata[i*4+1]=( (r1 & 0xF8) | ( g1 >> 5) );
        rgbdata[i*4+2]=( ((g2 & 0x1C) << 3) | ( b2 >> 3) );
	    rgbdata[i*4+3]=( (r2 & 0xF8) | ( g2 >> 5) );

	}
}

