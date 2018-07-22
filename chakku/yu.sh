echo $1
#OCAMLRUNPARAM='l=4G' ../kuwa/1/decode_trace  ./../../ICFPC/defaultTraceF/FA$1.nbt > input/FA/FA$1.txt
echo $HOME/Programing/CompetitivePrograming/ICFPC/problemF/FA${1}_tgt.mdl
./src/a.out ./input/FA/FA${1}.txt $HOME/Programing/CompetitivePrograming/ICFPC/problemF/FA${1}_tgt.mdl > output/FA/FA${1}.txt
OCAMLRUNPARAM='l=4G' $HOME/Programing/CompetitivePrograming/ICFP2018/kuwa/1/encode_trace $HOME/Programing/CompetitivePrograming/ICFP2018/chakku/output/FA/FA${1}.txt > submit/FA/FA${1}.nbt
