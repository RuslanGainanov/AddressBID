REM 1. Распаковать deploy\innosetup.zip в каталог deploy
REM 1.*. Заменить (если есть) deploy\base1.db на реальную базу
REM 2. Запустить deploy.cmd из консоли Qt (текущий каталог - deploy)
REM 3. Запустить setup.exe для установки приложения
REM 4. Пользоваться

cd ..
qmake.exe AddressBID.pro
mingw32-make.exe
mingw32-make.exe clean
cd release
copy ..\README.txt README.txt
windeployqt --release --no-quick-import --no-translations --no-system-d3d-compiler --no-webkit2 --no-angle AddressBID.exe
copy ..\deploy\AddressBID_setup_v2.iss AddressBID_setup_v2.iss
copy ..\deploy\base1.db base1.db
..\deploy\innosetup\ISCC.exe AddressBID_setup_v2.iss
copy Output\setup.exe ..\setup.exe
cd ..
rmdir /s /q release
rmdir /s /q debug