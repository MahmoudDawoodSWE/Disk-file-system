
Disk File System
Authored by :Mahmoud Dawood

=====================================Description===================================== 
disk simulation by  using indexed allocation method
 
explin how fils organized on the hard disk     

simulation "mechanism" of  load, write, read, delete, close, open, format command  
 
class----------------------------------- 
Fsfile save file detail  
FileDescriptor save FsFile and file name 
FsDisk the disk 

functions-------------------------------
CreateFile 
parametr - file name
  
do - create and open file 

return - Success "file descriptor" failure"-1"

--------------------
WriteToFile

parametr - 1-file discrptor 2-chars to write 3-length   

do - write to the file 

return - Success "1" failure"-1"
--------------------
ReadFromFile 
parametr - 1-file discrptor 2-buffer 3-length
   
do - read from the file to the buffer

return - Success "1" failure"-1"

--------------------
OpenFile
parametr -  file name

do - open file

return - Success "file descriptor" failure"-1"

--------------------
CloseFile
parametr - file descriptor 

do - closes the file

return - Success "name file" failure"-1"
-------------------- 
fsFormat
parametr - block size 

do - formats the disk

void 
--------------------
listAll 

do - print the disk

void
--------------------
DelFile
parametr - file name 

do - delete the file 
 
return - Success "1" failure"-1"
--------------------
decToBinary

parametr - 1-integer 2-char

do - convert the decimal val to binary and print into the char 

==================================== compile/Run ====================================
 g++ ex7.cpp -o ex7
 ./ex7
=====================================   Input   =====================================  
  options from  0 to 8   each number represents function  
====================================   Output   =====================================  
the results/failure reason
