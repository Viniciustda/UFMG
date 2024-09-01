ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: Tp.exe

Tp.exe: Tp.o
	gcc -o Tp.exe Tp.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

Tp.o: Tp.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c Tp.c
	
clean:
	del Tp.o
	del Tp.exe
