#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>

void* LeerAbajoArriba(void *param){
    FILE *inFile = fopen("./text.txt","r");
    if (inFile == NULL){
        perror("open file fails: ");
        return(EXIT_FAILURE);
    }

    mqd_t mq = mq_open("/mq0", O_WRONLY);
    char str[64];

    char buffer[64];
    char *status =  NULL;
    u_int32_t renglones = 0;
    u_int32_t renglonesLeidos = 0;
    u_int32_t mitadRenglones = (renglones/2);

    do{
        status = fgets(buffer, sizeof(buffer),inFile);
        if(status != NULL){
            renglones++;
        }
    }while (status !=NULL);

    char cadena[mitadRenglones];
    
    do{
        status = fgets(buffer, sizeof(buffer),inFile);
        if(status != NULL){
            if (renglonesLeidos == mitadRenglones)
            {
                *cadena = buffer;
            }
            renglonesLeidos++;
        }
    }while (status !=NULL /*&& (renglonesLeidos!=mitadRenglones)*/);
    for (size_t i = 0; i < mitadRenglones; i++)
    {
        printf("A2: %d\n", cadena[i]);
        printf("\n");
        /*fgets(str, sizeof(str), inFile); despuès de la ùltima coma habìa un stdin
        if(str[strlen(str) - 1 ] == '\n') str[strlen(str) - 1 ] = 0;*/
        mq_send(mq, cadena[i], strlen(cadena) + 1, 0);
    }

    fclose(inFile);
    return NULL;
}

void* RecibirMensaje(void *param){
mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 32;
    attr.mq_curmsgs = 0;

    mq = mq_open("/mq1", O_RDONLY | O_CREAT, 0644, &attr);
    char buff[32];

    while (1)
    {
        mq_receive(mq, buff, 32, NULL);
        printf("A1: %s\n", buff);
        if( strncmp(buff, "exit", strlen("exit")) == 0){
            break;
        }
    }

    mq_close(mq);
    mq_unlink("/mq1");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    pthread_t thread1;
    pthread_create(&thread1,NULL,&LeerAbajoArriba,NULL);
    pthread_t thread2;
    pthread_create(&thread2,NULL,&RecibirMensaje,NULL);
    exit(EXIT_SUCCESS);
}