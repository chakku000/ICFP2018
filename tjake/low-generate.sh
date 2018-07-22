mkdir -p ./low-trace-text
mkdir -p ./low-nbt

for i in `seq -f "%03g" 1 186`;
do
  echo "generate LA${i}.nbt"
  python3 multi-low-line.py ../models/LA${i}_tgt.mdl > ./low-trace-text/LA${i}_trace.txt && OCAMLRUNPARAM='l=4G' ../kuwa/1/encode_trace ./low-trace-text/LA${i}_trace.txt > ./low-nbt/LA${i}.nbt
done

