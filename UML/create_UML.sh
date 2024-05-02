#This script creates plant-UML code for all .h files in the src folder. See README.md for further information.

#remove current files
rm -r ./plant_UML/*

#clone directory structure
cd ../src &&
find . -type d -exec mkdir -p -- ../UML/plant_UML/{} \;
cd ..

#make uml files
all_header_files="hpp2plantuml";
for header_file in $(find . -regex "./src/.*.h";);
do
  hpp2plantuml -i ${header_file} -o UML/plant_UML/${header_file:6:-2}.puml
  all_header_files="${all_header_files} -i ${header_file}"
done
all_header_files="${all_header_files} -o UML/plant_UML/codebase.puml"
eval ${all_header_files}

echo "plant_UML files created successfully!"

