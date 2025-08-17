#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(30);
    ofSetWindowTitle("Yuyeong Pang");

    stageHeight = 50; // 스테이지 높이 설정
    characterX = (ofGetWidth() / 2) - 25; // 캐릭터 초기 위치 설정
    characterY = ofGetHeight() - 50 - stageHeight;
    characterSpeed = 5.0f;

    weaponSpeed = 10.0f;

    ballSpeedY[0] = -18.0f;
    ballSpeedY[1] = -15.0f;
    ballSpeedY[2] = -12.0f;
    ballSpeedY[3] = -9.0f;

    // 공 생성 시 app 포인터 초기화
    balls.push_back({ 50, 50, 3, -6, 0, this });

    weaponToRemove = -1;
    ballToRemove = -1;

    bool fontLoaded = gameFont.load("verdana.ttf", 40);
    if (!fontLoaded) {
        ofLogError() << "Font not loaded!";
    }
    totalTime = 100;
    startTime = ofGetElapsedTimeMillis();

    gameResult = "Game Over";
    isGameRunning = true;
}

//--------------------------------------------------------------
void ofApp::update() {
    if (!isGameRunning) {
        return;
    }

    // 캐릭터 이동
    if (ofGetKeyPressed(OF_KEY_LEFT)) {
        characterX -= characterSpeed;
    }
    if (ofGetKeyPressed(OF_KEY_RIGHT)) {
        characterX += characterSpeed;
    }

    characterX = ofClamp(characterX, 0, ofGetWidth() - 50); // 캐릭터의 크기는 50x50으로 가정

    // 무기 이동
    for (auto& weapon : weapons) {
        weapon.y -= weaponSpeed;
    }
    weapons.erase(std::remove_if(weapons.begin(), weapons.end(),
        [](const Weapon& w) { return w.y < 0; }), weapons.end());

    // 공 이동 및 충돌 처리
    for (auto& ball : balls) {
        ball.x += ball.speedX;
        ball.y += ball.speedY;

        if (ball.x < 0 || ball.x > ofGetWidth() - 50) ball.speedX *= -1;
        if (ball.y >= ofGetHeight() - stageHeight - 50) {
            ball.speedY = ballSpeedY[ball.imgIndex];
        }
        else {
            ball.speedY += 0.5f;
        }
    }

    // 충돌 처리
    for (int i = balls.size() - 1; i >= 0; i--) {
        ofRectangle ballRect(balls[i].x, balls[i].y, 50, 50);
        ofRectangle characterRect(characterX, characterY, 50, 50);

        if (ballRect.intersects(characterRect)) {
            isGameRunning = false;
            gameResult = "Game Over";
            break;
        }

        for (int j = weapons.size() - 1; j >= 0; j--) {
            ofRectangle weaponRect(weapons[j].x, weapons[j].y, 10, 20);

            if (weaponRect.intersects(ballRect)) {
                weaponToRemove = j;
                ballToRemove = i;

                // 맞추어진 공을 기준으로 자식 공들 생성
                if (balls[i].imgIndex < 3) {
                    auto ball = balls[i];
                    float newSize = 25; // 작은 공의 크기 25x25으로 가정

                    // 자식 공들을 생성하여 balls 벡터에 추가
                    balls.push_back({ ball.x + 25 - newSize / 2,
                                      ball.y + 25 - newSize / 2,
                                      -3, -6, ball.imgIndex + 1, this });

                    balls.push_back({ ball.x + 25 - newSize / 2,
                                      ball.y + 25 - newSize / 2,
                                      3, -6, ball.imgIndex + 1, this });
                }

                // 맞춘 공을 제거
                break; // 내부 반복문을 나가서 다음 공을 처리할 수 있도록 함
            }
        }

        // 무기와 공을 제거
        if (ballToRemove != -1) {
            balls.erase(balls.begin() + ballToRemove);
            ballToRemove = -1; // ballToRemove 초기화
        }
    }

    // 무기와 공을 제거
    if (weaponToRemove != -1) {
        weapons.erase(weapons.begin() + weaponToRemove);
        weaponToRemove = -1; // weaponToRemove 초기화
    }


    if (balls.empty()) {
        isGameRunning = false;
        gameResult = "Mission Complete";
    }

    int elapsedTime = (ofGetElapsedTimeMillis() - startTime) / 1000;
    if (totalTime - elapsedTime <= 0) {
        isGameRunning = false;
        gameResult = "Time Over";
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0, 0, 0); // 배경색을 검은색으로 설정

    // 스테이지 그리기
    ofSetColor(25, 36, 144); // 남색
    ofDrawRectangle(0, ofGetHeight() - stageHeight, ofGetWidth(), stageHeight);

    // 캐릭터 그리기
    ofSetColor(129, 213, 255); // 파란색
    ofDrawRectangle(characterX, characterY, 50, 50); // 캐릭터 크기 50x50

    // 무기 그리기
    ofSetColor(254, 226, 101); // 노란색
    for (const auto& weapon : weapons) {
        weapon.draw();
    }

    // 공 그리기
    for (const auto& ball : balls) {
        ball.draw();
    }

    // 타이머 그리기
    int elapsedTime = (ofGetElapsedTimeMillis() - startTime) / 1000;
    if (gameFont.isLoaded()) {
        if (isGameRunning) {
            gameFont.drawString("Time: " + std::to_string(totalTime - elapsedTime), 10, 40);
        }
        else {
            // 게임 오버 메시지 그리기
            ofSetColor(255, 255, 0); // 노란색
            float textWidth = gameFont.stringWidth(gameResult);
            float x = ofGetWidth() / 2 - textWidth / 2;
            gameFont.drawString(gameResult, x, ofGetHeight() / 2);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == ' ') { // 스페이스바 눌렀을 때 무기 발사
        Weapon weapon;
        weapon.x = characterX + 20; // 무기의 x 위치 (캐릭터 중앙)
        weapon.y = characterY;
        weapons.push_back(weapon);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

// Weapon 클래스의 draw 메서드 정의
void ofApp::Weapon::draw() const {
    ofDrawRectangle(x, y, 10, 20); // 무기의 크기 10x20
}

// Ball 클래스의 draw 메서드 정의
void ofApp::Ball::draw() const {
    ofDrawCircle(x + 25, y + 25, 25); // 공의 반지름 25, 중심점을 맞추기 위해 x, y에 25를 더함
}

