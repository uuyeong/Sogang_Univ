#include "tetris.h"
#include <unistd.h>

static struct sigaction act, oact;

Node* head = NULL;
int rank_num = 0;
RecNode *mNode = NULL;


int main(){
    int exit=0;

    initscr();
    noecho();
    keypad(stdscr, TRUE);

    srand((unsigned int)time(NULL));
    createRankList();

    //추가한 내용
    recRoot = (RecNode*)malloc(sizeof(RecNode));
	recRoot->level = 0;
	for (int i = 0; i < HEIGHT; i++){
		for (int j = 0; j < WIDTH; j++){
			recRoot->recField[i][j] = field[i][j];
		}
	}
	recRoot->accumulatedScore = 0;


    while(!exit){
        clear();
        switch(menu()){
        case MENU_PLAY: play(); break;
        case MENU_RANK: rank(); break;
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
    nextBlock[2]=rand()%7; //두번째 다음 블럭의 ID를 임의로 생성한다.

    //10주차 추가 내용
    mNode = NULL;
    recommend(recRoot);


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
    DrawBox(10,WIDTH+10,4,8); //nextblock이 하나 더 생겼기 때문에 추가한다.

    /* score를 보여주는 공간의 태두리를 그린다.*/
    //next block 테투리가 하나 더 생기기 때문에 위치를 아래로 내려준다.
    move(17,WIDTH+10);
    printw("SCORE");
    DrawBox(18,WIDTH+10,1,8);
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
    case ' ':    /* space key*/
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
    move(19,WIDTH+11);
    printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
    int j;
    for(int i = 0; i < 4; i++ ){
        move(4+i,WIDTH+13);
        for( j = 0; j < 4; j++ ){
            if(!(block[nextBlock[1]][0][i][j])){
                printw(" ");

            }
            else{
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
        }
        
        move(11+i, WIDTH+13);

        for (j = 0; j < 4; j++){
            if (!(block[nextBlock[2]][0][i][j])){
                printw(" ");
            }
            else{
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
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
    int i, j;
    int prev_y = blockY;
    int prev_x = blockX;
    int prev_r = blockRotate;
    
    switch (command){
        case KEY_RIGHT:
            prev_x--;
            break;
        case KEY_LEFT:
            prev_x++;
            break;
        case KEY_DOWN:
            prev_y++;
            break;
        case KEY_UP:
            if (blockRotate) prev_r--;
            else prev_r = 3;
            break;
    }
    
    //2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            if (block[currentBlock][prev_r][i][j] == 1){
                if (i + prev_y >= 0 && i + prev_y <= HEIGHT){
                    move(i + prev_y + 1, j + prev_x + 1);
                    printw(".");
                }
            }
        }
    }


    //3. 새로운 블록 정보를 그린다.
    DrawField();
    DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
    DrawBox(0, 0, HEIGHT, WIDTH);

    move(HEIGHT, WIDTH + 11);
}

void BlockDown(int sig){
    // user code
    //강의자료 p26-27의 플로우차트를 참고한다.
    
    //블록을 한 칸 내릴 수 있을때 일어나는 수행이다.
    if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)){
        blockY++;
    }
    //만약 블록을 내릴 수 없고 블록의 y좌표가 –1이면, game over를 1로 갱신한다.
    else if (blockY == -1)
        gameOver = 1;
        //y좌표가 –1이 아니면, AddBlockToField() 함수를 사용하여 블록을 필드에 추가하고, 점수를 계산하여 누적하며, 필드 정보를 갱신하여 다시 그린다.
    else{
        score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        score += DeleteLine(field);
        for(int i=0;i<BLOCK_NUM-1;i++) {
                nextBlock[i]=nextBlock[i+1];
            }
        nextBlock[BLOCK_NUM-1]=rand()%7;
        blockRotate = 0;

        //10주차 추가 내용
        mNode = NULL;

        blockY = -1;
        blockX = WIDTH/2-2;
        DrawNextBlock(nextBlock);

        //10주차 추가 내용
        for (int i = 0; i < HEIGHT; i++){
			for (int j = 0; j < WIDTH; j++){
				recRoot->recField[i][j] = field[i][j];
			}
		}
        recommend(recRoot);


        PrintScore(score);
        DrawField();
    }
    DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
    timed_out = 0;

}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
    // user code

    //Block이 추가된 영역의 필드값을 바꾼다.
    //주어진 필드상의 위치에 현재 블록을 쌓는다.

    int touched = 0;

    for (int i = blockY; i < blockY + 4; i++){
        for (int j = blockX; j < blockX + 4; j++){
            if (block[currentBlock][blockRotate][i-blockY][j-blockX] == 1){
                f[i][j] = 1;
                if (f[i + 1][j] || i + 1 >= HEIGHT) touched=touched+1;
            }
        }
    }
    int result = touched * 10;
    return result;
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
    int i;
    char tile = '/';
    for (i = y; i <= HEIGHT-1; i++){
        if (!(CheckToMove(field, nextBlock[0], blockRotate, i, x))) break;
    }
    DrawBlock(i - 1, x, nextBlock[0], blockRotate, tile);
    
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    //기존의 DrawBlock() 함수의 위치에 삽입하여 움직임이 갱신될 때마다 현재 블록과 함께 그림자를 그린다.
    //DrawBlock(), DrawShadow() 함수를 호출한다.
    char tile = ' ';

    //10주차 구현 내용
    RecNode *p = mNode;
    for (int i = 0; i < VISIBLE_BLOCKS - 1; i++)
        p = p -> parent;
    DrawRecommend(p->recBlockY, p->recBlockX, p->curBlockID, p->recBlockRotate);


    DrawBlock(y, x, blockID, blockRotate, tile);
    DrawShadow(y, x, blockID, blockRotate);
}



void createRankList(){
    // 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성

    // 1. 파일은 연다.
    FILE *fp;
    fp = fopen("rank.txt", "r");
    Node *p;
    p = head;
    
    // 2. 정보를 읽어온다.
    if (fp == NULL){
        fp = fopen("rank.txt", "w");
    }
    else{
        fscanf(fp, "%d", &rank_num);
        for (int i = 0; i < rank_num; i++){
            // 3. LinkedList로 저장한다.
            Node* newNode = (Node*)malloc(sizeof(Node));
            fscanf(fp, "%s", newNode->name);
            fscanf(fp, "%d", &(newNode->score));
            newNode->link = NULL;
            if (!head){
                head = newNode;
                p = head;
            }
            else{
                p->link = newNode;
                p = newNode;
            }
        }
    }
    // 4. 파일을 닫는다.
    fclose(fp);
}


void rank(){
    // user code
    //목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
    int x = 1, i;
    int y = rank_num;
    int count = 0;
    int remove;
    char name[NAMELEN];
    Node *p, *d;
    p = head;
    d = p;

    clear();
    move(0,0);
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");

    switch(wgetch(stdscr)){
        case '1':
            echo();
            printw("X: ");
            scanw("%d", &x);
            printw("Y: ");
            scanw("%d", &y);
            noecho();

            printw("       name       |   score   \n");
            printw("------------------------------\n");
            if ( rank_num == 0 || x > y || x > rank_num || y > rank_num || x <= 0 || y <= 0 )
                printw("\nsearch failure: no rank in the list\n");
            else{
                for (i = 0; i < x - 1 && p != NULL; i++) {
                    p = p->link;
                }

                for (i = x; i <= y && p != NULL; i++) {
                    printw(" %-17s| %-10d\n", p->name, p->score);
                    p = p->link;
                }
            }
            break;
        case '2':
            printw("Input the name: ");
			echo();
			scanw("%s", name);
			noecho();
			printw("       name       |   score   \n");
			printw("------------------------------\n");

			while (p){
				if (strcmp(p->name, name) == 0){
					printw(" %-17s| %-10d\n", p->name, p->score);
					count++;
				}
				if ((!p->link))
					break;
				p = p->link;
			}

			if (count == 0)
				printw("\nsearch failure: no name in the list\n");
			break;

        case '3': 
            printw("Input the rank: "); 
            refresh(); 
            echo();
            scanw("%d", &remove);
            noecho();
            
            if (remove < 1 || remove > rank_num){
                printw("\nsearch failure: the rank not in the list");
            } 
            else {
                if (remove) head = p -> link;
                else {
                    for ( i = 1; i < remove - 1; i++){
                        p = p->link;
                    }
                    d = p->link;
                    p->link = d->link;
                }
                free(d);
                rank_num--;
                printw("\nresult: the rank deleted\n");
                writeRankFile();
            }
            break;


        default:
            break;
            
    }
    getch();

}

void writeRankFile(){
    FILE *fp;
    fp = fopen("rank.txt", "r");
    int cur_num;
    Node *p = head;
    
    fscanf(fp, "%d", &cur_num);
    if (cur_num == rank_num) return;
    fclose(fp);

    fp = fopen("rank.txt", "w");
    fprintf(fp, "%d\n", rank_num);
    while (p){
		fprintf(fp, "%s %d\n", p->name, p->score);
		p = p->link;
	}
    fclose(fp);
}

void newRank(int score){
    // user code
    // 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    Node* ptr = head;
    char newname[NAMELEN];

    clear();
    echo();
    move(0,0);
    printw("your name: ");
    getstr(newname);
    strcpy(newNode->name, newname);
    noecho();

    rank_num = rank_num + 1;
    newNode->score = score;
    newNode->link = NULL;

    if(head == NULL) head = newNode;
    else if (ptr->score < score){
        newNode->link = ptr;
        ptr = newNode;
        head = ptr;
    }
    else{
        while (ptr->link != NULL && ptr->link->score > score) {
            ptr = ptr->link;
        }
    
        newNode->link = ptr->link;
        ptr->link = newNode;
    }

    writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
    // user code
    DrawBlock(y, x, blockID, blockRotate, 'R');
}

int recommend(RecNode *root){
    int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	for (int i = 0; i < HEIGHT; i++){
		for (int j = 0; j < WIDTH; j++) recRoot->recField[i][j] = field[i][j];
	}

	int lv = root->level+1;
	int cur_ID = nextBlock[lv-1];
    int index = 0, delete_line = 0, touched = 0;
	int k, l, m, n, flag;

	RecNode *p;

	
	if (lv > VISIBLE_BLOCKS){
		if (!(mNode)) mNode = root;
		else if (root->accumulatedScore > mNode->accumulatedScore) mNode = root;
		return 0;
	}
    else if (lv < VISIBLE_BLOCKS + 1){
        root->child = (RecNode**)malloc(36*sizeof(RecNode*));
    }
		
	for (int i = -3; i < WIDTH; i++){
		for (int j = 0; j < 4; j++){
			if (CheckToMove(root->recField, cur_ID, j, 0, i)){
				p = root->child[index]=(RecNode*)malloc(sizeof(RecNode));
				root->child[index]->level = lv;
				root->child[index]->parent = root;

				for (k = 0; k < HEIGHT; k++)
					for (l = 0; l < WIDTH; l++)
						root->child[index]->recField[k][l] = root->recField[k][l];

				for (m = 0; m < HEIGHT; m++)
					if (!(CheckToMove(root->child[index]->recField, cur_ID, j, m + 1, i))) break;
				root->child[index]->curBlockID = cur_ID;
				root->child[index]->recBlockY = m;
                root->child[index]->recBlockX = i;
				root->child[index]->recBlockRotate = j;
	
				touched = 0;
				for (k = 0; k < BLOCK_HEIGHT; k++){
					for (l = 0; l < BLOCK_WIDTH; l++){
						if (block[cur_ID][j][k][l]){
							p->recField[m+k][i+l] = 1;
							if (m+k == (HEIGHT - 1)) touched=touched+1;
							else if (p->recField[m+k+1][i+l]) touched=touched+1;
						}
					}
				}

				delete_line = 0;

				for (k = 0; k < HEIGHT; k++){
					flag = 0;
					for (l = 0; l < WIDTH; l++){
						if (p->recField[k][l] == 1) flag++;
						else break;
					}
					if (flag == WIDTH) {
                        delete_line=delete_line+1;
                    }
				}
				p->accumulatedScore = root->accumulatedScore + (touched*10) + (delete_line*delete_line*100);
				recommend(root->child[index]);
				index = index+1;
			}
		}
	}
    return max;
}

void recommendedPlay(){
    // user code
}




