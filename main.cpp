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

std::string GetRandomName()
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
	//注意记得初始化
	std::string name = "Empty";
	int x = 0, y = 0, health = 1000;
	bool AliveState = true;
	static int number, AliveNumber;
	double r = 1;//精灵半径
	int deltax = 0, deltay = 0;//似乎可以删掉deltax和deltay,待删
	COLORREF color = UINT_MAX;//windows下32位无符号整数
	SOCKET socket = INVALID_SOCKET;//windows下64位无符号整数
public:
	Spirit()
	{
		number++;
		AliveNumber++;
		name = GetRandomName();
		x = rand() % 201;
		y = rand() % 201;
		vis[x][y] = 1;
	}

	Spirit(std::string name_, int x_, int y_) : name(name_), x(x_), y(y_)
	{
		number++;
		AliveNumber++;
		vis[x][y] = 1;
	}

	Spirit(int x_, int y_, double r_) :x(x_), y(y_), r(r_) {}

	Spirit(std::string name_, int x_, int y_, double r_, COLORREF color_) :name(name_), x(x_), y(y_), r(r_), color(color_) {}

	Spirit(std::string name_, int x_, int y_, double r_, COLORREF color_, SOCKET socket_) :name(name_), x(x_), y(y_), r(r_), color(color_), socket(socket_) {}

	void TakeDamage(int damage)
	{
		if (AliveState == true)
		{
			dodge(damage);
			if (health <= damage)
			{
				health = 0;
				AliveState = false;
				AliveNumber--;
				vis[x][y]--;
			}
			else
			{
				health -= damage;
			}
		}
	}

	void SetPosition(int newx, int newy)
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

	void SetName(std::string name_) { name = name_; }

	void SetColor(int color_) { color = color_; }

	int GetPositionX() { return x; }

	void SetPositionX(int x_) { x = x_; }

	int GetPositionY() { return y; }

	void SetPositionY(int y_) { y = y_; }

	void AddDeltay(int add) { deltay += add; }

	int GetDeltay() { return deltay; }

	void AddDeltax(int add) { deltax += add; }

	int GetDeltax() { return deltax; }

	double GetRadius() { return r; }

	void SetRadius(double r_) { r = r_; }

	void SetAliveState(bool state) { AliveState = state; }

	COLORREF GetColor() { return color; }

	std::string GetName() { return name; }

	int GetHealth() { return health; }

	int GetNumber() { return number; }

	int GetAliveNumber() { return AliveNumber; }

	bool GetAliveState() { return AliveState; }

	SOCKET GetSocket() { return socket; }

	void GetInfo()
	{
		std::cout << "name is " << name << "\n";
		std::cout << "health is " << health << "\n";
		std::cout << "AliveState is " << std::boolalpha << AliveState << "\n";
		std::cout << "x is " << x << "\n";
		std::cout << "y is " << y << "\n";
		std::cout << "\n";
	}

	void attack(Spirit& target)
		// 额外技能1:攻击
	{
		int damage = rand() % 500;
		if (target.AliveState)
		{
			if (!AliveState)
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
		if (!AliveState)
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
		if (!AliveState)
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

	Json::Value ToJson()const
	{
		Json::Value val;
		val["x"] = x;
		val["y"] = y;
		val["r"] = r;
		val["name"] = name;
		val["AliveState"] = AliveState;
		val["AliveNumber"] = AliveNumber;
		val["color"] = (int)color;
		val["socket"] = socket;
		return val;
	}
};
int Spirit::number = 0;
int Spirit::AliveNumber = 0;

void CompulsoryTask();
void SelectedTask();
void ServerWork();
void Test();
SocketManager socketManager;

int main()
{
	srand(time(0));
	//CompulsoryTask();
	SelectedTask();
	return 0;
}

void CompulsoryTask()
{
	Spirit a;
	a.GetInfo();

	Spirit b("LETTER", 100, 100);
	b.GetInfo();
	b.TakeDamage(100);
	b.GetInfo();
	b.SetPosition(200, 200);
	b.GetPositionX();
	b.GetPositionY();
	b.GetName();
	b.GetHealth();
	b.GetAliveNumber();

	Spirit arr[10];
	for (int i = 0; i < 10; i++)
	{
		std::string name = GetRandomName();
		int x = rand() % 200;
		int y = rand() % 200;
		arr[i] = Spirit(name, x, y);
		std::cout << i + 1 << "th spirit:\n";
		arr[i].GetInfo();
	}

	for (int i = 0; i < 10; i++)
	{
		arr[i].TakeDamage(rand() % 1500);
		arr[i].heal(rand() % 1000);
		arr[i].move(rand() % 201, rand() % 201);
		arr[i].attack(arr[(i + 1) % 10]);
	}
	for (int i = 0; i < 10; i++)
	{
		arr[i].GetInfo();
	}
}

void Start();
void Init();
void Draw();
void Delay(DWORD);
void CreateFood();
void MovePlayer();
void MoveEnemy();
void Judge();
void SetCursor();
bool ClientProcessMsg();
void ServerProcessMsg();
const int width = 800, height = 780;
const int mapw = width * 4, maph = height * 4;
const int foodnum = 5000;
const int EnemyNum = 100;
std::string UserName;
struct FOOD {
	int x, y, type;
	bool state = false;
	COLORREF color;
}food[foodnum];
Spirit Player("LETTER", width / 2, height / 2, 10, RGB(0, 100, 97));
std::vector<Spirit>Players;
Spirit Enemy[EnemyNum];
const int FPS = 75;

void Test()
{
	//正常
	char msg[] = "Hi from Client";
	send(socketManager.GetClientSocket(), msg,sizeof(msg),0);
}

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
		//Start();//设置开始界面
		BeginBatchDraw();//批量绘图,消除闪烁问题
		socketManager.StartClient();
	}
	/*while (1)
	{
		Test();
	}*/
	//要先等到服务端的"Init"和"Info"命令初始化
	do {

	} while (!ClientProcessMsg());
	//std::cout << Player.ToJson();
	std::cout << "Client go into while\n";
	while (1)
	{	 
		ClientProcessMsg();
		Draw();//画出玩家,食物,敌人
		//MovePlayer();//玩家移动,需要加入服务端的判定  
		Delay(1000 / FPS);//锁帧
		FlushBatchDraw();
	}
}

void AssignAddress()
{
	std::string name = GetRandomName();
	int PlayerX = rand() % (mapw - 10) + 5;
	int PlayerY = rand() % (maph - 10) + 5;
	double PlayerR = rand() % 5 + 5;
	while (1)
	{
		//判断精灵和敌人是否相交
		int f = 1;
		for (int i = 0; i < EnemyNum; i++)
		{
			if (Enemy[i].GetAliveState() == false)
			{
				continue;
			}
			int x = Enemy[i].GetPositionX();
			int y = Enemy[i].GetPositionY();
			double r = Enemy[i].GetRadius();
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
			if (Players[i].GetAliveState() == false)
			{
				continue;
			}
			int x = Players[i].GetPositionX();
			int y = Players[i].GetPositionY();
			double r = Players[i].GetRadius();
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
	//?Player的颜色和Client显示的颜色不相同
	COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
	//COLORREF color = RGB(165, 0, 255);
	SOCKET ClientSocket = socketManager.GetClientSockets().back();
	Players.push_back(Spirit(name, PlayerX, PlayerY, PlayerR, color, ClientSocket));

	Json::Value msg;
	msg["type"] = "Self";
	msg["name"] = name;
	msg["x"] = PlayerX;
	msg["y"] = PlayerY;
	msg["r"] = PlayerR;
	msg["color"] = (int)color;

	socketManager.ServerSendMsg(ClientSocket, msg);
}
Json::Value FoodToJson()
{
	Json::Value val;
	for (int i = 0; i < foodnum; i++)
	{
		Json::Value tmp;
		tmp["x"] = food[i].x;
		tmp["y"] = food[i].y;
		tmp["state"] = food[i].state;
		tmp["type"] = food[i].type;
		tmp["color"] = (int)food[i].color;
		val.append(tmp);
	}
	return val;
}
void ServerSendInfo()
{
	Json::Value EnemyJson, PlayersJson;
	for (int i = 0; i < EnemyNum; i++)
	{
		EnemyJson[i] = Enemy[i].ToJson();
	}
	for (int i = 0; i < Players.size(); i++)
	{
		PlayersJson[i] = Players[i].ToJson();
	}
	Json::Value msg;
	msg["type"] = "Info";
	msg["food"] = FoodToJson();
	msg["Enemy"] = EnemyJson;
	msg["Players"] = PlayersJson;//Json数组,元素是对象
	for (int i = 0; i < Players.size(); i++)
	{
		if (Players[i].GetAliveState())
		{
			int ClientSocket = Players[i].GetSocket();
			if (!socketManager.ServerSendMsg(ClientSocket, msg))
			{
				std::cout <<i<<"th "<<"Player death\n";
				Players[i].SetAliveState(0);
			}
		}
	}
}
void ServerProcessMsg()
{
	//先改变所有Player的位置再去judge最后发送Info
	for (int i = 0; i < Players.size(); i++)
	{
		if (Players[i].GetAliveState())
		{
			int x = Players[i].GetPositionX();
			int y = Players[i].GetPositionY();
			double r = Players[i].GetRadius();
			Json::Value msg;
			int ClientSocket = Players[i].GetSocket();
			msg = socketManager.ServerRecMsg(ClientSocket);
			if (msg["type"] == "Control")
			{
				std::string direction = msg["direction"].asString();
				if (direction == "W" && y - r >= 5) {
					Players[i].SetPositionY(y - 5);
					y = Players[i].GetPositionY();
					Players[i].AddDeltay(5);
				}
				if (direction == "S" && y + r + 5 <= maph) {
					Players[i].SetPositionY(y + 5);
					Players[i].AddDeltay(-5);
				}
				if (direction == "A" && x - r >= 5) {
					Players[i].SetPositionX(x - 5);
					x = Players[i].GetPositionX();
					Players[i].AddDeltax(5);
				}
				if (direction == "D" && x + 5 + r <= mapw) {
					Players[i].SetPositionX(x + 5);
					Players[i].AddDeltax(-5);
				}
			}
		}
	}
}
bool ClientProcessMsg()
{
	static int f1 = 0, f2 = 0;
	Json::Value msg = socketManager.ClientRecMsg();
	if (msg == Json::nullValue)
	{
		return false;
	}

	if (msg["type"].asString() == "Self")//求掉asString似乎效果一样
	{
		f1 = 1;
		Player.SetPositionX(msg["x"].asInt());
		Player.SetPositionY(msg["y"].asInt());
		Player.SetRadius(msg["r"].asDouble());
		Player.SetName(msg["name"].asString());
		Player.SetColor(msg["color"].asInt());
	}
	else if (msg["type"].asString() == "Info")
	{
		f2 = 1;
		for (int i = 0; i < foodnum; i++)
		{
			food[i].type = msg["food"][i]["type"].asInt();
			food[i].x = msg["food"][i]["x"].asInt();
			food[i].y = msg["food"][i]["y"].asInt();
			food[i].state = msg["food"][i]["state"].asBool();
			food[i].color = msg["food"][i]["color"].asInt();
		}
		for (int i = 0; i < EnemyNum; i++)
		{
			Enemy[i].SetPositionX(msg["Enemy"][i]["x"].asInt());
			Enemy[i].SetPositionY(msg["Enemy"][i]["y"].asInt());
			Enemy[i].SetRadius(msg["Enemy"][i]["r"].asDouble());
			Enemy[i].SetColor(msg["Enemy"][i]["color"].asInt());
			Enemy[i].SetAliveState(msg["Enemy"][i]["AliveState"].asBool());
		}

		Players.clear();
		Spirit tmp;
		for (int i = 0; i < msg["Players"].size(); i++)
		{
			tmp.SetPositionX(msg["Players"][i]["x"].asInt());
			tmp.SetPositionY(msg["Players"][i]["y"].asInt());
			tmp.SetRadius(msg["Players"][i]["r"].asDouble());
			tmp.SetColor(msg["Players"][i]["r"].asInt());
			tmp.SetAliveState(msg["Players"][i]["r"].asBool());
			tmp.SetName(msg["Players"][i]["r"].asString());
			Players.push_back(tmp);
			//setorigin的原理就是:你往右下走,旁边的东西相对你往左上走,所以origin就要往左上
			setorigin(-Player.GetPositionX() + width / 2, -Player.GetPositionY() + height / 2);
		}
	}
	else
	{
		std::cout << "ClientProcessMsg Fail\n";
		std::cout << msg << "\n";//   "type" : 2,
		return false;
	}
	return (f1 & f2);
}
void ServerWork()
{
	//其实应该每一步都检查是否成功的还有失败的应对措施，“有时间”再改
	Init();//初始化食物和enemy
	std::cout << "Server Init Success\n";
	while (1)
	{
		if (socketManager.ServerAcceptClient())
		{
			AssignAddress();//分配地址
		}
		ServerSendInfo();
		ServerProcessMsg();
		CreateFood();
		MoveEnemy();
		Judge();//判断玩家,敌人,食物是否有重合
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

void Init()//服务端专用
{
	//初始化食物和Enemy
	CreateFood();
	for (int i = 0; i < EnemyNum; i++)
	{
		std::string name = GetRandomName();
		int x = rand() % (mapw - 10) + 5;
		int y = rand() % (maph - 10) + 5;
		double r = rand() % 5 + 5;
		Enemy[i] = Spirit(name, x, y, r, RGB(rand() % 256, rand() % 256, rand() % 256));
		//std::cout << Enemy[i].GetColor() << "\n";
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
	 
	//std::cout << Player.ToJson();
	int PlayerX = Player.GetPositionX();
	int PlayerY = Player.GetPositionY();
	double PlayerR = Player.GetRadius();
	setfillcolor(Player.GetColor());
	if (PlayerX<PlayerR || PlayerX + PlayerR>mapw || PlayerY < PlayerR || PlayerY + PlayerR>maph)
	{
		HWND hwnd = GetHWnd();
		MessageBox(hwnd, _T("    You are too big"), _T("WIN"), MB_ICONWARNING);
		closegraph();
		std::cout << "2 Big\n";
	}
	fillcircle(Player.GetPositionX(), Player.GetPositionY(), (double)Player.GetRadius());

	//绘制敌人
	for (int i = 0; i < EnemyNum; i++)
	{
		if (!Enemy[i].GetAliveState())
		{
			continue;
		}
		setfillcolor(Enemy[i].GetColor());
		fillcircle(Enemy[i].GetPositionX(), Enemy[i].GetPositionY(), (double)Enemy[i].GetRadius());
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
	//向服务端发送msg
	//先test Player的情况再Test enemy
	int x = Player.GetPositionX();
	int y = Player.GetPositionY();
	double r = Player.GetRadius();
	Json::Value SendMsg, RecMsg;
	SendMsg["type"] = "Control";
	if (GetAsyncKeyState('W'))
	{
		SendMsg["direction"] = "W";
	}
	if (GetAsyncKeyState('S'))
	{
		SendMsg["direction"] = "S";
	}
	if (GetAsyncKeyState('A'))
	{
		SendMsg["direction"] = "A";
	}
	if (GetAsyncKeyState('D'))
	{
		SendMsg["direction"] = "D";
	}
	if (!SendMsg["direction"].isNull())
	{
		socketManager.ClientSendMsg(SendMsg);
	}
	 
}

void MoveEnemy()//服务端专用
{
	const int k = 5;//控制移动速度
	for (int i = 0; i < EnemyNum; i++)
	{
		if (Enemy[i].GetAliveState() == false)
		{
			continue;
		}
		int f = rand() % 2 ? 1 : -1;
		int r = Enemy[i].GetRadius();
		int x = Enemy[i].GetPositionX();
		int y = Enemy[i].GetPositionY();
		if (rand() % 2)
		{
			if (x + k * f + r <= mapw && x - r + k * f >= 0)
			{
				Enemy[i].SetPositionX(x + k * f);
			}
		}
		else
		{
			if (y + k * f + r <= maph && y - r + k * f >= 0)
			{
				Enemy[i].SetPositionY(y + k * f);
			}
		}
	}
}

void Judge()//服务端专用,需要判断玩家之间,玩家和AI
{
	for (int i = 0; i < Players.size(); i++)
	{
		if (!Players[i].GetAliveState())
		{
			continue;
		}
		int PlayerX = Players[i].GetPositionX();
		int PlayerY = Players[i].GetPositionY();
		double PlayerR = Players[i].GetRadius();

		//玩家和食物
		const int K1 = 10;//控制玩家体积增长
		for (int j = 0; j < foodnum; j++)
		{
			if (!food[j].state)
			{
				continue;
			}
			double dis = sqrt((food[j].x - PlayerX) * (food[j].x - PlayerX) +
				(food[j].y - PlayerY) * (food[j].y - PlayerY));//确保不会爆int
			double r = Player.GetRadius();
			if (dis <= r + 1)
			{
				food[i].state = false;
				double NewVolumn = Pi * r * r + K1;
				double NewRadius = sqrt(NewVolumn / Pi);
				Players[i].SetRadius(NewRadius);
			}
		}
	}

	//敌人和食物
	const int K2 = 1;//控制敌人体积增长
	for (int i = 0; i < EnemyNum; i++)
	{
		int x = Enemy[i].GetPositionX();
		int y = Enemy[i].GetPositionY();
		double r = Enemy[i].GetRadius();
		for (int j = 0; j < foodnum; j++)
		{
			double dis = sqrt((food[j].x - x) * (food[j].x - x) + (food[j].y - y) * (food[j].y - y));//确保不会爆int
			if (dis <= r + 1 && food[j].state == true)
			{
				food[i].state = false;
				double NewVolumn = Pi * r * r + K2;
				double NewRadius = sqrt(NewVolumn / Pi);
				Enemy[i].SetRadius(NewRadius);
			}
		}

	}

	//玩家和敌人
	for (int i = 0; i < Players.size(); i++)
	{
		if (!Players[i].GetAliveState())
		{
			continue;
		}
		for (int j = 0; j < EnemyNum; j++)
		{
			if (!Enemy[j].GetAliveState())
			{
				continue;
			}
			int x = Enemy[j].GetPositionX();
			int y = Enemy[j].GetPositionY();
			double r = Enemy[j].GetRadius();
			int PlayerX = Players[i].GetPositionX();
			int PlayerY = Players[i].GetPositionY();
			double PlayerR = Players[i].GetRadius();
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
					Enemy[j].SetAliveState(false);
					double NewVolumn = Pi * (r * r + PlayerR * PlayerR);
					double NewRadius = sqrt(NewVolumn / Pi);
					Players[i].SetRadius(NewRadius);
				}
			}
		}
	}

	// 敌人和敌人
	for (int i = 0; i < EnemyNum; i++)
	{
		for (int j = 0; j < EnemyNum; j++)
		{
			if (i != j && Enemy[i].GetAliveState() && Enemy[j].GetAliveState())
			{
				int x1 = Enemy[i].GetPositionX();
				int y1 = Enemy[i].GetPositionY();
				double r1 = Enemy[i].GetRadius();
				int x2 = Enemy[j].GetPositionX();
				int y2 = Enemy[j].GetPositionY();
				double r2 = Enemy[j].GetRadius();
				double dis = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));//确定不会爆int
				if (dis <= r1 + r2)
				{
					if (r1 >= r2)
					{
						Enemy[j].SetAliveState(false);
						double NewVolumn = Pi * (r1 * r1 + r2 * r2);
						double NewRadius = sqrt(NewVolumn / Pi);
						Enemy[i].SetRadius(NewRadius);
					}
					else
					{
						Enemy[i].SetAliveState(false);
						double NewVolumn = Pi * (r1 * r1 + r2 * r2);
						double NewRadius = sqrt(NewVolumn / Pi);
						Enemy[j].SetRadius(NewRadius);
					}
				}
			}
		}
	}

}
