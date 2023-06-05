import subprocess
import matplotlib.pyplot as plt

# Recibe la cantidad de clientes (ejecuciones) y los pasos entre cada ejecución
clientes = int(input('Ingrese la cantidad de clientes: '))
pasos = int(input('Ingrese los pasos entre cada ejecución: '))

# Funciones para encontrar un string entre dos strings

def find_between( s, first, last ):
    try:
        start = s.index( first ) + len( first )
        end = s.index( last, start )
        return s[start:end]
    except ValueError:
        return ""

def find_between_r( s, first, last ):
    try:
        start = s.rindex( first ) + len( first )
        end = s.rindex( last, start )
        return s[start:end]
    except ValueError:
        return ""

# Definir el número de ejecuciones y el rango de valores para el último parámetro
num_ejecuciones = 10000
valores_parametro = range(1, clientes+1, pasos)

# Listas para almacenar los resultados de cada ejecución
uso_servidor = []
c_de_erlang = []
valores_parametro_usados = []
diferencia = []

# Ejecutar el archivo n veces, variando el último parámetro cada vez
for i in valores_parametro:
    # Seleccionar un valor aleatorio para el último parámetro
    valores_parametro_usados.append(i)
    
    # Actualizar el archivo param.txt con el nuevo valor del último parámetro
    with open('param.txt', 'r') as f:
        lines = f.readlines()
    lines[-1] = str(i) + '\n'
    with open('param.txt', 'w') as f:
        f.writelines(lines)
    
    # Ejecutar el archivo deseado y guardar el uso del servidor
    subprocess.run(['./simuladorMMm'], text=True)

    #Es

    with open('result.txt', 'r') as f:
        result = f.read()
    
        
    #Toma lo que está después de "Uso del servidor" + tabulación y antes de un salto de línea
    uso = float(find_between(result, 'Uso del servidor', '\n'))
    c_de_erlang.append(float(find_between_r(result, 'C de Erlang:', '')))
    uso_servidor.append(uso)
    diferencia.append(abs(uso - c_de_erlang[-1]))


# Graficar la diferencia entre c de erlang y el uso del servidor
plt.plot(valores_parametro_usados, c_de_erlang, label='C de Erlang')
plt.plot(valores_parametro_usados, uso_servidor, label='Uso del servidor')
plt.xlabel('Número de clientes')
plt.ylabel('Uso del servidor')
plt.title('Diferencia entre C de Erlang y Uso del servidor')
plt.legend()
plt.show()

# Graficar la diferencia entre c de erlang y el uso del servidor
plt.plot(valores_parametro_usados, diferencia)
plt.xlabel('Número de clientes')
plt.ylabel('Diferencia')
plt.title('Diferencia entre C de Erlang y Uso del servidor')
plt.show()


# Guardar los resultados en un archivo nuevo llamado exp.csv
with open('exp.csv', 'w') as f:
    f.write('Número de Clientes, C de Erlang, Uso del servidor\n')
    for i in range(len(valores_parametro_usados)):
        f.write(str(valores_parametro_usados[i]) + ',' + str(c_de_erlang[i]) + ',' + str(uso_servidor[i]) + '\n')


