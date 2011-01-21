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
source ../../scripts/COMPILE_FLAG

# Nom du programme*
PROG_NAME="sleto"

#DIVERS
CC="gcc"
SYSTEM=`uname`
PWD=`pwd`

SIMULATOR_PATH=$PWD/../..

#les bibliotheques et leurs chemins d'acces
GRAPHICLIBPATH="$PWD/../../lib/$SYSTEM/graphique"
GRAPHICLIB="graphique"

SCRIPTLIBPATH="$PWD/../../lib/$SYSTEM/script"

GTKINCLUDES="`pkg-config --cflags gtk+-2.0 gthread-2.0`"
GTKLIB=`pkg-config --libs gtk+-2.0 gthread-2.0`

# Initialisation des libs, includes et flags
LIBS="$GTKLIB -lmxml"
INCLUDES="$GTKINCLUDES -I$PWD/include/leto -I$PWD/../../shared/include -I$PWD/include -I$PWD/include/shared -I."
CFLAGS="-g3 -Wall -pedantic -D_REENTRANT -D_GNU_SOURCE -DDAEMON_COM -D`uname` -Wno-variadic-macros"

# Gestion des parametres passes au Create_Makefile
CFLAGS="$CFLAGS -DUSE_CAIRO"

#Version finale des libs, includes et flags
FINALINCLUDES="$INCLUDES"
FINALLIBS="$LIBS -L$SCRIPTLIBPATH $SCRIPTLIB -L$GRAPHICLIBPATH $GRAPHICLIB"
FINALCFLAGS="$CFLAGS -DLETO -DSYMBOLIQUE_VERSION"

#Les repertoires de destination des fichiers compiles
BINDIR=$PWD/../../bin/$SYSTEM
HOMEBINDIR=$HOME/bin_leto_prom/.
OBJDIR="$PWD/obj/$SYSTEM/$PROG_NAME"
mkdir -p $OBJDIR

#le fichier xml pour le menu de leto
XML="./xml/menu_leto.xml"

#Gestion des fichiers a compiler
SOURCES=`find src/ -depth -type f -name '*.c' | sed -e '/cc_leto.c/d'`
OBJECTS=""

####################################################
#Creation du Makefile
####################################################

MAKEFILE="Makefile.$PROG_NAME"

#ecrasement du Makefile precedent
echo "" > $MAKEFILE
#regle par defaut
echo "default: $PROG_NAME" >> $MAKEFILE
echo "" >> $MAKEFILE

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
echo "$PROG_NAME: $OBJECTS" >> $MAKEFILE
echo -e "\t@mkdir -p $BINDIR" >> $MAKEFILE
echo -e "\t@echo \"[Link objects...] \"">> $MAKEFILE
echo -e "\t@$CC $OBJECTS -o $BINDIR/$PROG_NAME $FINALLIBS " >> $MAKEFILE
echo -e "\t@cp $XML $BINDIR/. " >> $MAKEFILE
echo -e "\t@cp $BINDIR/$PROG_NAME $XML $HOMEBINDIR " >> $MAKEFILE
echo "" >> $MAKEFILE

#regles additionnelles
echo "reset:" >> $MAKEFILE
echo -e "\trm -f  $OBJDIR/*.o $BINDIR/$PROG_NAME $HOMEBINDIR/$PROG_NAME" >> $MAKEFILE
echo "" >> $MAKEFILE

echo "clean:" >> $MAKEFILE
echo -e "\trm -f  $OBJDIR/*.o " >> $MAKEFILE
echo "" >> $MAKEFILE

echo "install:" >> $MAKEFILE
echo -e "\t@cp $BINDIR/$PROG_NAME $HOMEBINDIR " >> $MAKEFILE
echo "" >> $MAKEFILE
