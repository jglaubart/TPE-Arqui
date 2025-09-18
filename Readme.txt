# 🖥️ TPE – Arquitectura de las Computadoras

Este proyecto implementa un **sistema mínimo** basado en la distribución **Barebones del ITBA**.  
Sobre ese esqueleto se desarrolla una **mini shell** que corre en **modo usuario**, con utilidades básicas de depuración y demostración, junto con un pequeño juego interactivo.

---

## 📑 Índice

- [✨ Funcionalidades principales](#-funcionalidades-principales)  
- [📦 Dependencias](#-dependencias)  
- [🛠️ Compilación](#️-compilación)  
- [▶️ Ejecución](#️-ejecución)  
- [🎮 Demo](#-demo)  
- [👥 Autores](#-autores)  

---

## ✨ Funcionalidades principales

| Comando            | Descripción |
|--------------------|-------------|
| `help` / `clear`  | Muestra la ayuda integrada y limpia la pantalla. |
| `time`             | Consulta el reloj del sistema y lo imprime en pantalla. |
| `inforeg`          | Vuelca los registros de propósito general *(requiere activar la captura con `CTRL`)*. |
| `zerodiv` / `invopcode` | Fuerzan excepciones (división por cero y op-code inválido) para probar el manejo de errores del kernel. |
| `changeFontSize`   | Ajusta el tamaño de fuente en tiempo de ejecución. |
| `golf`             | Inicia **Pongis Golf**, un mini juego de física implementado en **Userland**. |

📌 La shell procesa **un comando por línea** y separa automáticamente el primer argumento en caso de ser necesario.

---

## 📦 Dependencias

- [🐋 Docker](https://www.docker.com/)  
  Imagen utilizada: **`agodio/itba-so:2.0`** (incluye toolchain y QEMU).  
- [QEMU](https://www.qemu.org/) instalado localmente  
  Binario utilizado: **`qemu-system-x86_64`**

---

## 🛠️ Compilación

1. Posicionarse en la raíz del proyecto.
2. Ejecutar:

   ```bash
   ./compile.sh
