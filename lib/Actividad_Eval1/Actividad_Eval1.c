#include "Actividad_Eval1.h"
#include "stm32f103xb.h"
#include <string.h>
#include "ctype.h"
#define normal 3
#define restringido 4 
#define libre 5
int ledRojo = 8;
int ledVerde = 7;
int resultado;
int max, uni, dec, cen, input;
volatile int ocupados;
volatile char mode;
char tecMat[4][4]={
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
int f[4] = {0, 1, 2, 3};
int c[4] = {4, 5, 6, 7};
void estInit(){
    RCC->APB2ENR|= RCC_APB2ENR_IOPBEN ;
    for(int i = 0; i < 4; i++){
        // ponemos todos los puertos que vamos a usar en 0
        GPIOA->CRL &= ~(0XF << ((f[i] * 4)+ i));
        GPIOA->CRH &= ~(0XF << ((c[i] * 4)+ i));
        GPIOB->CRH &= ~(0XF << ((ledVerde * 4) + i)); 
        GPIOB->CRL &= ~(0XF << ((ledRojo * 4) + i));
        GPIOC->CRL &= ~(0XF << ((normal * 4) + i));
        GPIOC->CRL &= ~(0XF << ((restringido * 4) + i));
        GPIOC->CRL &= ~(0XF << ((libre * 4) + i));
    } 
    for(int i = 0; i < 4; i++){
        GPIOA->CRH |= (0b0100 << (f[i] * 4)); //aprovechamos para setear todas las filas y columnas como entradas flotantes
        GPIOA->CRH |= (0b0100 << (c[i] * 4));
    }
    GPIOC->CRL |= (0b0100 << (normal * 4)); //aprovechamos para setear todos los botones como entradas flotantes
    GPIOC->CRL |= (0b0100 << (restringido * 4)); 
    GPIOC->CRL |= (0b0100 << (libre * 4));
    GPIOB->CRL |= (0b0010 << (ledRojo * 4)); //salida
    GPIOB->CRH |= (0b0010 << (ledVerde * 4)); //salida
}

void teclado(int f1, int f2, int f3, int f4, int c1, int c2, int c3, int c4){
    int filas[4] = {f1, f2, f3, f4};
    int columnas[4] = {c1, c2, c3, c4};
    for(int i = 0; i < 4; i++){
        GPIOB->BSRR |= (1 << filas[i]);
        for(int j = 0; j < 4; j++){
            if(GPIOB->IDR & (1 << columnas[j]) == 0){
                switch(input){
                    case 0:
                        uni = (tecMat[i][j] - '0') * 1; //le resto '0' porque la variable me devuelve el codigo ASCII
                        input += 1; //por lo que, haciendo eso, obtengo el valor original (como si lo casteara)
                    break;
                    case 1:
                        dec = (tecMat[i][j] - '0') * 10;
                        input += 1;
                    break;
                    case 2:
                        cen = (tecMat[i][j] - '0') * 100;
                        input = 0;
                    break;
                }
                max = uni + dec + cen;
            }
        }
    }
}

int estMod(char modo){
    modo=tolower(modo);//hace al switch case insensible a las mayusculas/minusculas
    switch(modo){
        case 'l':
        //si esta libre, no se cuentan los autos que ingresan, por lo que solo prendo el led verde
            GPIOB->BSRR |= (1 << ledVerde);
            GPIOB->BSRR |= ((1 << ledRojo) + 16);
        break;
        case 'r':
        //si esta reservado, no puede entrar ningun auto, independientemente de la capacidad maxima
            GPIOB->BSRR |= (1 << ledRojo);
            GPIOB->BSRR |= ((1 << ledVerde) + 16);
        break;
        case 'n':
        //en modo normal, el usuario ingresa la capacidad maxima del estacionamiento
        //si se alcanza esa capacidad, el estacionamiento no admite mas autos
            teclado(f[1], f[2], f[3], f[4], c[1], c[2], c[3], c[4]);
            if(max <= ocupados){
                GPIOB->BSRR |= (1 << ledRojo);
                GPIOB->BSRR |= ((1 << ledVerde) + 16);
            }
            else{
                GPIOB->BSRR |= (1 << ledVerde);
                GPIOB->BSRR |= ((1 << ledRojo) + 16);
            }
            
        break;
    }
    
}
void EXTI0_IRQHandler(void){ //ISR del sensor 1
    if(EXTI->PR & (1 << 0)){ //Sí se cumple con la interrupcion, 
        ocupados++;
        EXTI->PR |= (1 << 0);
    }
}
void EXTI1_IRQHandler(void){//ISR del sensor 2
    if(EXTI->PR & (1 << 1)){ //Sí se cumple con la interrupcion, 
        ocupados--;
        EXTI->PR |= (1 << 1);
    }
}
void EXTI2_IRQHandler(void){//ISR del boton de normal
    if(EXTI->PR & (1 << 2)){
        mode = 'n'; 
        EXTI->PR |= (1 << 2);
    }
}
void EXTI3_IRQHandler(void){//ISR del boton de restringido
    if(EXTI->PR & (1 << 3)){
        mode = 'r'; 
        EXTI->PR |= (1 << 3);
    }
}
void EXTI4_IRQHandler(void){//ISR del boton de libre
    if(EXTI->PR & (1 << 4)){
        mode = 'l'; 
        EXTI->PR |= (1 << 4);
    }
}
