# Shell Paralela
Projecto Sistemas Operativos - IST, 1.º semestre 2015-2016

## Instalação, compilação e execução (Linux) ##

1. Clonar o repositório (`git clone https://github.com/pbucho/SO2015.git` ou `git clone git@github.com:fatherboard/SO2015.git`)
1. Executar o comando `make`
1. Executar `./par-shell`

## Comandos ##

- `pathname [arg1 ... arg5]` executa o programa contido no ficheiro indicado pelo *pathname*, como processo filho, passando-lhe os argumentos opcionais que sejam indicados (até um máximo de 5 argumentos permitidos). O *pathname* pode referir-se a um programa na própria directoria de execução da `par-shell` ou na *path* do utilizador.
- `exit` termina a `par-shell` de forma ordeira. Em particular, espera pela terminação de todos os processos filho (incluindo aqueles que não tenham ainda terminado aquando da ordem de `exit`). Após todos os processos filho terem terminado, a `par-shell` deve apresentar no *stdout* o *pid* e o inteiro devolvido por cada processo filho.

### Comunicação entre processos ###

É necessário estabelecer um protocolo de aplicação para comunicação entre os processos da `par-shell` e dos `par-shell-terminal` (protocolo PSP, par-shell protocol :P )

- `REG [PID]` - enviado por um novo `par-shell-terminal` à `par-shell`, indica que foi criado um novo terminal e que a `par-shell` deve fazer o registo do seu PID.
- `DIE [PID]` - enviado por um `par-shell-terminal` à `par-shell`, indica que o terminal vai ser encerrado e que a `par-shell` deve eliminar o registo do seu PID.
- `STATS` - enviado por um `par-shell-terminal` à `par-shell`, pede que a `par-shell` envie ao terminal a informação relativa à execução de processos.
- `RSTATS [DATA]` - enviado pela `par-shell` a um terminal, é a resposta ao comando `stats`.