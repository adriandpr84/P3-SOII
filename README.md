# SSOOIIGLE - Buscador de Texto Multihilo Concurrente (PARTE II)

**Autores:** Manuel Martín-Tembleque García-Serrano & Adrián Díaz-Plaza Remesal  
**Fecha:** Mayo 2026  
**Proyecto:** Práctica 3 - Sistemas Operativos II

---

## 1. Introducción
**SSOOIIGLE** es la evolución del buscador de texto, diseñado ahora para simular la búsqueda simultánea de varios usuarios en un sistema operativo multiusuario. En esta versión, los usuarios se modelan como entidades activas mediante hilos concurrentes. 

El sistema incorpora un planificador que garantiza una atención proporcional (80% Premium, 20% Gratis) evitando la inanición, un diccionario para generar búsquedas aleatorias, y un **sistema de pago** modelado como un proceso independiente. La comunicación entre el buscador y el sistema de pago se realiza de forma robusta mediante colas de mensajes IPC (System V).

## 2. Arquitectura del Proyecto
La estructura de directorios se ha diseñado de forma modular para separar claramente las responsabilidades del sistema (clientes, pagos, motor de búsqueda y utilidades):

```text 
.
├── code
│   ├── include
│   │   ├── client
│   │   │   ├── Client.hpp
│   │   │   ├── ClientManager.hpp
│   │   │   └── Dictionary.hpp
│   │   ├── payment
│   │   │   ├── PaymentProcess.hpp
│   │   │   ├── PaymentProxy.hpp
│   │   │   └── SearchMonitor.hpp
│   │   ├── search
│   │   │   ├── ChunkSearcher.hpp
│   │   │   ├── LibraryIndexer.hpp
│   │   │   ├── PriorityScheduler.hpp
│   │   │   ├── SearchManager.hpp
│   │   │   └── SearchService.hpp
│   │   └── utils
│   │       ├── CommonTypes.hpp
│   │       ├── ConcurrentQueue.hpp
│   │       ├── Occurrence.hpp
│   │       ├── PaymentTypes.hpp
│   │       ├── SearchResultsTypes.hpp
│   │       ├── SearchTypes.hpp
│   │       └── SystemConfig.hpp
│   ├── src
│   │   ├── client
│   │   │   ├── Client.cpp
│   │   │   ├── ClientManager.cpp
│   │   │   └── Dictionary.cpp
│   │   ├── payment
│   │   │   ├── PaymentProcess.cpp
│   │   │   ├── PaymentProxy.cpp
│   │   │   └── SearchMonitor.cpp
│   │   ├── search
│   │   │   ├── ChunkSearcher.cpp
│   │   │   ├── LibraryIndexer.cpp
│   │   │   ├── PriorityScheduler.cpp
│   │   │   ├── SearchManager.cpp
│   │   │   └── SearchService.cpp
│   │   └── SSOOIIGLE.cpp
│   └── tests
│       ├── CMakeLists.txt
│       ├── test_priority_scheduler.cpp
│       └── test_search_engine.cpp
├── libros
│   ├── 17 LEYES DEL TRABAJO EN EQUIPO.txt
│   ├── 21 LEYES DEL LIDERAZGO - JOHN C. MAXWELL.txt
│   ├── 25 MANERAS DE GANARSE A LA GENTE - .txt
│   ├── Abe Shana - La última sirena.txt
│   ├── ACTITUD DE VENCEDOR - JOHN C. MAXWELL.txt
│   ├── El Oro Y La Ceniza - Abecassis Eliette .txt
│   ├── SEAMOS PERSONAS DE INFLUENCIA - JOHN MAXWELL.txt
│   └── VIVE TU SUEÑO - JOHN MAXWELL.txt
├── Makefile
└── README.md
```

* **`code/src/` y `code/include/`**: Contienen la lógica de implementación (`.cpp`) y cabeceras (`.hpp`) divididas por módulos:
    * **`client/`**: Gestión, orquestación y modelado de los hilos clientes.
    * **`payment/`**: Lógica del proceso independiente de pagos (IPC), el proxy de comunicación y los monitores de sincronización.
    * **`search/`**: Motor core del buscador, indexación concurrente, fragmentación de tareas y colas de prioridad.
    * **`utils/`**: Estructuras de datos compartidas, colas thread-safe y definiciones de tipos globales.
* **`code/src/SSOOIIGLE.cpp`**: Orquestador principal. Gestiona la bifurcación de procesos (`fork()`), la inicialización de los servicios y el apagado seguro.
* **`code/tests/`**: Pruebas unitarias para validar componentes críticos del motor.
* **`libros/`**: Directorio con los archivos de texto (`.txt`) utilizados como fuentes para realizar las búsquedas simultáneas.
* **`Makefile`**: Script de automatización para la compilación del proyecto.

---

## 3. Compilación

Para compilar todo el proyecto y generar el ejecutable, sitúate en la raíz del proyecto y ejecuta:

```bash
make
```

Esto compilará los archivos fuente y generará el ejecutable final (habitualmente en una carpeta `bin/` o en la raíz, según la configuración del Makefile).

---

## 4. Ejecución

Puedes ejecutar la simulación utilizando el Makefile (si tiene una regla definida para ello) o invocando directamente el binario generado.

### Ejecución directa

```bash
make run
```

### Ejecución con argumentos

```bash
make run ARGS="<n_clientes> <n_replicas> <directorio_libros>"
```


### Interrupción segura
El programa está diseñado para manejar la señal `SIGINT`. Si pulsas `Ctrl+C` durante la ejecución, el generador de clientes se detendrá, se esperará a que las búsquedas en curso finalicen y los procesos (incluyendo el sistema IPC) se cerrarán de forma limpia, liberando los recursos del sistema operativo.

---

## 5. Resultados
Para evitar colapsar la salida estándar por terminal (stdout), los resultados de búsqueda de cada cliente no se imprimen por pantalla. En su lugar, el sistema genera automáticamente un directorio llamado `simulation_results/` donde se creará un archivo `.txt` independiente por cada cliente con sus estadísticas de tiempo y las coincidencias encontradas.

---

## 6. Limpieza
Para eliminar los archivos objeto, limpiar el binario generado y borrar el directorio de resultados de la simulación:
```bash
make clean 
```