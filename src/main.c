#include "stm32f103xb.h"
#include "Actividad_Eval1.h"
#define sen1 1
#define sen2 2
#define normal 3
#define restringido 4 
#define libre 5
int lectura1, lectura2;
volatile char mode;
volatile int ocupados;
int lectura;
int main(void){
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    AFIO->EXTICR[0] &= ~(0xF << (lectura1 * 4));
    AFIO->EXTICR[1] &= ~(0xF << (lectura2 * 4));
    AFIO->EXTICR[2] &= ~(0xF << (normal * 4));
    AFIO->EXTICR[3] &= ~(0xF << (restringido * 4));
    AFIO->EXTICR[4] &= ~(0x0 << (libre%4 * 4)); 
    AFIO->EXTICR[4] |=  (0x0 << (libre%4 * 4)); 

    for(int i = 1; i < 5; i++){
        EXTI->IMR  |= (1 << i);
        EXTI->RTSR |= (1 << i);   
        EXTI->FTSR &= ~(1 << i);
    }
    
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI3_IRQn, 0); //el boton de restringido es el de mayor prioridad
    NVIC_SetPriority(EXTI2_IRQn, 1); //los botones de normal y libre tienen el mismo nivel de prioridad
    NVIC_SetPriority(EXTI4_IRQn, 1);
    NVIC_SetPriority(EXTI0_IRQn, 2);//los sensores estan un nivel debajo de los botones, ya que solo sensaran cuando el modo normal este activo
    NVIC_SetPriority(EXTI1_IRQn, 2);//PREGUNTA: ¿Se podria cambiar el nivel de prioridad de una interrupcion mediante, por ejemplo, un if? RTA: Si, se puede pero no es muy normal. Se recomienda no hacerlo. GetPriority sirve para eso tmb
    while (1){

       lectura1 = GPIOA -> IDR & (1 << sen1);
       lectura2 = GPIOA -> IDR & (1 << sen2);
       estMod(mode);
    }
    return 0;
    
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
// prueba subir git 