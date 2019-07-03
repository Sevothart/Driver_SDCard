#ifndef OPERACOES_H_
#define OPERACOES_H_

//Definição de Handler para SDSPI chamados nas operações de montagem e desmontagem.
SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;

//Definição de ponteiro de arquivos usados nas operações de escrita e leitura.
FILE *fonte, *destino;

// MACRO DE CONVERSÃO DE STRING
#define STR_(n)             #n
#define STR(n)              STR_(n)
// Número do drive utilizado no FATFS
#define DRIVE_NUM           0

// Buffer utilizado na cópia de arquivos.
#define COPIA_BUFF_TAM       2048
unsigned char buffer_copia[COPIA_BUFF_TAM + 1];

// Variáveis para manter controle do procedimento de cópia de arquivos.
unsigned int bytesRead = 0;
unsigned int bytesWritten = 0;
unsigned int filesize;
unsigned int totalBytesCopied = 0;

// Texto padrão a ser injetado nos arquivos criados.
const char textopadrao[] = \
"######################################################################\n"
"Texto conteudo de inputfile, caso o arquivo nao exista no cartao SD.\n"
"Se ja existir, o arquivo nao sera modificado.\n"
"######################################################################\n";

const char csvarray[] = \
"Cidade, Estado, Habitantes,\n"
"Joinville, Santa Catarina, 50,\n"
"Curitiba, Parana, 150,\n"
"Salvador, Bahia, 200,\n";

void MountSD(void){

    //montar e registrar o cartão SD
    System_printf("Inicia a task para montar o cartão.\n");
    System_flush();

    SDSPI_Params_init(&sdspiParams);
    sdspiHandle = SDSPI_open(Board_SDSPI0, DRIVE_NUM, &sdspiParams);
    if (sdspiHandle == NULL) {
        System_abort("\tErro na inicialização do cartão SD.\n");
    }
    else {
        System_printf("\tDrive %u foi montado.\n", DRIVE_NUM);
    }
    System_flush();
}

int IsFile(const char* file){
    //vejo se o arquivo já existe no FATFS:(n)
    System_printf("\tInicia função para verificar se o arquivo existe.\n");
    System_flush();
    fonte = fopen(file, "rb");
    if(!fonte) {
        System_printf("\t\tNão existe arquivo chamado \"%s\" no cartão.\n", file);
        System_flush();
        return 0;
    }else{
        System_printf("\t\tExiste um arquivo chamado \"%s\" no cartão.\n", file);
        System_flush();
        return 1;
    }

}

void CreateFile(const char* file, const char textarray[] ){
    System_printf("Inicia a task para criar um arquivo.\n");
    System_flush();

    if( !IsFile(file) ){
        // se nao existe, eu crio um agora
        fonte = fopen(file, "wb+");
        if (!fonte) {
            // se mesmo assim eu nao conseguir criar, erro no sd
            System_printf("\tErro: \"%s\" não pode ser criado.\n"
                          "\tVerificar conexões e integridade do cartão SD.\n"
                          "\tSaindo...\n", file);
            System_flush();
        }else{

            // se nao existia, e criei, preencho com o textarray
            fwrite(textarray, 1, strlen(textarray), fonte);
            fflush(fonte);

            //volto o ponteiro para o inicio do arq
            rewind(fonte);
            fclose(fonte);
            System_printf("\tEscrevi no cartão SD\n");
            System_flush();

        }
    }else{
        System_printf("\tNão escrevi no cartão SD\n");
        System_flush();
    }
}

void ShowFileContent(const char* file){
    System_printf("Inicia a task que mostra o conteúdo de um arquivo.\n");
    System_flush();

    fonte = fopen(file, "rb");
    if (!fonte) {
        System_printf("\tArquivo inexistente \"%s\"\n"
                      "\tSaindo...\n", file);
        System_flush();
    }

    while (1) {
        bytesRead = fread(buffer_copia, 1, COPIA_BUFF_TAM, fonte);
        if (bytesRead == 0) {
            break; //Erro ou EOF
        }
        buffer_copia[bytesRead] = '\0';
        System_printf("\n%s\n", buffer_copia);
        System_flush();
    }
    fclose(fonte);
}

void RemoveFile(const char* file){

    System_printf("Inicia a task para remover um arquivo.\n");
    System_flush();
    if( !IsFile(file) ){
        System_printf("\tNão existe arquivo a ser removido.\n");
    }
    else{
        System_printf("\tExiste arquivo a ser removido.\n");
        System_flush();
        if( remove(file) == 0){
            System_printf("\tArquivo removido com sucesso.\n");
        }
        else{
            System_printf("\tArquivo não pode ser removido.\n");
        }
    }
    System_flush();
}

void CopyFile(const char* file_fonte, const char* file_destino){

    System_printf("Iniciando a task que copia o conteúdo de um arquivo em outro.\n");
    System_flush();

    //verifico se o arquivo fonte de cópia existe
    if( !IsFile(file_fonte) ){

        System_printf("\tO arquivo \"%s\" não existe para servir de fonte.\n"
                      "\tSaindo...\n", file_fonte);
        System_flush();

    }else{

        //crio o arquivo de destino
        destino = fopen(file_destino, "wb");
        if (!destino) {

            System_printf("\tErro na criação do novo arquivo destino \"%s\"\n"
                          "\tSaindo...\n", file_destino);
            System_flush();

        }else{
            System_printf("\tComeçando a cópia.\n");
            System_flush();

            //inicio o processo de cópia
            while (true) {
               //Read from source file
               bytesRead = fread(buffer_copia, 1, COPIA_BUFF_TAM, fonte);
               if (bytesRead == 0) {
                   break;
               }

               //Write to destino file
               bytesWritten = fwrite(buffer_copia, 1, bytesRead, destino);
               if (bytesWritten < bytesRead) {
                   System_printf("Cartão SD cheio (;-;).\n");
                   break;
               }

               //Update the total number of bytes copied
               totalBytesCopied += bytesWritten;
            }
            fflush(destino);

            //tamanho do arquivo fonte, para conferir se o arquivo completo foi copiado
            fseek(fonte, 0, SEEK_END);
            filesize = ftell(fonte);
            rewind(fonte);

            //fechar arquivo fonte e destino
            fclose(fonte);
            fclose(destino);

            System_printf("File \"%s\" (%u Bytes) copiados para \"%s\" (Escritos %u Bytes)\n",
                         file_fonte, filesize, file_destino, totalBytesCopied);
        }
    }
}

void UnmountSD(void){
    System_printf("Inicia a task para desmontar o cartão.\n");
    System_flush();

    SDSPI_close(sdspiHandle);
    System_printf("\tDrive %u foi desmontado.\n\n", DRIVE_NUM);
    System_flush();
}

#endif
