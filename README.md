<p align="center">
  <img src="logos/Cheeta.png" alt="Cheeta" width="200"/><img src="logos/OctoCatty.png" alt="Cheeta" width="200"/><br>
  Cheeta & OctoCatty<br>
</p>

# 🐆 Cheeta — Lightweight Interpreted Language

  ![Cheeta Version](https://img.shields.io/badge/cheeta-v0.8-blue)
  ![Build](https://img.shields.io/badge/build-passing-brightgreen)
  ![License](https://img.shields.io/badge/license-MCPL-green)
  ![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey)
  ![OctoCatty](https://img.shields.io/badge/octocatty-package_manager-orange)
  ![Registry](https://img.shields.io/badge/registry-enabled-blueviolet)

Cheeta is a fast, modular, interpreted programming language written in C++, designed to be extensible and beginner-friendly. It supports:

* Variable declarations
* Arithmetic expressions
* If conditions
* While-style loops
* Modular function definitions
* A CLI package manager: `octocatty`

---

## ✨ Example Script: `example.ct`

```cheeta
fetch std
fetch math

set <x:std::integer> to 5

if <get;x> < 10 then call <std::print:function> with parms ("Less than 10")

call <math::double:function> with parms (5)
call <std::exit:function> with parms (0)
```

---

## 🛠 Compile Cheeta

```bash
g++ cheeta.cpp -o cheeta --std=c++17
```

Run a script:

```bash
./cheeta example.ct
```

---

## 📦 Package Management with `octocatty`

### 🔧 Compile:

```bash
g++ octocatty.cpp -o octocatty --std=c++17
```

### 🔍 Available commands:

```bash
./octocatty list
./octocatty install <module>
./octocatty info <module>
./octocatty publish <file>
```

### ✅ Example:

```bash
./octocatty install math
./octocatty info math
```

---

## 📁 Directory Structure

```
cheeta/
├── cheeta.cpp          # Core interpreter
├── octocatty.cpp       # Package manager CLI
├── cheeta.cpp          # Core interpreter, compiled 4 MacOS
├── octocatty.cpp       # Package manager CLI, compiled 4 MacOS
├── registry.json       # Module registry
├── modules/            # Installed modules (*.cts)
│   └── std.cts
│   └── math.cts
└── example.ct          # Sample script
```

---

## 🔌 Create a Module

```cheeta
#module: math
#version: 0.1
#desc: simple math ops

define <math::double:function> (x;std::integer) {
  call <std::print:function> with parms (<get;x> + <get;x>)
}
```

---

## 🐙 Publish to GitHub (via `octocatty`)

```bash
./octocatty publish math.cts
```

Automatically commits to `cheeta-community` and updates `registry.json`.

---

## 📜 License

MCPL (c) zimoshi
