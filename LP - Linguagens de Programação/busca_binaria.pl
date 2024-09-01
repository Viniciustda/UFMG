#!/usr/bin/perl

# Caminho do perl

# instalação:
# apt-get install perl

# execução:
# perl busca_binaria.pl testes_busca/1/numeros inteiro

# formatacao entrada:
# numeros separados por linha
# tamanho não necessário(?)

# Bibliotecas:

# Não precisa mas detecta erros
# use strict; 
# use warnings;

# Funções:

# Função para realizar a busca binária
sub busca_binaria {
    my ($array, $buscando) = @_;
    my ($menor, $maior) = (0, $#$array);

    while ($menor <= $maior) {
        my $meio = int(($menor + $maior) / 2);
        
        if ($array->[$meio] == $buscando) {
            return 1;  # Número encontrado
        } elsif ($array->[$meio] < $buscando) {
            $menor = $meio + 1;
        } else {
            $maior = $meio - 1;
        }
    }

    return 0;  # Número não encontrado
}

# Função para verificar se a lista está ordenada (Não necessário se todos os casos forem ordenados)
# sub isOrdenado {
#     my @array = @_;
#     for my $i (1 .. $#array) {
#         return 0 if $array[$i] < $array[$i-1];
#     }
#     return 1;
# }

# Verifica se foram fornecidos dois argumentos
die "Uso: $0 <arquivo_lista> <inteiro_numero>\n" unless @ARGV == 2;

my ($arquivo, $numero) = @ARGV;

# Lê o arquivo e armazena os números em um arrayay
open my $fh, '<', $arquivo or die "Não foi possível abrir o arquivo $arquivo: $!";
my @array = map { chomp; $_ } <$fh>;
close $fh;

# Verifica se a lista está ordenada (Não necessário se todos os casos forem ordenados)
# die "A lista no arquivo não está ordenada.\n" unless isOrdenado(@array);

# Realiza a busca binária
my $result = busca_binaria(\@array, $numero);

if ($result) {
    print "O número $numero foi encontrado na lista.\n";
} else {
    print "O número $numero não foi encontrado na lista.\n";
}

