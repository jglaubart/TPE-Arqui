TPE Arquitectura de las Computadoras
====================================

Este proyecto implementa un sistema minimo basado en la distribucion Barebones del ITBA. Sobre ese esqueleto se desarrolla una **mini shell** que corre en modo usuario y ofrece utilidades basicas de depuracion y demostracion, junto con un pequeno juego interactivo.

## Funcionalidades principales
- `help` / `clear`: muestran la ayuda integrada y limpian la pantalla.
- `time`: consulta el reloj del sistema y lo imprime en pantalla.
- `inforeg`: vuelca los registros de proposito general (requiere activar la captura con `CTRL`).
- `zerodiv` / `invopcode`: fuerzan excepciones de division por cero y op-code invalido para probar el manejo de errores del kernel.
- `changeFontSize`: permite ajustar el tamano de fuente en tiempo de ejecucion.
- `golf`: inicia Pongis Golf, un mini juego de fisica implementado en `Userland`.

La shell procesa un comando por linea y separa automaticamente el primer argumento en caso de ser necesario.

## Dependencias
- Docker (la imagen usada es `agodio/itba-so:2.0`, que incluye toolchain y QEMU).
- QEMU instalado localmente para ejecutar la imagen resultante (se utiliza `qemu-system-x86_64`).

## Compilacion
1. Posicionarse en la raiz del proyecto.
2. Ejecutar `./compile.sh`.
   - El script crea (si no existe) y reutiliza el contenedor `tpe`, monta el proyecto en `/root` y corre la secuencia de `make` sobre Toolchain, Kernel, Userland e Image.
   - La primera ejecucion puede descargar la imagen de Docker indicada arriba.

Como alternativa, se puede correr manualmente `make` desde cada subdirectorio (`Bootloader`, `Kernel`, `Userland`, `Image`) siguiendo el `Makefile` principal, pero el script resume todo el flujo.

## Ejecucion
1. Tras compilar, ejecutar `./run.sh`.
2. El script detecta el sistema operativo host y lanza QEMU (`qemu-system-x86_64`) con los parametros de audio apropiados, usando `Image/x64BareBonesImage.qcow2`.

Una vez que QEMU arranca, se presenta la mini shell. Se puede escribir `help` para ver los comandos disponibles y explorar sus funcionalidades.
