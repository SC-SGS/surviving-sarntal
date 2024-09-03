#This script creates plant-UML code for all .h files in the src folder. See README.md for further information.

#remove current files
rm -r ./plant_UML/*

#clone directory structure
cd ../src &&
find . -type d -exec mkdir -p -- ../UML/plant_UML/{} \;
cd ..

#make uml files
all_header_files="hpp2plantuml";
terrain_header_files="hpp2plantuml";
for header_file in $(find . -regex "./src/.*\.h\(pp\)?";);
do
  hpp2plantuml -i ${header_file} -o UML/plant_UML/${header_file:6}.puml
  all_header_files="${all_header_files} -i ${header_file}"
  file_name=$(basename "${header_file}")
  if find "./src/terrain" -type f -name "${file_name}" | grep -q "${header_file}"; then
    terrain_header_files="${terrain_header_files} -i ${header_file}"
  fi
done
all_header_files="${all_header_files} -o UML/plant_UML/codebase.puml"
eval ${all_header_files}
terrain_header_files="${terrain_header_files} -o UML/plant_UML/terrain/FullTerrain.puml"
eval ${terrain_header_files}

echo "plant_UML files created successfully!"

