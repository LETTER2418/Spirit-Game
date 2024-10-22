#include <iostream>
#include<cstdlib>
#include<graphics.h>
#include<windows.h>
#include<conio.h>
#include<chrono>
#include<time.h>
using namespace std;
using ll = long long;
int vis[201][201]; // vis[x][y]=1表示(i,j)位置有精灵

string getRandomName()
{
    string name;
    for (int i = 0; i < 6; i++)
    {
        name += 'A' + (rand() % 26);
    }
    return name;
}
class Spirit
{

    string name = "Empty";
    int x = 0, y = 0, health = 1000;
    bool aliveState = true;
    static int number, alivenumber;
    int r=1;//精灵半径
public:
    Spirit()
    {
        number++;
        alivenumber++;
        name = getRandomName();
        int x = rand();
        int y = rand();
        // vis[x][y] = 1;
    }

    Spirit(string name_, int x_, int y_) : name(name_), x(x_), y(y_)
    {
        number++;
        alivenumber++;
        vis[x][y] = 1;
    }

    Spirit(int x_,int y_,int r_):x(x_),y(y_),r(r_){}

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

    int getPositionY()
    {
        return y;
    }

    string getName()
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

    void getInfo()
    {
        cout << "name is " << name << "\n";
        cout << "health is " << health << "\n";
        cout << "aliveState is " << boolalpha << aliveState << "\n";
        cout << "x is " << x << "\n";
        cout << "y is " << y << "\n";
        cout << "\n";
    }

    void attack(Spirit& target)
        // 额外技能1:攻击
    {
        int damage = rand() % 500;
        if (target.aliveState)
        {
            if (!aliveState)
            {
                cout << name << " is not alive\n\n";
            }
            else
            {

                cout << name << " attack " << target.name << "\n";
                dodge(damage);
                if (target.health <= damage)
                {
                    target.health = 0;
                    cout << target.name << " is killed \n";
                    vis[target.x][target.y]--;
                }
                else
                {
                    target.health -= damage;
                    cout << target.name << " 's new health is " << target.health << "\n";
                }
                cout << "\n";
            }
        }
        else
        {
            cout << target.name << " is not alive\n\n";
        }
    }

    void heal(int add)
        // 额外技能2:治疗
    {
        if (!aliveState)
        {
            cout << name << " is not alive\n\n";
        }
        else
        {
            health = min(health + add, 1000);
            cout << name << "'s new health is " << health << "\n\n";
        }
    }

    void move(int newx, int newy)
        // 额外技能3:移动
    {
        if (!aliveState)
        {
            cout << name << " is not alive\n\n";
        }
        else
        {
            vis[x][y]--;
            x = newx, y = newy;
            vis[x][y]++;
            cout << name << "'s new position is (" << x << " " << y << ")\n\n";
        }
    }

    void dodge(int& damage)
        // 额外技能4:闪避,每次受到攻击有概率躲避攻击
    {
        if (rand() < rand())
        {
            damage = 0;
            cout<< "successfully dodge\n\n";
        }
    }

};
int Spirit::number = 0;
int Spirit::alivenumber = 0;

void SelectedTask();

int main()
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
        string name = getRandomName();
        int x = rand() % 200;
        int y = rand() % 200;
        arr[i] = Spirit(name, x, y);
        cout << i + 1 << "th spirit:\n";
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

    SelectedTask();
}

void Start();
void Init();
void Draw();
void Move();
void Delay(DWORD ms);
const int width = 800, height = 780;
const int mapw = width * 4, maph = height * 4;
const int foodnum = 1000;
string UserName;
struct FOOD {
    int x, y, r;
    bool state = false;
    COLORREF color;
}food[foodnum];

void SelectedTask()
{
    initgraph(width, height);//初始化绘图窗口 
    Start();//设置开始界面
    BeginBatchDraw();//批量绘图,消除闪烁问题
    while (1)
    {
        Init();//初始化玩家和其他精灵和食物
        Draw();
        Move();
        FlushBatchDraw();
        Delay(500);
        //_getch();
    }
}

void Start()
{
    IMAGE img;
    loadimage(&img, _T("images/Start.jpg"));
    putimage(0, 0, &img);
    settextstyle(50, 0, _T("微软雅黑")); // 设置字体大小为50，使用微软雅黑字体
    outtextxy(width / 2, height / 2, _T("按任意键"));
    //_getch();
    //outtextxy(width * 3 / 4, height / 2, "Game start now!!!");
    //登陆界面:待完善
}

void CreateFood()//刷新食物
{
    for (int i = 0; i < foodnum; i++)
    {
        if (food[i].state==false)
        {
            food[i].state = true;
            food[i].x = rand() % mapw;
            food[i].y = rand() % maph;
            food[i].r = rand() % 20+1;
            food[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
            //cerr << i << " "<<food[i].x<<" "<<food[i].y<<"\n";
        }
    }
}

void Init()
{
    //初始化食物，显示之前初始化的精灵位置
    CreateFood();

}

void Draw()//绘制地图,展示精灵和食物位置
{
    cleardevice();
    setfillcolor(WHITE);
    setlinecolor(WHITE);
    fillrectangle(0, 0, mapw,maph);
    for (int i = 0; i < foodnum; i++)
    {
        setfillcolor(food[i].color);
        fillcircle(food[i].x, food[i].y,food[i].r+1);
    }
     
}

void Delay(DWORD ms)								 
{
    static DWORD oldtime = GetTickCount();

    /*if (GetTickCount() - oldtime < ms)
        Sleep(ms-(GetTickCount() - oldtime));*/
    while (GetTickCount() - oldtime < ms)
        Sleep(1);

    oldtime = GetTickCount();
}

void Move()
{

}
