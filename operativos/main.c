#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>
#include  <time.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080

/*
Instituto Tecnológico de Costa Rica
Escuela de Ingeniería en Computación

Principios de Sistemas Operativos

Documentación del Primer Proyecto Programado: 
    Simulador de Planificador de CPU

Realizado por:
    Bernold Abarca Zúñiga, 2018113109
    Felipe Pacheco Cerdas, 2018114503
    Jeremy Tencio Morales, 2018087030

Profesora:
    Ericka Marín Shumann

Fecha de Entrega: 
    30/10/2020

II Semestre 2020
*/

struct paramsAuto {
    int maxBurst;
    int minBurst;
    int tasaMax;
    int tasaMin;
};


typedef struct paramsAuto* p_paramsAuto;

int masProcesos = 1, restante = 0, tiempo = -2, tiempoPrincipal = 0;

int crearClienteSocket(char *informacionProceso){
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nHa fallado la conexion con el servidor por lo que se detendra la creacion de mas procesos\n");
        masProcesos = 0;
        return -1;
    }

    // Envia la informacion
    send(sock , informacionProceso , 1024 , 0 );

    if (!strcmp (informacionProceso,"FIN")) {
        printf("\n---Mensaje del hilo con id %lu---\nSe ha enviado la siguiente informacion al servidor:\n%s\nSe ha terminado el envio de procesos y este hilo\n", pthread_self(), informacionProceso);
        return 0;
    }

    // Recibe le mensaje con el PID
    valread = read( sock , buffer, 1024);
    printf("\n---Mensaje del pthread con id %lu---\nSe ha enviado la siguiente informacion al servidor:\n%s\nEl servidor responde con:\n%s*El hilo ha terminado su ejecucion\n", pthread_self(), informacionProceso, buffer );
}

void avisarFinProcesos() {
    crearClienteSocket("FIN");
}

void hacerProcesoManual(char *linea){
    if (!masProcesos) return;
    tiempo+=restante+2;
    restante=tiempoPrincipal-2;
    printf("\n---Mensaje del pthread con id %lu---\nSoy el hilo con la informacion para enviar de %sEste hilo se va a dormir %i segundos antes de enviar los datos\n", pthread_self(),linea, 2);
    printf("\nEl servidor se encuentra en el segundo [%i] y la informacion llegara al servidor en el segundo [%i]\n",(tiempo-2 > 0) ? tiempo-2: 0 ,tiempo);
    sleep (2) ; //Espera 2 segundos para enviar la informacion
    crearClienteSocket(linea);
}

void ClienteManual (){

    char * linea = NULL;
    size_t len = 0, read;

    FILE* fp = fopen("/home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/entrada.txt", "r");
    if (fp == NULL)
        exit(1); // No existe el archivo

    while ((read = getline(&linea, &len, fp)) != -1 && masProcesos) {
        //printf("Retrieved line of length %zu:\n", read);

        pthread_t hiloProcesoNuevo;
        pthread_create(&hiloProcesoNuevo, NULL , hacerProcesoManual, (void *) linea);
        //pthread_join(hiloProcesoNuevo, NULL);

        int upper = 8, lower = 3;
        int num = (rand() % (upper - lower + 1)) + lower;

        printf("\n+++Mensaje del pthread con id %lu [PRINCIPAL]+++\nEl hilo se va a dormir %i segundos antes de crear el siguiente proceso\n", pthread_self(), num);
        sleep (num) ;
    }
    fclose(fp);
    avisarFinProcesos();
    printf("La creacion de procesos de forma manual se ha detenido\n");
}

void crearProcesoAutomatico(p_paramsAuto parametros){
    if (!masProcesos) return;
    int burst = (rand() % (parametros->maxBurst - parametros->minBurst + 1)) + parametros->minBurst;

    int upperPriori = 5, lowerPriori = 1;
    int prioridad = (rand() % (upperPriori - lowerPriori + 1)) + lowerPriori;
    char * str = malloc(1024);
    snprintf(str, 1024, "%i %i", burst, prioridad);
   // printf("%s", str);
    tiempo+=restante+2;
    restante=tiempoPrincipal-2;
    printf("\n---Mensaje del pthread con id %lu---\nSoy el hilo con la informacion para enviar de %s\nEste hilo se va a dormir %i segundos antes de enviar los datos\n", pthread_self(),str, 2);
    printf("\nEl servidor se encuentra en el segundo [%i] y la informacion llegara al servidor en el segundo [%i]\n",(tiempo-2 > 0) ? tiempo-2: 0 ,tiempo);
    sleep (2) ; //Espera 2 segundos para enviar la informacion
    crearClienteSocket(str);
}

p_paramsAuto obtenerParametrosAutomatico() {
    p_paramsAuto parametros = malloc(sizeof(struct paramsAuto));

    printf( "Ingrese el valor maximo para el Burst: ");
    scanf("%d",  &parametros->maxBurst);
    printf( "Ingrese el valor minimo para el Burst: ");
    scanf("%d",  &parametros->minBurst);
    printf( "Ingrese la duracion maxima, en segundos, para la tasa de creación: ");
    scanf("%d",  &parametros->tasaMax);
    printf( "Ingrese la duracion minima, en segundos, para la tasa de creación: ");
    scanf("%d",  &parametros->tasaMin);

    return parametros;
}

void detenerAutomatico() {
    int opcionDetener;
    for(;;) {
        scanf("%i", &opcionDetener);
        if (opcionDetener == 1) {
            masProcesos = 0;
            avisarFinProcesos();
            break;
        } else
            printf("Opcion incorrecta para detener procesos automaticos\n");
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
void ClienteAutomatico(){
    printf("- Como ha escogido el modo automatico de creacion de procesos, y este se ejecuta indefinidamente, debe presionar el numero 1 y ENTER para detenerlo.\n\n- Ademas, a continuacion se solicitan una serie de parametros para la aleatoriedad de los valores\n");

    p_paramsAuto parametros = obtenerParametrosAutomatico();

    pthread_t hiloDetenerAutomatico;
    pthread_create(&hiloDetenerAutomatico, NULL , detenerAutomatico , NULL);

    while (masProcesos) {

        pthread_t  hiloProcesoNuevo;
        int tiempoDormir = (rand() % (parametros->tasaMax - parametros->tasaMin + 1)) + parametros->tasaMin;
        tiempoPrincipal=tiempoDormir;
        pthread_create(&hiloProcesoNuevo, NULL , crearProcesoAutomatico , parametros);
        //pthread_join(hiloProcesoNuevo, NULL);

        printf("\n+++Mensaje del pthread con id %lu [PRINCIPAL]+++\nEl hilo se va a dormir %i segundos antes del crear el siguiente proceso\n", pthread_self(), tiempoDormir);
        sleep(tiempoDormir);
    }
    printf("\nLa creacion de procesos de forma automatica se ha detenido\n");
}
#pragma clang diagnostic pop

int  main() {
    // Semilla de aleatoriedad arbitraria para que cada
    // ejecución resulte distinta
    srand (time(NULL));

    printf("!Bienvenido al Cliente del Simulador del Planificador de CPU!\n\n");

    printf("*** Opciones para los Modos de Creacion de Procesos para el Simulador ***\n");
    printf("Digite alguna de las siguientes opciones mediante su correspondiente digito para elegir el modo de creacion de procesos\n");
    printf("1. Modo Manual\n");
    printf("2. Modo Automatico\n");

    int modo = 0;

    while (modo != 1 && modo != 2) {
        printf("Digite la opcion del modo que desee: ");
        scanf("%i", &modo);
        if (modo != 1 && modo != 2) printf("Opcion incorrecta. Intentelo de nuevo.\n");
    }

    if (modo == 1) ClienteManual();
    else ClienteAutomatico();

    return 0;
}
