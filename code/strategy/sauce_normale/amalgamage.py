import sys
import os
import re
import time

# AMALGAMAGE only with "root" modules (no submodules)
# use "/" in the path as args
# MAKES Sure that all lua files don't have a "\n" at the end
#& C:/Users/Jonathan/anaconda3/envs/robot_sim/python.exe d:/Sync/Code/Robotique/CDR2024/robotSim/lua_scripts/amalgamage.py D:/Sync/Code/Robotique/CDR2024/robotSim/lua_scripts/main2.lua

def err_abort(error_msg):
    print(error_msg)
    input("press anything to close...")
    return sys.exit(1)

def get_module_raw(original_file_path, filename) -> str:
    somewhat_root_path = original_file_path.split("/")[:-1]
    file_path = "/".join(somewhat_root_path) + "/" + filename + ".lua"
    if not os.path.exists(file_path):
        err_abort('Module not found: ' + file_path)
    list_content = []
    with open(file_path, 'r') as f:
        file_content = f.read()
        for i, x in enumerate(reversed(file_content)): #remove the return at the end
            if x == '\n':
                list_content = (file_content[:-i] + '\n').splitlines()
                break
        if len(list_content) < 3:
            print(f" !! Warning !! may be missing return MODULENAME in {file_path}")
        list_content = [x for x in list_content if not re.search(r'require\(\"', x)] #remove line of require
        
        
        return '\n'.join(list_content) + '\n'


if __name__ == '__main__':
    #opening main file
    try:
        file_to_amalgame = sys.argv[1]
    except IndexError:
        err_abort("no arguments given ! need FULL path of lua file with '/' ")
    if not os.path.exists(file_to_amalgame):
        err_abort('File not found - put the correct path with "/" as arg of the command and FULL path')
    file_content = ''
    with open(file_to_amalgame, 'r') as f:
        file_content = f.read()

    #regex matching
    matchs = re.split(r"(require[(]\"|')(.+)(\"|'[)])", file_content) # Improve it ? 

    list_main_file = file_content.splitlines()
    main_content = '\n'.join([x for x in list_main_file if not re.search(r'require\(\"', x)]) #remove line of require

    #getting modules names (using set to remove duplicates)
    file_names_to_amalgame = [matchs[i-1] for i in range(len(matchs)) if (i+1) % 4 == 0] # index 3, index 7, index 11, ...
    
    modules = []

    #getting modules
    for file_name in file_names_to_amalgame:
        modules.append(get_module_raw(file_to_amalgame, file_name))
    # make the amalgamation
        file_path = file_to_amalgame[:-4] + "_amalg.lua"
    with open(file_path, 'w') as f:
        for module in modules:
            f.write(module)
        f.write(main_content)
    print('Amalgamation done !' + '\n' + 'File: ' + file_path)

#TODO : add a \n at the end, and remove the returns


