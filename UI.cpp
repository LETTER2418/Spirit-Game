#include "UI.h"
void DrawButton(int x1, int y1, int w1, int h1, int x2, int y2,int w2,int h2, std::string s,int flag) {
	setfillcolor(flag?RED:LIGHTRED);  
	//setcolor(RED);//设置前景色
	fillrectangle(x1, y1, x1 + w1, y1 + h1);
	setbkmode(TRANSPARENT);
	settextcolor(WHITE); // 设置文字颜色为白色
	settextstyle(h2,w2, _T("华文行楷"));//modiy lpszface
	outtextxy(x2, y2, s.c_str());

}
bool IsButtonClicked(int x1, int y1,int x2,int y2,int x3,int y3)
{
	return (x1 >= x2 && x1 <= x3 && y1 >= y2 && y1 <= y3);
}
bool IsMouseOverButton(int x1, int y1, int x2, int y2, int x3, int y3)
{
	return (x1 >= x2 && x1 <= x3 && y1 >= y2 && y1 <= y3);
}
