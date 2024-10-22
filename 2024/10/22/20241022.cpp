#include <iostream>
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

    void takeDamage(int damage)
    {
        if (aliveState == true)
        {
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
        cout << "aliveStae is " << boolalpha << aliveState << "\n";
        cout << "x is " << x << "\n";
        cout << "y is " << y << "\n";
        cout << "\n";
    }

    void display()
    {
        for (int i = 0; i <= 200; i++)
        {
            for (int j = 0; j <= 200; j++)
            {
                if (vis[i][j])
                {
                    cout << "*";
                }
                else
                {
                    cout << " ";
                }
            }
            cout << "\n";
        }
    }

    void attack(Spirit& target)
        // 额外技能1:攻击
    {
        int damage = rand() % 500;
        if (target.aliveState)
        {
            if (aliveState)
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
        if (aliveState)
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
        if (aliveState)
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
            cout << "successfully dodge\n";
        }
    }

};
int Spirit::number = 0;
int Spirit::alivenumber = 0;

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
    }
    for (int i = 0; i < 10; i++)
    {
        arr[i].getInfo();
    }

}
