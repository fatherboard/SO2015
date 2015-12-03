# Shell Paralela
Projecto Sistemas Operativos - IST, 1.º semestre 2015-2016

## Instalação, compilação e execução (Linux) ##

1. Clonar o repositório (`git clone https://github.com/pbucho/SO2015.git` ou `git clone git@github.com:fatherboard/SO2015.git`)
2. Executar o comando `make`
3. Executar `./par-shell`
4. Executar `./par-shell-terminal par-shell-in` em cada terminal que se queira que comunique com par-shell

## Comandos ##

- `pathname [arg1 ... arg5]` executa o programa contido no ficheiro indicado pelo *pathname*, como processo filho, passando-lhe os argumentos opcionais que sejam indicados (até um máximo de 5 argumentos permitidos). O *pathname* pode referir-se a um programa na própria directoria de execução da `par-shell` ou na *path* do utilizador.
- `exit` termina a `par-shell-terminal` de forma ordeira, enviando uma mensagem a par-shell a dizer que terminou.
- `exit-global` termina a par-shell (e todos os terminais atraves de um sinal). Em particular, espera pela terminação de todos os processos filho. Após todos os processos filho terem terminado, a `par-shell` deve apresentar no *stdout* e em *log.txt* o *pid*, tempo de execucao e o inteiro devolvido por cada processo filho.

### Comunicação entre processos ###

É necessário estabelecer um protocolo de aplicação para comunicação entre os processos da `par-shell` e dos `par-shell-terminal` (protocolo PSP, par-shell protocol :P )

- `REG [PID]` - enviado por um novo `par-shell-terminal` à `par-shell`, indica que foi criado um novo terminal e que a `par-shell` deve fazer o registo do seu PID.
- `DIE [PID]` - enviado por um `par-shell-terminal` à `par-shell`, indica que o terminal vai ser encerrado e que a `par-shell` deve eliminar o registo do seu PID.
- `stats` - enviado por um `par-shell-terminal` à `par-shell`, pede que a `par-shell` envie ao terminal a informação relativa à execução de processos.
- `RSTATS [DATA]` - enviado pela `par-shell` a um terminal, é a resposta ao comando `stats`.
