splitImgFiles=".GifImg"
splitImgFilesNoT=".GifImgNoAlpha"
cartoonedFiles=".cartoonedGifImg"

cartooningCommand="./singleImgCartoon"

#create folders
mkdir $splitImgFiles
mkdir $cartoonedFiles
mkdir $splitImgFilesNoT

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
    ffmpeg -i $f -filter_complex "color=black,format=rgb24[c];[c][0]scale2ref[c][i];[c][i]overlay=format=auto:shortest=1,setsar=1" $splitImgFilesNoT/${f#$splitImgFiles}
    $cartooningCommand $splitImgFilesNoT/${f#$splitImgFiles} $cartoonedFiles${f#$splitImgFiles} &
    pids[${i}]=$!
    let i=i+1

    if [ $(expr $i % 32) == "0" ]; then
      for pid in ${pids[*]}; do
        wait $pid
      done
    fi
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
rm -rf $splitImgFilesNoT