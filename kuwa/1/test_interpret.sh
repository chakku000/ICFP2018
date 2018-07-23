#!/usr/bin/zsh

test -o

# FA

MDL_DIR='../../mdl'
NBT_DIR='../../nbt'

echo 'foo' ${MDL_DIR} ${NBT_DIR}

for i in `seq -f "%03g" 1 186`;
do
  mdl="${MDL_DIR}/FA${i}_tgt.mdl"
  nbt="${NBT_DIR}/FA${i}.nbt"
  OCAMLRUNPARAM='l=4G' ./model ${mdl} > tmp_expected.txt
  OCAMLRUNPARAM='l=4G' ./interpret ${mdl} ${nbt} empty dump > tmp_actual.txt
  diff tmp_expected.txt tmp_actual.txt > /dev/null && echo "FA${i} OK" || echo "FA${i} NG"
done

# FD

for i in `seq -f "%03g" 1 186`;
do
  mdl="${MDL_DIR}/FD${i}_src.mdl"
  nbt="${NBT_DIR}/FD${i}.nbt"
  OCAMLRUNPARAM='l=4G' ./model ${mdl} empty > tmp_expected.txt
  OCAMLRUNPARAM='l=4G' ./interpret ${mdl} ${nbt} dump > tmp_actual.txt
  diff tmp_expected.txt tmp_actual.txt > /dev/null && echo "FD${i} OK" || echo "FD${i} NG"
done

# FR

for i in `seq -f "%03g" 1 115`;
do
  mdl_src="${MDL_DIR}/FR${i}_src.mdl"
  mdl_tgt="${MDL_DIR}/FR${i}_tgt.mdl"
  nbt="${NBT_DIR}/FR${i}.nbt"
  OCAMLRUNPARAM='l=4G' ./model ${mdl_tgt} > tmp_expected.txt
  OCAMLRUNPARAM='l=4G' ./interpret ${mdl_src} ${nbt} dump > tmp_actual.txt
  diff tmp_expected.txt tmp_actual.txt > /dev/null && echo "FD${i} OK" || echo "FD${i} NG"
done
