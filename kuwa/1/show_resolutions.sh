test -o

for i in `seq -f "%03g" 1 186`;
do
  r=`OCAMLRUNPARAM='l=4G' ./model "../../models/LA${i}_tgt.mdl" | head -n 1`
  echo ${i} ${r}
done
