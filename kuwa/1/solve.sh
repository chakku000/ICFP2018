test -o

mkdir out || :
rm out/*

for i in `seq -f "%03g" 1 186`;
do
  echo "compressing LA${i}..."
  OCAMLRUNPARAM='l=4G' ./compress "../../trace/LA${i}.nbt" > "out/LA${i}.nbt"
done

cd out
zip submission.zip *

