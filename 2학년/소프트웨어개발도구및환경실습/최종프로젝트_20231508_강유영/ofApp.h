#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

    // 캐릭터, 무기, 공 클래스 선언
    class Weapon {
    public:
        float x, y;
        void draw() const;
    };

    class Ball {
    public:
        float x, y, speedX, speedY;
        int imgIndex;
        const ofApp* app; // ofApp 인스턴스를 참조하는 포인터 추가
        void draw() const;
    };

    float stageHeight;
    float characterX, characterY;
    float characterSpeed;

    std::vector<Weapon> weapons;
    std::vector<Ball> balls;

    float weaponSpeed;
    float ballSpeedY[4];

    int weaponToRemove;
    int ballToRemove;

    ofTrueTypeFont gameFont;
    int totalTime;
    int startTime;
    std::string gameResult;

    bool isGameRunning;
};
