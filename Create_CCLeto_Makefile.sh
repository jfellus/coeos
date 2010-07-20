#!/bin/bash
####################################################
#script permettant de generer un Makefile pour compiler cc_leto
#v2.0 Maillard M. & Baccon J.C.
#v3.0 Hirel J.
###################################################



####################################################
#Definition des chemins d'acces, options de compile etc...
####################################################
# Nom du programme*
PROG_NAME="cc_leto"

#DIVERS
CC="gcc"
SYSTEM=`uname`
PWD=`pwd`

#les bibliotheques et leurs chemins d'acces
GRAPHICLIBPATH="$HOME/bin_leto_prom/Libraries"
GRAPHICLIB="graphique"

SCRIPTLIBPATH="$PWD/../../lib/$SYSTEM/script"
SCRIPTLIB="script_polyline"

MATHLIB=" -lm"

GTKINCLUDES=" `pkg-config --cflags gtk+-2.0 gthread-2.0`"
GTKLIB=`pkg-config --libs gtk+-2.0 gthread-2.0`

# Initialisation des libs, includes et flags
LIBS="$GTKLIB $MATHLIB -pthread -lmxml -L$GRAPHICLIBPATH -l$GRAPHICLIB"
INCLUDES="$GTKINCLUDES -I$PWD/../../shared/include/ -I$PWD/include/ -I$PWD/include/leto/ -I$PWD/include/shared -I$PWD/"
CFLAGS=" -O2  -Wall  -D_REENTRANT -D_GNU_SOURCE -DDAEMON_COM -D`uname` -DAVEUGLE -Wno-variadic-macros"

#Version finale des libs, includes et flags
FINALINCLUDES="$INCLUDES"
FINALLIBS="-L$SCRIPTLIBPATH -l$SCRIPTLIB $LIBS"
FINALCFLAGS="$CFLAGS -DCOUDES_POLYLINE"

#Les repertoires de destination des fichiers compiles
BINDIR=$PWD/../../bin/$SYSTEM
HOMEBINDIR=$HOME/bin_leto_prom/.
OBJDIR="$PWD/obj/$SYSTEM/$PROG_NAME"
mkdir -p $OBJDIR

#Gestion des fichiers a compiler
SOURCES="src/leto/cc_leto/cc_leto.c ./src/leto/leto_global_var.c ./src/leto/script.c ./src/leto/creation.c ./src/leto/ecrit_res8.3.c ./src/leto/export.c ./src/leto/leto_cle2.c ./src/leto/outils.c ./src/leto/gere_coudes.c src/shared/bend.c"
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
  #CHEMIN=`echo $i | sed -e s@$FICHIER.c@@`
  # Correction BUG : devrait fonction sur tous les chemins
  CHEMIN=`dirname $i`

  echo "$OBJDIR/$FICHIER.o:$i" >> $MAKEFILE
  echo -e "\t@echo \"[processing $i...]\"">> $MAKEFILE
  echo -e "\t@(cd $CHEMIN; $CC $FINALCFLAGS $FINALINCLUDES -c -o $OBJDIR/$FICHIER.o $FICHIER.c)" >> $MAKEFILE
  echo "" >> $MAKEFILE
  OBJECTS="$OBJECTS $OBJDIR/${FICHIER}.o"
done

#pour l'edition de liens et le lien sur le binaire
echo "$PROG_NAME: $OBJECTS" >> $MAKEFILE
echo -e "\t@mkdir -p $BINDIR" >> $MAKEFILE
echo -e "\t@echo \"[Link objects...] \"">> $MAKEFILE
echo -e "\t@$CC  $OBJECTS -o $BINDIR/$PROG_NAME $FINALLIBS" >> $MAKEFILE
echo -e "\t@cp $BINDIR/$PROG_NAME $HOMEBINDIR " >> $MAKEFILE
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
