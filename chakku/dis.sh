for i in `seq -f "%03g" 1 186`;
do
    echo $i
    bash ./dis_part.sh $HOME/Programing/CompetitivePrograming/ICFPC/defaultTraceF/FD${i}.nbt $HOME/Programing/CompetitivePrograming/ICFP2018/chakku/submit/FD/FD${i}.nbt
done
