#include "./Console_Engine.h"

void Setup(){
}

void Update(void* c_void){
    Console* c = (Console*)c_void;
    SetColor(c,FOREGROUND_GREEN);
    RenderPoint(c,(Vf2d){ 0.0f,10.0f },L'@');
    RenderPoint(c,(Vf2d){ 2.0f,11.0f },L'@');
    RenderPoint(c,(Vf2d){ 4.0f,12.0f },L'@');
    RenderPoint(c,(Vf2d){ 6.0f,13.0f },L'@');
    RenderPoint(c,(Vf2d){ 8.0f,14.0f },L'@');
}

int main(){
    Console c = CreateConsole(160.0f,80.0f,Setup,Update);
    Start(&c);
    return 0;
}