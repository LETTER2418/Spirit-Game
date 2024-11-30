#include "UI.h"
#include "SocketManager.h"
#include "MusicManager.h"
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
		name += 'A' + (rand() % 26);
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
	int id = 0;
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

	void SetId(int id_) { id = id_; }

	int GetId() { return id; }

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

void Start();
void CompulsoryTask();
void SelectedTask();
void ServerWork();
SocketManager socketManager;
static int cnt = 0;

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
void FlushFood();
void MovePlayer();
void MoveEnemy();
void Judge();
void SetCursor();
bool ClientProcessMsg();
void ServerProcessMsg();
void UpdateCientsState();
const int width = 800, height = 780;
const int mapw = width * 4, maph = height * 4;
const int foodnum = 2000;
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
const int FPS = 70;//单客户端时70很丝滑
Json::Value msg;
Json::Value EnemyJson, PlayersJson;

void SelectedTask()
{
	 

	//首先生成服务端(这个服务端不能被关闭)

	if (socketManager.StartServer())
	{
		/*FILE* file;
		freopen_s(&file, "E:\\server_output.txt", "w", stdout);*/
		MusicStart();//如果只有一台机器则只在服务端开启，防止重复
		ServerWork();
	}
	else
	{
		/*FILE* file;
		freopen_s(&file, "E:\\client_output.txt", "w", stdout);*/
		initgraph(width, height);//初始化绘图窗口 
		SetCursor();
		Start();//设置开始界面
		BeginBatchDraw();//批量绘图,消除闪烁问题
		socketManager.StartClient();
	}
	//要先等到服务端的"Init"和"Info"命令初始化
	std::cout << "Client start process msg\n";
	while (!ClientProcessMsg());
	std::cout << "Client go into while\n";
	while (1)
	{
		ClientProcessMsg();
		Draw();//画出玩家,食物,敌人
		MovePlayer();//玩家移动,需要加入服务端的判定  
		socketManager.ClientProcessSendMsgList();
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
	msg["id"] = (int)Players.size() - 1;
	if (Players.size() == 1)
	{
		msg["name"] = "LETTER";
		Players[0].SetName("LETTER");
	}
	socketManager.ServerAddSendMsgList(ClientSocket, msg);
	std::cout << "Server finish assigning address\n";
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
	static int f = 0;
	f++;
	if (f % 5 == 0)//减少信息发送量
	{
		return;
	}
	for (int i = 0; i < EnemyNum; i++)
	{
		EnemyJson[i] = Enemy[i].ToJson();
	}
	for (int i = 0; i < Players.size(); i++)
	{
		PlayersJson[i] = Players[i].ToJson();
	}
	msg = Json::nullValue;
	msg["type"] = "Info";
	msg["food"] = FoodToJson();
	msg["Enemy"] = EnemyJson;
	msg["Players"] = PlayersJson;//Json数组,元素是对象
	msg["cnt"] = cnt;
	//std::cout << msg["Players"] << "\n";
	for (int i = 0; i < Players.size(); i++)
	{
		if (Players[i].GetAliveState())
		{
			int ClientSocket = Players[i].GetSocket();
			socketManager.ServerAddSendMsgList(ClientSocket, msg);
		}
	}
}
void ServerProcessMsg()
{
	static int f = 0, id = 0, c = 0;
	for (int i = id; i < Players.size(); i++)
	{
		if (Players[i].GetAliveState())
		{
			//std::cout << i << "th Plyaer\n";
			int x = Players[i].GetPositionX();
			int y = Players[i].GetPositionY();
			double r = Players[i].GetRadius();
			Json::Value msg;
			int ClientSocket = Players[i].GetSocket();
			msg = socketManager.ServerRecvMsg(ClientSocket);
			if (msg.isObject() && msg["type"] == "Control")
			{
				std::string direction = msg["direction"].asString();
				//std::cout << ++cnt << "th " << direction << " Control\n";
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
				if (direction == "F")//n个客户端会发n*_次'F'(因为不能保证按一次只get了一个'F')
				{
					f = 1;
					if (c % (Players.size() * 4) == 0)
					{
						id = (id + 1) % Players.size();
					}
					c++;
				}
			}
		}
		if (f)
		{
			break;
		}
	}
}
bool ClientProcessMsg()
{
	static int f1 = 0, f2 = 0;
	msg = socketManager.ClientRecvMsg();

	if (msg == Json::nullValue)
	{
		return false;
	}
	else if (msg.isString() && msg.asString() == "big")
	{
		HWND hwnd = GetHWnd();
		MessageBox(hwnd, _T("    You are too big"), _T("WIN"), MB_ICONWARNING);
		closegraph();
		std::cout << "You are 2 Big(WIN)\n";
		exit(0);
	}
	else if (msg.isString() && msg.asString() == "eaten")
	{
		HWND hwnd = GetHWnd();
		MessageBox(hwnd, _T("    You are eaten"), _T("LOSE"), MB_ICONWARNING);
		closegraph();
		std::cout << "You are eaten(LOSE)\n";
		exit(0);
	}
	if (!msg.isObject())
	{
		std::cout << "ClientProcessMsg error";
		exit(0);
	}
	if (msg["type"].asString() == "Self")//求掉asString似乎效果一样
	{
		f1 = 1;
		Player.SetPositionX(msg["x"].asInt());
		Player.SetPositionY(msg["y"].asInt());
		Player.SetRadius(msg["r"].asDouble());
		Player.SetName(msg["name"].asString());
		Player.SetColor(msg["color"].asInt());
		Player.SetId(msg["id"].asInt());
		//std::cout << Player.GetId() << " id\n";
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
			//msg是ToJson的msg
			tmp.SetPositionX(msg["Players"][i]["x"].asInt());
			tmp.SetPositionY(msg["Players"][i]["y"].asInt());
			tmp.SetRadius(msg["Players"][i]["r"].asDouble());
			tmp.SetColor(msg["Players"][i]["color"].asInt());
			tmp.SetAliveState(msg["Players"][i]["AliveState"].asBool());
			tmp.SetName(msg["Players"][i]["name"].asString());
			Players.push_back(tmp);

		}
		int id = Player.GetId();
		Player.SetPositionX(Players[id].GetPositionX());
		Player.SetPositionY(Players[id].GetPositionY());
		Player.SetRadius(Players[id].GetRadius());
		//std::cout << Players[id].GetPositionX() << " " << Players[id].GetPositionY() << "\n";
		//setorigin的原理就是:你往右下走,旁边的东西相对你往左上走,所以origin就要往左上
		setorigin(-Player.GetPositionX() + width / 2, -Player.GetPositionY() + height / 2);
	}
	else
	{
		std::cout << "ClientProcessMsg Fail\n";
		std::cout << msg << "\n";
		return false;
	}
	return (f1 & f2);
}
void ServerWork()
{
	Init();//初始化食物和enemy
	while (1)
	{
		if (socketManager.ServerAcceptClient())
		{
			AssignAddress();//分配地址
		}
		//先改变所有Player的位置再去judge最后发送Info
		ServerProcessMsg();
		MoveEnemy();
		Judge();//判断玩家,敌人,食物是否有重合
		ServerSendInfo();
		socketManager.ServerProcessClientsSendMsgList();
		FlushFood();
		UpdateCientsState();
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
	initgraph(width, height);
	int x1, y1, w1, h1;
	DrawButton(x1 = width / 4, y1 = height * 3 / 8, w1 = width / 2, h1 = height / 4,
		width / 4, height * 7 / 16, width / 26, height / 8, "开始游戏", 1);
	int f = 1;
	while (1)
	{
		ExMessage msg;
		if (peekmessage(&msg, EM_MOUSE))
		{
			if (IsMouseOverButton(msg.x, msg.y, x1, y1, x1 + w1, y1 + h1))
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					break; // 退出主循环
				}
				else if (msg.message == WM_MOUSEMOVE)
				{
					f = 0;
				}
			}
			else
			{
				f = 1;
			}
			DrawButton(x1 = width / 4, y1 = height * 3 / 8, w1 = width / 2, h1 = height / 4,
				width / 4, height * 7 / 16, width / 26, height / 8, "开始游戏", f);
		}
	}
}

void FlushFood()//刷新食物
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
void UpdateCientsState()
{
	auto mp = socketManager.GetClientsState();
	for (auto [x, y] : mp)
	{
		for (int i = 0; i < Players.size() && !y; i++)
		{
			if (Players[i].GetSocket() == x)
			{
				Players[i].SetAliveState(y);
				break;
			}
		}
	}
}
void Init()//服务端专用
{
	//初始化食物和Enemy
	FlushFood();
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

	//绘制敌人
	for (int i = 0; i < EnemyNum; i++)
	{
		if (Enemy[i].GetAliveState())
		{
			int x = Enemy[i].GetPositionX();
			int y = Enemy[i].GetPositionY();
			int color = Enemy[i].GetColor();
			int r = Enemy[i].GetRadius();
			setfillcolor(color);
			fillcircle(x, y, r);
			setbkcolor(color);
			settextcolor(BLACK);
			settextstyle(20, 0, _T("Consolas"));
			outtextxy(x - 14, y - 5, (Enemy[i].GetName().c_str()));
		}
	}

	//绘制玩家
	for (int i = 0; i < Players.size(); i++)
	{
		if (Players[i].GetAliveState())
		{
			int PlayerX = Players[i].GetPositionX();
			int PlayerY = Players[i].GetPositionY();
			double PlayerR = Players[i].GetRadius();
			int color = Players[i].GetColor();
			setfillcolor(color);
			fillcircle(PlayerX, PlayerY, PlayerR);
			setbkcolor(color);
			settextcolor(BLACK);
			settextstyle(20, 0, _T("Consolas"));
			outtextxy(PlayerX - 14, PlayerY - 5, (Players[i].GetName().c_str()));

		}
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
	int x = Player.GetPositionX();
	int y = Player.GetPositionY();
	double r = Player.GetRadius();
	Json::Value SendMsg, RecvMsg;
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
	if (GetAsyncKeyState('F'))
	{
		SendMsg["direction"] = "F";//同一主机按F切换client
	}

	if (!SendMsg["direction"].isNull())
	{
		static int cnt = 0;
		std::cout << ++cnt << "th " << SendMsg["direction"] << " control\n";

		socketManager.ClientAddSendMsgList(SendMsg);
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

		if (2 * PlayerR >= min(mapw, maph))
		{
			Players[i].SetAliveState(0);
			Json::Value msg = "big";
			socketManager.ServerAddSendMsgList(Players[i].GetSocket(), msg);
			continue;
		}

		//玩家和食物
		const int K1 = 100;//控制玩家体积增长
		for (int j = 0; j < foodnum; j++)
		{
			double dis = sqrt((food[j].x - PlayerX) * (food[j].x - PlayerX) +
				(food[j].y - PlayerY) * (food[j].y - PlayerY));//确保不会爆int
			double r = Players[i].GetRadius();
			if (dis <= r + 1 && food[j].state)
			{

				food[j].state = false;
				double NewVolumn = Pi * r * r + K1;
				double NewRadius = sqrt(NewVolumn / Pi);
				if (PlayerX < NewRadius)
				{
					Players[i].SetPositionX(NewRadius);
				}if (PlayerY < NewRadius)
				{
					Players[i].SetPositionY(NewRadius);
				}
				PlayerX = Players[i].GetPositionX();
				PlayerY = Players[i].GetPositionY();
				if (PlayerX + NewRadius > mapw)
				{
					Players[i].SetPositionX(mapw - NewRadius);
				}if (PlayerY + NewRadius > maph)
				{
					Players[i].SetPositionY(maph - NewRadius);
				}
				Players[i].SetRadius(NewRadius);
				//std::cout << "judge\n";
				//std::cout << Players[i].GetRadius() << "\n";
			}
		}

		// 玩家和玩家
		for (int j = 0; j < Players.size(); j++)
		{
			if (i == j || !Players[j].GetAliveState())
			{
				continue;
			}
			int x1 = Players[i].GetPositionX();
			int y1 = Players[i].GetPositionY();
			double r1 = Players[i].GetRadius();
			int x2 = Players[j].GetPositionX();
			int y2 = Players[j].GetPositionY();
			double r2 = Players[j].GetRadius();
			double dis = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));//确定不会爆int
			if (dis <= r1 + r2)
			{
				double NewVolumn = Pi * (r1 * r1 + r2 * r2);
				double NewRadius = sqrt(NewVolumn / Pi);
				if (r1 >= r2)
				{
					Players[j].SetAliveState(0);
					Players[i].SetRadius(NewRadius);
					Json::Value msg = "eaten";
					socketManager.ServerAddSendMsgList(Players[j].GetSocket(), msg);
					if (x1 < NewRadius)
					{
						Players[i].SetPositionX(NewRadius);
					}
					if (y1 < NewRadius)
					{
						Players[i].SetPositionY(NewRadius);
					}
					x1 = Players[i].GetPositionX();
					y1 = Players[i].GetPositionY();
					if (x1 + NewRadius > mapw)
					{
						Players[i].SetPositionX(mapw - NewRadius);
					}
					if (y1 + NewRadius > maph)
					{
						Players[i].SetPositionY(maph - NewRadius);
					}
				}
				else
				{
					Players[i].SetAliveState(0);
					Players[j].SetRadius(NewRadius);
					Json::Value msg = "eaten";
					socketManager.ServerAddSendMsgList(Players[i].GetSocket(), msg);
					if (x2 < NewRadius)
					{
						Players[j].SetPositionX(NewRadius);
					}
					if (y2 < NewRadius)
					{
						Players[j].SetPositionY(NewRadius);
					}
					x2 = Players[j].GetPositionX();
					y2 = Players[j].GetPositionY();
					if (x2 + NewRadius > mapw)
					{
						Players[j].SetPositionX(mapw - NewRadius);
					}
					if (y2 + NewRadius > maph)
					{
						Players[j].SetPositionY(maph - NewRadius);
					}
				}
			}
		}
	}

	//敌人和食物
	const int K2 = 300;//控制敌人体积增长
	for (int i = 0; i < EnemyNum; i++)
	{
		int x = Enemy[i].GetPositionX();
		int y = Enemy[i].GetPositionY();
		double r = Enemy[i].GetRadius();
		for (int j = 0; j < foodnum; j++)
		{
			double dis = sqrt((food[j].x - x) * (food[j].x - x) + (food[j].y - y) * (food[j].y - y));//确保不会爆int
			if (dis <= r + 1 && food[j].state)
			{
				food[j].state = false;
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
					Players[i].SetAliveState(0);
					Json::Value msg = "eaten";
					socketManager.ServerAddSendMsgList(Players[i].GetSocket(), msg);
					continue;
				}
				else
				{
					Enemy[j].SetAliveState(0);
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
					double NewVolumn = Pi * (r1 * r1 + r2 * r2);
					double NewRadius = sqrt(NewVolumn / Pi);
					if (r1 >= r2)
					{
						Enemy[j].SetAliveState(0);
						Enemy[i].SetRadius(NewRadius);
						if (x1 < NewRadius)
						{
							Enemy[i].SetPositionX(NewRadius);
						}
						if (y1 < NewRadius)
						{
							Enemy[i].SetPositionY(NewRadius);
						}
						x1 = Enemy[i].GetPositionX();
						y1 = Enemy[i].GetPositionY();
						if (x1 + NewRadius > mapw)
						{
							Enemy[i].SetPositionX(mapw - NewRadius);
						}
						if (y1 + NewRadius > maph)
						{
							Enemy[i].SetPositionY(maph - NewRadius);
						}
					}
					else
					{
						Enemy[i].SetAliveState(0);
						Enemy[j].SetRadius(NewRadius);
						if (x2 < NewRadius)
						{
							Enemy[j].SetPositionX(NewRadius);
						}
						if (y2 < NewRadius)
						{
							Enemy[j].SetPositionY(NewRadius);
						}
						x2 = Enemy[j].GetPositionX();
						y2 = Enemy[j].GetPositionY();
						if (x2 + NewRadius > mapw)
						{
							Enemy[j].SetPositionX(mapw - NewRadius);
						}
						if (y2 + NewRadius > maph)
						{
							Enemy[j].SetPositionY(maph - NewRadius);
						}
					}
				}
			}
		}
	}

}
