#pragma once
#include <graphics.h>
#include<string>
void DrawButton(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2, std::string s,int flag);
bool IsButtonClicked(int x1, int y1, int x2, int y2, int x3, int y3);
bool IsMouseOverButton(int x1, int y1, int x2, int y2, int x3, int y3);
