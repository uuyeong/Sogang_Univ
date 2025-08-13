#include "str.h"
#include <iostream>
#include <string.h>
#include <cstring>

using namespace std;

//leng을 입력받아 공백 문자열을 만든다.
Str::Str(int leng){
    if(leng<0){ //예외 처리
        cerr<<"Error!"<<endl;
        len = 0;
    }
    else{
        len = leng;
        str = new char[len+1];
        for(int i=0;i<=leng;i++){
            str[i] = ' ';
        }
        str[len] = '\0';
    }
}

//string내용을 입력받아 초기화
Str::Str(char *neyong){
    if(neyong==NULL){//예외 처리
        cerr << "Error!"<<endl;
        len = 0;
        str = new char[1];
        str[0] = '\0';
    }
    else{
        len = strlen(neyong);
        str = new char[len +1];
        strcpy(str, neyong);

    }
}

//소멸자
Str::~Str(){
    delete[] str;
}

//string 길이 리턴
int Str::length(void){
    return len;
}

//string 내용 리턴
char* Str::contents(void){
    return str;
}

// string의 내용을 다른 string과 비교 (매개변수가 Str타입일 때)
int Str::compare(class Str& a){
    int tmp = strcmp(str, a.contents());
    return tmp;
}

// string의 내용을 다른 string과 비교 (매개변수가 char *타입일 때)
int Str::compare(const char *a){
    int tmp = strcmp(str, a);
    return tmp;
}

//입력받은 a의 내용을 복사
void Str::operator=(const char *a){
    delete[] str;
    if(a==NULL){ //예외 처리
        len = 0;
        str = new char[0];
        str[0] = '\0';
    }
    else{
        len = strlen(a);
        str = new char[len +1];
        strcpy(str, a);
    }
    //len = static_cast<int>(strlen(a));
}

//입력받은 a의 내용을 복사
void Str::operator=(class Str& a){
    delete[] str;
    len = a.length();
    str = new char[len +1];
    strcpy(str, a.contents());
}
