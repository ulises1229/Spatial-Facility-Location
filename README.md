[comment]: < TODO: replace these images for ENES, UNAM and CIGA Logos or simply remove>
[comment]: < ![PROBIOMASA 01](/images/probiomasa01.png)![PROBIOMASA 01](/images/probiomasa02.png)![PROBIOMASA 01](/images/probiomasa03.png) >

# Optimization Tool Overview

The optimization tool contains a collection of algorithms coded in C++ that take input data as above-ground biomass and friction maps to calculate an optimal position for a biomass power plant. 

# 1. Installation

## 1.1 System Requirements

#### 1.1.1 Hardware Requirements 
  Minimum:
  * Dual core processor  1 GHz  
  * RAM Memory de 4 GB
  * At least 1 GB of free disk space
  
  Recomended:
  * Quad core processor 3 GHz 
  * RAM Memory 16 GB
  * At least 4 GB of freee disk space (depend on input dataset and map resolution)

#### 1.1.2 Software Requirements
  The optimization tool was sucessfully tested using the CentOS Linux distribution on its version 7. In addition, it is mandatory to install the following libraries:

  * GDAL at least versión 2.0 
  * OpenCV at least version 3.0
  * Tclap  at leas berion 1.0 
  
## 1.2 Library Installation

The optimization algorithms use the

#### 1.2.1 OpenCV  
  Open the comand line and execute the following commands: 
  
  1. Acquires sudo privilleges:  
  ``` bash
  $ su
  ```
  2. Actualizar el gestor de paquetes de CENTOS:
  ``` bash
  $ yum -y update
  ```
  3. Instalar librerías básicas del sistema (incluida la librería GCC) denominada
“Development Tools” mediante los siguientes comandos en terminal:
 ``` bash
  $ yum groupinstall "Development Tools"
  ```
   si esta instrucción falla, probar con las siguientes:
  ``` bash
  $ yum group mark install "Development Tools"
  $ yum group update "Development Tools"
  ```
  Después, instalar los siguientes paquetes:
  ``` bash
  $ yum install cmake gcc gtk2-devel pkgconfig
  ```
 **Nota:** algunos paquetes pueden ya estar instalados
 
 
 4. OpenCV se utiliza para la creación de imágenes, por ende, es necesario instalar las siguientes librerías para llevarla a cabo sin problemas:
 ``` bash
  $ yum install libpng-devel libjpeg-turbo-devel jasper-devel openexr-devel libtiff-devel libwebp-devel
  ```
  
 5. Descargar la librería OpenCV en su versión 3.2 mediante el siguiente comando: 
  ``` bash
  $ wget https://github.com/opencv/opencv/archive/3.1.0.zip -O opencv-3.1.0.zip
  ```
  
  6. Descomprimir el archivo descargado:
  ``` bash
  $ unzip opencv-3.1.0.zip
  ```
  
  7. Una vez descomprimido, navegar al directorio creado e instalar los archivos:
  ``` bash
  $ cd opencv-3.1.0
  $ mkdir build
  $ cd build
  $ cmake -D CMAKE_BUILD_TYPE=DEBUG -D CMAKE_INSTALL_PREFIX=/usr/local
  ..
  $ make
  $ make install
  ```
  **Nota:** en esta parte se puede demorar varios minutos.
  
  8. Crear un enlace hacia la librería para evitar problemas con la misma:
   ``` bash
   $ export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/
   $ echo '/usr/local/lib/' >> /etc/ld.so.conf.d/opencv.conf
   $ ldconfig
   ```
   
  
#### GDAL
   Desde una terminal, hacer lo siguiente: 
  
  1. Convertirse en _Superusuario_:  
  ``` bash
  $ su
  ``` 
  
  2. Instalar el repositorio y los paquetes de EPEL:
  ``` bash
  $ yum install epel-release.noarch
  ``` 
  
  3. Actualizar la lista de repositorios:
  ``` bash
  $ yum repolist
  ``` 
  
  4. Instalar la librería GDAL:
  ``` bash
  $ yum install gdal-devel.x86_64
  ``` 



### The optimization algorithms can be used in two ways.

## 1. Web-based platform
http://www.mofuss.unam.mx/optimization

[comment]: < TODO: replace this image for an updatet one>


![PROBIOMASA 01](/images/webpage.png)

## 2. Docker Container 
#### Docker Installation
 
 Install docker toolbox on your computer
 Instalar docker toolbox en su computadora siguiendo las siguientes instrucciones <br />
 https://docs.docker.com/docker-for-windows/install/#download-docker-for-windows
 
 ## Creación de un contenedor de docker
 
 Descargar el Dockerfile de la página del curso
 
 http://www.wegp.unam.mx/static/courses/argentina2018/Dockerfile
 
 Abrir la terminal de docker 
 
 ![PROBIOMASA 01](/images/quick.png) 
 
 Cambiarse a la capeta donde se descargo el Dockerfile con el comando change directory  cd <Directorio>.
 
 Ejemplo:

 ![PROBIOMASA 01](/images/terminal.png) 
 
 ``` bash
 cd C:\Users\Ulises\Desktop\Docker
 ``` 
 Utilizar el siguiente comando para ejecutar el contenedor de docker
 
 ``` bash
 docker build -t curso_optimizacion .
  ```
 
 Se deberá esperar al rededor de 20 minutos (Depende de la velocidad de internet y de su computadora).
 
 Unz vez completada la instlación, verificar el ID del contenedor a través del siguiente comando.
 
  ``` bash
 docker images 
  ```
 

### Correr el contenedor utilizando el volumen que se creo

 ```
docker run -it  --name optimizacion_1 -v optimizacion:/storage curso_optimizacion:latest 
 ```
 
 ![PROBIOMASA 01](/images/run.png) 
 
 ### Copiar datos de fricción y biomasa
 Ubicar las capas de ficción y biomasa en un directorio y utilizar el comando de docker cp para copiar las capas dentro del contenedor.
 
  ``` bash
 docker cp  C:\Users\Ulises\Desktop\Docker\capas\Honduras_biomass.tif 622bdc51d747:\storage 
 docker cp  C:\Users\Ulises\Desktop\Docker\capas\Honduras_fricc.tif 622bdc51d747:\storage 
  ```
 
 
### Crear un volumen (Opcional)
![PROBIOMASA 01](/images/images.png) 
  
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
     
   -e <string>, --usable <string>
     (requerido)  Indica si la biomasa del mapa es usable en su totalidad o no (ANPs, Waterbodies).
     * y = Sí
     * n = No
    
   --version
     Displays version information and exits.

   -h,  --help
     Despliega información de uso. 
```     
 
Ejecución del programa con un mapa de Haití, algoritmo: A*, validación: 1, heurística: Diagonal, demanda en watts: 30, eficiencia de planta: 40, producción de planta: 80, porcentaje de pérdida en planta: 30. 
``` bash
 ./optimization -b ./Haiti_biomass.tif -f ./Haiti_fricc.tif -a a -r Haiti -v 1 -u d -w 30 -i 40 -p 80 -l 30 -e y
 
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

