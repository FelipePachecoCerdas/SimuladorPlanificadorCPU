#include <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>
#include "malloc.h"
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#define TRUE   1
#define FALSE  0
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


struct proceso_PCB;

typedef struct proceso_PCB* enlace;

struct proceso_PCB {
    int pid, tiempoLlegada, tiempoFinalizacion, burst, burstOriginal, prioridad;
    enlace siguiente;
};

int pidActual = 0, segundoActual = 0,quantum;

int procesoIniciado=FALSE;

enlace listaProcesos = NULL;

pthread_t hiloJobScheduler, hiloCpuScheduler, hiloTimer;

enlace listaProcesosTerminados = NULL;

int opcionAlgoritmo;

int procesosTerminados = FALSE;
int ultimoProcesoImpreso = -1, cantidadDeEjecuciones = -1, cantidadSegundosCpuOcioso;

enlace procesoActual=NULL;
enlace procesoSiguiente=NULL;
enlace procesoAnterior=NULL;

int comenzado = FALSE;

void detenerSimulacion();

int borrarTemporal(){
    enlace listaProcesosIterar=listaProcesos;
    if(listaProcesos->pid==procesoActual->pid){
        procesoSiguiente=listaProcesos->siguiente;
        procesoAnterior=NULL;
        listaProcesos=listaProcesos->siguiente;
    }else{
        while (listaProcesosIterar){
            if(procesoActual->pid==listaProcesosIterar->siguiente->pid) {
                procesoAnterior=listaProcesosIterar;
                procesoSiguiente=listaProcesosIterar->siguiente->siguiente;
                listaProcesosIterar->siguiente = listaProcesosIterar->siguiente->siguiente;
                break;
            }
            listaProcesosIterar=listaProcesosIterar->siguiente;
        }
    }

    procesoActual->siguiente=NULL;
}

int *algoritmoFifo(int *arreglo){
    procesoActual = (enlace) listaProcesos;
    arreglo[0] = procesoActual->pid;
    arreglo[1] = procesoActual->burst;
    procesoActual->burst=0;
    return arreglo;
}

int *algoritmoSJF(int *arreglo){
    procesoActual=listaProcesos;
    enlace listaProcesosIterar=listaProcesos->siguiente;
    while (listaProcesosIterar){
        if(procesoActual->burst>listaProcesosIterar->burst) {
            procesoActual = listaProcesosIterar;
        }
        listaProcesosIterar=listaProcesosIterar->siguiente;
    }
    arreglo[0] = procesoActual->pid;
    if(opcionAlgoritmo==3){
        arreglo[1] = 1;
        procesoActual->burst-=1;
    }else {
        arreglo[1] = procesoActual->burst;
        procesoActual->burst = 0;
    }
    return arreglo;
}

int *algoritmoHPF(int *arreglo){
    procesoActual=listaProcesos;
    enlace listaProcesosIterar=listaProcesos->siguiente;
    while (listaProcesosIterar){
        if(procesoActual->prioridad>listaProcesosIterar->prioridad) {
            procesoActual = listaProcesosIterar;
        }
        listaProcesosIterar=listaProcesosIterar->siguiente;
    }

    arreglo[0] = procesoActual->pid;
    if(opcionAlgoritmo==5){
        arreglo[1] = 1;
        procesoActual->burst-=1;
    }else {
        arreglo[1] = procesoActual->burst;
        procesoActual->burst = 0;
    }
    return arreglo;
}

int *algoritmoRoundRobin(int *arreglo){

    if(procesoActual==NULL){
        procesoActual = (enlace) listaProcesos;
    }else {
        enlace listaProcesosIterar=listaProcesos;
        while (listaProcesosIterar) {
            if (procesoActual->pid == listaProcesosIterar->pid) {
                procesoActual = listaProcesosIterar->siguiente;
                break;
            }
            listaProcesosIterar = listaProcesosIterar->siguiente;
        }
        if(procesoActual==NULL){
            procesoActual = (enlace) listaProcesos;
        }
    }
    if(procesoActual->burst<=quantum){
        arreglo[1] = procesoActual->burst;
        procesoActual->burst=0;
    }else{
        arreglo[1] = quantum;
        procesoActual->burst-=quantum;
    }
    arreglo[0] = procesoActual->pid;
    return arreglo;
}
int * seleccionAlgoritmo(){
    int *infoProceso;
    int arreglo[2];
    switch (opcionAlgoritmo) {
        case 1://Algoritmo FIFO
            infoProceso = algoritmoFifo(arreglo);
            break;

        case 2://Algoritmo SJF
            infoProceso = algoritmoSJF(arreglo);
            break;

        case 3://Algoritmo SJF Apropiativo
            infoProceso = algoritmoSJF(arreglo);
            break;

        case 4://Algoritmo HPF
            infoProceso = algoritmoHPF(arreglo);
            break;

        case 5: //Algoritmo HPF Apropiativo
            infoProceso = algoritmoHPF(arreglo);
            break;

        case 6: //Algoritmo Round Robin
            infoProceso=algoritmoRoundRobin(arreglo);
            break;

    }
    return infoProceso;
}

void CpuScheduler(){
    segundoActual++;
    int *proceso_seleccionado;
    while(TRUE) {
        if (listaProcesos == NULL && procesosTerminados == TRUE) {
            detenerSimulacion();
        } else if (listaProcesos == NULL) {
            cantidadSegundosCpuOcioso++;
            printf("\n[%d] CPU Ocioso durante 1 segundo\n",segundoActual);
            sleep(1);
            enlace l = listaProcesosTerminados;
            while (l && l->siguiente) l = l->siguiente;
            if (listaProcesos && listaProcesos->tiempoLlegada == l->tiempoFinalizacion) cantidadSegundosCpuOcioso--;
            /*enlace l = listaProcesosTerminados;
            while (l->siguiente) l = l->siguiente;
            if (alguienNuevo && l->tiempoFinalizacion == l->siguiente->tiempoLlegada) cantidadSegundosCpuOcioso--;*/
        } else {
            proceso_seleccionado = seleccionAlgoritmo();

            if(opcionAlgoritmo==6){
                if(ultimoProcesoImpreso!=-1) {
                    printf("[%i] Proceso %i ejecutado por: %i segundos [quedan %d segundos]\n",segundoActual,ultimoProcesoImpreso,cantidadDeEjecuciones, procesoActual->burst);
                }
                ultimoProcesoImpreso = proceso_seleccionado[0];
                cantidadDeEjecuciones = proceso_seleccionado[1];
                printf("\n[%i] Proceso %d de burst %d [%d] y prioridad %d ejecutándose...\n",segundoActual-((!procesoActual->pid && segundoActual==1)?1:0), proceso_seleccionado[0], procesoActual->burst+proceso_seleccionado[1], procesoActual->burstOriginal, procesoActual->prioridad);
            }else {
                if (ultimoProcesoImpreso == -1) {
                    ultimoProcesoImpreso = proceso_seleccionado[0];
                    cantidadDeEjecuciones = proceso_seleccionado[1];
                    procesoIniciado = FALSE;
                } else if (ultimoProcesoImpreso == proceso_seleccionado[0]) {
                    cantidadDeEjecuciones += proceso_seleccionado[1];
                } else {
                    printf("[%i] Proceso %i ejecutado por: %i segundos [quedan %d segundos]\n",segundoActual,ultimoProcesoImpreso,cantidadDeEjecuciones, procesoActual->burst);
                    ultimoProcesoImpreso = proceso_seleccionado[0];
                    cantidadDeEjecuciones = proceso_seleccionado[1];
                    procesoIniciado = FALSE;
                }
                if (procesoIniciado == FALSE) {
                    printf("\n[%i] Proceso %d de burst %d [%d] y prioridad %d ejecutándose...\n",segundoActual-((!procesoActual->pid && segundoActual==1)?1:0), proceso_seleccionado[0], procesoActual->burst+proceso_seleccionado[1], procesoActual->burstOriginal, procesoActual->prioridad);
                    procesoIniciado = TRUE;
                }
            }
            borrarTemporal();
            sleep(proceso_seleccionado[1]);
            if (procesoActual->burst == 0) {
                printf("[%i] Proceso %i ejecutado por: %i segundos [quedan %d segundos]\n",segundoActual,ultimoProcesoImpreso,cantidadDeEjecuciones, procesoActual->burst);
                ultimoProcesoImpreso=-1;
                cantidadDeEjecuciones=-1;
                procesoActual->tiempoFinalizacion=segundoActual; //+ ((!procesoActual->pid) ? 1: 0);
                if (listaProcesosTerminados == NULL) {
                    listaProcesosTerminados = procesoActual;
                } else {
                    if(listaProcesosTerminados->pid>procesoActual->pid){
                        procesoActual->siguiente=listaProcesosTerminados;
                        listaProcesosTerminados=procesoActual;
                    }else {
                        enlace listaProcesosIterar = listaProcesosTerminados;
                        while (listaProcesosIterar->siguiente && listaProcesosIterar->siguiente->pid < procesoActual->pid) {
                            listaProcesosIterar = listaProcesosIterar->siguiente;
                        }
                        procesoActual->siguiente = listaProcesosIterar->siguiente;
                        listaProcesosIterar->siguiente = procesoActual;
                    }
                }
                procesoActual=procesoAnterior;
            } else {
                if(procesoAnterior==NULL){

                    enlace lista=listaProcesos;
                    listaProcesos=procesoActual;
                    listaProcesos->siguiente=lista;
                }else{
                    enlace listaProcesosIterar = listaProcesos;
                    while (listaProcesosIterar) {
                        if(listaProcesosIterar==procesoAnterior){
                            procesoSiguiente=listaProcesosIterar->siguiente;
                            listaProcesosIterar->siguiente=procesoActual;
                            listaProcesosIterar->siguiente->siguiente=procesoSiguiente;
                            break;
                        }
                        listaProcesosIterar = listaProcesosIterar->siguiente;
                    }
                }
            }
        }
    }
}

void Timer() {
    for (;;) {
        sleep(1);
        segundoActual++;
    }
}

enlace crearProcesoPCB(int pid, int burst, int prioridad){
    enlace nuevoProceso = malloc(sizeof(struct proceso_PCB));
    nuevoProceso->pid = pid;
    nuevoProceso->burstOriginal = nuevoProceso->burst = burst;
    nuevoProceso->prioridad = prioridad;
    nuevoProceso->tiempoLlegada = segundoActual;
    nuevoProceso->tiempoFinalizacion = 0;
    nuevoProceso->siguiente = NULL;
    return nuevoProceso;
};


int insertarProceso(char *informacion) {
    int burst, prioridad;
    sscanf(informacion, "%i %i", &burst, &prioridad);
    enlace nuevoProceso = crearProcesoPCB(pidActual++, burst, prioridad);

    if (listaProcesos == NULL){
        listaProcesos = nuevoProceso;
        return nuevoProceso->pid;
    }

    enlace listaProcesosIterar = listaProcesos;
    while (listaProcesosIterar->siguiente)
        listaProcesosIterar = listaProcesosIterar->siguiente;
    listaProcesosIterar->siguiente = nuevoProceso;
    return nuevoProceso->pid;
}

void JobScheduler(){
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] ,
            max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //a message
    char *message = malloc(1024);

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("\nEl servidor se ha iniciado y esta escuchando en el puerto %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Esperando conexiones cliente ...");

    while(TRUE) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("\n[%i] Nueva conexion de cliente con socket fd de %d, ip %s y puerto %d\n"
                   , segundoActual, new_socket , inet_ntoa(address.sin_addr) , ntohs
                    (address.sin_port));

            valread = read( new_socket , buffer, 1024);

            if (!strcmp (buffer,"FIN")) {
                procesosTerminados = TRUE;
            } else {
                int pidNuevoProceso = insertarProceso(buffer);
                printf("[%i] Nuevo proceso creado del cliente con PID %i\n", segundoActual, pidNuevoProceso);

                if (!comenzado) {
                    pthread_create(&hiloTimer, NULL, Timer, NULL);
                    pthread_create(&hiloCpuScheduler, NULL, CpuScheduler, NULL);
                    comenzado = TRUE;
                }

                //puts("Welcome message sent successfully");
                sprintf(message, "Se ha creado el proceso con PID %i para incluirlo en la simulacion\n",
                        pidNuevoProceso);
                //send new connection message
                if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                    perror("send");
                }

                //add new socket to array of sockets
                for (i = 0; i < max_clients; i++) {
                    //if position is empty
                    if (client_socket[i] == 0) {
                        client_socket[i] = new_socket;
                        //printf("Adding to list of sockets as %d\n" , i);

                        break;
                    }
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    //printf("Cliente con ip %s y puerto %d se ha desconectado \n" ,
                    //       inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                    //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }

}

void desplegarColaProcesos(enlace listaProcesosImprimir) {
    if (listaProcesosImprimir == NULL) {
        printf("Actualmente la cola de procesos se encuentra vacia\n");
        return;
    }

    enlace listaProcesosIterar = listaProcesosImprimir;

    printf((listaProcesosImprimir == listaProcesos) ? "\nCola de Procesos:\n": "\nCola de Procesos Terminados:\n");
    printf("%-20s %-20s %-20s %-20s %-20s %-20s\n", "PID (Identificador)", "Burst Actual", "Burst Original", "Prioridad", "Tiempo Llegada", "Tiempo Finalizacion");
    while (listaProcesosIterar) {
        printf("%-20i %-20i %-20i %-20i %-20i %-20i \n", listaProcesosIterar->pid,  listaProcesosIterar->burst,
               listaProcesosIterar->burstOriginal, listaProcesosIterar->prioridad,
               listaProcesosIterar->tiempoLlegada, listaProcesosIterar->tiempoFinalizacion);
        listaProcesosIterar = listaProcesosIterar->siguiente;
    }

    if (listaProcesosImprimir == listaProcesos && procesoActual) {
        printf("\nProceso Actual del CPU:\n");
        printf("%-20s %-20s %-20s %-20s %-20s %-20s\n", "PID (Identificador)", "Burst Actual", "Burst Original",
               "Prioridad", "Tiempo Llegada", "Tiempo Finalizacion");
        printf("%-20i %-20i %-20i %-20i %-20i %-20i \n", procesoActual->pid, procesoActual->burst,
               procesoActual->burstOriginal, procesoActual->prioridad,
               procesoActual->tiempoLlegada, procesoActual->tiempoFinalizacion);

    }
}

void detenerSimulacion() {
    // Se van a matar los hilos por seguridad
    pthread_cancel(hiloJobScheduler);
    pthread_cancel(hiloTimer);

    printf("\nSe ha detenido la simulacion del planificador de CPU\nA continuacion se muestra la siguiente informacion resumen de la simulacion\n\n");
    int cantidadProcesos = 0, acumuladoTAT = 0, acumuladoWT = 0;
    for (enlace listaProcesosIterar = listaProcesosTerminados; listaProcesosIterar; listaProcesosIterar = listaProcesosIterar->siguiente) cantidadProcesos++;
    printf("Cantidad de procesos ejecutados: %i\n", cantidadProcesos);
    printf("Cantidad de segundos con CPU ocioso: %i\n", cantidadSegundosCpuOcioso);
    printf("\nTabla resumen de procesos\n\n");
    printf("%-25s %-25s %-25s\n", "PID (Identificador)", "TAT (Turn Around Time)", "WT (Waiting Time)");
    for (enlace listaProcesosIterar = listaProcesosTerminados; listaProcesosIterar; listaProcesosIterar = listaProcesosIterar->siguiente) {
        int tat = listaProcesosIterar->tiempoFinalizacion - listaProcesosIterar->tiempoLlegada;
        int wt = tat - listaProcesosIterar->burstOriginal;
        acumuladoTAT += tat;
        acumuladoWT += wt;
        printf("%-25i %-25i %-25i\n", listaProcesosIterar->pid, tat, wt);
    }
    double promedioWT = (acumuladoWT * 1.0) / cantidadProcesos;
    printf("\nPromedio de Waiting Time: %f\n", promedioWT);
    double promedioTAT = (acumuladoTAT * 1.0) / cantidadProcesos;
    printf("\nPromedio de Turn Around Time: %f\n", promedioTAT);

    pthread_cancel(hiloCpuScheduler);
}

int main() {
    printf("!Bienvenido al Servidor del Simulador del Planificador de CPU!\n\n");

    printf("*** Opciones para Algoritmos del Simulador del Planificador de CPU ***\n");
    printf("Digite alguna de las siguientes opciones mediante su correspondiente digito para elegir el algoritmo\n");
    printf("1. FIFO (First In First Out)\n");
    printf("2. SJF (Shortest Job First)\n");
    printf("3. SJF Apropiativo (Shortest Job First)\n");
    printf("4. HPF (High Priority First)\n");
    printf("5. HPF Apropiativo (High Priority First)\n");
    printf("6. Round Robin\n");

    for(;;) {
        printf("Digite la opcion: ");
        scanf("%i", &opcionAlgoritmo);

        if (opcionAlgoritmo <= 0 || opcionAlgoritmo >= 7) {
            printf("Opcion incorrecta. Intentelo de nuevo.\n");
            continue;
        }

        if (opcionAlgoritmo == 6) {
            printf("Indique el quantum deseado para el algoritmo Round Robin: ");
            scanf("%i", &quantum);
        }
        break;

    }

    pthread_create(&hiloJobScheduler, NULL , JobScheduler , NULL);

    int opcionSimulador = 0;
    printf("\n ***Menu del Simulador del Planificador de CPU ***\n");
    printf("Digite alguno de las siguientes opciones en cualquier momento mediante su correspondiente digito para ejecutar la accion\n");
    printf("1. Consultar cola de procesos\n");
    printf("2. Detener simulacion\n");
    printf("3. Consultar lista de procesos terminados\n");
    for(;;) {
        scanf("%i", &opcionSimulador);
        if (opcionSimulador != 1 && opcionSimulador != 2 && opcionSimulador != 3) {
            printf("Opcion incorrecta. Intentelo de nuevo.\n");
            continue;
        }

        if (opcionSimulador == 1) desplegarColaProcesos(listaProcesos);
        else if (opcionSimulador == 2) detenerSimulacion();
        else desplegarColaProcesos(listaProcesosTerminados);

    }
    printf("\n");

    pthread_join(hiloJobScheduler, NULL);
    pthread_join(hiloCpuScheduler, NULL);
    pthread_join(hiloTimer, NULL);

    return 0;
}

#pragma clang diagnostic pop