#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;

int memorySize = 30000;
int memory[30000];
int pnt = 0;
int cnt = 0;
int c;
string temp;
int open = 0;
int close = 0;
long prg;
unsigned long Max;
ifstream inFile;
string fileName;
string program;
string str;

unsigned long maxIterations = 4000000;
bool debugFlag = false;
bool allowInfinite = false;
bool errorFlag = false;
bool wrappedFlag = false;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

struct token {
    char type;
    long argument;
};
vector <token> tokens;
int pile;
const char * mapping = "0123456789abcdefghijklmnopqrstuvwxyz";

string decToRadix(int num, int radix) {
    string rem;
    int temp = 0;
    string result = "";
    while (num > 0) {
        temp = num % radix;
        result = mapping[temp] + result;
        num = num / radix;
    }
    if (result == "") {
        result = "0";
    }
    return result;
}

void collectInput() {
    SetConsoleTextAttribute(hConsole, 11);
    cout << endl << "Input>";
    SetConsoleTextAttribute(hConsole, 15);
    getline(cin, temp);
    if (temp[0] == '\n' || (temp[0] == '\r' && temp[1] == '\n')) {
        memory[pnt] = 0;
    } else {
        memory[pnt] = int(temp[0]);
    }
}

void printCells(int n) {
    int i;
    int j = n + 10;
    SetConsoleTextAttribute(hConsole, 14);
    cout << "{Memory View} ";
    SetConsoleTextAttribute(hConsole, 10);
    for (i = n; i < j; i++) {
        cout << "[ ";
        SetConsoleTextAttribute(hConsole, 14);
        cout << decToRadix(memory[i], 16) << " ";
        SetConsoleTextAttribute(hConsole, 12);
        cout << i;
        SetConsoleTextAttribute(hConsole, 10);
        cout << "]";
    }
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl;
}

bool loopOk(vector < token > & n, long length) {
    long i;
    for (i = 0; i < length; i++) {
        if (n[i].type == '[') {
            pile+=1;
        } else if (n[i].type == ']') {
            pile-=1;
        }
    }
    if (pile == 0) {
        return true;
    } else {
        return false;
    }
}

void parse(string program) {
    long length = program.length();
    long numberOfOccurence = 0;
    token temp;
    char whatWeHave;
    long i, pntr, cntr, luck;
    for (i = 0; i < length; i++) {
        if (i < length - 1) {
            switch (program[i]) {
                case '>':
                    whatWeHave = '>';
                    numberOfOccurence += 1;
                    break;
                case '<':
                    whatWeHave = '<';
                    numberOfOccurence += 1;
                    break;
                case '[':
                    whatWeHave = '[';
                    break;
                case ']':
                    whatWeHave = ']';
                    break;
                case '.':
                    whatWeHave = '.';
                    numberOfOccurence += 1;
                    break;
                case ',':
                    whatWeHave = ',';
                    numberOfOccurence += 1;
                    break;
                case '+':
                    whatWeHave = '+';
                    numberOfOccurence += 1;
                    break;
                case '-':
                    whatWeHave = '-';
                    numberOfOccurence += 1;
                    break;
                default:
                    continue;
            }
            if (whatWeHave == '[' || whatWeHave == ']') {
                temp.type = whatWeHave;
                temp.argument = 0;
                tokens.push_back(temp);
                numberOfOccurence = 0;
            } else if (whatWeHave != program[i + 1]) {
                temp.type = whatWeHave;
                temp.argument = numberOfOccurence;
                tokens.push_back(temp);
                numberOfOccurence = 0;
            }
        } else {
            if (numberOfOccurence == 0) {
                temp.type = program[i];
                temp.argument = 1;
                tokens.push_back(temp);
                numberOfOccurence = 0;
            } else {
                numberOfOccurence += 1;
                temp.type = whatWeHave;
                temp.argument = numberOfOccurence;
                tokens.push_back(temp);
                numberOfOccurence = 0;
            }
        }
    }

    //now parse the loop
    long tokensLength = tokens.size();
    if (!loopOk(tokens, tokensLength)) {
        SetConsoleTextAttribute(hConsole, 10);
        cout << "Details: Loop brackets are not pair! " << endl;
        SetConsoleTextAttribute(hConsole, 15);
        errorFlag = true;
    }
    if (!errorFlag) {
        for (i = 0; i < tokensLength; i++) {
            if (tokens[i].type == '[') {
                cntr = 1;
                pntr = i + 1;
                while (cntr && pntr < tokensLength) {
                    token tmp = tokens[pntr];
                    if (tmp.type == '[') {
                        cntr += 1;
                    } else if (tmp.type == ']') {
                        cntr -= 1;
                    }

                    if (cntr == 0) {
                        luck = pntr;
                        tokens[i].argument = luck;
                        tokens[luck].argument = i;
                        break;
                    }
                    pntr += 1;
                }
            }
            //cout << tokens[i].type << " " << tokens[i].argument << endl;
        }
    }

    if (errorFlag) {
        SetConsoleTextAttribute(hConsole, 12);
        cout << "Parse error! " << endl;
        SetConsoleTextAttribute(hConsole, 15);
    } else {
        SetConsoleTextAttribute(hConsole, 10);
        cout << "Parsed successfully! " << endl;
        SetConsoleTextAttribute(hConsole, 15);
    }
}

void command(char Token, long Arg) {
    long i;
    switch (Token) {
        case '>':
            for (i = 0; i < Arg; i++) {
                ++pnt;
                if (pnt > memorySize - 1) {
                    if (wrappedFlag) {
                        pnt = 0;
                    } else {
                        SetConsoleTextAttribute(hConsole, 12);
                        cout << endl << "Error: Unexpected overflow of memory." << endl;
                        cout << "   at compile > tmp > token " << cnt << endl;
                        SetConsoleTextAttribute(hConsole, 15);
                        errorFlag = true;
                        break;
                    }
                }
            }
            break;
        case '<':
            for (i = 0; i < Arg; i++) {
                --pnt;
                if (pnt < 0) {
                    if (wrappedFlag) {
                        pnt = memorySize - 1;
                    } else {
                        SetConsoleTextAttribute(hConsole, 12);
                        cout << endl << "Error: Unexpected underflow of memory." << endl;
                        cout << "   at compile > tmp > token " << cnt << endl;
                        SetConsoleTextAttribute(hConsole, 15);
                        errorFlag = true;
                        break;
                    }
                }
            }
            break;
        case '+':
            for (i = 0; i < Arg; i++) {
                ++memory[pnt];
                if (memory[pnt] > 255) {
                    memory[pnt] = 0;
                }
            }
            break;
        case '-':
            for (i = 0; i < Arg; i++) {
                --memory[pnt];
                if (memory[pnt] < 0) {
                    memory[pnt] = 255;
                }
            }

            break;
        case '.':
            for (i = 0; i < Arg; i++) {
                unsigned char output = memory[pnt];
                cout << output;
            }
            break;
        case ',':
            for (i = 0; i < Arg; i++) {
                collectInput();
            }
            break;
        case '[':
            //leftBracket();
            if (memory[pnt] == 0) {
                cnt = Arg;
            }
            break;
        case ']':
            //rightBracket();
            if (memory[pnt] > 0) {
                cnt = Arg;
            }
            break;
    }
}

void compile(bool isInfinite) {
    token tmp;
    if (isInfinite) {
        while (cnt < prg && !errorFlag) {
            tmp = tokens[cnt];
            command(tmp.type, tmp.argument);
            if (debugFlag) {
                printCells(pnt);
            }
            cnt++;
        }
    } else {
        while (cnt < prg && !errorFlag) {
            if (Max > maxIterations) {
                SetConsoleTextAttribute(hConsole, 12);
                cout << endl << "Error: Maximum program iterations exceeded." << endl;
                SetConsoleTextAttribute(hConsole, 15);
                errorFlag = true;
            }
            tmp = tokens[cnt];
            command(tmp.type, tmp.argument);
            if (debugFlag) {
                printCells(pnt);
            }
            Max++;
            cnt++;
        }
    }

    if (errorFlag) {
        SetConsoleTextAttribute(hConsole, 12);
        cout << endl << "An error occured. Exiting the program." << endl;
        SetConsoleTextAttribute(hConsole, 15);
    } else {
        SetConsoleTextAttribute(hConsole, 10);
        cout << "Compiled Successfully! " << endl;
        SetConsoleTextAttribute(hConsole, 15);
    }

}

void showHelp(string exeName) {
    cout << "Current path of brainfuck compiler: ";
    SetConsoleTextAttribute(hConsole, 11);
    cout << exeName << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "Usage: ";
    SetConsoleTextAttribute(hConsole, 11);
    cout << "[path of compiler] ";
    SetConsoleTextAttribute(hConsole, 10);
    cout << "file ";
    SetConsoleTextAttribute(hConsole, 14);
    cout << "[Commands] " << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "   or  ";
    SetConsoleTextAttribute(hConsole, 11);
    cout << "[path of compiler] ";
    SetConsoleTextAttribute(hConsole, 14);
    cout << "[Options]" << endl << endl;
    /** COMMANDS **/
    cout << "Commands: " << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "  -d    --debug        Logs memory tape to the console" << endl;
    cout << "  -i    --infinite     Infinite iterations            " << endl;
    cout << "  -w    --wrapped      Memory cells are wrapped/joined" << endl;
    /** OPTIONS **/
    SetConsoleTextAttribute(hConsole, 14);
    cout << "Options: " << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "  -h    --help         See this help option           " << endl;

}

int main(int argc, char * argv[]) {
    SetConsoleTextAttribute(hConsole, 15);
    string nameExe = argv[0];
    if (argc < 2) {
        showHelp(nameExe);
    } else {
        if (argc > 2) {
            for (int i = 2; i < argc; i++) {
                if (0 == strcmp(argv[i], "-d") || 0 == strcmp(argv[i], "--debug")) {
                    debugFlag = true;
                } else if (0 == strcmp(argv[i], "-i") || 0 == strcmp(argv[i], "--infinite")) {
                    allowInfinite = true;
                } else if (0 == strcmp(argv[i], "-w") || 0 == strcmp(argv[i], "--wrapped")) {
                    wrappedFlag = true;
                }
            }
        }
        if(0 == strcmp(argv[1], "-h") || 0 == strcmp(argv[1], "--help")){
            showHelp(nameExe);
        }else{
            inFile.open(argv[1]);
            if (!inFile) {
                SetConsoleTextAttribute(hConsole, 12);
                cout << "Error: Unknown Command or File! " << endl;
                SetConsoleTextAttribute(hConsole, 15);
            } else {
                while (getline(inFile, str)) {
                    program += str;
                }
                parse(program);
                prg = tokens.size();
                compile(allowInfinite);
            }
        }
    }
    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
