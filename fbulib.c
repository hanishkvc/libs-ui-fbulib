/*
 * fbulib.c - A library of useful fb functions
 * v20160509_1620
 * HKVC, GPL, Jan2000+
 *
 */
#define _XOPEN_SOURCE 1000
#define _XOPEN_SOURCE_EXTENDED 1000
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/select.h>
#include <linux/input.h>

#include "f9x18.h"

#include "dprints.h"

#define FBERROR(x) printf("ERROR:fbulib: in line %s:%d: %s\n", __FUNCTION__, __LINE__, strerror(errno));

#define FBCTL(fd, cmd, arg)			\
	if(ioctl(fd, cmd, arg) == -1) {	\
		FBERROR("ioctl failed");	\
		exit(1); }

#define DISPLAY_OPAQUE 0xFF000000;
#define DISPLAY_TRANSPARENT 0x00000000;

int gFBLineLen = 1024;
int gFBScreenHeight = 600;
int gFBNumScreens = -1;

struct fb_var_screeninfo gFBVar;
struct fb_fix_screeninfo gFBFix;

void sync_fb(int fFB)
{
	if(fFB == -1) return;

	if(fsync(fFB) != 0) { /* Have to check this further later */
		//perror("ERR:fbulib: During fsync fFB:");
		sync();
	}
}

void draw_rect(int fFB, int x, int y, int xlen, int ylen, unsigned long color)
{
	int iS;
	int xC,yC;

	if(fFB == -1) return;

	for(iS = 0; iS < gFBNumScreens; iS++) {
	y = y+gFBScreenHeight*iS;

	dprintf("INFO:fbulib:draw_rect: x(%d), y(%d), xlen(%d), ylen(%d), color(0x%lx)\n", x, y, xlen, ylen, color);

	for ( yC=y; yC < (y+ylen); yC++) {
		//dprintf("INFO:fbulib:draw_rect:D2: x(%d), y(%d), xlen(%d), ylen(%d), color(0x%lx)\n", x, yC, xlen, ylen, color);
		lseek(fFB, (yC*gFBLineLen+(x*4)), SEEK_SET);
		for(xC=x; xC<(x+xlen); xC++)
			write(fFB, &color, 4);
	}
	sync_fb(fFB);

	}
}

void draw_point(int fFB, int x, int y, unsigned long color)
{
	int iS;

	if(fFB == -1) return;

	for(iS = 0; iS < gFBNumScreens; iS++) {
	y = y+gFBScreenHeight*iS;

	lseek(fFB, (y*gFBLineLen+(x*4)), SEEK_SET);
	write(fFB, &color, 4);
#ifdef FORCE_SYNC
	sync_fb(fFB);
#endif
	}
}


void draw_text(int fFB, int x, int y, char *sText, unsigned long color)
{
	int xC;
	int *lFontBase;
	int lFontLine;
	int i,j,k;
	int iMask;

	int sLen = strlen(sText);

	if(fFB == -1) return;

	dprintf("INFO:fbulib:draw_text: x(%d), y(%d), sText(%s), color(0x%lx)\n", x, y, sText, color);
	for(i = 0; i < sLen; i++) {
		lFontBase = FK_FontBitmapsArray[(int)sText[i]];
		if(lFontBase == NULL)
			lFontBase = FK_FontBitmapsArray[(int)'?'];
		xC = x+i*FK_FontXLen+2;
		for(j = 0; j < FK_FontYLen; j++) {
			lFontLine = lFontBase[j];
			iMask = 0x8000;
			for(k = 0; k < FK_FontXLen; k++) {
				if(iMask&lFontLine)			
					draw_point(fFB, xC+k, y+j, color);
				iMask >>= 1;
			}
		}
	}
	sync_fb(fFB);
}

void draw_text_ext(int fFB, int x, int y, char *sText, unsigned long color, unsigned long backcolor)
{
	if(fFB == -1) return;
	draw_rect(fFB, x-2, y-2, x+4+((strlen(sText)+1)*FK_FontXLen), FK_FontYLen+4, backcolor);
	draw_text(fFB, x, y, sText, color);
}

void info_fb()
{
    if (gFBNumScreens == -1) {
        printf("WARN:fbulib: Call open_fb before calling info_fb\n");
        return;
    }
    printf("INFO:fbulib: res %d,%d virtual %d,%d, line_len %d\n",
        gFBVar.xres, gFBVar.yres, gFBVar.xres_virtual, gFBVar.yres_virtual, gFBFix.line_length);
    printf("INFO:fbulib: dim %d,%d\n", gFBVar.width, gFBVar.height);
    printf("INFO:fbulib: bits_per_pixel %d\n", gFBVar.bits_per_pixel);

    printf("INFO:fbulib: NumOfScreens = %d\n",gFBNumScreens);
    if(gFBVar.bits_per_pixel != 32) {
	printf("ERR:fbulib: bits_per_pixel not supported\n");
    }
}

int open_fb()
{
    int fd;

    fd = open("/dev/graphics/fb0", O_RDWR);
    if(fd == -1) {
        fprintf(stderr,"WARN:fbulib: NO graphics/fb0 trying fb0\n");
        fd = open("/dev/fb0", O_RDWR);
        if(fd == -1) {
            fprintf(stderr,"WARN:fbulib: NO fb0, disabling gui\n");
            return -1;
        }
        fprintf(stderr,"INFO:fbulib: gui enabled\n");
    }
    FBCTL(fd, FBIOGET_VSCREENINFO, &gFBVar);
    FBCTL(fd, FBIOGET_FSCREENINFO, &gFBFix);

    gFBLineLen = gFBFix.line_length;
    gFBScreenHeight = gFBVar.yres;
    gFBNumScreens = gFBVar.yres_virtual/gFBVar.yres;

#ifdef DEBUG_INFO
    info_fb();
#endif
    return fd;
}


#define COLOR_TITLE 0xFF0000FF
#define COLOR_ACTIVE 0xFFFF0000
#define COLOR_INACTIVE 0xFF0000FF
#define BCOLOR_ACTIVE 0xFF808000
#define BCOLOR_INACTIVE 0xFF000000

#ifdef PRG_TEST

#define S_X 10
#define S_Y 10
#define S_XLEN 800
#define S_YLEN 500

#define DL_X 300
#define DL_Y 120

#define T_X 240
#define T_Y 60

#define L_Y 20

#define STITLE "FBULib By HKVC" 

int main(int argc, char **argv)
{
	int fFB;

	fFB=open_fb();
	if((argc > 1) && (argv[1][0] == '1'))
		draw_rect(fFB, 0, 0, gFBVar.xres, gFBVar.yres, BCOLOR_INACTIVE);

	if(argc < 3)
		exit(0);

	int yPos = strtoul(argv[2],NULL,0) * L_Y;

	if((yPos < 0) || (yPos > (gFBVar.yres-L_Y)))
		yPos = L_Y;
	int xPos = L_Y;

	if(argc < 4)
		exit(0);

	if(argc < 5)
		draw_text(fFB, xPos, yPos, argv[3], COLOR_TITLE);
	else
		draw_text_ext(fFB, xPos, yPos, argv[3], COLOR_TITLE, BCOLOR_INACTIVE);

	close(fFB);
	return 0;
}

#endif

