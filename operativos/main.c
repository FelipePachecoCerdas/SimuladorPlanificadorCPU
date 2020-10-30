#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>


#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080

struct paramsAuto {
    int maxBurst;
    int minBurst;
    int tasaMax;
    int tasaMin;
};


typedef struct paramsAuto* p_paramsAuto;

int masProcesos = 1;

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
        printf("\nConnection Failed \n");
        return -1;
    }

    // Envia la informacion
    send(sock , informacionProceso , 1024 , 0 );

    // Recibe le mensaje con el PID
    valread = read( sock , buffer, 1024);
    printf("\nMensaje del pthread con id %lu:\nSe ha enviado la siguiente informacion al servidor => %s\n%sEl hilo ha terminado su ejecucion\n", pthread_self(), informacionProceso, buffer );
}

void hacerProcesoManual(char *linea){
    sleep (2) ; //Espera 2 segundos para enviar la informacion
    crearClienteSocket(linea);
}

void ClienteManual (){

    char * linea = NULL;
    size_t len = 0, read;

    FILE* fp = fopen("/home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/entrada.txt", "r");
    if (fp == NULL)
        exit(1); // No existe el archivo

    while ((read = getline(&linea, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);

        pthread_t hiloProcesoNuevo;
        pthread_create(&hiloProcesoNuevo, NULL , hacerProcesoManual, (void *) linea);
        //pthread_join(hiloProcesoNuevo, NULL);

        int upper = 8, lower = 3;
        int num = (rand() % (upper - lower + 1)) + lower;
        sleep (num) ;
    }
    fclose(fp);
}

void crearProcesoAutomatico(p_paramsAuto parametros){
    int burst = (rand() % (parametros->maxBurst - parametros->minBurst + 1)) + parametros->minBurst;

    int upperPriori = 5, lowerPriori = 1;
    int prioridad = (rand() % (upperPriori - lowerPriori + 1)) + lowerPriori;

    char * str = malloc(1024);
    snprintf(str, 1024, "%i %i", burst, prioridad);
   // printf("%s", str);

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
        pthread_create(&hiloProcesoNuevo, NULL , crearProcesoAutomatico , parametros);
        //pthread_join(hiloProcesoNuevo, NULL);

        int tiempoDormir = (rand() % (parametros->tasaMax - parametros->tasaMin + 1)) + parametros->tasaMin;
        sleep(tiempoDormir);
    }
    printf("\nLa creacion de procesos de forma automatica se ha detenido\n");
}
#pragma clang diagnostic pop

int  main() {
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
