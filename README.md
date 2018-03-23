![PROBIOMASA 01](/images/probiomasa01.png)![PROBIOMASA 01](/images/probiomasa02.png)![PROBIOMASA 01](/images/probiomasa03.png)

# Manual de Uso

# Instalación de un contenedor de Docker
 
 Instalar docker toolbox en su computadora siguiendo las siguientes instrucciones <br />
 https://docs.docker.com/docker-for-windows/install/#download-docker-for-windows
 
 ## Creación de un contenedor de docker
 
 Descargar el Dockerfile de la página del curso
 
 http://www.wegp.unam.mx/static/courses/argentina2018/Dockerfile
 
 -Abrir una terminal y cambiarse a la capeta donde se descargo el Dockerfile con el comando #cd
 -Utilizar el siguiente comando para ejecutar el contenedor de docker
 
 ``` bash
 docker build -t curso_optimizacion.
 
  ```
 
 
 Se deberá esperar al rededor de 20 minutos (Depende de la velocidad de internet y de su computadora).
 
 Unz vez completada la instlación, verificar el ID del contenedor a través del siguiente comando.
 
  ``` bash
 docker images (obtener ID del contenedor creado "curso_optimizacion")
  ```
  
 Crear un volumen en docker
 
 ``` bash
 docker volume create optimization
 
  ```
 Mostrar el volúmen
 
 ``` bash
 docker volume ls 
 ```
 
 Ver detalles del volúmen
 
 ``` bash
 
 docker volume inspect optimization
 
 ```

Correr el contenedor utilizando el volumen que se creo

 ```
docker run -it  --name optimization_1 -v optimization:/storage curso_optimizacion:latest 
 ```
 
 
 # Uso de la herramienta a través de linea de comandos
 
Uso:
 ``` bash
 {-s <float>|-w <float>} [-p <float>] 
  [-l <float>] [-i <float>]
  [-g <int>] -v <int> [-r <string>]
  [-u <char>] -a <char> -f <string>
  -b <string> [--] [--version] [-h]
```

Donde: 
``` bash
   -s <float>,  --demand <float>
     (requerido)  Demanda de biomasa.
         -- OR --
   -w <float>,  --watts <float>
     (requerido)  Demanda en watts.


   -p <float>,  --production <float>
     Porcentaje anual de producción/operación de la planta (%).

   -l <float>,  --loss <float>
     Porcentaje de pérdida esperada en planta (%).

   -i <float>,  --humidity <float>
     Contenido de humedad (%).

   -g <int>,  --grids_to_validate <int>
     Número de grids a validar (solo para validación personalizada).

   -v <int>,  --validation <int>
     (requerido)  Opción de validación
     * 1 = Elige el mejor punto de la lista.
     * 2 = Valida todos los puntos cuya biomasa total de su grid sea mayor al promedio de todo el mapa.
     * 3 = Validación personalizada.

   -r <string>,  --region <string>
     Nombre de la región/país.

   -u <char>,  --heuristic <char>
     Heurística a utilizar en búsqueda. 
     * d = Heurísitca diagonal
     * e = Heurísitica euclideana
     * m = Heurísitca manhattan
     * a = Mayores o iguales a la media (solo búsqueda en profundidad)
     * b = Mejores dos nodos (solo búsqueda en profundidad)

   -a <char>,  --algorithm <char>
     (requerido)  Algoritmo de búsqueda.
     * A, a = Algoritmo A*
     * B, b = Búsqueda en profundidad

   -f <string>,  --friction <string>
     (requerido)  Ruta absoluta a mapa de fricción friction_map.tif

   -b <string>,  --biomass <string>
     (requerido)  Ruta absoluta a mapa de biomasa biomass_map.tif

   --version
     Displays version information and exits.

   -h,  --help
     Despliega información de uso. 
```     
 
Ejecución del programa con un mapa de Haití, algoritmo: A*, validación: 1, heurística: Diagonal, demanda en watts: 30, eficiencia de planta: 40, producción de planta: 80, porcentaje de pérdida en planta: 30. 
``` bash
 ./optimization -b ./Haiti_biomass.tif -f ./Haiti_fricc.tif -a a -r Haiti -v 1 -u d -w 30 -i 40 -p 80 -l 30
 
    TIFF to matrix = 0.22 secs. 
    Relation: 1167.22
    No. 1 / 1
    Source = 1943, 1479
    Cost Distance time = 0.46 secs.
    Size: 1775
    Sum: 100168.125000
    Total Cost: 434142.562500
    A* Search time = 0.120000 secs.
    Creating the final route image took 0.200000 secs.
    
    
``` 

