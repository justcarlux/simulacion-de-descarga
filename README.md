# 📦 simulacion-de-descarga

Simulación interactiva desarrollada en C++, [raylib](https://www.raylib.com/) y [raygui](https://github.com/raysan5/raygui) que modela el proceso de descarga y traslado de mercancía entre muelles portuarios como una variante del problema de las Torres de Hanói con dos tipos de cajas, utilizando un **algoritmo recursivo** para el cálculo óptimo de movimientos. Desarrollado para la materia de **Taller de Estructuras de Datos** en la **Universidad de Oriente**.

# Compilación y Ejecución

Este proyecto está construido a partir de la plantilla multiplataforma **raylib-quickstart**. Las instrucciones completas de configuración, compilación y ejecución para Windows, Linux y macOS se encuentran detalladas en el repositorio original de la plantilla:

- [raylib-extras/raylib-quickstart: A simple cross platform template for windows, linux, and MacOS](https://github.com/raylib-extras/raylib-quickstart)

# Modos de la Aplicación

La aplicación permite parametrizar el número de **cajas grandes** (entre 1 y 4), el número de **cajas pequeñas** (un número par entre 2 y 8) y la cantidad de **muelles auxiliares** (entre 1 y 4), además de los muelles de **origen** y **destino**.

# Reglas

- Existen dos tipos de cajas: **grandes** y **pequeñas**. Las cajas pequeñas siempre se agrupan de dos en dos.
- En cada movimiento se traslada **una caja grande** o **un grupo de dos cajas pequeñas**.
- Un grupo de cajas pequeñas puede colocarse encima de una caja grande o de otro grupo de cajas pequeñas.
- Una caja grande puede colocarse encima de otra caja grande, pero **nunca** encima de un grupo de cajas pequeñas.
- El objetivo es trasladar todas las cajas del muelle origen al muelle destino.

### 1. Modo Manual: Botón `Empezar juego`:

Permite al usuario resolver el traslado de forma manual arrastrando y soltando las cajas con el ratón.

https://github.com/user-attachments/assets/050944d9-99e7-464f-8194-f9fe02300b3d

### 2. Modo Automático: Botón `Empezar simulación`:

Calcula y reproduce de manera automática la resolución óptima paso a paso.

https://github.com/user-attachments/assets/7c44b567-3d09-48f7-ba51-fbfd1310f0ba

# Créditos

- **Tipografía**: [Ubuntu - Google Fonts](https://fonts.google.com/specimen/Ubuntu).
- **Sprite del camión**: [White box truck on transparent background mockup for delivery and logistics | Free PSD](https://www.magnific.com/free-psd/white-box-truck-transparent-background-mockup-delivery-logistics_409092819.htm#fromView=keyword&page=1&position=3&uuid=b3c09c44-85e8-4acf-9f73-b6e343f2bc93&track=ais_hybrid&query=Truck+sprite) en Magnific.
- **Sprite de las cajas**: [Box | OpenGameArt.org](https://opengameart.org/content/box-2) en OpenGameArt.
