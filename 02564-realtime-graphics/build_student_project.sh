#!/bin/bash
echo "----- building 02564 for students ------"
echo "Remove old student folder"
rm -rf ../02564_stud
echo "Create new student folder ../02564_stud"
mkdir ../02564_stud
echo "Copy everything"
cp -r . ../02564_stud
echo "Remove svn"
rm -rf `find ../02564_stud -type d -name .svn`
echo "Remove shell scripts"
rm ../02564_stud/*.sh
echo "Remove qtc_packaging"
rm -rf ../02564_stud/qtc_packaging
echo "Remove pro files"
rm ../02564_stud/*.pro.user
echo "Custom pro.user files for 2014"
mv ../02564_stud/TerrainScene.pro.user.2014 ../02564_stud/TerrainScene.pro.user
mv ../02564_stud/PoolScene.pro.user.2014 ../02564_stud/PoolScene.pro.user 
echo "Remove insert code blocks in source code"
find . -name *.h -type f -exec sh remove_student_code.sh {} ';'
find . -name *.cpp -type f -exec sh remove_student_code.sh {} ';'
find . -name *.c -type f -exec sh remove_student_code.sh {} ';'
find . -name *.frag -type f -exec sh remove_student_code.sh {} ';'
find . -name *.vert -type f -exec sh remove_student_code.sh {} ';'
find . -name *.geom -type f -exec sh remove_student_code.sh {} ';'
rm -rf ../02564_stud/sensors_stuff

# Copy and delete files into sub projects ( http://superuser.com/a/529861 )
# TerrainScene
rm -rf ../02564_stud_terrain
cp -r ../02564_stud ../02564_stud_terrain
cd ../02564_stud_terrain
shopt -s extglob
rm !(TerrainScene.pro|TerrainScene.qrc|TerrainScene.pro.user)
cd exercises
rm -rf !(TerrainScene)
cd ../shaders
rm -rf !(TerrainScene)
cd ..

# PoolScene
rm -rf ../02564_stud_pool
cp -r ../02564_stud ../02564_stud_pool
cd ../02564_stud_pool
rm !(PoolScene.pro|PoolScene.qrc|PoolScene.pro.user)
cd exercises
rm -rf !(PoolScene)
cd ../shaders
rm -rf !(PoolScene)
cd ..
