# 📚 Navegador de Algoritmos de Búsqueda

Aplicación Windows completamente autónoma con interfaz gráfica profesional para navegar una jerarquía de temas sobre algoritmos de búsqueda y estructuras de datos.

## ⚡ Inicio Rápido

### Opción 1: Script Batch (Más fácil)
```
1. Doble clic en: compilar.bat
2. Espera a que termine
3. Doble clic en: ejecutar.bat
```

### Opción 2: VS Code
```
1. Presiona: Ctrl + Shift + B
2. Selecciona: "Compilar Proyecto"
3. Ejecuta: output\app.exe
```

### Opción 3: Línea de comandos
```powershell
g++ -std=c++11 -static -static-libgcc -static-libstdc++ -Wall -g src/app.cpp -o output/app.exe
output\app.exe
```

## 📋 Requisitos

- **Windows 7 o superior**
- **MinGW con g++** (descarga desde https://www.mingw-w64.org/)
- **~10 MB de espacio** para el ejecutable

**⭐ MÁS FÁCIL:** Si no tienes MinGW, descárgalo e instálalo siguiendo:
https://sourceforge.net/projects/mingw-w64/files/

## 🎨 Interfaz

```
┌─────────────────────────────────────────────────┐
│ Navegador de Algoritmos                         │
├───────────────────┬───────────────────────────┤
│                   │                           │
│  🌳 Árbol de      │  📝 Detalles del         │
│     Temas (1/3)   │     tema (2/3)           │
│                   │                           │
│  • Algoritmos     │  [Selecciona un tema    │
│    • Búsquedas    │   para ver detalles]    │
│    • Hash         │                           │
│  • Otras          │                           │
│  • Índices        │                           │
│                   │                           │
└───────────────────┴───────────────────────────┘
```

## 📖 Contenidos Disponibles

### 📚 Algoritmos de Búsqueda
- Búsquedas internas y externas
  - Secuencial
  - Binaria
  - Por transformación de claves
- Funciones hash/dispersión
  - Módulo, Cuadrado, Truncamiento, Conversión de bases
- Análisis de complejidad
- Solución de colisiones
  - Reasignación, Arreglos anidados, Encadenamiento

### 🌳 Otras Búsquedas Internas
- Búsquedas por residuos
- Árboles de búsqueda digital
- Tablas de índices
- Método de la rejilla
- Árboles 2D
- Análisis de algoritmos

### 📁 Índices para Archivos
- Definición de índices
- Índices primarios
- Índices secundarios
- Índices multiniveles
- Cálculo de accesos a disco
- Análisis

## 🔧 Solución de Problemas

| Problema | Solución |
|----------|----------|
| "g++ not found" | Instala MinGW desde https://www.mingw-w64.org/ |
| No se crea app.exe | Verifica que src/app.cpp exista |
| App no abre | Ejecuta compilar.bat primero |
| Interfaz cortada | Redimensiona la ventana (1200x700 recomendado) |

## 🔍 Características

✅ **Completamente autónomo** - Un solo .exe, sin dependencias
✅ **Interfaz profesional** - TreeView jerárquico con emojis
✅ **Fácil de compilar** - Scripts batch incluidos
✅ **Portable** - Copia app.exe a cualquier lado
✅ **Rápido** - Compilación estática (~10 MB)

## 📂 Estructura del Proyecto

```
Proyecto/
├── src/
│   └── app.cpp          # Código fuente único
├── output/
│   └── app.exe          # Ejecutable (se genera)
├── .vscode/
│   └── tasks.json       # Tareas de VS Code
├── compilar.bat         # Script de compilación
├── ejecutar.bat         # Script de ejecución
├── INSTRUCCIONES.txt    # Guía detallada
└── README.md           # Este archivo
```

## 🎯 Uso

1. **Compila** con `compilar.bat` o tu editor favorito
2. **Abre** `app.exe` desde la carpeta output
3. **Navega** haciendo clic en los temas del árbol
4. **Expande** temas para ver sub-opciones

## 💡 Notas

- El código usa **Windows API nativa** - funciona en cualquier Windows
- Compilación **estática** - el .exe es completamente independiente
- **Sin frameworks externos** - solo C++11 y Windows API
- Diseño **responsive** - se ajusta al redimensionar

## 📝 Licencia

Proyecto educativo - Semestre 6, Ciencias de la Computación II

---

**¿Necesitas ayuda?**
1. Revisa `INSTRUCCIONES.txt` para más detalles
2. Compila con `compilar.bat` para ver mensajes de error claros
3. Verifica que MinGW esté correctamente instalado
