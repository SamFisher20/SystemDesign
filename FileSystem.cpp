/*
  A simulation of file system, where files can be created,deleted,copied,modified,version controled.
*/

#include<iostream>
#include<unordered_map>
#include<string>
using namespace std;

template<typename A> struct customVerDataType{    //Template class for versionControl stack data type, versionNo means the version number assosiated with each iteration of the file and content data member means the contents for each versions
    int versionNo;
    A content;
};

template<typename A> class versionControl{    //Template class for storing versions for a file, a stack is maintained for storing every iterations.  
    stack<customVerDataType<A>> versionStack;
    public:
        versionControl(){   
            while(!versionStack.empty()){
                versionStack.pop();
            }
        }

        void addNewVersion(A newContent){   //Function for adding new version into the stack
            customVerDataType<A> temp;
            temp.content=newContent;
            if(versionStack.empty()){
                temp.versionNo=0;
                versionStack.push(temp);
            }else{
                temp.versionNo=this->lastestVersion()+1;
                versionStack.push(temp);
            }
        }

        A revert(int versionNum){   //Function for reverting back to older version
            if(versionNum==0){
                A dummy;
                return dummy;
            }
            while(this->lastestVersion()!=versionNum){
                versionStack.pop();
            }
            return versionStack.top().second;
        }

        int lastestVersion(){
            return versionStack.top().versionNo;
        }
};

class file{   //Abstract file type class, where every new type of file can inherit it 
    public:
        virtual string getName()=0;
        virtual string getFileType()=0;
        virtual string revertToVer()=0;
};

class textFile: public file{    //Text file class which represents text file, a name,content and a versionControl type instance is assossiated to every text file
    string fileName;
    string content;
    versionControl<string>* strVersions;
    public:
        textFile(string Name){
            this->fileName=Name;
            strVersions=new versionControl();
        }

        string getName(){
            return this->fileName;
        }

        string getFileType(){
            return "txt";
        }

        void append(string text){   //Function for appending data
            this->content+=text;
            strVersions->addNewVersion(this->content);
        }

        void trunc(string text){   //Function for truncating data
            this->content=text;
            strVersions->addNewVersion(this->content);
        }

        string revertToVer(int verNo){    
            if(verNo>strVersions->lastestVersion() || verNo<0){
                cout<<"No such version exists for this file/n";
            }else{
                this->content=strVersions->revert(verNo);
                cout<<"Reverted to version number "<<verNo<<"/n";
            }
        }

        void showContent(){
            int size=this->content.size();
            for(int i=0;i<size;i++){
                cout<<this->content[i];
            }
        }

        void deleteThisFile(){    //Function to delete the current file instance
            delete this;
        }

        ~textFile(){
            cout<<this->getName<<" File Deleted";
        }
};

class directory{    //Class that represent a directory in the file system, Each directory can contain zero-many files as well as directories. N-arry data structure is used to represent the hierarchical structure the file system.
    string dirName;
    unordered_map<string,file*> allFiles;
    unordered_map<string,directory*> nestedDir;

    public:
        directory(string name){
            this->dirName=name;
        }
        
        bool doDirExists(string dirName){
            if(nestedDir.find(name)!=nestedDir.end()){
                return true;
            }
            return false;
        }
        
        bool doFileExists(string dirName){
            if(allFiles.find(name)!=allFiles.end()){
                return true;
            }
            return false;
        }

        string touch(file* newFile){    //    Functions to add new file to current directory
            if(doFileExists(newFile.getName()) || doDirExists(newFile.getName())){
                return "File/Dir with name "+newFile.getName()+" already exists";
            }
            allFiles[newFile.getName()]=newFile;
            return "Created new file";

        }

        string mkdir(file* newDir){   //    Functions to add new directory to current directory
            if(doFileExists(newFile.getName()) || doDirExists(newFile.getName())){
                return "File/Dir with name "+newDir.getName()+" already exists";
            }
            nestedDir[newDir.getName()]=newDir;
            return "Created new directory";
        }

        file* getFileFromName(string filName){
            return allFiles[filName];
        }
        
        directory* getDirFromName(string dirName){
            return nestedDir[dirName];
        }

        string getName(){
            return this->dirName;
        }
        

        void deleteCurrDir(){   //Fuction to delete the whole current directory
            for(auto File:allFiles){
                deleteFile(File.first);
            }

            for(auto dir:nestedDir){
                deleteNestDir(dir.first);
            }
            delete this;
        }

        void deleteFile(string name){   //Function to delete a file from current directory
            allFiles[name]->deleteThisFile();
            allFiles.erase(name);
        }

        void deleteNestDir(string name){    //Function to delete a file from current directory
            nestedDir[name]->deleteCurrDir();
            nestedDir.erase(name);
        }
        
        ~directory(){
            cout<<this->getName()<<" directory deleted";
        }
};

class fileOperations{   //Class with enum type variables
    public:
        static enum operation{
            CREATE,
            DELETE
        }
}

class fileSystem{
    static fileSystem* manager;
    directory* fileManger;
    fileSystem(){
        fileManger=new directory("ROOT");
    }

    public:
        static fileSystem* getInstance(){
            if(manager==null)
                manager=new fileSystem();
            return manager;
        }

        bool fileUtil(file* File,string &path,directory* fileManger,fileOperations type){   //Util fuction to check the path given, if it valid or not. If it is valid then, it will do desired operation based of "type" variable
            string childDir;
            int i;
            for(i=0;i<path.size();i++){
                if(path[i]=='/'){
                    break;
                }else if(doHaveValidCharac(path[i])){
                    currDir+=path[i];
                }else{
                    return false;
                }
            }
            if(fileManager->ifFolderExists(childDir)){
                if(i<path.size())
                    return relocatefileUtil(File,substr(i),fileManger->getDirFromName(childDir));
                else{
                    if(type==fileOperations.operation.CREATE)
                        fileManager->touch(File);
                    else if(type==fileOperations.operation.DELETE)
                        fileManager->deleteFile(File->getName());
                    return true;
                }
            }else{
                return false;
            }
        }

        string copyFile(file* File,string &path){
            directory* tempDir=fileManger;
            file* newFile=new textFile(File);
            if(relocatefileUtil(newFile,path,tempDir,fileOperations.operation.CREATE)){
                return File->getName()+" has been copied to path: "+path;
            }else{
                return "No such path exists";
            }
        }

        string moveFile(file* File,string &path){
            directory* tempDir=fileManger;
            file* newFile=new textFile(File);
            if(relocatefileUtil(newFile,path,tempDir,fileOperations.operation.CREATE)){
                File->delete();
                return File->getName()+" has been moved to path: "+path;
            }else{
                return "No such path exists";
            }
        }

        string createFile(file* File,string &path){
            directory* tempDir=fileManger;
            if(relocatefileUtil(File,path,tempDir,fileOperations.operation.CREATE)){
                return File->getName()+" has been created in path: "+path;
            }else{
                return "No such path exists";
            }
        }

        string deleteFile(file* File,string &path){
            directory* tempDir=fileManger;
            string fileName=File->getName();
            if(relocatefileUtil(File,path,tempDir,fileOperations.operation.DELETE)){
                return fileName+" has been deleted from path: "+path;
            }else{
                return "No such path exists";
            }
        }     
};
