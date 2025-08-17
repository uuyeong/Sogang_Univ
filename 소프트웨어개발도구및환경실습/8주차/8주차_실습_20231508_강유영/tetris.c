#include "tetris.h"
#include <unistd.h>

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9,WIDTH+10);
	printw("SCORE");
	DrawBox(10,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(11,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
        
    //현재 필드의 상태, 현재 블록의 ID와 회전수, 이동할 좌표 입력받아 해당 블록이 정해진 위치로 이동할 수 있는지 없는지를 체크한다.
    int x, y;
    for(int i=0;i<4;i++) {
        for(int j=0;j<4;j++) {
            if(block[currentBlock][blockRotate][i][j]==1) {
                y=blockY+i;
                x=blockX+j;
                if(x<0||x>=WIDTH||y<0||y>=HEIGHT){
                    return 0;
                }
                // 블록을 놓으려고 하는 필드에 이미 블록이 쌓였는지 확인한다.
                if(f[y][x]==1) {
                    return 0;
                }
            }
        }
    }
    return 1;
    }


void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
    int prev_y=0;
    int prev_x=0;
    int prev_r=blockRotate;
        
    switch(command){
        case KEY_RIGHT:
            prev_x=-1;
            break;
        case KEY_LEFT:
            prev_x=1;
            break;
        case KEY_DOWN:
            prev_y=-1;
            break;
        case KEY_UP:
            prev_r=(blockRotate+3)%4;
            break;
    }
    
    
    //2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
    int t;
    t=blockY+1;
    while(CheckToMove(f, currentBlock, prev_r,++t, blockX)==1);
        
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(block[currentBlock][prev_r][i][j]==1){
                move(i+blockY+1+prev_y, j+blockX+1+prev_x);
                printw("%c", '.');
            }
        }
    }

    //3. 새로운 블록 정보를 그린다.
    DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
}

void BlockDown(int sig){
	// user code
	//강의자료 p26-27의 플로우차트를 참고한다.
    
    
    //블록을 한 칸 내릴 수 있을때 일어나는 수행이다.
    if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)) {
        blockY++;
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
    }
    else {
        
        AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        //만약 블록을 내릴 수 없고 블록의 y좌표가 –1이면, game over를 1로 갱신한다.
        if(blockY==-1) gameOver = 1;
        
        //y좌표가 –1이 아니면, AddBlockToField() 함수를 사용하여 블록을 필드에 추가하고, 점수를 계산하여 누적하며, 필드 정보를 갱신하여 다시 그린다.
        else {
            score+=DeleteLine(field);
            for(int i=0;i<BLOCK_NUM-1;i++) {
                nextBlock[i]=nextBlock[i+1];
            }
            nextBlock[BLOCK_NUM-1]=rand()%7;
            DrawNextBlock(nextBlock);

            blockY=-1;
            blockX=WIDTH/2-2;
            blockRotate=0;
            PrintScore(score);
        }
        DrawField();
    }
    timed_out=0;
}

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//Block이 추가된 영역의 필드값을 바꾼다.
    //주어진 필드상의 위치에 현재 블록을 쌓는다.
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            if(block[currentBlock][blockRotate][i][j]==1) f[blockY+i][blockX+j]=1;
        
    }
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
    int count;
    int stuff;
    int i,j,k;
    count=0;
    for(i =0;i< HEIGHT;i++) {
        stuff=1;
        for(j=0;j<WIDTH;j++) {
            if(f[i][j]==0) {
                stuff=0;
                break;
            }
        }
        //2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
        if(stuff){
            count++;
            for(k=i-1;k>=0;k--) {
                for(j=WIDTH;j>0;j--) {
                    f[k+1][j]=f[k][j];
                }
            }
        }
    }
    //지운 라인수에 대한 점수를 계산해 리턴한다.
    return count*count*100;
    
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
    
    //그림자의 타일
    char tile = '/';
    for (int i = y; i < HEIGHT; i++){
        if (!(CheckToMove(field, nextBlock[0], blockRotate, i, x))) break;
    }
    DrawBlock(i - 1, x, nextBlock[0], blockRotate, tile);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    //기존의 DrawBlock() 함수의 위치에 삽입하여 움직임이 갱신될 때마다 현재 블록과 함께 그림자를 그린다.
    //DrawBlock(), DrawShadow() 함수를 호출한다.
    
    RecNode *p = mNode;
    
    for (int i = 0; i < VISIBLE_BLOCKS - 1; i++) p = p -> parent;
    
    DrawRecommend(p->recBlockY, p->recBlockX, p->curBlockID, p->recBlockRotate);
    
    DrawBlock(y, x, blockID, blockRotate, ' ');
    DrawShadow(y, x, blockID, blockRotate);

}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}

