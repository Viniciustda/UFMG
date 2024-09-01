ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: passaro_andante.exe teclado.exe bouncer.exe missil.exe frogger.exe tela.exe enduro-0.exe

passaro_andante.exe: passaro_andante.o
	gcc -o passaro_andante.exe passaro_andante.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

passaro_andante.o: passaro_andante.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c passaro_andante.c
	
bouncer.exe: bouncer.o
	gcc -o bouncer.exe bouncer.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

bouncer.o: bouncer.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c bouncer.c	
	
teclado.exe: teclado.o
	gcc -o teclado.exe teclado.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

teclado.o: teclado.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c teclado.c	

missil.exe: missil.o
	gcc -o missil.exe missil.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

missil.o: missil.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c missil.c		
	
frogger.exe: frogger.o
	gcc -o frogger.exe frogger.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

frogger.o: frogger.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c frogger.c		

tela.exe: tela.o
	gcc -o tela.exe tela.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

tela.o: tela.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c tela.c	
	
enduro-0.exe: enduro-0.o
	gcc -o enduro-0.exe enduro-0.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

enduro-0.o: enduro-0.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c enduro-0.c	
	
	
clean:
	del passaro_andante.o 
	del passaro_andante.exe
	del missil.o 
	del missil.exe
	del teclado.o 
	del teclado.exe
	del bouncer.o 
	del bouncer.exe	
	del frogger.o
	del frogger.exe
	del tela.o
	del tela.exe
	del enduro-0.o
	del enduro-0.exe

