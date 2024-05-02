# Create plant-uml diagrams

## Goal
The script `create_UML.sh` creates plant-uml code for all `.h` files in the src folder. These files can be viewed as UML-diagrams to get a better overview of the code.

## Prerequisites

### !!! Warning !!!
Do not put any files into the `UML/plant_UML` folder as this folder will be cleared every time the script is executed!

### To run the script
1. Install the [hpp2plantuml](https://github.com/thibaultmarin/hpp2plantuml) tool according to the instructions in the README.
2. Allow execution of the shell script by running: `chmod +x ./create_UML.sh` in the UML-folder
3. Run the script with `./create_UML.sh`

### To display the plant-uml diagrams
Install the [PlantUML Integration](https://plugins.jetbrains.com/plugin/7017-plantuml-integration) plugin for CLion.

## Function Description
Running the script removes the contents of the `UML/plant_UML` folder and copies the folder structure from `src` into this directory. In place of any `header_file.h` in the `src` folder and its subdirectories, a corresponding `header_file.puml` file will be generated, all other files will be ignored.
Additionally, one file `codebase.puml` will be generated which represents all `.h` files in `src`.
The quality of the plant-uml diagrams being created is limited by the functionality the hpp2plantuml tool provides.

I hope this helps with understanding the codebase!