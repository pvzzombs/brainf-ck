#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

namespace patch{
    template <typename T> std::string to_string(const T& n){
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}
int maxCell = 255;
int minCell = 0;
int maxCellSize = 30000;
int minCellSize = 10;
int memorySize = maxCellSize;
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
vector <int> poop;
int pile;
const char * mapping = "0123456789abcdefghijklmnopqrstuvwxyz";

string padStringZeroes(string Num, int padlength){
    int length = int(Num.length());
    while(length < padlength)
    {
        Num = "0" + Num;
        length++;
    }
    return Num;
}

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

void coloredMessage(string msg, WORD colorA, WORD colorB){
    SetConsoleTextAttribute(hConsole, colorA);
    cout << msg;
    SetConsoleTextAttribute(hConsole, colorB);
}

void collectInput() {
    coloredMessage("\nInput>", 11, 15);
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
    SetConsoleTextAttribute(hConsole, 10);
    cout << "[Memory 0x";
    cout << padStringZeroes(decToRadix(n, 16), 4) << "] ";
    for (i = n; i < j; i++) {
        SetConsoleTextAttribute(hConsole, 14);
        cout << padStringZeroes(decToRadix(memory[i], 16), 2) << " ";
    }
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl;
}

bool loopOk(vector <token>& n, long length) {
    long i;
    long sizeOfPoop;
    for (i = 0; i < length; i++) {
        if (n[i].type == '[') {
            pile+=1;
            poop.push_back(1);
        } else if (n[i].type == ']') {
            pile-=1;
            poop.pop_back();
        }
    }
    sizeOfPoop = poop.size();
    if (pile == 0 && sizeOfPoop == 0) {
        return true;
    } else {
        return false;
    }
}

void pushToken(char Char, long NOC, vector <token>& n){
    token temp;
    temp.type = Char;
    temp.argument = NOC;
    n.push_back(temp);
}

void switchToken(char& Token, char& Char, long& NOC, bool& skipToken){
    switch (Token) {
        case '>':
            Char = '>';
            NOC += 1;
            break;
        case '<':
            Char = '<';
            NOC += 1;
            break;
        case '[':
            Char = '[';
            break;
        case ']':
            Char = ']';
            break;
        case '.':
            Char = '.';
            NOC += 1;
            break;
        case ',':
            Char = ',';
            NOC += 1;
            break;
        case '+':
            Char = '+';
            NOC += 1;
            break;
        case '-':
            Char = '-';
            NOC += 1;
            break;
        default:
            skipToken = true;
    }
}

void parse(string program) {
    long length = program.length();
    long numberOfOccurence = 0;
    char whatWeHave;
    bool skipToken;
    long i, pntr, cntr;
    for (i = 0; i < length; i++) {
        if (i < length - 1) {
            switchToken(program[i], whatWeHave, numberOfOccurence, skipToken);
            if(skipToken){
                skipToken = false;
                continue;
            }
            if (whatWeHave == '[' || whatWeHave == ']') {
                pushToken(whatWeHave, 0, tokens);
                numberOfOccurence = 0;
            } else if (whatWeHave != program[i + 1]) {
                pushToken(whatWeHave, numberOfOccurence, tokens);
                numberOfOccurence = 0;
            }
        } else {
            if (numberOfOccurence == 0) {
                pushToken(program[i], 1, tokens);
                numberOfOccurence = 0;
            } else {
                pushToken(whatWeHave, (numberOfOccurence+1), tokens);
                numberOfOccurence = 0;
            }
        }
    }

    //now parse the loop
    long tokensLength = tokens.size();
    if (!loopOk(tokens, tokensLength)) {
        coloredMessage("Details: Loop brackets are not in pair! \n", 10, 15);
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
                        tokens[i].argument = pntr;
                        tokens[pntr].argument = i;
                        break;
                    }
                    pntr += 1;
                }
            }
            //cout << tokens[i].type << " " << tokens[i].argument << endl;
        }
    }

    if (errorFlag) {
        coloredMessage("Parse error! \n", 12, 15);
    } else {
        coloredMessage("Parsed successfully! \n", 10, 15);
    }
}

void command(char Token, long Arg) {
    string msg;
    //char* apnd;
    long i;
    switch (Token) {
        case '>':
            for (i = 0; i < Arg; i++) {
                ++pnt;
                if (pnt > memorySize - 1) {
                    if (wrappedFlag) {
                        pnt = 0;
                    } else {
                        msg = "\nError: Unexpected overflow of memory.\n   at compile.tmp.token ";
                        msg.append(patch::to_string(cnt));
                        coloredMessage(msg, 12, 15);
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
                        msg = "\nError: Unexpected underflow of memory.\n   at compile.tmp.token ";
                        msg.append(patch::to_string(cnt));
                        coloredMessage(msg, 12, 15);
                        errorFlag = true;
                        break;
                    }
                }
            }
            break;
        case '+':
            for (i = 0; i < Arg; i++) {
                ++memory[pnt];
                if (memory[pnt] > maxCell) {
                    memory[pnt] = minCell;
                }
            }
            break;
        case '-':
            for (i = 0; i < Arg; i++) {
                --memory[pnt];
                if (memory[pnt] < minCell) {
                    memory[pnt] = maxCell;
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
            if (memory[pnt] == 0) {
                cnt = Arg;
            }
            break;
        case ']':
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
    coloredMessage(exeName + "\n", 11, 15);
    cout << "Usage: ";
    coloredMessage("[path of compiler] ", 11, 10);
    cout << "file ";
    coloredMessage("[Commands] \n", 14, 15);
    cout << "   or  ";
    coloredMessage("[path of compiler] ", 11, 14);
    cout << "[Options]" << endl << endl;
    /** COMMANDS **/
    cout << "Commands: " << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "  -d    --debug        Logs memory tape to the console  " << endl;
    cout << "  -i    --infinite     Infinite iterations              " << endl;
    cout << "  -m    --max          Set the maximum range of the cell" << endl;
    cout << "  -n    --min          Set the minimum range of the cell" << endl;
    cout << "  -w    --wrapped      Memory cells are wrapped/joined  " << endl;
    cout << "  -s    --size         Set the size of the memory tape  " << endl;
    /** OPTIONS **/
    coloredMessage("Options: \n", 14, 15);
    cout << "  -h    --help         See this help option             " << endl;
    coloredMessage("Exammple \n", 14, 15);
    cout << "  main file.b --infinite --wrapped --size 100 -m 65535  " << endl;
    cout << "  - sets the compiler w/ infinite tape & iterations, w/ cell size of 65535 and 100 real cells" << endl;
    cout << "  main file.b --debug                                   " << endl;
    cout << "  - sets the compiler in debug mode                 " << endl;

}

int main(int argc, char * argv[]) {
    SetConsoleTextAttribute(hConsole, 15);
    string nameExe = argv[0];

    bool waitingForOption = false;
    char waiterOfOption;
    if (argc < 2) {
        showHelp(nameExe);
    } else {
        if (argc > 2) {
            for (int i = 2; i < argc; i++) {
                if(waitingForOption){
                    waitingForOption = false;
                    switch (waiterOfOption){
                    case 's' :
                        memorySize = atoi(argv[i]);
                        if(memorySize < minCellSize || memorySize > maxCellSize){
                            memorySize = maxCellSize;
                        }
                        break;
                    case 'm' :
                        maxCell = atoi(argv[i]);
                        if(maxCell <= minCell){
                            maxCell = 255;
                        }
                    case 'n' :
                        minCell = atoi(argv[i]);
                        if(minCell >= maxCell){
                            minCell = 0;
                        }
                    }
                }
                if (0 == strcmp(argv[i], "-d") || 0 == strcmp(argv[i], "--debug")) {
                    debugFlag = true;
                } else if (0 == strcmp(argv[i], "-i") || 0 == strcmp(argv[i], "--infinite")) {
                    allowInfinite = true;
                } else if (0 == strcmp(argv[i], "-w") || 0 == strcmp(argv[i], "--wrapped")) {
                    wrappedFlag = true;
                } else if (0 == strcmp(argv[i], "-s") || 0 == strcmp(argv[i], "--size")) {
                    waitingForOption = true;
                    waiterOfOption = 's';
                } else if (0 == strcmp(argv[i], "-m") || 0 == strcmp(argv[i], "--max")) {
                    waitingForOption = true;
                    waiterOfOption = 'm';
                } else if (0 == strcmp(argv[i], "-n") || 0 == strcmp(argv[i], "--min")) {
                    waitingForOption = true;
                    waiterOfOption = 'n';
                }
            }
        }
        if(0 == strcmp(argv[1], "-h") || 0 == strcmp(argv[1], "--help")){
            showHelp(nameExe);
        }else{
            inFile.open(argv[1]);
            if (!inFile) {
                coloredMessage("Error: Unknown Command or File! \n", 12, 15);
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
