#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

void FirstRow(vector<int>& maze, int width, int& id);
void VerticalWalls(vector<int>& vertical_walls, vector<int>& maze, int width);
void HorizontalWalls(vector<int>& horizontal_walls, vector<int>& maze, int width, int& id);
void update(vector<int>& maze, vector<int>& vertical_walls, int width);
void finalizeLastRow(vector<int>& maze, vector<int>& vertical_walls, int width);
void writeRow(ofstream& file, const vector<int>& walls, bool horizontal, int width);
void writeLine(ofstream& file, int width);
void writeFirstRow(ofstream& file, const vector<int>& vertical_walls, int width);

int main() {
    int WIDTH, HEIGHT;
    cout<<"width: ";
    cin >> WIDTH;
    cout<<"height: ";
    cin>> HEIGHT;

    ofstream file("maze.maz");

    if (!file.is_open()) {
        cerr << "Error: can't open file" << endl;
        return 1;
    }

    srand(time(NULL)); 

    vector<int> maze(WIDTH, 0);
    vector<int> vertical_walls(WIDTH - 1, 0);
    vector<int> horizontal_walls(WIDTH, 0);

    int id = 1;

    //첫번째 행 초기화
    FirstRow(maze, WIDTH, id);
    VerticalWalls(vertical_walls, maze, WIDTH); 
    
    //첫번째 행 작성
    writeLine(file, WIDTH); 
    writeFirstRow(file, vertical_walls, WIDTH);

    for (int i = 1; i < HEIGHT - 1; i++) {
        HorizontalWalls(horizontal_walls, maze, WIDTH, id);
        writeRow(file, horizontal_walls, true, WIDTH);
        VerticalWalls(vertical_walls, maze, WIDTH);
        writeRow(file, vertical_walls, false, WIDTH);
        update(maze, vertical_walls, WIDTH);
    }

    //마지막 행 끝내기, 작성
    HorizontalWalls(horizontal_walls, maze, WIDTH, id);
    writeRow(file, horizontal_walls, true, WIDTH);
    finalizeLastRow(maze, vertical_walls, WIDTH);
    writeRow(file, vertical_walls, false, WIDTH);
    writeLine(file, WIDTH);

    cout<<"result file: maze.maz"<<endl;
    file.close();
    return 0;
}

void FirstRow(vector<int>& maze, int width, int& id) {
    //첫번째 행을 집합 ID로 초기화
    for (int i = 0; i < width; i++) {
        maze[i] = id++;
    }
}

void VerticalWalls(vector<int>& vertical_walls, vector<int>& maze, int width) {
    //세로 벽 설정
    for (int i = 0; i < width - 1; i++) {
        if (maze[i] != maze[i + 1] && rand() % 2 == 0) {
            vertical_walls[i] = 0;
            int old_set = maze[i + 1];
            for (int j = 0; j < width; j++) {
                if (maze[j] == old_set) {
                    maze[j] = maze[i];
                }
            }
        } else {
            vertical_walls[i] = 1;
        }
    }
}

void HorizontalWalls(vector<int>& horizontal_walls, vector<int>& maze, int width, int& id) {
    //가로 벽 설정
    vector<bool> set_has_open_cell(width, false);

    for (int i = 0; i < width; i++) {
        if (rand() % 2 == 0) {
            horizontal_walls[i] = 0;
            set_has_open_cell[maze[i]] = true;
        } else {
            horizontal_walls[i] = 1;
        }
    }

    for (int i = 0; i < width; i++) {
        if (!set_has_open_cell[maze[i]]) {
            horizontal_walls[i] = 0;
            set_has_open_cell[maze[i]] = true;
        }
    }

    for (int i = 0; i < width; i++) {
        if (horizontal_walls[i] == 1) {
            maze[i] = id++;
        }
    }
}

void update(vector<int>& maze, vector<int>& vertical_walls, int width) {
    //세로 벽 업데이트 후 집합을 합친다
    for (int i = 0; i < width - 1; i++) {
        if (vertical_walls[i] == 0) {
            if (maze[i] != maze[i + 1]) {
                int old_set = maze[i + 1];
                for (int j = 0; j < width; j++) {
                    if (maze[j] == old_set) {
                        maze[j] = maze[i];
                    }
                }
            }
        }
    }
}

void finalizeLastRow(vector<int>& maze, vector<int>& vertical_walls, int width) {
    //마지막 행을 하나의 집합으로 합친다
    for (int i = 0; i < width - 1; i++) {
        if (maze[i] != maze[i + 1]) {
            vertical_walls[i] = 0;
            int old_set = maze[i + 1];
            for (int j = 0; j < width; j++) {
                if (maze[j] == old_set) {
                    maze[j] = maze[i];
                }
            }
        } else {
            vertical_walls[i] = 1;
        }
    }
}

void writeRow(ofstream& file, const vector<int>& walls, bool horizontal, int width) {
    if (horizontal) {
        //행을 작성
        file << "+";
        for (int i = 0; i < width; i++) {
            if (walls[i] == 1) {
                file << "-+";
            } else {
                file << " +";
            }
        }
        file << "\n";
    } else {
        file << "|";
        for (int i = 0; i < width - 1; i++) {
            file << " ";
            if (walls[i] == 1) {
                file << "|";
            } else {
                file << " ";
            }
        }
        file << " |\n";
    }
}

void writeLine(ofstream& file, int width) {
    //라인을 작성
    file << "+";
    for (int i = 0; i < width; i++) {
        file << "-+";
    }
    file << "\n";
}

void writeFirstRow(ofstream& file, const vector<int>& vertical_walls, int width) {
    //첫번째 행을 작성
    file << "|"; 
    for (int i = 0; i < width - 1; i++) {
        file << " ";
        if (vertical_walls[i] == 1) {
            file << "|";
        } else {
            file << " ";
        }
    }
    file << " |\n";
}

