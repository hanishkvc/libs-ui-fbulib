#ifndef __FBULIB__
#define __FBULIB__
/*
 * fbulib.h - A library of useful fb functions
 * v20160509_1621
 * HanishKVC, GPL, 2000+
 *
 */

#include <linux/fb.h>

extern struct fb_var_screeninfo gFBVar;
extern struct fb_fix_screeninfo gFBFix;

void sync_fb(int fFB);
void draw_rect(int fFB, int x, int y, int xlen, int ylen, unsigned long color);
void draw_point(int fFB, int x, int y, unsigned long color);
void draw_text(int fFB, int x, int y, char *sText, unsigned long color);
void draw_text_ext(int fFB, int x, int y, char *sText, unsigned long color, unsigned long backcolor);
void info_fb();
int open_fb();

#endif
