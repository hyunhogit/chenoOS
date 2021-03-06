#include "apilib.h"

int rand(void);		/* 0~32767의 범위에서 난수를 발생 */

void HariMain(void)
{
	char *buf;
	int win, i, x, y;
	api_initmalloc();
	buf = api_malloc(150 * 100);
	win = api_openwin(buf, 150, 100, -1, "stars2");
	api_boxfilwin(win + 1,  6, 26, 143, 93, 0 /* 검정 */);
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) +  6;
		y = (rand() %  67) + 26;
		api_point(win + 1, x, y, 3 /* 노랑 */);
	}
	api_refreshwin(win,  6, 26, 144, 94);
	api_end();
}
