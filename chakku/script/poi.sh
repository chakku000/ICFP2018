for i in `seq -f "%03g" 1 186`;
do
    echo $i
    bash ./yu.sh $i
done
