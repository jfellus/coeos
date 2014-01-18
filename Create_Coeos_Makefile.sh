#!/bin/bash
####################################################
#script permettant de generer un Makefile pour compiler leto
#v2.0 Maillard M. & Baccon J.C.
#v2.1 Lagarde M. & Archambault F.
#v3.0 Hirel J.
###################################################


####################################################
#Definition des chemins d'acces, options de compile etc...
####################################################
####################################################
#definition de $CFLAGS $FLAGS_OPTIM $FLAGS_DEBUG $INCLUDE2 (pour specif mac)
####################################################
source ../scripts/COMPILE_FLAG



#DIVERS
CC="gcc"
SYSTEM=`uname`
PWD=`pwd`

GTKINCLUDES="`pkg-config --cflags gtk+-2.0 gthread-2.0`"
GTKLIB=`pkg-config --libs gtk+-2.0 gthread-2.0`

ALL_CONFIGURATIONS=(debug release)

CFLAGS="-Wall -pedantic -D_REENTRANT -D_GNU_SOURCE -DDAEMON_COM -D`uname` -Wno-variadic-macros -DSYMBOLIQUE_VERSION"

MAKEFILE="Makefile.coeos"
	
#ecrasement du Makefile precedent
	echo "" > $MAKEFILE
#regle par defaut
	echo "default: coeos coeos_debug" >> $MAKEFILE
	echo "" >> $MAKEFILE
	echo "clean: clean_release clean_debug" >> $MAKEFILE
	echo "" >> $MAKEFILE
	echo "reset: reset_release reset_debug" >> $MAKEFILE
	echo "" >> $MAKEFILE


for CONFIGURATION in ${ALL_CONFIGURATIONS[@]}
do

# Nom du programme

	if [ $CONFIGURATION == "debug" ]
	then
		FINALCFLAGS="$CFLAGS -g"
		POSTFIX="_debug"
		
	else
		POSTFIX=""
		FINALCFLAGS="$CFLAGS -O2"	
	fi
	PROG_NAME="coeos$POSTFIX"
	
	# Initialisation des libs, includes et flags
	LIBS="$GTKLIB -lmxml -lm -ldl"
	INCLUDES="$GTKINCLUDES -I$PWD/include/leto -I$SIMULATOR_PATH/shared/include -I$PWD/include -I$PWD/include/shared -I."

	#Version finale des libs, includes et flags
	FINALINCLUDES="$INCLUDES"
	FINALLIBS="-L$SCRIPTLIBPATH -lscript_symb -L$GRAPHICLIBPATH -l${GRAPHICLIB} $LIBS"
	
	OBJDIR=$OBJPATH/$PROG_NAME
	
	mkdir -p $OBJDIR
	
#le fichier xml pour le menu de leto
	XML="./xml/menu_leto_coeos.xml"
	
#Gestion des fichiers a compiler
	SOURCES=`find src/ -depth -type f -name '*.c' | sed -e '/cc_leto.c/d'`
	OBJECTS=""
	
####################################################
#Creation du Makefile
####################################################
	

	
# creer les regles
#pour chaque  .o
	for i in $SOURCES
	do
	  echo "processing '$i'"
	  FICHIER=`basename $i .c`
	  CHEMIN=`echo $i | sed -e s@$FICHIER.c@@`
	  echo "$OBJDIR/$FICHIER.o:$i" >> $MAKEFILE
	  echo -e "\t@echo \"[processing $i...]\"">> $MAKEFILE
	  echo -e "\t@(cd $CHEMIN; $CC $FINALCFLAGS $FINALINCLUDES -c -o $OBJDIR/$FICHIER.o $FICHIER.c)" >> $MAKEFILE
	  echo "" >> $MAKEFILE
	  OBJECTS="$OBJECTS $OBJDIR/$FICHIER.o"
	done
	
#pour l'edition de liens et le lien sur le binaire
	echo "$PROG_NAME: $OBJECTS  " >> $MAKEFILE
	echo -e "\t@mkdir -p $BINDIR" >> $MAKEFILE
	echo -e "\t@echo \"[Link objects...] \"">> $MAKEFILE
	echo -e "\t@$CC $OBJECTS -o $BINDIR/$PROG_NAME $FINALLIBS -rdynamic" >> $MAKEFILE
	echo -e "\t@cp $XML  $DIR_BIN_LETO_PROM/. " >> $MAKEFILE
	echo -e "\t@cp $BINDIR/$PROG_NAME $DIR_BIN_LETO_PROM " >> $MAKEFILE
    echo -e "\t@cp  resources/coeos_icon.png $DIR_BIN_LETO_PROM/resources/coeos_icon.png" >> $MAKEFILE

	echo "" >> $MAKEFILE

#regles additionnelles
	echo "clean_$CONFIGURATION:" >> $MAKEFILE
	echo -e "\trm -f  $OBJDIR/*.o " >> $MAKEFILE
	echo "" >> $MAKEFILE

	echo "reset_$CONFIGURATION:clean_$CONFIGURATION" >> $MAKEFILE
	echo -e "\trm -f  $BINDIR/$PROG_NAME $DIR_BIN_LETO_PROM/$PROG_NAME" >> $MAKEFILE
	echo "" >> $MAKEFILE
	
done





