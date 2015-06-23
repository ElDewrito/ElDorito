rmdir build /s /q
mkdir build
cd build
IF EXIST "C:\Program Files (x86)\CMake\bin\" (
  "C:\Program Files (x86)\CMake\bin\cmake" -G "Visual Studio 12" --build ..
) ELSE (
  IF EXIST "C:\Program Files\CMake\bin\" (
    "C:\Program Files\CMake\bin\cmake" -G "Visual Studio 12" --build ..
  ) ELSE (
    cmake -G "Visual Studio 12" --build ..
  )
)