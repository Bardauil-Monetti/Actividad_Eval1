#include "Actividad_Eval1.h"
#include "stm32f103xb.h"
#include <string.h>
int ledRojo = 8;
int ledVerde = 7;
int normal = 9;//boton
int restringido = 10;//boton
int libre = 11;//boton
int sen1 = 12;
int sen2 = 13;
int resultado, ocupados;
int max, uni, dec, cen, input;
char tecMat[4][4]={
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
int f1 = 0, f2 = 1, f3 = 2, f4 = 3;
int c1 = 4, c2 = 5, c3 = 6, c4 = 7;
void estInit(){
    RCC->APB2ENR|= RCC_APB2ENR_IOPBEN ; 
    GPIOA->CRL &= ~(0xF << (normal * 4)) | ~(0XF << (normal * 4) + 1) | ~(0XF << (normal * 4) + 2) | ~(0XF << (normal * 4) + 3);
    GPIOC->CRL &= ~(0xF << (restringido * 4)) | ~(0XF << (restringido * 4) + 1) | ~(0XF << (restringido * 4) + 2) | ~(0XF << (restringido * 4) + 3);
    GPIOA->CRH &= ~(0xF << (sen1 * 4)) | ~(0XF << (sen1 * 4) + 1) | ~(0XF << (sen1 * 4) + 2) | ~(0XF << (sen1 * 4) + 3);
    GPIOC->CRH &= ~(0xF << (sen2 * 4)) | ~(0XF << (sen2 * 4) + 1) | ~(0XF << (sen2 * 4) + 2) | ~(0XF << (sen2 * 4) + 3);
    GPIOD->CRL &= ~(0xF << (libre * 4)) | ~(0XF << (libre * 4) + 1) | ~(0XF << (libre * 4) + 2) | ~(0XF << (libre * 4) + 3);
    GPIOB->CRL &= ~(0xF << (ledVerde * 4)) | ~(0XF << (ledVerde * 4) + 1) | ~(0XF << (ledVerde * 4) + 2) | ~(0XF << (ledVerde * 4) + 3);
    GPIOB->CRH &= ~(0xF << (ledRojo * 4)) | ~(0XF << (ledRojo * 4) + 1) | ~(0XF << (ledRojo * 4) + 2) | ~(0XF << (ledRojo * 4) + 3);
    GPIOB->CRL |= (0b0010 << (ledRojo * 4)); //salida
    GPIOB->CRH |= (0b0010 << (ledVerde * 4)); //salida
    GPIOA->CRL |= (0b1000 << (normal * 4)); //entrada pull-down (porque me estoy fijando en el flanco de subida)
    GPIOC->CRL |= (0b1000 << (restringido * 4)); //entrada pull-down (porque me estoy fijando en el flanco de subida)
    GPIOA->CRH |= (0b1000 << (sen1 * 4)); //entrada pull-down (porque me estoy fijando en el flanco de subida)
    GPIOC->CRH |= (0b1000 << (sen2 * 4)); //entrada pull-down (porque me estoy fijando en el flanco de subida)
    GPIOD->CRL |= (0b1000 << (libre * 4)); //entrada pull-down (porque me estoy fijando en el flanco de subida)
}

char teclado(int f1, int f2, int f3, int f4, int c1, int c2, int c3, int c4){
    int filas[4] = {f1, f2, f3, f4};
    int columnas[4] = {c1, c2, c3, c4};
    for(int i = 0; i < 4; i++){
        GPIOB->BSRR |= (1 << filas[i]);
        for(int j = 0; i < 4; j++){
            if((GPIOB->IDR & (1 << columnas[j])) == 0){
                switch(input){
                    case 0:
                        uni = tecMat[i][j] * 1;
                        input += 1; 
                    break;
                    case 1:
                        dec = tecMat[i][j] * 10;
                        input += 1;
                    break;
                    case 2:
                        cen = tecMat[i][j] * 100;
                        input = 0;
                    break;
                }
                max = uni + dec + cen;
                if (ocupados > max){ //Si se ocupan todos los espacios, se bloquea la entrada a los nuevos
                    GPIOB->BSRR |= (1 << ledRojo);
                    GPIOB->BSRR |= ((1 << ledVerde) + 16);
                }
            }
        }
    }
}

void estMod(char modo){
    switch(modo){
        case 'l':
            GPIOB->BSRR |= (1 << ledVerde);
            GPIOB->BSRR |= ((1 << ledRojo) + 16);
        break;
        case 'r':
            GPIOB->BSRR |= (1 << ledRojo);
            GPIOB->BSRR |= ((1 << ledVerde) + 16);
        break;
        case 'n':
            teclado(f1, f2, f3, f4, c1, c2, c3, c4);
        break;
    }
    
}