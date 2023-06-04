/* Definiciones externas para el sistema de colas simple */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.cpp" /* Encabezado para el generador de numeros aleatorios */
#include "erlang.cpp" /* Encabezado para la formula de Erlang C */

#define LIMITE_COLA 100     /* Capacidad maxima de la cola */
#define OCUPADO 1           /* Indicador de Servidor Ocupado */
#define LIBRE 0             /* Indicador de Servidor Libre */
#define NUMERO_SERVIDORES 5 /* Numero de servidores */

int sig_tipo_evento, num_clientes_espera, num_esperas_requerido, num_eventos,
    num_entra_cola, estado_servidor[NUMERO_SERVIDORES];
float area_num_entra_cola, area_estado_servidor, media_entre_llegadas, media_atencion,
    tiempo_simulacion, tiempo_llegada[LIMITE_COLA + 1], tiempo_ultimo_evento, tiempo_sig_evento[2 + NUMERO_SERVIDORES], /*1: Llegada 2...n: Salida del n serv. */
    total_de_esperas;
FILE *parametros, *resultados;

void inicializar(void);
void controltiempo(void);
void llegada(void);
void salida(void);
void reportes(void);
void actualizar_estad_prom_tiempo(void);
float expon(float mean);

int main(void) /* Funcion Principal */
{
    /* Abre los archivos de entrada y salida */

    parametros = fopen("param.txt", "r");
    resultados = fopen("result.txt", "w");

    /* Especifica el numero de eventos para la funcion controltiempo. */

    num_eventos = 1 + NUMERO_SERVIDORES;

    /* Lee los parametros de enrtrada. */

    fscanf(parametros, "%f %f %d", &media_entre_llegadas, &media_atencion,
           &num_esperas_requerido);

    /* Escribe en el archivo de salida los encabezados del reporte y los parametros iniciales */

    fprintf(resultados, "Sistema de Colas Simple\n\n");
    fprintf(resultados, "Tiempo promedio de llegada%11.3f minutos\n\n",
            media_entre_llegadas);
    fprintf(resultados, "Tiempo promedio de atencion%16.3f minutos\n\n", media_atencion);
    fprintf(resultados, "Numero de clientes%14d\n\n", num_esperas_requerido);
    fprintf(resultados, "Numero de servidores%14d\n\n", NUMERO_SERVIDORES);

    /* iInicializa la simulacion. */

    inicializar();

    printf("num_eventos: %d\n", num_eventos);

    /* Corre la simulacion mientras no se llegue al numero de clientes especificaco en el archivo de entrada*/

    while (num_clientes_espera < num_esperas_requerido)
    {
        /*logs de debugueo*/

        printf("tiempo simulacion: %f\n", tiempo_simulacion);
        for (int i = 0; i < NUMERO_SERVIDORES; i++)
        {
            printf("estado servidor %d: %d\n", i, estado_servidor[i]);
        }
        printf("num_entra_cola: %d\n", num_entra_cola);
        printf("tiempo_ultimo_evento: %f\n", tiempo_ultimo_evento);
        printf("tiempo_sig_evento: %f\n", tiempo_sig_evento[1]);
        printf("num_clientes_espera: %d\n", num_clientes_espera);
        printf("total_de_esperas: %f\n", total_de_esperas);
        printf("area_num_entra_cola: %f\n", area_num_entra_cola);
        printf("area_estado_servidor: %f\n", area_estado_servidor);
        printf("sig_tipo_evento: %d\n", sig_tipo_evento);
        printf("\n");

        /* Determina el siguiente evento */

        controltiempo();

        /* Actualiza los acumuladores estadisticos de tiempo promedio */

        actualizar_estad_prom_tiempo();

        /* Invoca la funcion del evento adecuado. */

        if (sig_tipo_evento == 1)
            llegada();
        else
            salida();
    }

    /* Invoca el generador de reportes y termina la simulacion. */

    reportes();


    fclose(parametros);
    fclose(resultados);

    return 0;
}

void inicializar(void) /* Funcion de inicializacion. */
{
    /* Inicializa el reloj de la simulacion. */

    tiempo_simulacion = 0.0;

    /* Inicializa las variables de estado */

    for (int i = 0; i < NUMERO_SERVIDORES; i++)
        estado_servidor[i] = LIBRE;
    num_entra_cola = 0;
    tiempo_ultimo_evento = 0.0;

    /* Inicializa los contadores estadisticos. */

    num_clientes_espera = 0;
    total_de_esperas = 0.0;
    area_num_entra_cola = 0.0;
    area_estado_servidor = 0.0;

    /* Inicializa la lista de eventos. Ya que no hay clientes, el evento salida
       (terminacion del servicio) no se tiene en cuenta */

    tiempo_sig_evento[1] = tiempo_simulacion + expon(media_entre_llegadas);
    for (int i = 2; i < NUMERO_SERVIDORES + 2; i++)
    {
        tiempo_sig_evento[i] = 1.0e+30;
    }
}

void controltiempo(void) /* Funcion controltiempo */
{
    int i;
    float min_tiempo_sig_evento = 1.0e+29;

    sig_tipo_evento = 0;

    /* Determina el tipo de evento del evento que debe ocurrir. */
    /* Lo determina tomando el siguiente evento mas cercano */
    for (i = 1; i <= num_eventos; ++i)
        if (tiempo_sig_evento[i] < min_tiempo_sig_evento)
        {
            min_tiempo_sig_evento = tiempo_sig_evento[i];
            sig_tipo_evento = i;
        }

    /* Revisa si la lista de eventos esta vacia. */

    if (sig_tipo_evento == 0)
    {

        /* La lista de eventos esta vacia, se detiene la simulacion. */

        fprintf(resultados, "\nLa lista de eventos esta vacia %f", tiempo_simulacion);
        exit(1);
    }

    /* TLa lista de eventos no esta vacia, adelanta el reloj de la simulacion. */

    tiempo_simulacion = min_tiempo_sig_evento;
}

void llegada(void) /* Funcion de llegada */
{
    float espera;

    /* Programa la siguiente llegada. */

    tiempo_sig_evento[1] = tiempo_simulacion + expon(media_entre_llegadas);

    /* Reisa si TODOS los servidores están OCUPADOS. */

    bool servidor_ocupado = true;
    int servidor_disponible;

    for (int i = 0; i < NUMERO_SERVIDORES; i++)
        if (estado_servidor[i] == LIBRE)
        {
            servidor_ocupado = false;
            servidor_disponible = i;
            break;
        }

    if (servidor_ocupado == true)
    {

        /* Verifica si hay condici�n de desbordamiento */

        if (num_entra_cola + 1 > LIMITE_COLA)
        {

            /* Se ha desbordado la cola, detiene la simulacion */

            fprintf(resultados, "\nDesbordamiento del arreglo tiempo_llegada a la hora");
            fprintf(resultados, "%f", tiempo_simulacion);
            exit(2);
        }

        /* Sservidor OCUPADO, aumenta el numero de clientes en cola */

        ++num_entra_cola;

        /* Todavia hay espacio en la cola, se almacena el tiempo de llegada del
            cliente en el ( nuevo ) fin de tiempo_llegada */

        tiempo_llegada[num_entra_cola] = tiempo_simulacion;
    }

    else
    {

        /*  El servidor esta LIBRE, por lo tanto el cliente que llega tiene tiempo de eespera=0
           (Las siguientes dos lineas del programa son para claridad, y no afectan
           el reultado de la simulacion ) */

        espera = 0.0;
        total_de_esperas += espera;

        /* Incrementa el numero de clientes en espera, y pasa el servidor a ocupado */
        ++num_clientes_espera;
        estado_servidor[servidor_disponible] = OCUPADO;

        /* Programa una salida ( servicio terminado ). */

        tiempo_sig_evento[2 + servidor_disponible] = tiempo_simulacion + expon(media_atencion);
    }
}

void salida(void) /* Funcion de Salida. */
{
    int i;
    float espera;

    /* Revisa si la cola esta vacia */

    if (num_entra_cola == 0)
    {

        /* La cola esta vacia, pasa el servidor a LIBRE y
        no considera el evento de salida*/
        estado_servidor[sig_tipo_evento - 2] = LIBRE;
        tiempo_sig_evento[sig_tipo_evento] = 1.0e+30;
    }

    else
    {

        /* La cola no esta vacia, disminuye el numero de clientes en cola. */
        --num_entra_cola;

        /*Calcula la espera del cliente que esta siendo atendido y
        actualiza el acumulador de espera */

        espera = tiempo_simulacion - tiempo_llegada[1];
        total_de_esperas += espera;

        /*Incrementa el numero de clientes en espera, y programa la salida. */
        ++num_clientes_espera;
        tiempo_sig_evento[sig_tipo_evento] = tiempo_simulacion + expon(media_atencion);

        /* Mueve cada cliente en la cola ( si los hay ) una posicion hacia adelante */
        for (i = 1; i <= num_entra_cola; ++i)
            tiempo_llegada[i] = tiempo_llegada[i + 1];
    }
}

void reportes(void) /* Funcion generadora de reportes. */
{
    /* Calcula y estima los estimados de las medidas deseadas de desempe�o */
    fprintf(resultados, "\n\nEspera promedio en la cola%11.3f minutos\n\n",
            total_de_esperas / num_clientes_espera);
    fprintf(resultados, "Numero promedio en cola%10.3f\n\n",
            area_num_entra_cola / tiempo_simulacion);
    fprintf(resultados, "Uso del servidor%15.6f\n\n",
            area_estado_servidor / tiempo_simulacion);
    fprintf(resultados, "Tiempo de terminacion de la simulacion%12.3f minutos", tiempo_simulacion);
    
    float erlang = erlangC((1/media_entre_llegadas), (1/media_atencion), NUMERO_SERVIDORES);

    fprintf(resultados, "\n\nValor de la fórmula C de Erlang: %f", erlang);
}

void actualizar_estad_prom_tiempo(void) /* Actualiza los acumuladores de
                                                       area para las estadisticas de tiempo promedio. */
{
    float time_since_last_event;

    /* Calcula el tiempo desde el ultimo evento, y actualiza el marcador
        del ultimo evento */

    time_since_last_event = tiempo_simulacion - tiempo_ultimo_evento;
    tiempo_ultimo_evento = tiempo_simulacion;

    /* Actualiza el area bajo la funcion de numero_en_cola */
    area_num_entra_cola += num_entra_cola * time_since_last_event;

    int servidor_ocupado = 1;

    /*Actualiza el area bajo la funcion indicadora de servidor ocupado*/
    for (int i = 0; i < NUMERO_SERVIDORES; i++)
    {
        if (estado_servidor[i] == LIBRE)
        {
            servidor_ocupado = 0;
            break;
        }
    }
    area_estado_servidor += servidor_ocupado * time_since_last_event;
}

float expon(float media)  /* Funcion generadora de la exponencias */
{
    /* Retorna una variable aleatoria exponencial con media "media"*/

    return -media * log(lcgrand(1));
}

