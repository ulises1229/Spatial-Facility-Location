FROM centos:7

RUN echo "root:optimization" | chpasswd

RUN mkdir optimization

WORKDIR optimization

RUN yum -y update

RUN yum group mark install "Development Tools"

RUN yum -y  install make cmake gcc gtk2-devel pkgconfig wget gcc gcc-c++ git

RUN yum -y install libpng-devel libjpeg-turbo-devel jasper-devel openexr-devel libtiff-devel libwebp-devel

RUN wget https://github.com/opencv/opencv/archive/3.1.0.zip -O opencv-3.1.0.zip

RUN yum install zip unzip -y

RUN unzip opencv-3.1.0.zip

WORKDIR opencv-3.1.0

RUN mkdir build

WORKDIR build

RUN cmake -D CMAKE_BUILD_TYPE=DEBUG -D CMAKE_INSTALL_PREFIX=/usr/local ..

RUN make

RUN make install

RUN export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/

RUN echo '/usr/local/lib/' >> /etc/ld.so.conf.d/opencv.conf

RUN ldconfig

RUN yum -y install epel-release.noarch

RUN yum repolist

RUN yum -y install gdal-devel.x86_64

RUN yum -y install git texlive texlive-*.noarch httpd
EXPOSE 80
WORKDIR /var/www/html/
RUN curl -c /tmp/cookies "https://drive.google.com/uc?export=download&id=19hlh9yLE48Kci_EheM_aeQ_RJ8rqBaJJ" > /tmp/intermezzo.html
RUN curl -L -b /tmp/cookies "https://drive.google.com$(cat /tmp/intermezzo.html | grep -Po 'uc-download-link" [^>]* href="\K[^"]*' | sed 's/\&amp;/\&/g')" > nrb.zip

RUN ls
RUN unzip nrb.zip
RUN rm nrb.zip
WORKDIR /optimization/
RUN ls
RUN git clone https://github.com/ulises1229/PowerPlantOptimization.git optimization-tool

RUN ls

WORKDIR optimization-tool

RUN tar -vzxf tclap-1.2.2.tar.gz

WORKDIR tclap-1.2.2/include/

RUN mv tclap /usr/include/

WORKDIR  /optimization/optimization-tool/src/

RUN g++ -std=c++11 main.cpp -o optimization -L/usr/local/lib -L/usr/include -lopencv_core -lgdal -lopencv_imgproc -lopencv_flann -lopencv_highgui -lopencv_imgcodecs bellford.o dijkstra.o DisplayImage.o distance.o exploracion.o tree.o

RUN export PATH=$PATH:/optimization/optimization-tool/src/optimization
