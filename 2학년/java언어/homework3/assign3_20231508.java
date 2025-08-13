import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.Timer;  // Timer 추가
import java.util.TimerTask;  // TimerTask 추가

public class assign3_20231508 extends Frame {

    // 공의 개수 설정
    final int NUM_BALLS = 5;
    ArrayList<Ball> balls = new ArrayList<>();
    double radius = 8;
    double speed = 4;
    public assign3_20231508() {
        // 윈도우 제목 설정
        setTitle("Ball Collision");

        // 윈도우 크기 설정
        setSize(700, 500);

        // 윈도우 종료 버튼 설정
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent we) {
                System.exit(0);
            }
        });
        // 창이 표시된 후 공 초기화
        addComponentListener(new ComponentAdapter() {
            @Override
            public void componentShown(ComponentEvent e) {
                initializeBalls();
            }
        });


        // 애니메이션을 위한 타이머 설정
        Timer timer = new Timer();
        timer.scheduleAtFixedRate(new TimerTask() {
            public void run() {
                moveBalls();
                checkCollisions();
                repaint();  // 화면을 갱신하도록 호출
            }
        }, 0, 5);  // 0ms 후 시작하여 10ms마다 반복
    }
    public void initializeBalls() {
        balls.clear(); // 기존 공 초기화
        int centerX = getWidth() / 2;
        int centerY = getHeight() / 2;

        // 공 생성
        balls.add(new Ball(centerX, centerY-2*radius, radius, 0, -1*speed));
        balls.add(new Ball(centerX+2*radius, centerY, radius, speed, 0));
        balls.add(new Ball(centerX-2*radius, centerY, radius, -1*speed, 0));
        balls.add(new Ball(centerX-2*radius, centerY+2*radius, radius, speed/Math.sqrt(2*speed*speed)*-1*speed, speed/Math.sqrt(2*speed*speed)*speed));
        balls.add(new Ball(centerX+2*radius, centerY+2*radius, radius, speed/Math.sqrt(2*speed*speed)*speed, speed/Math.sqrt(2*speed*speed)*speed));
    }

    // 공을 그리기 위한 paint() 메소드
    @Override
    public void paint(Graphics g) {
        Image offscreen = createImage(getWidth(), getHeight());
        Graphics offgc = offscreen.getGraphics();

        // 백그라운드 색상 설정
        offgc.setColor(Color.WHITE);
        offgc.fillRect(0, 0, getWidth(), getHeight());

        // 공 그리기
        for (Ball ball : balls) {
            offgc.setColor(Color.BLACK);
            offgc.fillOval((int) (ball.x - ball.radius), (int) (ball.y - ball.radius), (int) (ball.radius * 2), (int) (ball.radius * 2));
        }

        g.drawImage(offscreen, 0, 0, this);
    }

    // 공을 이동시키는 메소드
    public void moveBalls() {
        for (Ball ball : balls) {
            ball.x += ball.xSpeed;
            ball.y += ball.ySpeed;

            // 벽에 부딪히면 방향 반전
            if (ball.x - ball.radius < 0 || ball.x + ball.radius > getWidth()) {
                ball.xSpeed = -ball.xSpeed;
            }
            if (ball.y - ball.radius < 0 || ball.y + ball.radius > getHeight()) {
                ball.ySpeed = -ball.ySpeed;
            }
        }
    }
    // 공들이 충돌했는지 확인하는 메소드
    public void checkCollisions() {
    // 충돌 확인을 위한 리스트 복사 (ConcurrentModificationException 방지)
        ArrayList<Ball> newBalls = new ArrayList<>(); // 새로운 공을 저장할 리스트

        for (int i = 0; i < balls.size(); i++) {
            for (int j = i + 1; j < balls.size(); j++) {
                Ball ball1 = balls.get(i);
                Ball ball2 = balls.get(j);

            // 두 공 사이의 거리 계산
                double distance = Math.sqrt(Math.pow(ball1.x - ball2.x, 2) + Math.pow(ball1.y - ball2.y, 2));

            // 두 공이 충돌하면
                if (distance < ball1.radius + ball2.radius) {
                    // 공 리스트에서 삭제 (일단 추가용 리스트로 관리)
                    newBalls.addAll(splitBall(ball1)); // ball1을 분열하여 추가
                    newBalls.addAll(splitBall(ball2)); // ball2를 분열하여 추가

                    // 기존 공 제거
                    balls.remove(j); // 뒤쪽 공부터 삭제
                    balls.remove(i); // 앞쪽 공 삭제

                    j--; // 배열 크기가 줄어들므로 j 인덱스 조정
                    break; // 하나의 충돌만 처리하고 반복 종료
                }
            }
        }
        // 새로운 공 추가
        balls.addAll(newBalls);
    }
    // 공을 반으로 나누는 메서드
    private ArrayList<Ball> splitBall(Ball ball) {
        ArrayList<Ball> splitBalls = new ArrayList<>();

       // 반지름을 절반으로 줄임
        double newRadius = ball.radius / 2;

        // 최소 크기 확인
        if (newRadius >= 1) {
            // 랜덤한 속도로 두 개의 새로운 공 생성
            splitBalls.add(new Ball(ball.x+2*radius, ball.y+2*radius, newRadius, ball.xSpeed-ball.xSpeed+ball.ySpeed*0.5, -ball.ySpeed+ball.xSpeed*0.5));
            splitBalls.add(new Ball(ball.x, ball.y, newRadius, -ball.xSpeed-ball.ySpeed*0.5, -ball.ySpeed-ball.xSpeed*0.5));
        }
        return splitBalls;
    }


    // 공을 표현하는 내부 클래스
    class Ball {
        double x, y, radius, xSpeed, ySpeed;

        public Ball(double x, double y, double radius, double xSpeed, double ySpeed) {
            this.x = x;
            this.y = y;
            this.radius = radius;
            this.xSpeed = xSpeed;
            this.ySpeed = ySpeed;
        }
    }

    // 메인 메소드
    public static void main(String[] args) {
        // 새 객체를 생성하고 화면을 보이도록 설정
        assign3_20231508 b = new assign3_20231508();
        b.setVisible(true);  // 윈도우를 표시하도록 설정
    }
}