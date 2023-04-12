# Animación con C desde Celular

El proyecto consta de una animación en 3 dimensiones, cuyo movimiento es controlado desde un Celular conectado a una computadora.

## Requisitos

* [Instalación de Flutter](https://flutter.dev/docs/get-started/install)
* [C/C++](https://www.cprogramming.com/tutorial/c-tutorial.html)

## Clonar el repositorio

```bash
git clone https://github.com/FrancoGL20/Animacion-con-C-desde-Celular.git
```

## Flutter (sensores)

### Instalación de dependencias de Flutter

```bash
cd sensores
flutter pub get
```

* Resolver problemas en iOS

    Resolver problemas de certificado de equipo de desarrollo en Xcode.

    ```bash
    cd sensores
    open ios/Runner.xcworkspace
    ```

    Runner -> Targets -> Runner -> Signing -> Development Team -> [Elige tu equipo]

  * Instalar paquetes de CocoaPods

    ```bash
    cd sensores
    cd ios
    pod install
    ```

* Resolver problemas en Android

Se muestran en consola directamente.

### Ejecutar el proyecto de Flutter

```bash
cd sensores
flutter run
```

## Códigos de C (sockets_stream)

* Dependencias necesarias para compilar los códigos

```c
#if WIN32
#include <windows.h>
#include <GL/glut.h>
#endif
#if __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
```

* Comandos para compilar

  * En macOS:

    ```bash
    cd sockets_stream
    gcc stream_server.c -o stream_server.o
    gcc deadpool.c -o deadpool.o -framework OpenGL -framework GLUT
    ```

  * En Linux:

    ```bash
    cd sockets_stream
    gcc stream_server.c -o stream_server.o
    gcc deadpool.c -o deadpool.o -lGL -lGLU -lglut
    ```

  * En Windows

    ```cmd
    cd sockets_stream
    gcc stream_server.c -o stream_server.o
    gcc deadpool.c -o deadpool.o -lopengl32 -lglu32 -lglut32
    ```

* Comandos para ejecutar

```bash
./stream_server.o & ./deadpool.o
```

### Screenshots

* Flutter
  * sensores
    ![alt text](https://i.postimg.cc/2ycXD1DF/Screen-Shot-2023-04-12-at-1-49-13-p-m.png)

* Codigos de C
  * deadpool.c
    ![alt text](https://i.postimg.cc/500JPJKG/Screenshot-2023-04-12-at-2-08-05-p-m.png)
  * stream_server.c
    ![alt text](https://i.postimg.cc/Qx18QykV/Screenshot-2023-04-12-at-2-27-16-p-m.png)

### Problemas comunes

* Dirección IP del servidor

  * En macOS y Linux

    ```bash
    ifconfig
    ```

  * En Windows

    ```cmd
    ipconfig
    ```

* Puerto del servidor (stream_server.c) ocupado
En la línea 49 del archivo stream_server.c modificar el valor de la variable. Por default esta en 63020. Que en ipv4 es 11510.

    ```c
    server.sin6_port = 63020;
    ```

Con la dirección IP y/o el puerto del servidor de la computadora que correra el programa de servidor (stream_server.c) modificar el documento del proyecto de flutter (lib/main.dart) en la línea 83.

```dart
final socket = await Socket.connect("10.104.85.35", 11510);
```

## Video demostración

<iframe
    width="640"
    height="480"
    src="https://www.youtube.com/watch?v=0tyqM-I_Pr8"
    frameborder="0"
    allow="autoplay; encrypted-media"
    allowfullscreen
>
</iframe>

## Referencias

[Librerias de Flutter - flutter_sensors](https://pub.dev/packages/flutter_sensors)

[Libreria de Flutter - dart:io](https://api.dart.dev/stable/2.19.6/dart-io/dart-io-library.html)

[Socket stream server en C](https://docs.oracle.com/cd/E19120-01/open.solaris/817-4415/6mjum5sou/index.html#sockets-87164)
