RIVER INF - RICARDO UBIRAJARA

COMO RODAR O PROJETO?
Primeiro baixe o MinGW diretamente do site oficial.
Descompacte o MinGW em qualquer pasta do sistema, por exemplo C:/MinGW.

Depois adicione o caminho da pasta bin do MinGW na variável de ambiente PATH do Windows.
Abra um terminal e verifique se funciona executando gcc --version.

Em seguida baixe a Raylib já compilada para MinGW.
Extraia a Raylib exatamente em C:/raylib mantendo as pastas include e lib.

Depois disso clone ou copie o projeto para qualquer pasta do seu computador.
Certifique-se de que o arquivo Makefile está na raiz do projeto.

Abra o prompt de comando ou o terminal na pasta do projeto.
Execute o comando "mingw32-make river-inf".

O executável do jogo será gerado na mesma pasta.
O jogo será executado automaticamente após a compilação.

---

ESTRUTURA DO PROJETO:
RIVER INF/
│   Makefile                     # Script de compilação do projeto (build, testes e execução)
│   readme.md                    # Documentação básica do projeto
│   readme.txt                   # Versão alternativa da documentação em txt
│   river-inf.exe                # Executável final do jogo compilado no Windows
│
└── src/                         # Código-fonte principal do projeto
    │   main.c                   # Ponto de entrada do programa e inicialização do jogo
    │
    ├── assets/                  # Recursos externos do jogo
    │   ├── airplanes/           # Imagens das skins do jogador
    │   │   air1.png             # Skin de avião 1
    │   │   air2.png             # Skin de avião 2
    │   │   air3.png             # Skin de avião 3
    │   │
    │   ├── maps/                # Arquivos de mapas das fases
    │   │   fase1.txt            # Layout da fase 1
    │   │   fase2.txt            # Layout da fase 2
    │   │   fase3.txt            # Layout da fase 3
    │   │   fase4.txt            # Layout da fase 4
    │   │   fase5.txt            # Layout da fase 5
    │   │
    │   └── obstacles/           # Imagens dos obstáculos do jogo
    │       bridge.png           # Ponte
    │       gas.png              # Gasolina / reabastecimento
    │       helicopter.png       # Helicóptero inimigo
    │       ship.png             # Navio inimigo
    │       terrain.png          # Terra
    │
    ├── core/                    # Implementação da lógica principal do jogo
    │   bullet.c                 # Sistema de tiros e projéteis
    │   game.c                   # Gerenciamento de estados e loop principal
    │   highscore.c              # Sistema de pontuação e ranking
    │   map.c                    # Leitura, carregamento e interpretação dos mapas
    │   obstacle.c               # Sistema de obstáculos, inimigos e colisões
    │   player.c                 # Lógica e controle do jogador
    │
    ├── include/                 # Headers (interfaces dos módulos)
    │   bullet.h                 # Interface do sistema de tiros
    │   defines.h                # Constantes globais e definições gerais
    │   game.h                   # Interface do módulo principal do jogo
    │   highscore.h              # Interface do sistema de pontuação
    │   map.h                    # Interface do sistema de mapas
    │   obstacle.h               # Interface do sistema de obstáculos
    │   player.h                 # Interface do sistema do jogador
    │
    └── tests/                   # Testes unitários dos módulos
        test_bullet.c            # Testes do sistema de tiros
        test_highscore.c         # Testes do sistema de pontuação
        test_map.c               # Testes do carregamento de mapas
        test_obstacle.c          # Testes do sistema de obstáculos
        test_player.c            # Testes da lógica do jogador
