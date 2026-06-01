#!/bin/bash

ARQ=$1
ENTRADA=$2
SAIDA=$3
V=10
TEMPOS=()
DESVIOS=()
MEDIA=0

> resultado.txt
 
for((i=0; i<V; i++)); do
	TEMPOS+=($(./$ARQ $ENTRADA $SAIDA))
	((MEDIA+=${TEMPOS[i]}))
done

MEDIA=$(($MEDIA/${V}))

for (( i=0; i<V; i++ )); do
	DESVIOS+=($((${TEMPOS[i]}-$MEDIA)))
done

echo "TEMPOS:" >> resultado.txt
echo "${TEMPOS[@]}" >> resultado.txt
echo "DESVIOS:" >> resultado.txt
echo "${DESVIOS[@]}" >> resultado.txt
echo "MEDIA: $MEDIA" >> resultado.txt	

