# ME2T2

0. Programa diseñado en lenguaje de programación C++ para simulación de sistemas de líneas de espera tipo M/M/1.
Apartir de éste se pueden hacer varios tipos de experimentos y extender el código para otros tipos de modelos de forma muy sencilla.

1. Se cambiaron los nombres de las variables de ingles a español.

2. El archivo de entrada "mm1.in" se cambió por "param.txt" ( para mayor claridad y fácil modificación de los parametros de entrada )

3. El archivo de salida "mm2.out" se cambió por "result.txt" ( para una más fácil lectura de las estadisticas )

4. El archivo "lcgrand.cpp" se tomó de
http://www.sju.edu/~sforman/courses/2000F_CSC_5835/, que es una página de simulaciones. Buscar en el link de Simlib.c ( una libreria para simulaciones )


## Instrucciones para compilar el simulador

g++ sistema_de_colas.cpp -o simulador