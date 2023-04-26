from shutil import copyfile
import sys
import os


class ProductionCreator(object):

    _PRODUCTION_FOLDER = "Output\\Production"
    _LIBS = "Libs"
    _INCLUDE = "Include"

    _LIB_OUTPUT = "Output\\NTFSLib\\Binaries"
    _LIB_NAME = "NTFSLib.lib"

    _CODE_FILES = "NTFSLib"
    _HEADER_SUFFIX = ".h"

    def __init__(self, solution_directory):
        self._solution_directory = solution_directory
        self._production_directory = self._in_solution(self._PRODUCTION_FOLDER)

    def _in_solution(self, rest_of_path):
        return os.path.join(self._solution_directory, rest_of_path)

    def _in_production(self, rest_of_path):
        return os.path.join(self._production_directory, rest_of_path)

    def _create_fodler(self, folder_path):
        if not os.path.exists(folder_path):
            os.makedirs(folder_path)

    def create_lib_folders(self, machine, version):
        temp_dir = self._in_production(os.path.join(self._LIBS, machine, version))
        self._create_fodler(temp_dir)
        copyfile(self._in_solution(os.path.join(self._LIB_OUTPUT, machine, version, self._LIB_NAME)), os.path.join(temp_dir, self._LIB_NAME))

    def create_include_folders(self):
        self._create_fodler(self._in_production(self._INCLUDE))
        code_folder = self._in_solution(self._CODE_FILES)
        folders = os.walk(code_folder)
        for folder in folders:
            my_code_folder = self._in_production(os.path.join(self._INCLUDE, folder[0][len(code_folder) + 1:]))
            self._create_fodler(my_code_folder)
            for file in folder[2]:
                if file.endswith(self._HEADER_SUFFIX):
                    copyfile(os.path.join(folder[0], file), os.path.join(my_code_folder, file))



def main(solution_directory, machine, version):
    creator = ProductionCreator(solution_directory)
    creator.create_lib_folders(machine, version)
    creator.create_include_folders()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {0} <NTFSLib Solution Directory\Machine\Version>".format(sys.argv[0]))
        print("e.g. script.py C:\\MySolution\\Win32\\Debug")
    else:
        # That's ugly.
        parts = sys.argv[1].split("\\")
        parts[0] += "\\"
        main(os.path.join(*parts[:-2]), parts[-2], parts[-1])