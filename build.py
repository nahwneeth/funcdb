cpp_ver         =   17

enable_logging  =   False
enable_debug    =   False

output_dir      =   ['out']
exe_name        =   'funcdb.exe'

source_dir      =   ['source']
source_files    =   [
                        ['b-plus-tree.cc'],
                        ['file-helper.cc'],
                        ['internal-node.cc'],
                        ['leaf-node.cc'],
                        ['main.cc'],
                        ['meta-command.cc'],
                        ['node.cc'],
                        ['row-info.cc'],
                        ['statement.cc'],
                        ['string-helper.cc'],
                        ['table.cc']
                    ]

#################################################################
#################################################################

import sys
import os

def run_with(args):
    command     = []
    script_dir  = os.path.dirname(os.path.abspath(__file__))

    abs_output_dir = os.path.join(script_dir, *output_dir)
    exe_path   = '"' + os.path.join(abs_output_dir, exe_name) + '"'
    
    abs_source_dir = os.path.join(script_dir, *source_dir)
    abs_source_files = [('"' + os.path.join(abs_source_dir, *cpp_file) + '"') for cpp_file in source_files]

    if not os.path.exists(abs_output_dir):
        os.makedirs(abs_output_dir)

    os.chdir(abs_output_dir)

    command = [
        'g++',
        '-std=c++' + str(cpp_ver),
        '-Wpedantic',
        '-Wall',
        '-Wextra',
        '-g' if enable_debug else '-O2',
        *abs_source_files,
        '-o',
        exe_path
    ]

    if enable_logging:
        command.append('-DLOGS')

    compile_command = ' '.join(command)
    print (compile_command)

    os.system(compile_command)
        

if __name__ == '__main__':
    args = sys.argv
    run_with(args[1:])