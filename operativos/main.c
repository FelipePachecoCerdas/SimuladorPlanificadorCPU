#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>


#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080

struct automatico {
    int maximo;
    int minimo;
    int tasaMax;
    int tasaMin;
};




int hacerSocket(char *hello ){
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock , hello , strlen(hello) , 0 );
    //printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );

}

void hacerProcesoManu(void *input){
    char *line;
    line = (char *) input;
    printf("%s", line);

    usleep (2000000) ; //Espera 2 segundos para enviar la informacion
    hacerSocket(line);
}

void clienteManual (){

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("/home/bernold/CLionProjects/operativos/entrada.txt", "r");
    if (fp == NULL)
        exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);

        pthread_t  h1;
        pthread_create(&h1, NULL , hacerProcesoManu , (void *) line);
        pthread_join(h1, NULL);

        int upper = 8;
        int lower = 3;
        int num = (rand() % (upper - lower + 1)) + lower;
        num = num * 1000000;
        usleep (num) ;

    }

    fclose(fp);
}

void crearProcesoAuto(void *input){
    int upperBurst = ((struct automatico*)input)->maximo;
    int lowerBurst = ((struct automatico*)input)->minimo;

    int burst = (rand() % (upperBurst - lowerBurst + 1)) + lowerBurst;

    int upperPriori = 5;
    int lowerPriori = 1;
    int prioridad = (rand() % (upperPriori - lowerPriori + 1)) + lowerPriori;

    char pr1[11];
    sprintf(pr1 , "%ld" , burst);
    char pr2[11];
    sprintf(pr2 , "%ld" , prioridad);


    char str[80];
    strcpy(str, pr1);
    strcat(str, " ");
    strcat(str, pr2);
    //strcat(str, "\n");
    printf("%s", str);
    char * pointer = &str[0];

    usleep (2000000) ; //Espera 2 segundos para enviar la informacion
    hacerSocket(pointer);

}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
void clienteAuto(void *input){
    int seguir = 1;

    int upperTasa = ((struct automatico*)input)->tasaMax;
    int lowerTasa = ((struct automatico*)input)->tasaMin;
    while(seguir){

        pthread_t  h1;
        pthread_create(&h1, NULL , crearProcesoAuto , (void *) input);
        pthread_join(h1, NULL);

        int dormir = (rand() % (upperTasa - lowerTasa + 1)) + lowerTasa;
        dormir = dormir * 1000000;
        usleep (dormir) ;

    }
}
#pragma clang diagnostic pop


int  main()
{
    int modo;

    printf( "Digite 1 si desea modo automatico o 0 para el modo manual: ");
    scanf("%d",  &modo);

    if (modo){
        int maxi;
        int mini;
        int creacionMax;
        int creacionMin;

        printf( "Ingrese el valor maximo para el Burst: ");
        scanf("%d",  &maxi);
        printf( "Ingrese el valor minimo para el Burst: ");
        scanf("%d",  &mini);
        printf( "Ingrese la duracion maximo, en segundos, para la tasa de creación: ");
        scanf("%d",  &creacionMax);
        printf( "Ingrese la duracion minima, en segundos, para la tasa de creación: ");
        scanf("%d",  &creacionMin);

        struct automatico *seleccionado = (struct automatico *)malloc(sizeof(struct automatico));
        seleccionado->maximo = maxi;
        seleccionado->minimo = mini;
        seleccionado->tasaMax = creacionMax;
        seleccionado->tasaMin = creacionMin;

        clienteAuto ( (void *) seleccionado);
    }
    else{
        clienteManual();
    }





    //pthread_t  h1;
    //pthread_t  h2;

    /*
    pthread_create(&h1, NULL , slowprintf , (void *) hola);
    pthread_create(&h2, NULL , clienteManual , NULL);
    //pthread_create(&h2, NULL , clienteAuto , (void *) seleccionado);
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    printf ("Fin\n");*/


    return 0;
}
