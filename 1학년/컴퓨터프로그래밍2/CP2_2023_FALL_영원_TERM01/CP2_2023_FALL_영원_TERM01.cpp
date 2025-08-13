#include <iostream>
#include <cstdlib>
#include <ctime>
#include <png.h>
using namespace std;

const int imageWidth = 1200;   
const int imageHeight = 800;

void updateImage(const char *filename, int numBeads);
void drawBeads(png_bytep *row_pointers, int numBeads);
void drawCircle(png_bytep *row_pointers, int x, int y, int radius);
void saveImage(const char *filename, int width, int height, png_bytep *row_pointers);

void drawWin(png_bytep *row_pointers, int x, int y, int fontSize);
void drawLose(png_bytep *row_pointers, int x, int y, int fontSize);
void drawLine(png_bytep *row_pointers, int x1, int y1, int x2, int y2);


int main() {

    int width = 1200, height = 800;
    png_bytep *row_pointers = new png_bytep[height];
    for (int y = 0; y < height; y++)
        row_pointers[y] = new png_byte[width * 4];


    int playerbeads = 6;
    int computerbeads = 6;

    srand(static_cast<unsigned int>(time(nullptr)));

    while (playerbeads > 0 && playerbeads < 12) {

        updateImage("output.png", playerbeads);

        int computernumber = rand() % computerbeads + 1;   //computerNumber는 컴퓨터가 낸 랜덤값이다. 
        bool is_c_odd = (computernumber % 2 == 1);

    
        int playerchoice;

        do {
            cout << "홀수입니까? 짝수입니까? (홀수면 1, 짝수면 2를 입력하시오): ";
            cin >> playerchoice;
            
            if (playerchoice != 1 && playerchoice != 2) {
                cout << "1 또는 2를 입력해야합니다. 다시 입력해주세요." << endl;
                }
                
        } while (playerchoice != 1 && playerchoice != 2);


        if ((playerchoice % 2 == 1 && is_c_odd) || (playerchoice % 2 == 0 && !is_c_odd)) {
            playerbeads += computernumber;
            computerbeads -= computernumber;

            if (playerbeads >= 12) {
            cout << "당신은 모든 구슬을 얻었습니다!"<< endl;
            updateImage("output.png", playerbeads);
            break;
            } 
            else {cout << "이겼습니다!(현재 구슬 수: " << playerbeads << ") " << endl<<endl;
            updateImage("output.png", playerbeads);
            }



        } else {
            playerbeads -= computernumber;
            computerbeads += computernumber;
            
            
            if (playerbeads <= 0) {
            cout << "당신은 모든 구슬을 잃었습니다!" << endl;
            updateImage("output.png", playerbeads);
            break;
            }
            else {cout << "졌습니다!(현재 구슬 수: " << playerbeads << ") "<< endl<<endl;
            updateImage("output.png", playerbeads);
            }

        }

        int playerbet;
        do {
            cout << "구슬 몇 개를 걸겠습니까? (현재 구슬 수: " << playerbeads << "): ";
            cin >> playerbet;
            
            if (playerbet > playerbeads || playerbet <=0) {
                cout << "구슬 수가 잘못되었습니다. 다시 입력해주세요." << endl;
            }
                
        } while (playerbet > playerbeads || playerbet <=0);




        int computernumber_2 = rand() % 2;   //컴퓨터가 낸 랜덤값으로 0 또는 1를 출력한다. 
        
        if ((playerbet % 2 == computernumber_2)) {
            playerbeads -= playerbet;
            computerbeads += playerbet;
            
            if (playerbeads <= 0) {
            cout << "당신은 모든 구슬을 잃었습니다!" << endl;
            updateImage("output.png", playerbeads);
            break;
            }
            else {cout << "졌습니다!(현재 구슬 수: " << playerbeads << ") " << endl<<endl;
            updateImage("output.png", playerbeads);
            }

        } else {
            playerbeads += playerbet;
            computerbeads -= playerbet;
            
            if (playerbeads >= 12) {
            cout << "당신은 모든 구슬을 얻었습니다!" << endl;
            updateImage("output.png", playerbeads);
            break;
            } 
            else {cout << "이겼습니다!(현재 구슬 수: " << playerbeads << ") "<< endl<<endl;
            updateImage("output.png", playerbeads);
            }
        }

    }


    for (int y = 0; y < height; y++)
        delete[] row_pointers[y];
    delete[] row_pointers;

    return 0;
}



// 이미지 갱신 함수
void updateImage(const char *filename, int numBeads) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        cout << "File Error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        cout << "PNG Error!" << endl;
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        cout << "PNG Error!" << endl;
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    if (setjmp(png_jmpbuf(png))) {
        cout << "PNG Error!" << endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    png_init_io(png, fp);

    png_set_IHDR(
        png,
        info,
        imageWidth, imageHeight,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * imageHeight);
    for (int y = 0; y < imageHeight; y++) {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
    }

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth * 4; x += 4) {
            float gradient = static_cast<float>(y) / (imageHeight - 1);

            row_pointers[y][x] = static_cast<int>((1 - gradient) * 0 + gradient * 0);           
            row_pointers[y][x + 1] = static_cast<int>((1 - gradient) * 0 + gradient * 0);       
            row_pointers[y][x + 2] = static_cast<int>((1 - gradient) * 128 + gradient * 255);   
            row_pointers[y][x + 3] = 255;                                                    
        }
    }

    if (numBeads >= 12) {
        drawWin(row_pointers, imageWidth / 6, imageHeight / 3, imageHeight / 3);
    } else if (numBeads <= 0) {
        drawLose(row_pointers, imageWidth / 6, imageHeight / 3, imageHeight / 3);
    } else {
        drawBeads(row_pointers, numBeads);
    }
    saveImage(filename, imageWidth, imageHeight, row_pointers);

    for (int y = 0; y < imageHeight; y++) {
        free(row_pointers[y]);
    }

    free(row_pointers);

    fclose(fp);

    if (png && info) {
        png_destroy_write_struct(&png, &info);
    }
}

// Win 
void drawWin(png_bytep *row_pointers, int x, int y, int fontSize) {
    // 'W' 
    drawLine(row_pointers, x, y, x + fontSize / 4, y + fontSize);
    drawLine(row_pointers, x + fontSize / 4, y + fontSize, x + fontSize / 2, y);
    drawLine(row_pointers, x + fontSize / 2, y, x + 3 * fontSize / 4, y + fontSize);
    drawLine(row_pointers, x + 3 * fontSize / 4, y + fontSize, x + fontSize, y);

    // 'i' 
    drawLine(row_pointers, x + 5 * fontSize / 4 +60, y, x + 5 * fontSize / 4 +60, y + fontSize);

    // 'n' 
    drawLine(row_pointers, x + 2 * fontSize, y + fontSize, x + 2 * fontSize, y);
    drawLine(row_pointers, x + 2 * fontSize, y, x + 3 * fontSize, y + fontSize);
    drawLine(row_pointers, x + 3 * fontSize, y + fontSize, x + 3 * fontSize, y);
}


// Lose 
void drawLose(png_bytep *row_pointers, int x, int y, int fontSize) {
    // 'L' 
    drawLine(row_pointers, x, y, x, y + fontSize);          
    drawLine(row_pointers, x, y + fontSize, x + fontSize / 2, y + fontSize);  


    // 'o' 
    int startX_o = x + 2 * fontSize - 320;  
    int startY_o = y;
    int endY_o = y + fontSize;
    int width_o = fontSize / 2;  
    drawLine(row_pointers, startX_o, startY_o, startX_o, endY_o);
    drawLine(row_pointers, startX_o, startY_o, startX_o + width_o, startY_o);
    drawLine(row_pointers, startX_o, endY_o, startX_o + width_o, endY_o);
    drawLine(row_pointers, startX_o + width_o, startY_o, startX_o + width_o, endY_o);


 // 's' 
int startX_s = x + 3 * fontSize - 360;  
int startY_s = y;
int endY_s = y + fontSize;
drawLine(row_pointers, startX_s + fontSize / 2, startY_s, startX_s, startY_s);  
drawLine(row_pointers, startX_s, startY_s, startX_s, startY_s + fontSize / 2);
drawLine(row_pointers, startX_s, startY_s + fontSize / 2, startX_s + fontSize / 2, startY_s + fontSize / 2);
drawLine(row_pointers, startX_s + fontSize / 2, startY_s + fontSize / 2, startX_s + fontSize / 2, endY_s);
drawLine(row_pointers, startX_s + fontSize / 2, endY_s, startX_s, endY_s);



// 'e' 
int startX_e = x + 5 * fontSize - 655;  
int startY_e = y;
int endY_e = y + fontSize;
drawLine(row_pointers, startX_e, startY_e, startX_e, endY_e);
drawLine(row_pointers, startX_e, startY_e, startX_e + fontSize / 2, startY_e);  // 가로 길이를 현재의 절반으로 설정
drawLine(row_pointers, startX_e, startY_e + fontSize / 2, startX_e + fontSize / 2, startY_e + fontSize / 2);
drawLine(row_pointers, startX_e, endY_e, startX_e + fontSize / 2, endY_e);

}

void drawBeads(png_bytep *row_pointers, int numBeads) {
    const int circleRadius = 20;
    const int padding = 40;

    for (int i = 0; i < numBeads; i++) {
        int centerX = padding + i * (2 * circleRadius + padding);
        int centerY = imageHeight / 2;
        drawCircle(row_pointers, centerX, centerY, circleRadius);
    }
}

void drawCircle(png_bytep *row_pointers, int x, int y, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (x + dx >= 0 && x + dx < imageWidth && y + dy >= 0 && y + dy < imageHeight) {
                if (dx * dx + dy * dy <= radius * radius) {
                    
                    row_pointers[y + dy][4 * (x + dx)] = 255;      
                    row_pointers[y + dy][4 * (x + dx) + 1] = 255;  
                    row_pointers[y + dy][4 * (x + dx) + 2] = 255;  
                    row_pointers[y + dy][4 * (x + dx) + 3] = 255;  
                }
            }
        }
    }
}

void drawLine(png_bytep *row_pointers, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx, sy;

    if (x1 < x2) {
        sx = 1;
    } else {
        sx = -1;
    }

    if (y1 < y2) {
        sy = 1;
    } else {
        sy = -1;
    }

    int err = dx - dy;

    while (true) {
        if (x1 >= 0 && x1 < imageWidth && y1 >= 0 && y1 < imageHeight) {
            row_pointers[y1][4 * x1] = 255;      
            row_pointers[y1][4 * x1 + 1] = 255;  
            row_pointers[y1][4 * x1 + 2] = 255;  
            row_pointers[y1][4 * x1 + 3] = 255;  
        }

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err = err - dy;
            x1 = x1 + sx;
        }

        if (e2 < dx) {
            err = err + dx;
            y1 = y1 + sy;
        }
    }
}


void saveImage(const char *filename, int width, int height, png_bytep *row_pointers) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        cout << "Error!!" << endl;
        exit(EXIT_FAILURE);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        cout << "Error!!" << endl;
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        cout << "Error!!" << endl;
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    if (setjmp(png_jmpbuf(png))) {
        cout << "Error!!" << endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    png_init_io(png, fp);

    png_set_IHDR(
        png,
        info,
        width, height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    png_set_rows(png, info, row_pointers);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);

    fclose(fp);
    if (png && info) {
        png_destroy_write_struct(&png, &info);
    }
}


