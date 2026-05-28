## 先按下面最小命令跑通

```
$env:WIRESHARK_BASE_DIR="E:\WorkSpace\QT-Github\wireshark-third-party"
$env:CMAKE_PREFIX_PATH="E:\Qt\Qt5.12\6.10.2\msvc2022_64"

cmake -G "Visual Studio 17 2022" -A x64 `
  -S E:\WorkSpace\QT-Github\wireshark-master `
  -B E:\WorkSpace\QT-Github\wsbuild64 `
  -DWIRESHARK_BASE_DIR=E:\WorkSpace\QT-Github\wireshark-third-party `
  -DCMAKE_PREFIX_PATH=E:\Qt\Qt5.12\6.10.2\msvc2022_64

cmake --build E:\WorkSpace\QT-Github\wsbuild64 --config RelWithDebInfo --parallel

解决报错
Developer PowerShell for VS 2022
Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\Lib" -Recurse -Filter bcrypt.lib |
  Where-Object { $_.FullName -like "*\um\x64\bcrypt.lib" } |
  Sort-Object FullName -Descending |
  Select-Object -First 10 FullName
  
删除旧 build 目录，重新 CMake
Remove-Item "E:\WorkSpace\QT-Github\wsbuild64" -Recurse -Force
New-Item -ItemType Directory -Force "E:\WorkSpace\QT-Github\wsbuild64"

手动指定 Bcrypt_LIBRARY
$bcrypt = Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\Lib" -Recurse -Filter bcrypt.lib |
  Where-Object { $_.FullName -like "*\um\x64\bcrypt.lib" } |
  Sort-Object FullName -Descending |
  Select-Object -First 1

$bcrypt.FullName

然后重新配置，重点是加上 -DBcrypt_LIBRARY:FILEPATH=...
$env:WIRESHARK_BASE_DIR="E:\WorkSpace\QT-Github\wireshark-third-party"
$env:CMAKE_PREFIX_PATH="E:\Qt\Qt5.12\6.10.2\msvc2022_64"

$bcrypt = Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\Lib" -Recurse -Filter bcrypt.lib |
  Where-Object { $_.FullName -like "*\um\x64\bcrypt.lib" } |
  Sort-Object FullName -Descending |
  Select-Object -First 1

$bcryptCmake = $bcrypt.FullName -replace "\\","/"

cmake `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -S E:\WorkSpace\QT-Github\wireshark-master `
  -B E:\WorkSpace\QT-Github\wsbuild64 `
  -DWIRESHARK_BASE_DIR=E:\WorkSpace\QT-Github\wireshark-third-party `
  -DCMAKE_PREFIX_PATH=E:\Qt\Qt5.12\6.10.2\msvc2022_64 `
  "-DBcrypt_LIBRARY:FILEPATH=$bcryptCmake"
  
  再编译：cmake --build E:\WorkSpace\QT-Github\wsbuild64 --config RelWithDebInfo --parallel

检查 CMakeCache 是否已经修复
Select-String -Path "E:\WorkSpace\QT-Github\wsbuild64\CMakeCache.txt" -Pattern "Bcrypt_LIBRARY"
```

