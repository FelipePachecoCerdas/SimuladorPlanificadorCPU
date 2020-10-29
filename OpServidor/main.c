#include <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>

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

struct proceso_PCB;

typedef struct proceso_PCB* enlace;

struct proceso_PCB {
    int pid, tiempoLlegada, tiempoFinalizacion, burst, burstOriginal, prioridad;
    enlace siguiente;
};

int pidActual = 0, segundoActual = 0;
enlace listaProcesos = NULL;

void CpuScheduler() {
    // algo
}

void Timer() {
    for (;;) {
        segundoActual++;
        sleep(1);
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
    char *message = "Mensaje recibido correctamente \r\n";

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
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
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
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n"
                   , new_socket , inet_ntoa(address.sin_addr) , ntohs
                    (address.sin_port));

            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }
            valread = read( new_socket , buffer, 1024);
            printf("K %s K\n",buffer );
            int pidNuevoProceso = insertarProceso(buffer);
            printf("Nuevo proceso creado de pid %i\n", pidNuevoProceso);

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
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
                    printf("Host disconnected , ip %s , port %d \n" ,
                           inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

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

void desplegarColaProcesos() {
    enlace listaProcesosIterar = listaProcesos;
    while (listaProcesosIterar) {
        printf("%i %i %i %i %i %i \n", listaProcesosIterar->pid, listaProcesosIterar->tiempoLlegada,
               listaProcesosIterar->tiempoFinalizacion, listaProcesosIterar->burst,
               listaProcesosIterar->burstOriginal, listaProcesosIterar->prioridad);
        listaProcesosIterar = listaProcesosIterar->siguiente;
    }
}

void detenerSimulacion() {

}

int main() {
    printf("Opciones para Algoritmos del Simulador del Planificador de CPU\n");
    printf("Digite alguno de las siguientes opciones mediante su correspondiente digito para elegir el algoritmo\n");
    printf("1. FIFO (First In First Out)\n");
    printf("2. SJF (Shortest Job First)\n");
    printf("3. SJF Apropiativo (Shortest Job First)\n");
    printf("4. HPF (High Priority First)\n");
    printf("5. HPF Apropiativo (High Priority First)\n");
    printf("6. Round Robin\n");

    int opcionAlgoritmo;
    for(;;) {
        printf("Digite la opcion: ");
        scanf("%i", &opcionAlgoritmo);

        if (opcionAlgoritmo <= 0 || opcionAlgoritmo >= 7) {
            printf("Opcion incorrecta. Intentelo de nuevo.\n");
            continue;
        }

        if (opcionAlgoritmo == 6) {
            int quantum;
            printf("Indique el quantum deseado para el algoritmo Round Robin: ");
            scanf("%i", &quantum);
        }
        // ALGO AQUI (?
        break;

    }

    pthread_t hiloJobScheduler, hiloCpuScheduler, hiloTimer;

    pthread_create(&hiloTimer, NULL , Timer , NULL);
    pthread_create(&hiloJobScheduler, NULL , JobScheduler , NULL);
    pthread_create(&hiloCpuScheduler, NULL , CpuScheduler , NULL);

    int opcionSimulador = 0;
    printf("Menu del Simulador del Planificador de CPU\n");
    printf("Digite alguno de las siguientes opciones mediante su correspondiente digito para ejecutar la accion\n");
    printf("1. Consultar cola de procesos\n");
    printf("2. Detener simulacion\n");
    for(;;) {
        printf("Digite la opcion: ");
        scanf("%i", &opcionSimulador);
        if (opcionSimulador != 1 && opcionSimulador != 2) {
            printf("Opcion incorrecta. Intentelo de nuevo.\n");
            continue;
        }

        if (opcionSimulador == 1) desplegarColaProcesos();
        else detenerSimulacion();

    }

    pthread_join(hiloJobScheduler, NULL);
    pthread_join(hiloCpuScheduler, NULL);
    pthread_join(hiloTimer, NULL);

    return 0;
}

#pragma clang diagnostic pop