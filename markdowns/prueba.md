# 🟦 **Coding Challenge: Suma de Dos Enteros**

## 📝 **Descripción del Problema**

Dado un par de números enteros, implementa un algoritmo que **retorne la suma de ambos**, sin utilizar el operador `+` ni `-`.
Tu objetivo es resolverlo utilizando únicamente operaciones **bitwise** (operaciones a nivel de bits).

Este tipo de ejercicio es común en entrevistas técnicas para evaluar entendimiento de aritmética binaria y manipulación de bits.

---

## 🎯 **Objetivo**

Implementar una función:

```
int sumar(int a, int b)
```

que retorne la suma de `a` y `b` **sin usar operadores aritméticos tradicionales**.

---

## 🧠 **Idea Principal**

Para sumar dos enteros sin `+`, utilizamos lógica binaria:

1. **XOR (`^`)** suma los bits sin considerar “carry”.
2. **AND (`&`)** identifica dónde hay “carry”.
3. El “carry” se desplaza una posición a la izquierda con `<< 1`.
4. Repetimos hasta que ya no haya carry.

---

## 📘 **Ejemplos**

| Entrada           | Explicación                           | Salida |
| ----------------- | ------------------------------------- | ------ |
| `a = 3`, `b = 5`  | 3 (011), 5 (101) → resultado 8 (1000) | `8`    |
| `a = -2`, `b = 4` | Funciona también para negativos       | `2`    |
| `a = 0`, `b = 0`  | Caso básico                           | `0`    |

---

## 💡 **Solución (Java-like pseudocode)**

```java
int sumar(int a, int b) {
    while (b != 0) {
        int carry = (a & b) << 1;  // posiciones donde ambos bits son 1
        a = a ^ b;                 // suma binaria sin carry
        b = carry;                 // movemos el carry y repetimos
    }
    return a;
}
```

---

## 📈 **Complejidad**

* **Tiempo:**
  O(1) — limitado por el número fijo de bits del entero (32 bits).
* **Espacio:**
  O(1) — uso constante de memoria.

---
Complejidad espacial → constante, 128
