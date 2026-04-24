/*
    constructor:
        - Se encargará de inicializar todo el sistema de buscadores.
        - Recorre directorios y hace las comprobaciones de libros. Genera un std::vector con la lista de libros a procesar.
        - Lanza hilos SearchService

    destructor:
        - Es posible que se deba gestionar la finalizacion de hilos inferiores (SearchService). 
        - Se debe controlar que rompan su bucle y hacer su posterior join().
        Valorar más adelante si se deberian interrumpir todas las busquedas en curso o esperar a que finalicen. 

    execute_search():
        - Invocado por el cliente, introduce su peticion
        - Esa peticion se envuelve en una peticion interna que contendrá promise
        - Se determina a que cola debe ir
        - Se llama a priorityscheduler.enqueue()
        - Espera el resultado con future.get_value()
        - return del resultado al cliente

    get_petition():
        - Invocado por SearchService.
        priorityscheduler.pop()

    Ambos metodos son bloqueantes. 

    NO es un hilo, no contiene void operator()(). 
    Gestiona el acceso al monitor PriorityScheduler.
    al metodo execute_search entran hilos cliente y al get_search_petition entran hilos SearchService.
    
    */ 