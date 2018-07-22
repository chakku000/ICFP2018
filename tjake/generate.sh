mkdir -p ./trace-text
mkdir -p ./nbt

for i in `seq -f "%03g" 1 186`;
do
  echo "generate LA${i}.nbt"
  python3 multi-high-line.py ../models/LA${i}_tgt.mdl > ./trace-text/LA${i}_trace.txt
  OCAMLRUNPARAM='l=4G' ../kuwa/1/encode_trace ./trace-text/LA${i}_trace.txt > ./nbt/LA${i}.nbt
done

