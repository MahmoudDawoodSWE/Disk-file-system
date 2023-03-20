#include <iostream>
#include <vector>
//#include <map>
#include <assert.h>
#include <string.h>
//#include <math.h>
//#include <sys/types.h>
#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
using namespace std;

#define DISK_SIZE 256
// ============================================================================
void decToBinary(int n, char &c){
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0){
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--){
        if (binaryNum[j] == 1)
            c = c | 1u << j;
    }
}



// ============================================================================

class FsFile {
    int file_size;
    int block_in_use;
    int index_block;
    int block_size;

public:
    FsFile(int _block_size) {
        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;
    }

    int getfile_size() {
        return file_size;
    }
    //getters
    int getFile_block_in_use() {
        return block_in_use;
    }
    int getFile_index_block() {
        return index_block;
    }
    int getFile_block_size() {
        return block_size;
    }
    //setters
    void setFile_add_size(int size) {
        this -> file_size += size;
    }
    void setFile_block_add_1() {
        this -> block_in_use++;
    }
    void setFile_index_block(int indexBlock) {
        this -> index_block = indexBlock;
    }
};

// ============================================================================


class FileDescriptor {
    string file_name;
    FsFile* fs_file;
    bool inUse;

public:

    FileDescriptor(string FileName, FsFile* fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;
    }

    string getFileName() {
        return file_name;
    }
    FsFile* getFile_FsFile() {
        return fs_file;
    }
    bool getFile_inUse() const {
        return inUse;
    }
    //setters
    void setFile_inUse(bool in_use) {
        this -> inUse = in_use;
    }

};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"

// ============================================================================
class fsDisk {
    FILE *sim_disk_fd;
    bool is_formated;

    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied.
    int BitVectorSize;
    int *BitVector;

    vector <FileDescriptor>  MainDir;// Structure that links the file name to its FsFile

    vector <FileDescriptor> OpenFileDescriptors;//  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.

    // ------------------------------------------------------------------------
    int sizeOfTheBlock = 4;
    int freeBlockNum = 0;
    // ------------------------------------------------------------------------
public:
    fsDisk() {
        sim_disk_fd = fopen(DISK_SIM_FILE , "w+");
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
        is_formated = false;
    }
    // ------------------------------------------------------------------------

    ~fsDisk() {
        for (auto & i : MainDir) {
            delete (i.getFile_FsFile());
        }

        delete[] (BitVector);
        fclose(sim_disk_fd);
    }

    // ------------------------------------------------------------------------
    void listAll() {
        int i = 0;

        for (i = 0; i < MainDir.size();i++) {
            cout << "index: " << i << ": FileName: " << MainDir[i].getFileName() <<  " , isInUse: " << MainDir[i].getFile_inUse()<< endl;
        }

        char bufy;
        cout << "Disk content: '";
        for (i = 0; i < DISK_SIZE; i++)
        {
            cout << "(";
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fread(&bufy, 1, 1, sim_disk_fd);
            cout << bufy;
            cout << ")";
        }
        cout << "'" << endl;
    }
    // ------------------------------------------------------------------------
    void fsFormat( int blockSize = 4 ) {
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return;
        }
        this -> BitVectorSize = DISK_SIZE / blockSize;
        cout << "number of block  ";
        cout << BitVectorSize << "\n";
        this -> sizeOfTheBlock = blockSize;
        this -> freeBlockNum = DISK_SIZE / blockSize;
        if (!this -> is_formated) {
            this -> BitVector = new int[BitVectorSize];
            for (int i = 0; i < this->BitVectorSize; i++) {
                this -> BitVector[i] = 0;
            }
        }else{
            for (int i = 0; i < DISK_SIZE; i++) {
                int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
                ret_val = fwrite("\0", 1, 1, sim_disk_fd);
                assert(ret_val == 1);
            }

            for (auto & i : MainDir) {
                delete (i.getFile_FsFile());
            }

            for (int i = 0; i < this -> BitVectorSize; i++) {
                this->BitVector[i] = 0;
            }
            this -> OpenFileDescriptors.clear();
            this -> MainDir.clear();
        }
        this -> is_formated = true;

    }

    // ------------------------------------------------------------------------
    int CreateFile(string fileName) {
        if(!this -> is_formated){// the disk not initialized yet
            cout << " disk not initialized yet \n";
            return -1;
        }
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return -1;
        }
        for (int i = 0; i < MainDir.size(); i++){//found file with the same name
            if (fileName == MainDir[i].getFileName())
            {
                cout << " already created \n";
                return -1;
            }
        }
        FsFile* temp_file = new FsFile(this -> sizeOfTheBlock);
        FileDescriptor* temp_descriptor = new FileDescriptor(fileName, temp_file);
        //insert the file in MainDir and OpenFileDescriptors
        this -> MainDir.push_back(*temp_descriptor);
        for (int j = 0; j < OpenFileDescriptors.size(); j++) {//if we found empty place in the OpenFileDescriptors
            if (!OpenFileDescriptors[j].getFile_inUse()) {
                OpenFileDescriptors[j] = *temp_descriptor;
                delete temp_descriptor;
                return j;
            }
        }
        this -> OpenFileDescriptors.push_back(*temp_descriptor);// new place to the temp_descriptor
        delete temp_descriptor;
        return this->OpenFileDescriptors.size() - 1;
    }
    // ------------------------------------------------------------------------
    int OpenFile(string fileName) {
        if(!this -> is_formated){// the disk not initialized yet
            cout << " disk not initialized yet \n";
            return -1;
        }
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return -1;
        }
        for (int i = 0; i < MainDir.size(); i++){// not opened or not exist
            if (fileName == MainDir[i].getFileName()){
                if (MainDir[i].getFile_inUse()){// already opened
                    cout << " already opened \n ";
                    return -1;
                }
                MainDir[i].setFile_inUse(true);
                for (int j = 0; j < OpenFileDescriptors.size(); j++) {// find empty place in the OpenFileDescriptors
                    if (!OpenFileDescriptors[j].getFile_inUse()) {
                        OpenFileDescriptors[j] = MainDir[i];
                        return j;
                    }
                }
                OpenFileDescriptors.push_back(MainDir[i]);
                return OpenFileDescriptors.size() - 1;
            }
        }
        //not exist
        cout<< "not exist \n";
        return -1;
    }

    // ------------------------------------------------------------------------
    string CloseFile(int fd) {
        if(!this -> is_formated){// the disk not initialized yet
            cout << " disk not initialized yet \n";
            return "-1";
        }
        if (OpenFileDescriptors.size() <= fd || fd < 0){
            cout << " fd out of the limit \n";
            return "-1";
        }
        if (!OpenFileDescriptors[fd].getFile_inUse()){
            cout << " already closed \n";
            return "-1";
        }
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return "-1";
        }
        string temp = OpenFileDescriptors[fd].getFileName();
        //update inUse
        for (int i = 0; i < MainDir.size(); i++){
            if(temp == MainDir[i].getFileName()){
                MainDir[i].setFile_inUse(false);
            }
        }
        //this mean empty place in the OpenFileDescriptors
        OpenFileDescriptors[fd].setFile_inUse(false);

        return temp;
    }
    // ------------------------------------------------------------------------
    int WriteToFile(int fd, char *buf, int len ) {
        if(!this -> is_formated){// the disk not initialized yet
            cout << " disk not initialized yet \n";
            return -1;
        }
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return -1;
        }
        if (fd >= OpenFileDescriptors.size() || fd < 0){
            cout<< " out of the limit \n";
            return -1;
        }
        if (!OpenFileDescriptors[fd].getFile_inUse()){
            cout << " closed file -> empty place in OpenFileDescriptors \n";
            return -1;
        }
        FsFile* temp =  OpenFileDescriptors[fd].getFile_FsFile();
        char* index_block_buff = new char[temp -> getFile_block_size()];//content
        int lastBlockContentSize = temp -> getfile_size() % temp -> getFile_block_size();
        int freeSpaceLastBlock =  temp -> getFile_block_size() - lastBlockContentSize;
        int taken_char = 0;
        if(freeSpaceLastBlock == temp -> getFile_block_size())//actually we need  block
            freeSpaceLastBlock = 0;
        int fraction_size = (len - freeSpaceLastBlock) % temp -> getFile_block_size();
        int blockNeeded = (len - freeSpaceLastBlock) / temp -> getFile_block_size();
        if(fraction_size != 0)//block for the fraction
            blockNeeded++;

        if (blockNeeded > temp -> getFile_block_size() - temp -> getFile_block_in_use()){//no enough place in the file
            cout<< " no place in the file \n";
            delete[] index_block_buff;
            return -1;
        }
        if (blockNeeded > this -> freeBlockNum) {//no enough place in the disk
            cout<< " no place in the disk \n";
            delete[] index_block_buff;
            return -1;
        }
        if (temp -> getFile_index_block() == -1){
            if (blockNeeded + 1 > this -> freeBlockNum) {
                cout << " the disk is full \n";
                delete[] index_block_buff;
                return -1;
            }
            for (int i = 0; i < BitVectorSize; i++) {//find place to index block
                if (BitVector[i] == 0 ) {
                    temp -> setFile_index_block(i);
                    BitVector[i] = 1;
                    freeBlockNum--;
                    break;
                }
            }
        }

        //read the index block
        fseek(sim_disk_fd, temp -> getFile_index_block() * temp -> getFile_block_size(), SEEK_SET);
        fread(index_block_buff,  temp -> getFile_block_size(), 1, sim_disk_fd);
        if (freeSpaceLastBlock != 0){
            unsigned char last_block_char = index_block_buff[temp -> getFile_block_in_use() - 1];
            int last_block = (int) last_block_char ;
            last_block -= 48;
            if (len <= freeSpaceLastBlock){//if the len fit in the freeSpaceLastBlock
                blockNeeded = 0;
                fraction_size = 0;
                fseek(sim_disk_fd, (last_block * temp -> getFile_block_size()) + lastBlockContentSize, SEEK_SET);
                fwrite(buf, len, 1, sim_disk_fd);
                temp -> setFile_add_size(len);
                delete[] index_block_buff;
                return 1;
            }else {//fill the freeSpaceLastBlock
                fseek(sim_disk_fd, (last_block * temp -> getFile_block_size()) + lastBlockContentSize, SEEK_SET);
                fwrite(buf, freeSpaceLastBlock, 1, sim_disk_fd);
                taken_char += freeSpaceLastBlock;
            }
        }
        for (int j = 0; j < blockNeeded; j++) {
            int i;//empty place
            for (i = 0; i < BitVectorSize; i++) {
                if (BitVector[i] == 0) {
                    BitVector[i] = 1;
                    break;
                }
            }

            //print data in the new block
            fseek(sim_disk_fd, i * temp -> getFile_block_size(), SEEK_SET);
            if(j == blockNeeded - 1 && fraction_size != 0){
                fwrite(&buf[taken_char], fraction_size, 1, sim_disk_fd);
            }else{
                fwrite(&buf[taken_char], temp -> getFile_block_size(), 1, sim_disk_fd);
            }
            taken_char += temp -> getFile_block_size();
            //update index_block

            decToBinary(i + 48, index_block_buff[temp->getFile_block_in_use()]);
            fseek(sim_disk_fd, temp -> getFile_index_block() * temp -> getFile_block_size(), SEEK_SET);
            fwrite(index_block_buff, temp->getFile_block_size(), 1, sim_disk_fd);
            this ->freeBlockNum--;
            temp -> setFile_block_add_1();
        }
        delete[] index_block_buff;
        temp -> setFile_add_size(len);
        return 1;
    }
    // ------------------------------------------------------------------------
    int DelFile( string FileName ) {
        if(!this -> is_formated){// the disk not initialized yet
            cout << " disk not initialized yet \n";
            return -1;
        }
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return -1;
        }
        for (int j = 0; j < MainDir.size(); j++){// close
            if (FileName == MainDir[j].getFileName()){
                if(MainDir[j].getFile_inUse()){
                    cout<<"cannot delete open file \n";
                    return -1;
                }
                FsFile *temp = MainDir[j].getFile_FsFile();
                char* indexBlock_Buffer = new char[temp -> getFile_block_size()];
                fseek(sim_disk_fd, temp -> getFile_index_block() * temp->getFile_block_size(), SEEK_SET);
                fread(indexBlock_Buffer, temp -> getFile_block_size(), 1, sim_disk_fd);
                int delete_block_num ;
                char* emptyPlaceValue = new char[temp->getFile_block_size()];
                for (int i = 0; i < temp -> getFile_block_size(); i++){
                    emptyPlaceValue[i] ='\0';
                }
                for (int i = 0; i < temp -> getFile_block_in_use(); i++){
                    //delete the blocks
                    unsigned char delete_block_num_char = indexBlock_Buffer[i];
                    delete_block_num = (int) delete_block_num_char ;
                    delete_block_num -= 48;
                    BitVector[delete_block_num] = 0;
                    fseek(sim_disk_fd, delete_block_num * temp -> getFile_block_size(), SEEK_SET);
                    fwrite(emptyPlaceValue, temp -> getFile_block_size(), 1, sim_disk_fd);
                }
                //delete the index block
                BitVector[temp -> getFile_index_block()] = 0;
                fseek(sim_disk_fd, temp -> getFile_index_block() * temp->getFile_block_size(), SEEK_SET);
                fwrite(emptyPlaceValue, temp -> getFile_block_size(), 1, sim_disk_fd);
                if (MainDir[j].getFile_inUse()){
                    for (int o = 0; o < this -> OpenFileDescriptors.size(); o++){//close
                        if (FileName == this -> OpenFileDescriptors[o].getFileName()){
                            OpenFileDescriptors[o].setFile_inUse(false);
                        }
                    }
                }
                this -> freeBlockNum += MainDir[j].getFile_FsFile() -> getFile_block_in_use() + 1;
                delete this -> MainDir[j].getFile_FsFile();
                this -> MainDir.erase(this->MainDir.begin() + j);
                delete[](indexBlock_Buffer);
                delete[](emptyPlaceValue);

                break;
            }
            else if (j == MainDir.size() - 1){
                cout<<" file name not found in the MainDir \n";
                return -1;}
        }
        return 1;
    }
    // ------------------------------------------------------------------------
    int ReadFromFile(int fd, char* buf, int len ) {
        if(!this -> is_formated){// the disk not initialized yet
            cout << " disk not initialized yet \n";

            return -1;
        }
        if (fd >= OpenFileDescriptors.size() || fd < 0){
            cout<< " out of the limit \n";
            return -1;
        }
        if (!OpenFileDescriptors[fd].getFile_inUse()){
            cout << " closed file -> empty place in OpenFileDescriptors \n";
            return -1;
        }
        FsFile *temp = OpenFileDescriptors[fd].getFile_FsFile();
        if (len > temp -> getfile_size()){
            cout<< " too long request \n";
            return -1;
        }
        if (this -> sim_disk_fd == nullptr) {
            cout << " sim_disk_fd is null \n";
            return -1;
        }
        char* buffer_index_block = new char[temp -> getFile_block_size()];
        int blockNeeded = len / temp->getFile_block_size();
        if(len % temp->getFile_block_size() != 0)
            blockNeeded++;
        fseek(sim_disk_fd, temp ->getFile_index_block() * temp -> getFile_block_size(), SEEK_SET);
        fread(buffer_index_block, temp -> getFile_block_size(), 1, sim_disk_fd);
        for(int i = 0;i < blockNeeded;i++) {
            unsigned char char_block_num= buffer_index_block[i];
            int block_to_read = (int) char_block_num ;
            block_to_read -= 48;
            fseek(sim_disk_fd,  block_to_read * temp -> getFile_block_size(), SEEK_SET);
            fread(&buf[i * temp -> getFile_block_size()], temp -> getFile_block_size(), 1, sim_disk_fd);
        }
        buf[len] = '\0';
        delete[](buffer_index_block);
        return 1;


    }


};

int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
                delete fs;
                exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            default:
                break;
        }
    }

}
