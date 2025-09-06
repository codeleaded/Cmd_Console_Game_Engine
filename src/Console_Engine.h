#ifndef CONSOLE_ENGINE
#define CONSOLE_ENGINE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include <stdbool.h>

#define MAX_KEYS 41
#define MAX_BMOUSE 3

typedef struct Vf2d{
    float x;
    float y;
} Vf2d;

int RealKeys[] = { '0','1','2','3','4','5','6','7','8','9',
                   'A','B','C','D','E','F','G','H','I','J',
                   'K','L','M','N','O','P','Q','R','S','T',
                   'U','V','W','X','Y','Z',
                   VK_ESCAPE,VK_LSHIFT,VK_CONTROL,VK_SPACE,VK_TAB };

int RealMouse[] = { VK_LBUTTON,VK_MBUTTON,VK_RBUTTON };

typedef enum Keys{
    KEY_0 = 0,
    KEY_1 = 1,
    KEY_2 = 2,
    KEY_3 = 3,
    KEY_4 = 4,
    KEY_5 = 5,
    KEY_6 = 6,
    KEY_7 = 7,
    KEY_8 = 8,
    KEY_9 = 9,

    KEY_A = 10,
    KEY_B = 11,
    KEY_C = 12,
    KEY_D = 13,
    KEY_E = 14,
    KEY_F = 15,
    KEY_G = 16,
    KEY_H = 17,
    KEY_I = 18,
    KEY_J = 19,
    KEY_K = 20,
    KEY_L = 21,
    KEY_M = 22,
    KEY_N = 23,
    KEY_O = 24,
    KEY_P = 25,
    KEY_Q = 26,
    KEY_R = 27,
    KEY_S = 28,
    KEY_T = 29,
    KEY_U = 30,
    KEY_V = 31,
    KEY_W = 32,
    KEY_X = 33,
    KEY_Y = 34,
    KEY_Z = 35,

    KEY_ESC = 36,
    KEY_SHT = 37,
    KEY_CTR = 38,
    KEY_SPC = 39,
    KEY_TAB = 40
} Keys;

typedef enum Mouse{
    LBUTTON = 1,
    MBUTTON = 2,
    RBUTTON = 3
} Mouse;

typedef enum States{
    DOWN     = 0x01,
    UP       = 0x00,
    PRESSED  = 0x02,
    RELEASED = 0x04
} States;

typedef struct State{
    char s;
} State;

typedef struct Console{
    wchar_t* screen;
    HANDLE hConsole;
    DWORD dwBytesWritten;

    float ScreenWidth;
    float ScreenHeight;

    char Running;
    float ElapsedTime;

    void (*Setup)();
    void (*Update)(void*);

    State Keys[MAX_KEYS];
    State Mouse[MAX_BMOUSE];
    Vf2d MousePos;
} Console;

Console CreateConsole(float ScreenWidth,float ScreenHeight,void (*Setup)(),void (*Update)(void*)){
    Console c;
    c.Running = TRUE;
    c.ScreenWidth = ScreenWidth;
    c.ScreenHeight = ScreenHeight;
    c.Setup = Setup;
    c.Update = Update;
    c.screen = (wchar_t*)calloc((size_t)(c.ScreenWidth * c.ScreenHeight),sizeof(wchar_t));
	c.hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (!c.hConsole) {
		printf("Couldn't create ScreenBuffer!\n");
		c.Running = FALSE;
        free(c.screen);
		exit(0);
	}
	if (!SetConsoleActiveScreenBuffer(c.hConsole)) {
		printf("Couldn't set ScreenBuffer!\n");
		c.Running = FALSE;
		free(c.screen);
        exit(0);
	}
    
    return c;
}

void AddState(Console* c,State k,States s){
    k.s |= s;
}

void RemoveState(Console* c,State k,States s){
    k.s &= ~s;
}

BOOL GetKey(Console* c,Keys k,States s){
    return c->Keys[k].s & s;
}

BOOL GetMouse(Console* c,Mouse k,States s){
    return c->Mouse[k].s & s;
}

void Resets(Console* c){
    if(GetKey(c,KEY_Q,DOWN)){
        c->Running = FALSE;
    }
    
    for(int i = 0;i<MAX_KEYS;i++){
        RemoveState(c,c->Keys[i],PRESSED);
        RemoveState(c,c->Keys[i],RELEASED);

        if(GetAsyncKeyState(RealKeys[i]) & 0x8000){
            if(!(c->Keys[i].s & DOWN)){
                AddState(c,c->Keys[i],PRESSED);
            }
            AddState(c,c->Keys[i],DOWN);
        }else{
            if(c->Keys[i].s & DOWN){
                AddState(c,c->Keys[i],RELEASED);
            }
            RemoveState(c,c->Keys[i],DOWN);
        }
    }
    for(int i = 0;i<MAX_BMOUSE;i++){
        RemoveState(c,c->Mouse[i],PRESSED);
        RemoveState(c,c->Mouse[i],RELEASED);

        if(GetAsyncKeyState(RealMouse[i]) & 0x8000){
            if(!(c->Mouse[i].s & DOWN)){
                AddState(c,c->Mouse[i],PRESSED);
            }
            AddState(c,c->Mouse[i],DOWN);
        }else{
            if(c->Mouse[i].s & DOWN){
                AddState(c,c->Mouse[i],RELEASED);
            }
            RemoveState(c,c->Mouse[i],DOWN);
        }
    }
    
    POINT p;
    GetCursorPos(&p);
    c->MousePos.x = (float)p.x;
    c->MousePos.y = (float)p.y;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(c->hConsole, &csbi)) {
        COORD consoleCoord = csbi.dwCursorPosition;
        c->MousePos.x -= (float)consoleCoord.X;
        c->MousePos.y -= (float)consoleCoord.Y;
    }
}

void RenderPoint(Console* c,Vf2d p,wchar_t ch){
    if(p.x<0.0f||p.y<0.0f||p.x>=c->ScreenWidth||p.y>=c->ScreenHeight) return;
    c->screen[(int)((int)p.y * c->ScreenWidth + p.x)] = ch;
}

void SetColor(Console* c,char* col){
    swprintf_s(c->screen,5,L"%s",col);
}

void Repaint(Console* c){
    swprintf_s(c->screen, 20, L"FPS=%3.2f ", 1.0f / c->ElapsedTime);
    c->screen[(int)(c->ScreenWidth * c->ScreenHeight - 1)] = '\0';
    WriteConsoleOutputCharacterW(c->hConsole, c->screen, c->ScreenWidth * c->ScreenHeight, (COORD){ 0,0 }, &c->dwBytesWritten);
}

void Start(Console* c){
    clock_t tp1 = clock();
	c->Setup();

    while(c->Running){
        clock_t tp2 = clock();
		c->ElapsedTime = (float)(tp2-tp1)/CLOCKS_PER_SEC; 
		tp1 = tp2;
        Resets(c);
        c->Update(c);
        Repaint(c);
        Sleep(10);
    }

    free(c->screen);
}

#endif