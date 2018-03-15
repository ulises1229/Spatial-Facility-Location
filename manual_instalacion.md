# Instalación del Sistema

## Requerimientos del sistema

#### 1. Requerimientos de Hardware
  Mínimos:
  * Procesador Intel Core i5 de doble núcleo a 2,4 GHz o equivalente  
  * Memoria RAM de 12 GB  
  
  Recomendados:
  * Procesador Intel Core i7 de cuatro núcleos a 3,40 GHz o equivalente
  * Memoria RAM de 32 GB

#### 2. Requerimientos de Software
  Para garantizar que el sistema funcione, es necesario lo siguiente:
  * Sistema operativo **CENTOS 7**.
  * OpenCV v3.1
  * GDAL
  

## Instalación de Software
  
#### OpenCV  
  Desde una terminal, hacer lo siguiente: 
  
  1. Convertirse en _Superusuario_:  
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
  **Nota:** en esta parte, se puede demorar varios minutos.
  
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
