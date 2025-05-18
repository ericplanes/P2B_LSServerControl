# P2B_LSServerControl

**Pràctica 2 – Fase B**  
Control de temperatura i registre de dades per a sala de servidors  
Basat en microcontrolador **PIC18F4321**, en llenguatge **C cooperatiu**  
Editor recomanat: **Visual Studio Code**

---

## 🧠 Entorn de treball recomanat

### ✅ Visual Studio Code

Configuració necessària:
- Plugin **C/C++** de Microsoft
- Plugin **Makefile Tools** (opcional)

### `.vscode/c_cpp_properties.json` ha d’incloure:

```json
{
  "includePath": [
    "${workspaceFolder}",
    "/Applications/microchip/xc8/v3.00/pic/include", // Path al vostre compilador, aquest és de mac
    "/Applications/microchip/xc8/v3.00/pic/include/proc"
  ],
  "defines": ["__18F4321"],
  "compilerPath": "/usr/bin/clang",
  "intelliSenseMode": "macos-clang-x64",
  "cStandard": "c99"
}
```

🧪 Recorda que el fitxer correcte del micro és `pic18f4321.h`

---

## 🛠️ Compilació

Des de terminal (dins la carpeta `.X`):

```bash
make
```
Pots netejar la compilació també així:
```bash
make clean
```

---

## 💻 Programació del microcontrolador

1. Compila el `.hex`
2. Obre **MPLAB IPE**
3. Selecciona el dispositiu `PIC18F4321`
4. Carrega el `.hex` i programa

---

## 💬 Bones pràctiques

- Usa **branches i Pull Requests** per desenvolupar mòduls
- Posa en comú amb el grup els **fitxers compartits** com `dictionary.c`, `config.h`, etc.
- Evita pujar `.vscode/`, `.idea/` o `dist/` (ja inclòs al `.gitignore`)

---

## ✨ Autors
- Nom: **Eric Planes i Francesc Tur**
- Universitat: **La Salle – Universitat Ramon Llull**
- Curs: **Sistemes Digitals i Microprocessadors (2024-2025)**
