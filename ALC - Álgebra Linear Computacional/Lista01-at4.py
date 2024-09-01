import numpy as np

'''
Para calcular C = A*B, onde A ́e (m, n) e B ́e (n, p) usando uma soma de produtos externos (colunas vezes linhas), como  ́e que os laços a seguir devem ser reordenados?

For i = 1 to m
    For j = 1 to p 
        For k = 1 to n 
            C(i, j) = C(i, j) + A(i, k) ∗ A(k, j)


    A = 2 1     B = 6 4 7 2
        3 4         1 9 8 1
        5 7

        (mxn)   (nxp)
        (3x2)   (2x4)
        
        

'''

#A = np.array([[2, 1], [3, 4], [5, 7]])
#B = np.array([[6, 4, 7, 2], [1, 9, 8, 1]])
#C = A @ B

A = []

for i in range(3):
	A.append( [0] * 2 )

A[0][0] = 2
A[0][1] = 1
A[1][0] = 3
A[1][1] = 4
A[2][0] = 5
A[2][1] = 7

B = []

for i in range(2):
	B.append( [0] * 4 )

B[0][0] = 6
B[0][1] = 4
B[0][2] = 7
B[0][3] = 2
B[1][0] = 1
B[1][1] = 9
B[1][2] = 8
B[1][3] = 1
'''
#print('A.shape =', A.shape)
print('A =')
print(A)
print('\n')

#print('B.shape =', B.shape)
print('B =')
print(B)
print('\n')
'''
C = []

for i in range(3):
	C.append( [0] * 4 )
'''
#print('C.shape =', C.shape)
print('C =')
print(C)
print('\n')
'''
m = 3
n = 2
p = 4

for i in range(m):
    for j in range(p):
        for k in range(n):
            C[i][j] = C[i][j] + A[i][k] * B[k][j]
            

#print('C.shape =', C.shape)
print('C =')
print(C)
print('\n')
print('\n')
print('\n')

C[0][0] = 0
C[0][1] = 0
C[0][2] = 0
C[0][3] = 0
C[1][0] = 0
C[1][1] = 0
C[1][2] = 0
C[1][3] = 0
C[2][0] = 0
C[2][1] = 0
C[2][2] = 0
C[2][3] = 0

for k in range(n):
    for j in range(p):
        for i in range(m):
            C[i][j] = C[i][j] + A[i][k] * B[k][j]

#print('C.shape =', C.shape)
print('C =')
print(C)
print('\n')


'''
    A = 2 1     B = 6 4 7 2
        3 4         1 9 8 1
        5 7

        (mxn)   (nxp)
        (3x2)   (2x4)
        
'''