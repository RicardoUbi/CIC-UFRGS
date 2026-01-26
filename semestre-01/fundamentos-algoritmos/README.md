# Fundamentos de Algoritmos - INF05008

## *Português*

### Informações da Disciplina
* **Sigla:** INF05008
* **Etapa:** 1ª Etapa
* **Créditos:** 4
* **Carga Horária:** 60h

### Súmula e Tópicos Abordados
A disciplina introduz o pensamento computacional através do paradigma de **Programação Funcional**, utilizando a linguagem **Racket** e o ambiente **DrRacket**. Os tópicos incluem:
* **Desenvolvimento Sistemático:** Uso de receitas de projeto (Design Recipes) para construção de funções.
* **Tipos de Dados:** Dados atômicos, compostos (estruturas/registros) e estruturados (listas, árvores e grafos).
* **Recursão:** Domínio de recursão estrutural (sobre a forma dos dados) e recursão generativa.
* **Abstração:** Funções de alta-ordem e definições locais.
* **Algoritmos em Grafos:** Representação e busca de caminhos em estruturas de redes.

### Projeto em Destaque: Buscador de Caminhos em Grafos
* **Objetivo:** Implementar um algoritmo que encontra todos os caminhos possíveis entre dois nodos em um grafo (ex: de "Nashville" a "Chicago").
* **Abordagem:** * Representação do grafo como uma lista de estruturas (`define-struct nodo`).
    * Implementação de funções auxiliares para manipulação de listas e filtragem de vizinhos.
    * Uso de recursão mútua e busca com controle de nodos visitados para evitar ciclos infinitos.
* **Ferramentas:** Linguagem Racket (Advanced Student Language).


## *English*

### Course Information
* **ID:** INF05008
* **Level:** 1st Semester
* **Credits:** 4
* **Total Hours:** 60h

### Syllabus and Topics Covered
The course introduces computational thinking via the **Functional Programming** paradigm, using the **Racket** language and **DrRacket** environment. Key topics include:
* **Systematic Design:** Following Design Recipes to build robust and documented functions.
* **Data Types:** Atomic, compound (structs), and structural data (lists, trees, and graphs).
* **Recursion:** Mastering structural recursion (based on data definitions) and generative recursion.
* **Abstraction:** High-order functions and local definitions.
* **Graph Algorithms:** Representation and pathfinding within network structures.

### Highlighted Project: Graph Pathfinding
* **Goal:** Implement an algorithm to find all possible paths between two nodes in a graph (e.g., from "Nashville" to "Chicago").
* **Approach:**
    * Graph representation as a list of structures (`define-struct node`).
    * Implementation of helper functions for list manipulation and neighbor filtering.
    * Use of mutual recursion and backtracking with visited-node tracking to prevent infinite loops.
* **Tools:** Racket Language (Advanced Student Language).
