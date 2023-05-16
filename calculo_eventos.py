import pandas as pd
from collections import deque

# Usado para calcular el número promedio en cola POR FAVOR REVISAR

datos_eventos = pd.read_csv("datos_eventos.csv")

llegadas = deque([float(i.replace(",", ".")) for i in datos_eventos["tiempo_llegada"].values.tolist()])
salidas = deque([float(i.replace(",", ".")) for i in datos_eventos["tiempo_salida_servidor"].values.tolist()])

en_cola = 0
ultimo_evento = 0.0
area_colas = 0.0
tiempo_simulacion = max(salidas[-1], llegadas[-1])

while len(llegadas) > 0 and len(salidas) > 0:
    if salidas[0] < llegadas[0]:
        area_colas = en_cola * (salidas[0]-ultimo_evento)
        en_cola -= 1
        ultimo_evento = salidas.popleft()
    else:
        area_colas = en_cola * (llegadas[0]-ultimo_evento)
        en_cola += 1
        ultimo_evento = llegadas.popleft()

while len(llegadas) > 0:
    area_colas = en_cola * (llegadas[0]-ultimo_evento)
    en_cola += 1
    ultimo_evento = llegadas.popleft()

while len(salidas) > 0:
    area_colas = en_cola * (salidas[0]-ultimo_evento)
    en_cola -= 1
    ultimo_evento = salidas.popleft()

print(area_colas/tiempo_simulacion)

