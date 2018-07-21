echo $1
#OCAMLRUNPARAM='l=4G' ../kuwa/1/decode_trace ../trace/LA$1.nbt > input/LA$1.txt
./src/a.out ./input/LA$1.txt ../models/LA$1_tgt.mdl > output/LA$1.txt
OCAMLRUNPARAM='l=4G' ../kuwa/1/encode_trace output/LA$1.txt > submit/LA$1.nbt
