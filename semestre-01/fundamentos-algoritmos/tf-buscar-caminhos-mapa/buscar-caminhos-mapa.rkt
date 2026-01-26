;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-intermediate-lambda-reader.ss" "lang")((modname aula-grafos-modelo-slides) (read-case-sensitive #t) (teachpacks ((lib "draw.rkt" "teachpack" "htdp") (lib "image.rkt" "teachpack" "2htdp"))) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ((lib "draw.rkt" "teachpack" "htdp") (lib "image.rkt" "teachpack" "2htdp")) #f)))
;; ==================== Estruturas ====================
(define-struct nodo (nome vizinhos))

(define MAPA
  (list
   (make-nodo "Helena" (list "Denver" "Omaha" "Duluth" "Winnipeg"))
   (make-nodo "Denver" (list "Helena" "Santa-Fe" "Oklahoma" "Kansas" "Omaha"))
   (make-nodo "Santa-Fe" (list "Denver" "Oklahoma"))
   (make-nodo "Winnipeg" (list "Helena" "Duluth" "Sault-Marie"))
   
   (make-nodo "Duluth" (list "Helena" "Winnipeg" "Omaha" "Chicago" "Sault-Marie" "Toronto"))
   (make-nodo "Omaha" (list "Duluth" "Helena" "Denver" "Kansas" "Chicago"))
   (make-nodo "Kansas" (list "Omaha" "Oklahoma" "Denver" "Saint-Louis"))
   (make-nodo "Oklahoma" (list "Denver" "Santa-Fe" "Little-Rock" "Kansas"))

   (make-nodo "Little-Rock" (list "Oklahoma" "Saint-Louis" "Nashville"))
   (make-nodo "Saint-Louis" (list "Little-Rock" "Kansas" "Nashville" "Pittsburch" "Chicago"))
   (make-nodo "Chicago" (list "Saint-Louis" "Omaha" "Duluth" "Pittsburch" "Toronto"))
   (make-nodo "Sault-Marie" (list "Duluth" "Winnipeg" "Toronto"))

   (make-nodo "Toronto" (list "Sault-Marie" "Duluth" "Chicago" "Pittsburch"))
   (make-nodo "Pittsburch" (list "Toronto" "Chicago" "Saint-Louis" "Nashville"))
   (make-nodo "Nashville" (list "Pittsburch" "Atlanta" "Little-Rock" "Saint-Louis"))
   (make-nodo "Atlanta" (list "Nashville"))
  ))

;; ==================== Exercício 2 ====================
;; está-na-lista? : String ListaDeString -> Boolean
(define (está-na-lista? s ls)
  (cond
    [(empty? ls) #f]
    [(string=? s (first ls)) #t]
    [else (está-na-lista? s (rest ls))]))

;; subtrai-lista: ListaDeString ListaDeString -> ListaDeString
(define (subtrai-lista l1 l2)
  (cond
    [(empty? l1) empty]
    [(está-na-lista? (first l1) l2) (subtrai-lista (rest l1) l2)]
    [else (cons (first l1) (subtrai-lista (rest l1) l2))]))

;; ==================== Exercício 3 ====================
;; vizinhos: String Grafo ListaDeString -> ListaDeString
(define (vizinhos n G LN)
  (cond
    [(empty? G) empty]
    [(string=? n (nodo-nome (first G))) 
     (subtrai-lista (nodo-vizinhos (first G)) LN)]
    [else (vizinhos n (rest G) LN)]))

;; ==================== Exercício 5 ====================
;; monta-caminhos: String ListaDeListaDeString -> ListaDeListaDeString
(define (monta-caminhos nome lista)
  (cond
    [(empty? lista) empty]
    [else (cons (cons nome (first lista))
                (monta-caminhos nome (rest lista)))]))

;; encontra-todos-caminhos: String String Grafo ListaDeString -> ListaDeListaDeString
(define (encontra-todos-caminhos origem destino G visitadas)
  (cond
    [(string=? origem destino) (list (list destino))]
    [else
     (local
       [(define visitadas-nova (cons origem visitadas))
        (define vizinhos-nao-visitados (vizinhos origem G visitadas-nova))
        (define caminhos-vizinhos
          (encontra-todos-caminhos-lista vizinhos-nao-visitados destino G visitadas-nova))]
       (monta-caminhos origem caminhos-vizinhos))]))

;; encontra-todos-caminhos-lista: ListaDeString String Grafo ListaDeString -> ListaDeListaDeString
(define (encontra-todos-caminhos-lista origens destino G visitadas)
  (cond
    [(empty? origens) empty]
    [else
     (append (encontra-todos-caminhos (first origens) destino G visitadas)
             (encontra-todos-caminhos-lista (rest origens) destino G visitadas))]))

;; ==================== RESPOSTA ====================
(define todos-caminhos (encontra-todos-caminhos "Nashville" "Chicago" MAPA empty))
(define numero-de-caminhos (length todos-caminhos))
