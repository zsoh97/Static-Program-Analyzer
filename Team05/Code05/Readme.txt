1. This program should be run on a Windows computer, using CLion and built using cmake.

2. Open CLion. Click on File->Open.

3. Select the CMakeLists.txt file in the Team05\Code05 folder. Click Open as Project.

4. Once opened, click the hammer icon in the top right to build the project.

5. Once built, autotester.exe should appear in the Team05\Code05\cmake-build-debug\src\autotester directory.

6. autotester.exe takes in 3 arguments, <source file path> <query file path> <output file path>.

7. In the event that the autotester.exe does not appear, you can run it from CLion. Beside the hammer icon, click the dropdown box and click edit configurations. Click the autotester application and specify <source file path> <query file path> <output file path> in the program arguments. After that, run it using the play button.


NOTE:
The set of 8 system test queries in the Team05\Tests05\Basic_Queries directory are to be used on Basic_source.txt