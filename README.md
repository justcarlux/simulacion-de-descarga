# 📦 simulacion-de-descarga

Simulación interactiva desarrollada en C++ y [raylib](https://www.raylib.com/) que modela el proceso de descarga y traslado de mercancía entre muelles portuarios como una generalización del problema de las Torres de Hanói con múltiples postes, utilizando el algoritmo de **Frame-Stewart** para el cálculo óptimo de movimientos.

# Compilación y Ejecución

Este proyecto está construido a partir de la plantilla multiplataforma **raylib-quickstart**. Las instrucciones completas de configuración, compilación y ejecución para Windows, Linux y macOS se encuentran detalladas en el repositorio original de la plantilla:

- [raylib-extras/raylib-quickstart: A simple cross platform template for windows, linux, and MacOS](https://github.com/raylib-extras/raylib-quickstart)

# Modos de la Aplicación

La aplicación permite parametrizar tanto el número de **cajas** (entre 2 y 5) como la cantidad de **muelles auxiliares** (entre 1 y 4), además de los muelles de **origen** y **destino**.

### 1. Modo Manual: Botón `Empezar simulación`:

Permite al usuario resolver el traslado de forma manual arrastrando y soltando las cajas con el ratón.

[VIDEO]

### 2. Modo Automático: Botón `Empezar y ver solución de la simulación`:

Calcula y reproduce de manera automática la resolución óptima paso a paso.

[VIDEO]

# Créditos

- **Sprite del camión**: [White box truck on transparent background mockup for delivery and logistics | Free PSD](https://www.magnific.com/free-psd/white-box-truck-transparent-background-mockup-delivery-logistics_409092819.htm#fromView=keyword&page=1&position=3&uuid=b3c09c44-85e8-4acf-9f73-b6e343f2bc93&track=ais_hybrid&query=Truck+sprite) en Magnific.
- **Sprite de las cajas**: [Box | OpenGameArt.org](https://opengameart.org/content/box-2) en OpenGameArt.

