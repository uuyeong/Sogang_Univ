#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(30);
    ofSetWindowTitle("Yuyeong Pang");

    stageHeight = 50; // �������� ���� ����
    characterX = (ofGetWidth() / 2) - 25; // ĳ���� �ʱ� ��ġ ����
    characterY = ofGetHeight() - 50 - stageHeight;
    characterSpeed = 5.0f;

    weaponSpeed = 10.0f;

    ballSpeedY[0] = -18.0f;
    ballSpeedY[1] = -15.0f;
    ballSpeedY[2] = -12.0f;
    ballSpeedY[3] = -9.0f;

    // �� ���� �� app ������ �ʱ�ȭ
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

    // ĳ���� �̵�
    if (ofGetKeyPressed(OF_KEY_LEFT)) {
        characterX -= characterSpeed;
    }
    if (ofGetKeyPressed(OF_KEY_RIGHT)) {
        characterX += characterSpeed;
    }

    characterX = ofClamp(characterX, 0, ofGetWidth() - 50); // ĳ������ ũ��� 50x50���� ����

    // ���� �̵�
    for (auto& weapon : weapons) {
        weapon.y -= weaponSpeed;
    }
    weapons.erase(std::remove_if(weapons.begin(), weapons.end(),
        [](const Weapon& w) { return w.y < 0; }), weapons.end());

    // �� �̵� �� �浹 ó��
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

    // �浹 ó��
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

                // ���߾��� ���� �������� �ڽ� ���� ����
                if (balls[i].imgIndex < 3) {
                    auto ball = balls[i];
                    float newSize = 25; // ���� ���� ũ�� 25x25���� ����

                    // �ڽ� ������ �����Ͽ� balls ���Ϳ� �߰�
                    balls.push_back({ ball.x + 25 - newSize / 2,
                                      ball.y + 25 - newSize / 2,
                                      -3, -6, ball.imgIndex + 1, this });

                    balls.push_back({ ball.x + 25 - newSize / 2,
                                      ball.y + 25 - newSize / 2,
                                      3, -6, ball.imgIndex + 1, this });
                }

                // ���� ���� ����
                break; // ���� �ݺ����� ������ ���� ���� ó���� �� �ֵ��� ��
            }
        }

        // ����� ���� ����
        if (ballToRemove != -1) {
            balls.erase(balls.begin() + ballToRemove);
            ballToRemove = -1; // ballToRemove �ʱ�ȭ
        }
    }

    // ����� ���� ����
    if (weaponToRemove != -1) {
        weapons.erase(weapons.begin() + weaponToRemove);
        weaponToRemove = -1; // weaponToRemove �ʱ�ȭ
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
    ofBackground(0, 0, 0); // ������ ���������� ����

    // �������� �׸���
    ofSetColor(25, 36, 144); // ����
    ofDrawRectangle(0, ofGetHeight() - stageHeight, ofGetWidth(), stageHeight);

    // ĳ���� �׸���
    ofSetColor(129, 213, 255); // �Ķ���
    ofDrawRectangle(characterX, characterY, 50, 50); // ĳ���� ũ�� 50x50

    // ���� �׸���
    ofSetColor(254, 226, 101); // �����
    for (const auto& weapon : weapons) {
        weapon.draw();
    }

    // �� �׸���
    for (const auto& ball : balls) {
        ball.draw();
    }

    // Ÿ�̸� �׸���
    int elapsedTime = (ofGetElapsedTimeMillis() - startTime) / 1000;
    if (gameFont.isLoaded()) {
        if (isGameRunning) {
            gameFont.drawString("Time: " + std::to_string(totalTime - elapsedTime), 10, 40);
        }
        else {
            // ���� ���� �޽��� �׸���
            ofSetColor(255, 255, 0); // �����
            float textWidth = gameFont.stringWidth(gameResult);
            float x = ofGetWidth() / 2 - textWidth / 2;
            gameFont.drawString(gameResult, x, ofGetHeight() / 2);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == ' ') { // �����̽��� ������ �� ���� �߻�
        Weapon weapon;
        weapon.x = characterX + 20; // ������ x ��ġ (ĳ���� �߾�)
        weapon.y = characterY;
        weapons.push_back(weapon);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

// Weapon Ŭ������ draw �޼��� ����
void ofApp::Weapon::draw() const {
    ofDrawRectangle(x, y, 10, 20); // ������ ũ�� 10x20
}

// Ball Ŭ������ draw �޼��� ����
void ofApp::Ball::draw() const {
    ofDrawCircle(x + 25, y + 25, 25); // ���� ������ 25, �߽����� ���߱� ���� x, y�� 25�� ����
}

