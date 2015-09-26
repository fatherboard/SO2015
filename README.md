# SO2015
Projecto SO

## Comandos ##

 - `pathname [arg1 ... arg5]`
executa o programa contido no ficheiro indicado pelo *pathname*, como processo filho, passando-lhe os argumentos opcionais que sejam indicados (até um máximo de 5 argumentos permitidos). O processo filho é lançado em *background*, ou seja a `par-shell` não espera pela terminação dos processos filho que são lançados e fica pronta a lançar novos processos filho. Caso o lançamento de algum processo falhe (por exemplo, devido a um *pathname* inválido ou a erro na criação de processo filho), a `par-shell` deve apresentar uma mensagem reportando o erro no *stderr* mas a `par-shell` não deve terminar.
 - `exit`
termina a `par-shell` de forma ordeira. Em particular, espera pela terminação de todos os processos filho (incluindo aqueles que não tenham ainda terminado aquando da ordem de `exit`). Após todos os processos filho terem terminado, a `par-shell` deve apresentar no *stdout* o *pid* e o inteiro devolvido por cada processo filho.



