#ifndef SD_H_
#define SD_H_

// Bibliotecas C usadas em arquivos sequenciais.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Bibliotecas XDCtools.
#include <xdc/std.h>
#include <xdc/runtime/System.h>

// Bibliotecas BIOS
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

// Bibliotecas TI-RTOS
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>

// Bibliotecas Board e rotinas( que define as tasks dos sistema ).
#include "Board.h"
#include "rotinas.h"

#endif
