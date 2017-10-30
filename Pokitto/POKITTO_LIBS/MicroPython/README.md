# Compile to bytecode and freeze Python files

The tools and source files for compiling and freezing are copied from the MicroPython project

# Do only if the MicroPython repository is updated

Copy headers (*.h) from "MicroPython/py" folder to the "py"
Copy makeqstrdata.py to the "tools"
Copy mpy-tool.py to the "tools"
Copy mpy-cross.exe to the "tools"
Copy mpconfigport.h to this folder

Build  mpy-cross.exe for Windows like this:
- cd Micropython/mpy-cross
- copy mpconfigport.h from micropython/port/pokittosim-win
- make CROSS_COMPILE=i686-w64-mingw32-

Copy mpy-cross.exe to the "tools"

# The Micropython Pokitto project should automatically copy the file below

$ cp libmicropython.a ../../../pokittodev/Projects/MicroPython/
$ cp build/genhdr/qstrdefs.generated.h ../../../pokittodev/Projects/MicroPython/genhdr/
$ cp build/genhdr/qstrdefs.preprocessed.h ../../../pokittodev/Projects/MicroPython/genhdr/

# Compile and freeze

- Put py-files to src-py
- Normally this is added to the EmBitz-project, pre build steps
-- run compile_and_freeze_py_files.bat





