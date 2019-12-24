#include <iostream>
#include <cstdlib>
#include <new>
#include <string>
#include <fstream>

using namespace std;

int memory[5000];
int pnt = 0;
int cnt = 0;
int c;
string temp;
int open = 0;
int close = 0;
int prg;
unsigned int Max;
ifstream inFile;
string fileName;
string program;
string str;

void collectInput(){
    cout << "\n$ Input : ";
    getline(cin,temp);
    if(temp[0] == '\n' || (temp[0] == '\r' && temp[1] == '\n')){
        memory[pnt] = 0;
    }else{
        memory[pnt] = int(temp[0]);
    }
}

void leftBracket(){
    if(memory[pnt] == 0){
    open = 0;
    cnt++;
        while(cnt < prg){
            if(program[cnt] == ']' && open == 0){
                break;
            }else if(program[cnt] == '['){
                open++;
            }else if(program[cnt] == ']'){
                open--;
            }
            cnt++;
        }
    }
}

void rightBracket(){
    if(memory[pnt] > 0){
        close = 0;
        cnt--;
        while(cnt >= 0){
            if(program[cnt] == '[' && close == 0){
                break;
            }else if(program[cnt] == ']'){
                close++;
            }else if(program[cnt] == '['){
                close--;
            }
            cnt--;
        }
    }
}
void compile(){
    while(cnt < prg){
        if(Max > 4000000){
            cout << "Error : Maximum iteration exceeded!!!" << endl;
            break;
        }
        if(program[cnt] == '>'){
            ++pnt;
            if(pnt == 100){
                cout << "Error : Off Tape !" << endl;
                break;
            }
        }else if(program[cnt] == '<'){
            --pnt;
            if(pnt < 0){
                cout << "Error : Off Tape !" << endl;
                break;
            }
        }else if(program[cnt] == '+'){
            ++memory[pnt];
            if(memory[pnt] > 255){
                memory[pnt] = 0;
            }
        }else if(program[cnt] == '-'){
            --memory[pnt];
            if(memory[pnt] < 0){
                memory[pnt] = 255;
            }
        }else if(program[cnt] == '.'){
            cout << char(memory[pnt]);
        }else if(program[cnt] == ','){
            collectInput();
        }else if(program[cnt] == '['){
            leftBracket();
        }else if(program[cnt] == ']'){
            rightBracket();
        }
        Max++;
        cnt++;
    }
}

int main()
{
    system("color 0b");
    cout << "$ Enter File Path: ";
    getline(cin, fileName);
    inFile.open(fileName.c_str());
    if(!inFile){
        cout << "Error : Cannot Open File !" << endl;
        system("pause");
        return 0;
    }
    while(getline(inFile, str)){
        program += str;
    }
    //cout << "$ ";
    //getline(cin, program);
    prg = program.length();
    //cout << program << endl;
    //cout << prg << endl;

    compile();

    system("pause");
    return 0;
}
