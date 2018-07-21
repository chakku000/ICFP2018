test -o

for i in `seq -f "%03g" 1 186`;
do
  OCAMLRUNPARAM='l=4G' ./model "../../models/LA${i}_tgt.mdl" > tmp_expected.txt
  OCAMLRUNPARAM='l=4G' ./interpret "../../models/LA${i}_tgt.mdl" "../../trace/LA${i}.nbt" dump > tmp_actual.txt
  echo -n "${i} "
  diff tmp_expected.txt tmp_actual.txt && echo "OK" || echo "NG"
done

rm tmp_expected.txt tmp_actual.txt

