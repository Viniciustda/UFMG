/*
RELATÓRIO

1. Termo de compromisso

Os membros do grupo afirmam que todo o código desenvolvido para este
trabalho é de autoria própria.  Exceto pelo material listado no item 3
deste relatório, os membros do grupo afirmam não ter copiado
material da Internet nem obtiveram código de terceiros.

2. Membros do grupo e alocação de esforço

Vinicius Trindade Dias Abel <viniciustda@ufmg.br> 100%

3. Referências bibliográficas

http://en.wikipedia.org/wiki/Standard_streams
https://linux.die.net/man/2/pipe
https://linux.die.net/man/2/fork
https://linux.die.net/man/2/close
https://linux.die.net/man/3/exec
https://github.com
https://learn.microsoft.com/pt-br/cpp/c-language/?view=msvc-170
https://www.vivaolinux.com.br/dica/Utilizando-as-chamadas-de-sistema-dup()-e-dup2()/
https://www.ibm.com/docs/pt-br/aix/7.3?topic=e-exec-execl-execle-execlp-execv-execve-execvp-exect-fexecve-subroutine
https://www.programacaoprogressiva.net/2014/09/Pipes-em-C-Comunicao-entre-Processos-IPC-Interprocess-Communication.html

4. Estruturas de dados

Estruturas de dados:

struct execcmd: Representa um comando simples, contendo um array 
de strings (argv) para armazenar o comando e seus argumentos.

struct redircmd: Usada para comandos com redirecionamento de 
entrada (<) e saída (>), contendo o comando que será executado 
e informações sobre o arquivo que será redirecionado.

struct pipecmd: Representa comandos que utilizam pipes (|), 
ligando a saída de um comando à entrada de outro. Ela contém 
dois ponteiros para estruturas de comandos, um para o comando 
à esquerda e outro para o comando à direita do pipe.

Algoritmos:

Execução de comandos simples: Para a execução de comandos simples, 
usei execvp(), que busca o comando no PATH e o executa, 
substituindo o processo atual pelo novo processo. Em caso de erro, 
exibe uma mensagem e o processo é encerrado.

Redirecionamento de entrada/saída: Para redirecionamento, 
utilizei as funções dup2() para associar o descritor de arquivo correto ao stdin ou stdout. 
Então, o comando é executado com o redirecionamento adequado, 
utilizando o arquivo especificado para entrada ou saída.

Pipes: Para implementar comandos com pipes (|), 
utilizei pipe() para criar um canal entre dois processos. 
O processo à esquerda redireciona sua saída para o pipe, 
e o processo à direita redireciona sua entrada para o pipe. 
O processo pai aguarda ambos os processos filhos terminarem através de wait().
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* MARK NAME Vinicius Trindade Dias Abel */

/****************************************************************
 * Shell xv6 simplificado
 *
 * Este codigo foi adaptado do codigo do UNIX xv6 e do material do
 * curso de sistemas operacionais do MIT (6.828).
 ***************************************************************/

#define MAXARGS 10

/* Todos comandos tem um tipo.  Depois de olhar para o tipo do
 * comando, o código converte um *cmd para o tipo específico de
 * comando. */
struct cmd {
  int type; /* ' ' (exec)
               '|' (pipe)
               '<' or '>' (redirection) */
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // argumentos do comando a ser exec'utado
};

struct redircmd {
  int type;          // < ou > 
  struct cmd *cmd;   // o comando a rodar (ex.: um execcmd)
  char *file;        // o arquivo de entrada ou saída
  int mode;          // o modo no qual o arquivo deve ser aberto
  int fd;            // o número de descritor de arquivo que deve ser usado
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // lado esquerdo do pipe
  struct cmd *right; // lado direito do pipe
};

int fork1(void);  // Fork mas fechar se ocorrer erro.
struct cmd *parsecmd(char*); // Processar o linha de comando.

/* Executar comando cmd.  Nunca retorna. */
void
runcmd(struct cmd *cmd)
{
  //int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "tipo de comando desconhecido\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(0);
    /* MARK START task2
     * TAREFA2: Implemente codigo abaixo para executar
     * comandos simples. */

    if (execvp(ecmd->argv[0], ecmd->argv) == -1) {
        // Se execvp falhar, imprime o erro e sai
        fprintf(stderr, "Erro: comando %s não encontrado ou falhou ao executar.\n", ecmd->argv[0]);
        exit(-1);
    }

    /* MARK END task2 */
    break;

  case '>':
  case '<':
    rcmd = (struct redircmd*)cmd;
    /* MARK START task3
     * TAREFA3: Implemente codigo abaixo para executar
     * comando com redirecionamento. */

    int fd = open(rcmd->file, rcmd->mode, S_IRWXU);
    if (fd < 0) {
        perror("erro ao abrir arquivo");  // Exibe erro se falhar ao abrir
        exit(-1);
    }

    // Redireciona o descritor de arquivo para stdin ou stdout
    if (dup2(fd, rcmd->fd) < 0) {
        perror("erro ao redirecionar arquivo");
        exit(-1);
    }

    // Fecha o arquivo original
    close(fd);

    /* MARK END task3 */
    runcmd(rcmd->cmd);
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    /* MARK START task4
     * TAREFA4: Implemente codigo abaixo para executar
     * comando com pipes. */

    // Cria pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Cria o primeiro processo filho (comando à esquerda do pipe)
    if (fork() == 0) {
        // Redireciona stdout para o pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);  // Fecha leitura no processo filho
        close(pipefd[1]);  // Fecha escrita original no processo filho
        runcmd(pcmd->left);  // Executa o comando à esquerda do pipe
        exit(0);
    }

    // Cria o segundo processo filho (comando à direita do pipe)
    if (fork() == 0) {
        // Redireciona stdin para o pipe
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);  // Fecha escrita no processo filho
        close(pipefd[0]);  // Fecha leitura original no processo filho
        runcmd(pcmd->right);  // Executa o comando à direita do pipe
        exit(0);
    }

    // Fecha os pipes no processo pai
    close(pipefd[0]);
    close(pipefd[1]);

    // Espera os processos filhos terminarem
    wait(NULL);
    wait(NULL);
    
    /* MARK END task4 */
    break;
  }    
  exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  if (isatty(fileno(stdin)))
    fprintf(stdout, "$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int r;

  // Ler e rodar comandos.
  while(getcmd(buf, sizeof(buf)) >= 0){
    /* MARK START task1 */
    /* TAREFA1: O que faz o if abaixo e por que ele é necessário?
     * Insira sua resposta no código e modifique o fprintf abaixo
     * para reportar o erro corretamente. 
      
     * O bloco if verifica se o comando digitado começa com "cd ".
     * O comando `cd` altera o diretório atual do processo.
     * Portanto, o bloco é necessário para garantir que o shell
     * atualize corretamente o diretório de trabalho.*/
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      buf[strlen(buf)-1] = 0;
      if(chdir(buf+3) < 0)
        fprintf(stderr, "Erro: não foi possível mudar para o diretório %s\n", buf+3);
      continue;
    }
    /* MARK END task1 */

    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait(&r);
  }
  exit(0);
}

int
fork1(void)
{
  int pid;
  
  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

/****************************************************************
 * Funcoes auxiliares para criar estruturas de comando
 ***************************************************************/

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

/****************************************************************
 * Processamento da linha de comando
 ***************************************************************/

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '<':
    s++;
    break;
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

/* Copiar os caracteres no buffer de entrada, comeando de s ate es.
 * Colocar terminador zero no final para obter um string valido. */
char 
*mkcopy(char *s, char *es)
{
  int n = es - s;
  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;
  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;
  
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}

// vim: expandtab:ts=2:sw=2:sts=2

