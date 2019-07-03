#ifndef ROTINAS_H_
#define ROTINAS_H_

#include "operacoes.h"

//Defini��o do tamanho das pilhas, e pilhas das tasks do sistema.
#define TASKSTACKSIZE       3000
Char mainMenuStack[TASKSTACKSIZE], Rotina1Stack[TASKSTACKSIZE];
Char Rotina2Stack[TASKSTACKSIZE], Rotina3Stack[TASKSTACKSIZE];

//Defini��o de estruturas e par�matros para as tasks do sistema.
Task_Struct mainMenuStruct, Rotina1Struct, Rotina2Struct, Rotina3Struct;
Task_Params mainMenuParams, Rotina1Params, Rotina2Params, Rotina3Params;

//Defini��o de par�metros e handler chamados em semaphores_pend and post no sistema.
Semaphore_Params Rotina1SemParams, Rotina2SemParams, Rotina3SemParams, mainMenuSemParams;
Semaphore_Handle Rotina1Sem, Rotina2Sem, Rotina3Sem, mainMenuSem;

//DEFINI��O DAS TASKS DO SISTEMA ABAIXO:

/*----------------------------------------------------------
  Rotina que monta o cart�o, verifica se o file "x" existe.
  Se existir, o deleta, e desmonta o cart�o.
  Se n�o existir, informa o user e desmonta o cart�o.
  Representada pelo LED vermelho.
 -----------------------------------------------------------*/
Void Rotina1(){

    Board_initGeneral();
    Board_initGPIO();
    Board_initSDSPI();

    while(1){

        Semaphore_pend(Rotina1Sem, BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED2, Board_LED_ON);

        char file_pre[] = "fat:"STR(DRIVE_NUM)":\0";
        char file_pos[] = "\0";

        MountSD();

        System_printf("Digite o nome do arquivo a ser removido: ");
        System_flush(); scanf("%s", file_pos);
        const char* file = strcat( file_pre , file_pos );

        RemoveFile(file);
        UnmountSD();

        Semaphore_post(mainMenuSem);
        GPIO_write(Board_LED2, Board_LED_OFF);

    }
}

/*-----------------------------------------------------------------------
  Rotina que monta o cart�o, verifica se o file "x" existe.
  Se existir, mostra o arquivo, e desmonta o cart�o.
  Se n�o existir, cria um arquivo "x" preenchendo-o com conte�do padr�o.
  Logo ap�s, mostra o conte�do do arquivo no console e desmonta o cart�o.
  Representada pelo LED verde.
-------------------------------------------------------------------------*/
Void Rotina2(){

    Board_initGeneral();
    Board_initGPIO();
    Board_initSDSPI();

    while(1){

        Semaphore_pend(Rotina2Sem, BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED1, Board_LED_ON);

        char file_pre[] = "fat:"STR(DRIVE_NUM)":\0";
        char file_pos[] = "\0";

        MountSD();

        System_printf("Digite o nome do arquivo a ser criado: ");
        System_flush(); scanf("%s", file_pos);
        const char* file = strcat( file_pre , file_pos );

        CreateFile(file, csvarray);
        ShowFileContent(file);

        UnmountSD();

        Semaphore_post(mainMenuSem);
        GPIO_write(Board_LED1, Board_LED_OFF);
    }
}

/*-----------------------------------------------------------------------
  Rotina que monta o cart�o, verifica se o file "x" existe.
  Se n�o existir, avisa o usu�rio e desmonta o cart�o
  Se existir, faz uma c�pia do arquivo e o salva com o nome "copia.bin"
  Logo ap�s, mostra o conteudo do arquivo no console e desmonta o cart�o.
  Representada pelo LED azul
-------------------------------------------------------------------------*/
Void Rotina3(){

    Board_initGeneral();
    Board_initGPIO();
    Board_initSDSPI();

    while(1){

        Semaphore_pend(Rotina3Sem, BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED0, Board_LED_ON);

        char fonte_pre[] = "fat:"STR(DRIVE_NUM)":\0";
        char fonte_pos[] = "\0";

        MountSD();

        System_printf("Digite o nome do arquivo fonte para a c�pia: ");
        System_flush(); scanf("%s", fonte_pos);
        const char* file = strcat( fonte_pre , fonte_pos );

        const char newfile[] = "fat:"STR(DRIVE_NUM)":copia.bin";

        CopyFile(file, newfile);
        ShowFileContent(file);

        UnmountSD();

        Semaphore_post(mainMenuSem);
        GPIO_write(Board_LED0, Board_LED_OFF);
    }
}

Void mainMenuTask(){

    int escolha;

    while(1){
        System_printf("----------------------------------------------------------------------------------------\n"
                      "|                          BEM VINDO AO DRIVER DE CART�O SD!!                          |\n"
                      "|                                                                                      |\n"
                      "| Segue descri��o das rotinas implementadas.                                           |\n"
                      "| #Rotina 1: Deleta um arquivo do cart�o SD.                                           |\n"
                      "| #Rotina 2: Cria um arquivo no cart�o SD.                                             |\n"
                      "| #Rotina 3: Copia o conte�do de um arquivo.                                           |\n"
                      "| #Rotina 4: Encerrar driver de cart�o SD.                                             |\n"
                      "|                                                                                      |\n"
                      "| Digite o n�mero de acordo com a opera��o desejada.                                   |\n"
                      "----------------------------------------------------------------------------------------\n"
                      "Input: ");

        System_flush(); scanf("%d", &escolha);

        if(escolha == 1){
            System_printf("\t\t\tINICIANDO A ROTINA 1.\n");
            System_flush();
            Semaphore_post(Rotina1Sem);
            Semaphore_pend(mainMenuSem, BIOS_WAIT_FOREVER);

        }else if(escolha == 2){
            System_printf("\t\t\tINICIANDO A ROTINA 2.\n");
            System_flush();
            Semaphore_post(Rotina2Sem);
            Semaphore_pend(mainMenuSem, BIOS_WAIT_FOREVER);

        }else if(escolha == 3){
            System_printf("\t\t\tINICIANDO A ROTINA 3.\n");
            System_flush();
            Semaphore_post(Rotina3Sem);
            Semaphore_pend(mainMenuSem, BIOS_WAIT_FOREVER);

        }else if(escolha == 4){
            System_printf("\t\t\tENCERRANDO O PROGRAMA.\n");
            System_flush();
            BIOS_exit(0);
        }
    }
}

void InterfaceSDInit(){
    Task_Params_init(&mainMenuParams);
    mainMenuParams.priority = 1;
    mainMenuParams.stackSize = TASKSTACKSIZE;
    mainMenuParams.stack = &mainMenuStack;
    Task_construct(&mainMenuStruct, (Task_FuncPtr)mainMenuTask, &mainMenuParams, NULL);

    Task_Params_init(&Rotina1Params);
    Rotina1Params.priority = 1;
    Rotina1Params.stackSize = TASKSTACKSIZE;
    Rotina1Params.stack = &Rotina1Stack;
    Task_construct(&Rotina1Struct, (Task_FuncPtr)Rotina1, &Rotina1Params, NULL);

    Task_Params_init(&Rotina2Params);
    Rotina2Params.priority = 1;
    Rotina2Params.stackSize = TASKSTACKSIZE;
    Rotina2Params.stack = &Rotina2Stack;
    Task_construct(&Rotina2Struct, (Task_FuncPtr)Rotina2, &Rotina2Params, NULL);

    Task_Params_init(&Rotina3Params);
    Rotina3Params.priority = 1;
    Rotina3Params.stackSize = TASKSTACKSIZE;
    Rotina3Params.stack = &Rotina3Stack;
    Task_construct(&Rotina3Struct, (Task_FuncPtr)Rotina3, &Rotina3Params, NULL);

    Semaphore_Params_init(&Rotina1SemParams);
    Rotina1Sem = Semaphore_create(0, &Rotina1SemParams, NULL);

    Semaphore_Params_init(&Rotina2SemParams);
    Rotina2Sem = Semaphore_create(0, &Rotina2SemParams, NULL);

    Semaphore_Params_init(&Rotina3SemParams);
    Rotina3Sem = Semaphore_create(0, &Rotina3SemParams, NULL);

    Semaphore_Params_init(&mainMenuSemParams);
    mainMenuSem = Semaphore_create(0, &mainMenuSemParams, NULL);
}

#endif
