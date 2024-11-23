#include "SocketManager.h"
#include<windows.h>
#include<graphics.h>
#include <iostream>
#include<cstdlib>
#include<conio.h>
#include<chrono>
#include<time.h>
#include<vector>
#include<typeinfo> 

#define Pi 3.1415926535
using ll = long long;
int vis[3500][3500]; // vis[x][y]=1表示(i,j)位置有精灵

std::string getRandomName()
{
	std::string name;
	for (int i = 0; i < 3; i++)
	{
		name += 'a' + (rand() % 26);
	}
	return name;
}
class Spirit
{

	std::string name = "Empty";
	int x = 0, y = 0, health = 1000;
	bool aliveState = true;
	static int number, alivenumber;
	double r = 1;//精灵半径
	COLORREF color;
public:
	Spirit()
	{
		number++;
		alivenumber++;
		name = getRandomName();
		x = rand() % 201;
		y = rand() % 201;
		vis[x][y] = 1;
	}

	Spirit(std::string name_, int x_, int y_) : name(name_), x(x_), y(y_)
	{
		number++;
		alivenumber++;
		vis[x][y] = 1;
	}

	Spirit(int x_, int y_, double r_) :x(x_), y(y_), r(r_) {}

	Spirit(std::string name_, int x_, int y_, double r_, COLORREF color_) :name(name_), x(x_), y(y_), r(r_), color(color_) {}

	void takeDamage(int damage)
	{
		if (aliveState == true)
		{
			dodge(damage);
			if (health <= damage)
			{
				health = 0;
				aliveState = false;
				alivenumber--;
				vis[x][y]--;
			}
			else
			{
				health -= damage;
			}
		}
	}

	void setPosition(int newx, int newy)
	{
		if (newx < 0 || newx > 200)
		{
			throw "newx out of range\n";
		}
		if (newy < 0 || newy > 200)
		{
			throw "newy out of range\n";
		}
		x = newx;
		y = newy;
	}

	int getPositionX()
	{
		return x;
	}

	void setPositionX(int x_) { x = x_; }

	int getPositionY()
	{
		return y;
	}

	void setPositionY(int y_) { y = y_; }

	double getRadius()
	{
		return r;
	}

	void setRadius(double r_)
	{
		r = r_;
	}

	void setaliveState(bool state)
	{
		aliveState = state;
	}

	COLORREF getColor()
	{
		return color;
	}
	std::string getName()
	{
		return name;
	}

	int getHealth()
	{
		return health;
	}

	int getNumber()
	{
		return number;
	}

	int getAliveNumber()
	{
		return alivenumber;
	}

	bool getaliveState()
	{
		return aliveState;
	}

	void getInfo()
	{
		std::cout << "name is " << name << "\n";
		std::cout << "health is " << health << "\n";
		std::cout << "aliveState is " << std::boolalpha << aliveState << "\n";
		std::cout << "x is " << x << "\n";
		std::cout << "y is " << y << "\n";
		std::cout << "\n";
	}

	void attack(Spirit& target)
		// 额外技能1:攻击
	{
		int damage = rand() % 500;
		if (target.aliveState)
		{
			if (!aliveState)
			{
				std::cout << name << " is not alive\n\n";
			}
			else
			{

				std::cout << name << " attack " << target.name << "\n";
				dodge(damage);
				if (target.health <= damage)
				{
					target.health = 0;
					std::cout << target.name << " is killed \n";
					vis[target.x][target.y]--;
				}
				else
				{
					target.health -= damage;
					std::cout << target.name << " 's new health is " << target.health << "\n";
				}
				std::cout << "\n";
			}
		}
		else
		{
			std::cout << target.name << " is not alive\n\n";
		}
	}

	void heal(int add)
		// 额外技能2:治疗
	{
		if (!aliveState)
		{
			std::cout << name << " is not alive\n\n";
		}
		else
		{
			health = min(health + add, 1000);
			std::cout << name << "'s new health is " << health << "\n\n";
		}
	}

	void move(int newx, int newy)
		// 额外技能3:移动
	{
		if (!aliveState)
		{
			std::cout << name << " is not alive\n\n";
		}
		else
		{
			vis[x][y]--;
			x = newx, y = newy;
			vis[x][y]++;
			std::cout << name << "'s new position is (" << x << " " << y << ")\n\n";
		}
	}

	void dodge(int& damage)
		// 额外技能4:闪避,每次受到攻击有概率躲避攻击
	{
		if (rand() < rand())
		{
			damage = 0;
			std::cout << "successfully dodge\n\n";
		}
	}

};
int Spirit::number = 0;
int Spirit::alivenumber = 0;

void CompulsoryTask();
void SelectedTask();
void ServerWork();
void ClientWork();
SocketManager socketManager;
void Test()
{
	Json::Value object;
	Json::Reader reader;
	Json::Value array;
	array.append(1);
	object["name"] = "LETTTER";
	object["age"] = "18";
	object["array"] = array;
	
	/*std::string str = object.toStyledString();
	auto tmp = reader.parse(str,object);
	std::cout << str << "\n";*/
}
int main()
{
	 
	//CompulsoryTask();
	SelectedTask();
	return 0;
}


void ServerWork()
{
	//其实应该每一步都检查是否成功的，“有时间”再改
	while (1)
	{
		//如果accept一个客户端就分配给它精灵的地址,写成一个函数
		if (socketManager.ServerAcceptClient())
		{
			std::string name = getRandomName();
			int PlayerX = rand() % (mapw - 10) + 5;
			int PlayerY = rand() % (maph - 10) + 5;
			double PlayerR = rand() % 5 + 5;
			while (1)
			{
				//判断精灵和敌人是否相交
				int f = 1;
				for (int i = 0; i < enemynum; i++)
				{
					if (Enemies[i].getaliveState() == false)
					{
						continue;
					}
					int x = Enemies[i].getPositionX();
					int y = Enemies[i].getPositionY();
					double r = Enemies[i].getRadius();
					double dis = sqrt((x - PlayerX) * (x - PlayerX) + (y - PlayerY) * (y - PlayerY));//确定不会爆int
					if (dis <= r + PlayerR)
					{
						f = 0;
						break;
					}
				}
				//判断精灵之间是否相交
				for (int i = 0; i < Players.size(); i++)
				{
					if (Players[i].getaliveState() == false)
					{
						continue;
					}
					int x = Players[i].getPositionX();
					int y = Players[i].getPositionY();
					double r = Players[i].getRadius();
					double dis = sqrt((x - PlayerX) * (x - PlayerX) + (y - PlayerY) * (y - PlayerY));//确定不会爆int
					if (dis <= r + PlayerR)
					{
						f = 0;
						break;
					}
				}
				if (f)
				{
					break;
				}
				PlayerX = rand() % (mapw - 10) + 5;
				PlayerY = rand() % (maph - 10) + 5;
				PlayerR = rand() % 5 + 5;
			}
			COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
			Players.push_back(Spirit(name, PlayerX, PlayerY, PlayerR, color));
			Json::Value msg;
			msg["name"] = name;
			msg["x"] = PlayerX;
			msg["y"] = PlayerY;
			msg["r"] = PlayerR;
			msg["color"] = unsigned int(color);
			SOCKET Client = socketManager.GetClientSockets().back();
			socketManager.ServerSendMsg(Client, msg);
		}
		//接受每个客户端的消息
		socketManager.ServerRecClientsMsg();
		socketManager.ServerTest();//服务端向客户端发送数据成功
		Sleep(200);
	}
}
void ClientWork()
{
	socketManager.StartClient();
	while (1)
	{
		//socketManager.ClientSendMsg();
		socketManager.ClientTest();
		socketManager.ClientRecMsg();
		Sleep(200);
	}
}
void CompulsoryTask()
{
	Spirit a;
	a.getInfo();

	Spirit b("LETTER", 100, 100);
	b.getInfo();
	b.takeDamage(100);
	b.getInfo();
	b.setPosition(200, 200);
	b.getPositionX();
	b.getPositionY();
	b.getName();
	b.getHealth();
	b.getAliveNumber();

	Spirit arr[10];
	srand(time(0));
	for (int i = 0; i < 10; i++)
	{
		std::string name = getRandomName();
		int x = rand() % 200;
		int y = rand() % 200;
		arr[i] = Spirit(name, x, y);
		std::cout << i + 1 << "th spirit:\n";
		arr[i].getInfo();
	}

	for (int i = 0; i < 10; i++)
	{
		arr[i].takeDamage(rand() % 1500);
		arr[i].heal(rand() % 1000);
		arr[i].move(rand() % 201, rand() % 201);
		arr[i].attack(arr[(i + 1) % 10]);
	}
	for (int i = 0; i < 10; i++)
	{
		arr[i].getInfo();
	}
}

void Start();
void Init();
void Draw();
void Delay(DWORD ms);
void CreateFood();
void MovePlayer();
void MoveEnemy();
void Judge();
void SetCursor();

const int width = 800, height = 780;
const int mapw = width * 4, maph = height * 4;
const int foodnum = 5000;
const int enemynum = 100;
std::string UserName;
struct FOOD {
	int x, y, type;
	bool state = false;
	COLORREF color;
}food[foodnum];
Spirit Player("LETTER", width / 2, height / 2, 10, RGB(0, 100, 97));
std::vector<Spirit>Players;
Spirit Enemies[enemynum];
const int FPS = 75;

void SelectedTask()
{
	//首先生成服务端(这个服务端不能被关闭)
	if (socketManager.StartServer())
	{
		ServerWork();
	}
	else
	{
		initgraph(width, height);//初始化绘图窗口 
		SetCursor();
		Start();//设置开始界面
		BeginBatchDraw();//批量绘图,消除闪烁问题
		Init();//初始化Enemies和食物
		ClientWork();

	}
	 
	while (1)
	{
		Draw();//画出玩家,食物,敌人
		MovePlayer();//玩家移动
		MoveEnemy();//敌人移动
		Judge();//判断玩家,敌人,食物是否有重合
		CreateFood();//刷新食物
		Delay(1000 / FPS);//锁帧
		FlushBatchDraw();
	}
}

void SetCursor()
{
	HWND hwnd = GetHWnd();//获取窗口句柄
	HCURSOR hcur = LoadCursor(NULL, IDC_CROSS);			// 加载系统预置的鼠标样式
	SetClassLongPtr(hwnd, GCLP_HCURSOR, (long)hcur);	// 设置窗口类的鼠标样式
}

void Start()
{
	IMAGE img;
	loadimage(&img, _T("images/Start.jpg"));
	putimage(0, 0, &img);
	settextstyle(50, 0, _T("微软雅黑")); // 设置字体大小为50，使用微软雅黑字体
	//outtextxy(0, 0, _T("按任意键"));
	//_getch();
	Sleep(500);
	//outtextxy(width * 3 / 4, height / 2, "Game start now!!!");
	//登陆界面:待完善
}

void CreateFood()//刷新食物
{
	for (int i = 0; i < foodnum; i++)
	{
		if (food[i].state == false)
		{
			food[i].state = true;
			food[i].x = rand() % (mapw - 10) + 5;
			food[i].y = rand() % (maph - 10) + 5;
			food[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
			food[i].type = rand() % 3;
		}
	}
}

void Init()
{
	//初始化食物和Enemies
	CreateFood();
	for (int i = 0; i < enemynum; i++)
	{
		std::string name = getRandomName();
		int x = rand() % (mapw - 10) + 5;
		int y = rand() % (maph - 10) + 5;
		double r = rand() % 5 + 5;
		Enemies[i] = Spirit(name, x, y, r, RGB(rand() % 256, rand() % 256, rand() % 256));
	}
}


void Draw()//绘制地图,展示精灵和食物位置
{
	//玩家看到的界面(width,height),可以到达的矩形是(0,0,mapw,maph),其他地方用背景色填充
	setbkcolor(RGB(255, 0, 0));//red
	cleardevice();// 使用当前背景色清空绘图设备(默认绘图窗口)
	setfillcolor(RGB(111, 2, 225));//purple
	fillrectangle(0, 0, mapw, maph);//绘制玩家活动区域

	//绘制食物
	for (int i = 0; i < foodnum; i++)
	{
		setfillcolor(food[i].color);
		int type = food[i].type;
		//fillcircle(food[i].x, food[i].y, 10);
		if (type == 0)
		{
			solidpie(food[i].x, food[i].y, food[i].x + 10, food[i].y + 10, 0, 10);
		}
		else if (type == 1)
		{
			solidroundrect(food[i].x, food[i].y, food[i].x + 5, food[i].y + 5, 5, 5);
		}
		else
		{
			solidellipse(food[i].x, food[i].y, food[i].x + 3, food[i].y + 4);
		}
	}

	//绘制玩家
	int PlayerX = Player.getPositionX();
	int PlayerY = Player.getPositionY();
	double PlayerR = Player.getRadius();
	setfillcolor(BLUE);
	if (PlayerX<PlayerR || PlayerX + PlayerR>mapw || PlayerY < PlayerR || PlayerY + PlayerR>maph)
	{
		HWND hwnd = GetHWnd();
		MessageBox(hwnd, _T("    You are too big"), _T("WIN"), MB_ICONWARNING);
		closegraph();
	}
	fillcircle(Player.getPositionX(), Player.getPositionY(), (int)Player.getRadius());

	//绘制敌人
	for (int i = 0; i < enemynum; i++)
	{
		if (Enemies[i].getaliveState() == false)
		{
			continue;
		}
		setfillcolor(Enemies[i].getColor());
		fillcircle(Enemies[i].getPositionX(), Enemies[i].getPositionY(), (int)Enemies[i].getRadius());
	}
}

void Delay(DWORD ms)
{
	static DWORD oldtime = GetTickCount(); //返回系统启动以来经过的毫秒数,超过49.7天溢出清零
	while (GetTickCount() - oldtime < ms)
	{
		Sleep(1);//只sleep 1ms 误差更小
	}
	oldtime = GetTickCount();
}

void MovePlayer()
{
	static int deltax = 0, deltay = 0;
	int x = Player.getPositionX();
	int y = Player.getPositionY();
	double r = Player.getRadius();
	if ((GetAsyncKeyState('W')) && y - r >= 5) {
		Player.setPositionY(y - 5);
		y = Player.getPositionY();
		deltay += 5;
	}
	if ((GetAsyncKeyState('S')) && y + r + 5 <= maph) {
		Player.setPositionY(y + 5);
		deltay -= 5;
	}
	if ((GetAsyncKeyState('A')) && x - r >= 5) {
		Player.setPositionX(x - 5);
		x = Player.getPositionX();
		deltax += 5;
	}
	if ((GetAsyncKeyState('D')) && x + 5 + r <= mapw) {
		Player.setPositionX(x + 5);
		deltax -= 5;
	}
	setorigin(deltax, deltay);
}

void MoveEnemy()
{
	const int k = 5;//控制移动速度
	for (int i = 0; i < enemynum; i++)
	{
		if (Enemies[i].getaliveState() == false)
		{
			continue;
		}
		int f = rand() % 2 ? 1 : -1;
		int r = Enemies[i].getRadius();
		int x = Enemies[i].getPositionX();
		int y = Enemies[i].getPositionY();
		if (rand() % 2)
		{
			if (x + k * f + r <= mapw && x - r + k * f >= 0)
			{
				Enemies[i].setPositionX(x + k * f);
			}
		}
		else
		{
			if (y + k * f + r <= maph && y - r + k * f >= 0)
			{
				Enemies[i].setPositionY(y + k * f);
			}
		}
	}
}

void Judge()//服务端专用,需要判断玩家之间,玩家和AI
{
	int PlayerX = Player.getPositionX();
	int PlayerY = Player.getPositionY();
	double PlayerR = Player.getRadius();
	//玩家和食物
	const int K1 = 30;//控制玩家体积增长
	for (int i = 0; i < foodnum; i++)
	{
		double dis = sqrt((food[i].x - PlayerX) * (food[i].x - PlayerX) +
			(food[i].y - PlayerY) * (food[i].y - PlayerY));//确保不会爆int
		double r = Player.getRadius();
		if (dis <= r + 1)
		{
			food[i].state = false;
			double NewVolumn = Pi * r * r + K1;
			double NewRadius = sqrt(NewVolumn / Pi);
			Player.setRadius(NewRadius);
		}
	}

	//敌人和食物
	const int K2 = 1;//控制敌人体积增长
	for (int i = 0; i < enemynum; i++)
	{
		int x = Enemies[i].getPositionX();
		int y = Enemies[i].getPositionY();
		double r = Enemies[i].getRadius();
		for (int j = 0; j < foodnum; j++)
		{
			double dis = sqrt((food[j].x - x) * (food[j].x - x) + (food[j].y - y) * (food[j].y - y));//确保不会爆int
			if (dis <= r + 1 && food[j].state == true)
			{
				food[i].state = false;
				double NewVolumn = Pi * r * r + K2;
				double NewRadius = sqrt(NewVolumn / Pi);
				Enemies[i].setRadius(NewRadius);
			}
		}

	}

	//玩家和敌人
	for (int i = 0; i < enemynum; i++)
	{
		if (Enemies[i].getaliveState() == false)
		{
			continue;
		}
		int x = Enemies[i].getPositionX();
		int y = Enemies[i].getPositionY();
		double r = Enemies[i].getRadius();
		int PlayerX = Player.getPositionX();
		int PlayerY = Player.getPositionY();
		double PlayerR = Player.getRadius();
		double dis = sqrt((x - PlayerX) * (x - PlayerX) + (y - PlayerY) * (y - PlayerY));//确定不会爆int
		if (dis <= r + PlayerR)
		{
			if (PlayerR < r)
			{
				HWND hwnd = GetHWnd();
				MessageBox(hwnd, _T("    You are dead"), _T("LOSER"), MB_ICONWARNING);
				closegraph();
			}
			else
			{
				Enemies[i].setaliveState(false);
				double NewVolumn = Pi * (r * r + PlayerR * PlayerR);
				double NewRadius = sqrt(NewVolumn / Pi);
				Player.setRadius(NewRadius);
			}
		}
	}

	// 敌人和敌人
	for (int i = 0; i < enemynum; i++)
	{
		for (int j = 0; j < enemynum; j++)
		{
			if (i != j && Enemies[i].getaliveState() == true && Enemies[j].getaliveState() == true)
			{
				int x1 = Enemies[i].getPositionX();
				int y1 = Enemies[i].getPositionY();
				double r1 = Enemies[i].getRadius();
				int x2 = Enemies[j].getPositionX();
				int y2 = Enemies[j].getPositionY();
				double r2 = Enemies[j].getRadius();
				double dis = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));//确定不会爆int
				if (dis <= r1 + r2)
				{
					if (r1 >= r2)
					{
						Enemies[j].setaliveState(false);
						double NewVolumn = Pi * (r1 * r1 + r2 * r2);
						double NewRadius = sqrt(NewVolumn / Pi);
						Enemies[i].setRadius(NewRadius);
					}
					else
					{
						Enemies[i].setaliveState(false);
						double NewVolumn = Pi * (r1 * r1 + r2 * r2);
						double NewRadius = sqrt(NewVolumn / Pi);
						Enemies[j].setRadius(NewRadius);
					}
				}
			}
		}
	}

}
