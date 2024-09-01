.data
vetor: .word 0 0 0 0

##### START MODIFIQUE AQUI START #####
#
# Este espaço é para você definir as suas constantes e vetores auxiliares.
#
##### END MODIFIQUE AQUI END #####

.text
jal x1, contador
addi x14, x0, 2 # utilizado para correção
beq x14, x10, FIM # Verifica # de pares
beq x14, x11, FIM # Verifica # de ímpares

##### START MODIFIQUE AQUI START #####

contador: 

	# Inicializa o numero de pares e impares como 0
	addi x10, x0, 0
	addi x11, x0, 0

	# Inicializa x30 com 2, para realizar as comparacoes e descobrir se o numero eh par ou impar
	addi x30, x0, 2

	# Verifica se o vetor tem tamanho maior que 0, para realizar a contagem
	blt x0, x28, loop
	jalr x0, 0(x1)

	loop: 
		lw x29, 0(x7)			# x29 recebe numero do vetor

		addi x28, x28, -1		# tamanho do vetor eh decrementado em 1
		addi x7, x7, 4			# posiciona x7 no proximo numero do vetor

		rem x29, x29, x30		# calcula x29 mod 2
		beq x29, x0, par		# verifica se x29 = 0, se sim, o numero eh par e desvia para par

		addi x11, x11, 1		# caso contrario, o numero eh impar e x11 eh incrementado em 1
		blt x0, x28, loop		# verifica se tamanho do vetor > 0, se sim, voltamos para o loop
		jalr x0, 0(x1)			# caso contrario, termina a funcao contador

		par: 
			addi x10, x10, 1	# o numero eh par e x10 eh incrementado em 1
			blt x0, x28, loop	# verifica se tamanho do vetor > 0, se sim, voltamos para o loop
			jalr x0, 0(x1)		# caso contrario, termina a funcao contador

##### END MODIFIQUE AQUI END #####

FIM: addi x0, x0, 1