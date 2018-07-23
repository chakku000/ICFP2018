for i in `seq -f "%03g" 1 115`;
do
    echo $i
    bash ./re_part.sh $HOME/Programing/CompetitivePrograming/ICFPC/defaultTraceF/FR${i}.nbt $HOME/Programing/CompetitivePrograming/ICFP2018/chakku/submit/FR/FR${i}.nbt
done
