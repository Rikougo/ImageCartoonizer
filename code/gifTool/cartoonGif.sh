splitImgFiles=".GifImg"
cartoonedFiles=".cartoonedGifImg"

cartooningCommand="./singleImgCartoon"

#create folders
mkdir $splitImgFiles
mkdir $cartoonedFiles

#Get git delay
str=$(identify -verbose $1 | grep Delay)
if [[ $str =~ (Delay: ([^ ]*)) ]]; then
  delay=${BASH_REMATCH[2]}
else
  echo "no match found"
fi

#split gif
convert -coalesce $1 $splitImgFiles/%d.png

#cartoonize frames
FILES=$splitImgFiles"/*"
let i=0
let pids
for f in $FILES
do
    $cartooningCommand $f $cartoonedFiles${f#$splitImgFiles} &
    pids[${i}]=$!
    let i=i+1
done

#wait cartoonisation
for pid in ${pids[*]}; do
    wait $pid
done

#echo $delay
cd $cartoonedFiles
convert -delay $delay -loop 0 `ls -v` ../$2
cd ..

rm -rf $splitImgFiles
rm -rf $cartoonedFiles