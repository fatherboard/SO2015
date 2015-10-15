# Shell Paralela
Projecto Sistemas Operativos - IST, 1.º semestre 2015-2016

## Instalação, compilação e execução (Linux) ##

1. Clonar o repositório (`git clone https://github.com/fatherboard/SO2015.git` ou `git clone git@github.com:fatherboard/SO2015.git`)
1. Executar o comando `make`
1. Executar `./par-shell`

## Comandos ##

- `pathname [arg1 ... arg5]` executa o programa contido no ficheiro indicado pelo *pathname*, como processo filho, passando-lhe os argumentos opcionais que sejam indicados (até um máximo de 5 argumentos permitidos). O *pathname* pode referir-se a um programa na própria directoria de execução da `par-shell` ou na *path* do utilizador.
- `exit` termina a `par-shell` de forma ordeira. Em particular, espera pela terminação de todos os processos filho (incluindo aqueles que não tenham ainda terminado aquando da ordem de `exit`). Após todos os processos filho terem terminado, a `par-shell` deve apresentar no *stdout* o *pid* e o inteiro devolvido por cada processo filho.
