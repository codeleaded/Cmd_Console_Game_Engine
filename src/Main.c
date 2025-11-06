#include "/home/codeleaded/System/Static/Library/ConsoleEngine.h"

Vec2 p = {50.0f,50.0f};
Vec2 e = {100.0f,100.0f};

void C_Setup(Console* c){

}
void C_Update(Console* c){
    if(Console_Stroke(c,'W').DOWN){
        p.y -= 50.0f * c->ElapsedTime;
    }
    if(Console_Stroke(c,'S').DOWN){
        p.y += 50.0f * c->ElapsedTime;
    }
    if(Console_Stroke(c,'A').DOWN){
        p.x -= 50.0f * c->ElapsedTime;
    }
    if(Console_Stroke(c,'D').DOWN){
        p.x += 50.0f * c->ElapsedTime;
    }

    e = Console_Mouse(c);

    Console_Clear(c,CPIXEL_SOLID,FG_BLACK);

    Console_RenderTriangleWire(c,e,(Vec2){30.0f,30.0f},p,(CPixel){CPIXEL_SOLID,FG_YELLOW},1.0f);
    Console_RenderTriangleWire(c,e,(Vec2){10.0f,50.0f},p,(CPixel){CPIXEL_SOLID,FG_BLUE},1.0f);
}
void C_Delete(Console* c){

}

int main(){
    Console c;
    if(Console_Create(&c,L"Console Game Test",200,150,8,8,C_Setup,C_Update,C_Delete))
        Console_Start(&c);
    return 0;
}