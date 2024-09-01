#!/usr/bin/perl

# Caminho do perl

# instalação:
# apt-get install perl

# execução:
# perl multiplicacao_matrizes.pl testes_matriz/1/arquivo_a testes_matriz/1/arquivo_b arquivo_c-resultado_teste_1

# formatacao entrada:
# matriz visual - colunas separadas por espaço, linhas separadas por quebra linhas
# tamanho não necessário(?)

# Bibliotecas:

# Não precisa mas detecta erros
# use strict; 
# use warnings;

# Funções

# Função para ler uma matriz de um arquivo
sub leMatrizDoArquivo {
    my $arquivo = shift;
    open my $fh, '<', $arquivo or die "Não foi possível abrir $arquivo: $!";
    my @matriz;
    while (my $linha = <$fh>) {
        chomp $linha;
        push @matriz, [split /\s+/, $linha];
    }
    close $fh;
    return @matriz;
}

# Função para multiplicar duas matrizes
sub multiplicaMatrizes {
    my ($a, $b) = @_;
    my $tamanho = scalar @$a;
    my @result;
    for my $i (0..$tamanho-1) {
        for my $j (0..$tamanho-1) {
            my $sum = 0;
            for my $k (0..$tamanho-1) {
                $sum += $a->[$i][$k] * $b->[$k][$j];
            }
            $result[$i][$j] = $sum;
        }
    }
    return @result;
}

# Função para escrever uma matriz em um arquivo
sub escreveMatriz {
    my ($matriz, $arquivo) = @_;
    open my $fh, '>', $arquivo or die "Não foi possível abrir $arquivo para escrita: $!";
    for my $row (@$matriz) {
        print $fh join(' ', @$row), "\n";
    }
    close $fh;
}

# Main ###

# Verifica se foram fornecidos três argumentos
# Termina A menos que
die "Uso: $0 <arquivo_matriz_A> <arquivo_matriz_B> <arquivo_saida>\n" unless @ARGV == 3;

my ($arquivo_a, $arquivo_b, $arquivo_c) = @ARGV;

# Lê as matrizes dos arquivos
my @matriz_a = leMatrizDoArquivo($arquivo_a);
my @matriz_b = leMatrizDoArquivo($arquivo_b);

# Verifica se as matrizes são quadradas e compatíveis para multiplicação
# Especificação exige matrizes quadradas
my $tamanho = scalar @matriz_a;
die "As matrizes devem ser quadradas e de mesmo tamanho\n"
    unless $tamanho == scalar @matriz_b && $tamanho == scalar @{$matriz_a[0]} && $tamanho == scalar @{$matriz_b[0]};

# Realiza a multiplicação
my @result = multiplicaMatrizes(\@matriz_a, \@matriz_b);

# Escreve o resultado no arquivo de saída
escreveMatriz(\@result, $arquivo_c);

print "Multiplicação concluída. Resultado salvo em $arquivo_c\n";
