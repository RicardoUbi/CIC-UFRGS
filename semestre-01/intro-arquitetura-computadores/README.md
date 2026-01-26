# Introdução à Arquitetura de Computadores - INF01107

## *Português*

### Informações da Disciplina

* **Sigla:** INF01107 
* **Etapa:** 1ª Etapa 
* **Créditos:** 4 
* **Carga Horária:** 60h 

### Súmula e Tópicos Abordados

A disciplina fornece o suporte para o entendimento dos conceitos de computação sob o ponto de vista de hardware e software. Os tópicos incluem:

* **Representação de Dados:** Sistemas de numeração (binário, hexadecimal, octal), aritmética binária e representação de caracteres (ASCII, Unicode).
* **Organização de Computadores:** Estrutura básica da UCP, modelo de Von Neumann, ciclo de instrução e memória.
* **Programação de Baixo Nível:** Linguagem de máquina e programação simbólica (Assembly).
* **Simuladores:** Prática com os processadores didáticos **NEANDER** (8 bits) e **AHMES** (conjunto de instruções expandido).

### Projeto em Destaque: Simulador AHMES

* **Objetivo:** Desenvolvimento de um programa para calcular o Delta da Fórmula de Bhaskara (D = B^2 - 4*A*C) e determinar o número de raízes reais (R).

* **Restrições Técnicas:**
    * Operação com coeficientes (A, B, C) como números naturais de 1 byte (0 a 255).
    * Cálculo do Delta (D) como número de 2 bytes (Dx, Dy) em notação complemento de 2 (Big Endian).
    * Determinação de R: R = 2 para D > 0; R = 1 para D = 0; R = 0 para D < 0.
    * Gerenciamento fixo de memória: endereços 128d/80h (A) até 133d/85h (R).

## *English*

### Course Information

* **ID:** INF01107 
* **Level:** 1st Semester 
* **Credits:** 4 
* **Total Hours:** 60h 

### Syllabus and Topics Covered

The course provides a foundation for understanding computing from both hardware and software perspectives. Key topics include:

* **Data Representation:** Number systems (binary, hexadecimal, octal), binary arithmetic, and character representation (ASCII, Unicode).
* **Computer Organization:** Basic CPU structure, Von Neumann model, instruction cycles, and memory.
* **Low-Level Programming:** Machine language and symbolic programming (Assembly).
* **Simulators:** Practical experience with didactic processors: **NEANDER** (8-bit) and **AHMES** (expanded set).

### Highlighted Project: Ahmes Simulator

* **Objective:** Develop a program to calculate the Discriminant (Delta) of the Quadratic Formula (D = B^2 - 4*A*C) and determine the number of real roots (R).

* **Technical Constraints:**
    * Handling input coefficients (A, B, C) as 1-byte natural numbers (0 to 255).
    * Delta (D) calculation as a 2-byte integer (Dx, Dy) using Two's Complement (Big Endian).
    * Logic for R: R = 2 if D > 0; R = 1 if D = 0; R = 0 if D < 0.
    * Memory management: Specific mapping from address 128d/80h (A) to 133d/85h (R).
