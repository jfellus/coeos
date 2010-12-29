#!/bin/bash
####################################################
#script permettant de generer un Makefile pour compiler cc_leto
#v2.0 Maillard M. & Baccon J.C.
#v3.0 Hirel J.
###################################################

#PG pb: attention il faut la librairie graphique pour compiler cc_leto. C'est anormal: corriger le code source il manque des ifdef AVEUGLE

####################################################
#Definition des chemins d'acces, options de compile etc...
####################################################
source ../../scripts/COMPILE_FLAG

# Nom du programme*
PROG_NAME="cc_leto"

#les bibliotheques et leurs chemins d'acces
GRAPHICLIBPATH="$HOME/bin_leto_prom/Libraries"

SCRIPTLIBPATH="$PWD/../../lib/$SYSTEM/script"
SCRIPTLIB="script_polyline"

# Initialisation des libs, includes et flags
LIBS="$GTKLIB $MATHLIB -pthread -lmxml -L$GRAPHICLIBPATH $GRAPHICLIB"
INCLUDES="$GTKINCLUDES -I$PWD/../../shared/include/ -I$PWD/include/ -I$PWD/include/leto/ -I$PWD/include/shared -I$PWD/"

#Version finale des libs, includes et flags
CFLAGS="$CFLAGS -O2 -DAVEUGLE "
FINALINCLUDES="$INCLUDES"
FINALLIBS="-L$SCRIPTLIBPATH -l$SCRIPTLIB $LIBS"
FINALCFLAGS="$CFLAGS -DCOUDES_POLYLINE"

#Les repertoires de destination des fichiers compiles
BINDIR=$PWD/../../bin/$SYSTEM
OBJPATH="$OBJPATH/$PROG_NAME_SHORT"

mkdir -p $OBJPATH

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

  echo "$OBJPATH/$FICHIER.o:$i" >> $MAKEFILE
  echo -e "\t@echo \"[processing $i...]\"">> $MAKEFILE
  echo -e "\t@(cd $CHEMIN; $CC $FINALCFLAGS $FINALINCLUDES -c -o $OBJPATH/$FICHIER.o $FICHIER.c)" >> $MAKEFILE
  echo "" >> $MAKEFILE
  OBJECTS="$OBJECTS $OBJPATH/${FICHIER}.o"
done

#pour l'edition de liens et le lien sur le binaire
echo "$PROG_NAME: $OBJECTS" >> $MAKEFILE
echo -e "\t@mkdir -p $BINDIR" >> $MAKEFILE
echo -e "\t@echo \"[Link objects...] \"">> $MAKEFILE
echo -e "\t@$CC  $OBJECTS -o $BINDIR/$PROG_NAME $FINALLIBS" >> $MAKEFILE
echo -e "\t@cp $BINDIR/$PROG_NAME $DIR_BIN_LETO_PROM " >> $MAKEFILE
echo "" >> $MAKEFILE

#regles additionnelles
echo "reset:" >> $MAKEFILE
echo -e "\trm -f  $OBJPATH/*.o $BINDIR/$PROG_NAME $DIR_BIN_LETO_PROM/$PROG_NAME" >> $MAKEFILE
echo "" >> $MAKEFILE

echo "clean:" >> $MAKEFILE
echo -e "\trm -f  $OBJPATH/*.o " >> $MAKEFILE
echo "" >> $MAKEFILE

echo "install:" >> $MAKEFILE
echo -e "\t@cp $BINDIR/$PROG_NAME $DIR_BIN_LETO_PROM " >> $MAKEFILE
echo "" >> $MAKEFILE
