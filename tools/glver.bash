


grep " gl" * | grep -v "glm" |grep -v "//" | grep -v "=" |grep -v "Binary" | cut -d":" -f 2 | cut -d"(" -f 1 | awk '{print $1}' | sort | uniq >kk

for f in $(cat kk)
do


av=$(curl -s  https://www.opengl.org/sdk/docs/man/html/$f.xhtml | grep ">-</td>" | wc -l)

echo $av $f
 



done







rm kk
