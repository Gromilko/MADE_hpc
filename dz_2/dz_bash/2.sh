# !/ bin / bash
for i in {1..20} 
do
    my_array+=("object_$i")
done

for i in ${my_array[@]}
do
	echo $i
done