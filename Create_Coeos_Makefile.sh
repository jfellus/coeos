#!/bin/bash
################################################################################
# Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2014)
#promethe@ensea.fr
#
# Authors: P. Andry, J.C. Baccon, D. Bailly, A. Blanchard, S. Boucena, A. Chatty, N. Cuperlier, P. Delarboulas, P. Gaussier, 
# C. Giovannangeli, C. Grand, L. Hafemeister, C. Hasson, S.K. Hasnain, S. Hanoune, J. Hirel, A. Jauffret, C. Joulain, A. Karaouzène,  
# M. Lagarde, S. Leprêtre, M. Maillard, B. Miramond, S. Moga, G. Mostafaoui, A. Pitti, K. Prepin, M. Quoy, A. de Rengervé, A. Revel ...
#
# See more details and updates in the file AUTHORS 
#
# This software is a computer program whose purpose is to simulate neural networks and control robots or simulations.
# This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software. 
# You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". 
# As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, 
# users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability. 
# In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software, 
# that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge. 
# Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured 
# and, more generally, to use and operate it in the same conditions as regards security. 
# The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
################################################################################
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

CFLAGS="-Wall -std=c99 -pedantic -D_REENTRANT -D_GNU_SOURCE -DDAEMON_COM -D`uname` -Wno-variadic-macros -DSYMBOLIQUE_VERSION"

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
	INCLUDES="$GTKINCLUDES -I$PWD/include/leto -I$SIMULATOR_PATH/shared/include -I$PWD/include -I$PWD/include/shared -I. -I$HOME/.local/include"

	#Version finale des libs, includes et flags
	FINALINCLUDES="$INCLUDES"
	FINALLIBS="-L$SCRIPTLIBPATH -lscript_symb -L$GRAPHICLIBPATH -l${GRAPHICLIB} $LIBS -L$HOME/.local/lib -lblc"
	
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





