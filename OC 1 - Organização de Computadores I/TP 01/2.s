.data
vetor: .word 200, 190, 340, 100 # exemplo

##### START MODIFIQUE AQUI START #####
#
# Este espaço é para você definir as suas constantes e vetores auxiliares.
#
##### END MODIFIQUE AQUI END #####

.text
jal ra, main
# Ao final do reajuste (aplica_reajuste) você deve retornar o programa para a próxima instrução abaixo, que consiste na correção/verificação.

##### START INSTRUÇÃO DE CORREÇÃO/VERIFICAÇÃO #####

# utilizado para correção (considerando um limiar de 200 para o vetor de exemplo após a aplicação do reajuste.
addi a4, x0, 3 # configurando a quantidade de salários acima do limiar de 200.
beq a4, t0, FIM # Verifica a quantidade de salários acima do limiar.

##### END INSTRUÇÃO DE CORREÇÃO/VERIFICAÇÃO #####

main:
##### START MODIFIQUE AQUI START #####

addi x5, x0, 0			# x5 armazena a qunatidade de salarios acima do limiar e eh inicializado com 0
addi x6, x0, 3			# x6 sera utilizado para aplicar o reajuste nos salarios

sw x1, 0(x2)			# x2 armazena o primeiro endereco de retorno	### poderia fazer apenas (addi x2, x1, 0) nao vamos armazenar outro valor na pilha
jal x1, aplica_reajuste	# pula para a funcao aplica_reajuste e salva em x1 o novo endereco de retorno

lw x1, 0(x2)			# x1 recebe novamente o primeiro endereco de retorno, para sair de main		### se tivesse feito (addi x2, x1, 0), esta linha seria (addi x1, x2, 0)
jalr x0, 0(x1)			# sai de main

##### END MODIFIQUE AQUI END #####

# dica, você deve salvar o endereço da primeira chamada (em stack (sp)) para depois recuperá-lo.

aplica_reajuste:
##### START MODIFIQUE AQUI START #####

	lw x7, 0(x10)						# x7 recebe salario

	mul x7, x7, x6						# x7 eh multiplicado por 3
	srli x7, x7, 1						# x7 eh dividido por 2, ou seja, x7 foi multiplicado por 1.5, que eh correspondente ao reajuste de 50%

	sw x7, 0(x10)						# salva o novo salario no lugar do antigo

	blt x12, x7, L1						# se o limiar for menor que o novo salario, desvia para L1
	blt x12, x7, L1						# se o limiar for igual ao novo salario, desvia para L1
	addi x5, x5, 1						# se o novo salario for maior que o limiar, x5 eh incrementado em 1

	L1:
		addi x10, x10, 4				# atualiza para o proximo salario do vetor
		addi x11, x11, -1				# atualiza tamanho do vetor

		blt x0, x11, aplica_reajuste	# se ainda houver salario desatualizado, retorna para aplica_reajuste para atualiza-lo

		jalr x0, 0(x1)					# se todos os salarios foram atualizados, retorna para main

##### END MODIFIQUE AQUI END #####

FIM: addi x0, x0, 1