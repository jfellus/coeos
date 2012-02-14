#!/bin/bash
##############################################################################################
# Script faisant appel aux scripts de creation des Makefile des differents programmes du projet
# Genere aussi un makefile pour pouvoir gerer les differentes compilations
##############################################################################################

PROJECTS="coeos leto sleto cc_leto"
SYSTEM=$(uname)
PWD=$(pwd)
OBJDIR="$PWD/obj/$SYSTEM"

source ../scripts/COMPILE_FLAG

bash Create_Coeos_Makefile.sh $*
bash Create_Leto_Makefile.sh $*
bash Create_Sleto_Makefile.sh  $*
bash Create_CCLeto_Makefile.sh $*

# Resets the previous Makefile
echo "" > Makefile

#default rule: compile all projects
echo "default: " >> Makefile
echo -e "\t\$(MAKE) all --jobs=$NUMBER_OF_CORES " >> Makefile

echo "all: $PROJECTS" >> Makefile
echo "" >> Makefile

for i in $PROJECTS
do
    echo "$i:" >> Makefile
    echo -e "\t\$(MAKE) -f Makefile.$i" >> Makefile
    echo "" >> Makefile
done

echo "clean:" >> Makefile
for i in $PROJECTS
do
    echo -e "\t\$(MAKE) -f Makefile.$i clean" >> Makefile
done
echo "" >> Makefile

echo "reset:" >> Makefile
for i in $PROJECTS
do
    echo -e "\t\$(MAKE) -f Makefile.$i reset" >> Makefile
done
echo "" >> Makefile

echo "install:" >> Makefile
for i in $PROJECTS
do
    echo -e "\t\$(MAKE) -f Makefile.$i install" >> Makefile
done
echo "" >> Makefile
