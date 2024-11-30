#include "musicManager.h"
sf::Music music;
void MusicStart()
{
    // 加载音乐文件
     
    if (!music.openFromFile("music.wav")) {
        std::cout<< "无法加载音乐文件\n" ;
        exit(0);
        return ;
    }

    //设置音量 0-100
    music.setVolume(2);

    // 播放音乐
    music.play();

    // 设置循环播放
    music.setLoop(true);
}
