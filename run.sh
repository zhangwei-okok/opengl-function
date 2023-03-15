#!/bin/bash




ps=$(cd "$(dirname "$0")";pwd)

glxinfo -B | grep -i Mesa > ${ps}/mesa.log
CASE="basicPoints blending depth_test logicop template feed_back vertex_texture single_vertex_texture_shader multi_vertex_texture_shader complex"
cd ./bin

for case in $CASE
do
  ./$case
#  rm $case
done

m=$(cat ${ps}/mesa.log| wc -l)
if(($m > 2));then
    for file in $(ls ./)
    do
	 if [ "${file##*.}" = "bmp" ]; then
         	name=${file%%.*}
                mv $file $name"_mesa"".bmp"  
        fi
  done
fi

