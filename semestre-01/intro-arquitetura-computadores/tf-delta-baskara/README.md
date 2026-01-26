# Calculador de Delta e Raízes - Simulador AHMES

Este projeto consiste em um programa escrito em linguagem Assembly para o processador didático **AHMES**. O objetivo é calcular o Delta (D) de uma equação de segundo grau e determinar o número de raízes reais (R).

## *Português*

### Descrição do Problema
O programa resolve a expressão D = B^2 - 4*A*C, onde:
* **Coeficientes (A, B, C):** Números naturais de 1 byte (0 a 255).
* **Delta (Dx, Dy):** Resultado de 16 bits (2 bytes) em complemento de 2 (Big Endian).
* **Raízes (R):** * R = 2, se D > 0.
    * R = 1, se D = 0.
    * R = 0, se D < 0.

### Implementação Técnica
A solução utiliza:
1. **Multiplicação por Somas Sucessivas:** Como o conjunto de instruções original é limitado, a exponenciação (B^2) e a multiplicação (A*C) são feitas via loops de soma.
2. **Aritmética de 16 bits:** Manipulação de carry e borrow para garantir a precisão do cálculo do Delta em dois bytes (Dx e Dy).
3. **Lógica de Decisão:** Uso de saltos condicionais (JZ, JN, JNB) para classificar o valor de Delta e definir o valor de R.

### Mapeamento de Memória
Conforme especificação do trabalho:
* **80h (128d):** Entrada A
* **81h (129d):** Entrada B
* **82h (130d):** Entrada C
* **83h (131d):** Saída Dx (Byte mais significativo)
* **84h (132d):** Saída Dy (Byte menos significativo)
* **85h (133d):** Saída R (Quantidade de raízes)


## *English*

### Project Overview
An Assembly program for the **AHMES** didactic processor designed to calculate the Discriminant (D = B^2 - 4*A*C) and identify the number of real roots (R).

### Technical Specifications
* **Inputs (A, B, C):** 1-byte natural numbers (0-255).
* **Output Delta (Dx, Dy):** 16-bit signed integer using Two's Complement (Big Endian).
* **Root Logic:** * R = 2 if D > 0.
    * R = 1 if D = 0.
    * R = 0 if D < 0.

### Methodology
* **Multiplication:** Implemented via repetitive addition loops to compute B squared and A*C.
* **16-bit Subtraction:** Manual borrow handling to perform (B^2) - (4AC) across two memory positions.
* **Simulation:** Developed and tested using the **Hydra (PET Computação)** and **Daedalus** environments.

---

## Como Executar / How to Run

1. Carregue o arquivo `.ahd` ou `.mem` no simulador **Ahmes** ou **Hydra**.
2. Insira os valores de teste nos endereços de memória `128d`, `129d` e `130d`.
3. Certifique-se de que o PC (Program Counter) está em `0`.
4. Execute o programa até a instrução `HLT`.
5. Verifique os resultados em `131d`, `132d` e `133d`.