//Kareem Abdelaty
//30075331

#include "getDirStats.h"
#include "digester.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <queue>
#include <algorithm>
#include <set> 
#include <cstdio>
//unordered map to keep track of the most common file types
std::unordered_map<std::string,int> common;
//flag that keeps track of if popen should be called on directory entries one by one
bool flag;
//function the tests if a path points to  a directory
static bool
is_dir( const std::string & path)
{
  struct stat buff;
  if( 0 != stat( path.c_str(), & buff))
    return false;
  return S_ISDIR(buff.st_mode);
}
//function that takes a path and returns the file type
static void getFiletype(const std::string &pathAndCommand,bool lookingAtDirectory){
    char buffer[4096];   
     FILE *str = popen(pathAndCommand.c_str(), "r");
     flag = false;
     while(fgets(buffer, 4096, str)) {
        std::string input = std::string(buffer);
        input.pop_back(); 
        std::string delimiter = ",";
        //if popen failed read dirctory open one by one
        if((input.find("cannot open")!=std::string::npos)&&lookingAtDirectory){
            flag  = true;
        }
        //if popen was succesful read file types not including directories
        if(flag == false){
        size_t i = input.find(delimiter);
        if(i !=std::string::npos){
            std::string type = input.substr(0, i); 
            if(type.find("directory") != std::string::npos){
                continue;
        }
            if(common.find(type) != common.end()){
                common[type] = common[type] +1;
            }else{
                common[type] = 1;
            }
        }else{
            if(input.find("directory") != std::string::npos){
                continue;
            }
            if(common.find(input) != common.end()){
                common[input] = common[input] +1;
            }else{
                    common[input] = 1;
                    }
                }
           
        }   
        }
        pclose(str);     
}
// ======================================================================
// You need to re-implement this function !!!!
// ======================================================================
//
// getDirStats() computes stats about directory dir_name
// if successful, it return true and stores the results in 'res'
// on failure, it returns false, and res is in undefined state
//
bool
getDirStats(const std::string & dir_name, Results & res)
{
  DIR *dir;
  struct dirent *entry;
  std::string currentPath;
  std::unordered_map<std::string,std::vector<std::string>> dup; 
  std::queue<std::string> directories;
  std::string largestp = "";
  int largests = -1;
  int nf = 0;
  int nd = 0;
  int all = 0;
  
  //if root is not a directory return false
  if( ! is_dir(dir_name)){
    return false;
  }

  directories.push(dir_name);
  //preform BFS on directories strarting from root
  while(!directories.empty()){
     currentPath = directories.front();
     directories.pop();
     //if you failed to open a a directory return false
     if (!(dir = opendir(currentPath.c_str()))){
        return false;
		}
     std::string p = "file -b " + currentPath +'/' +"{.*,*}";
     //try to read file type for all directory enteires
     getFiletype(p,true);
     //read through contents of the directory
    while ((entry = readdir(dir))) {
        std::string name = entry-> d_name; 
        if ( name == "." || name == ".."){
            continue;
        }
        std::string pathname = currentPath+'/'+entry->d_name;
        //if you encounter a directory add it to the queue
        if (is_dir(pathname)) {
            ++nd;
            directories.push(pathname);
            
        }
        //otherwise get stats about the file
        else{
            ++nf;
            if(flag == true){
                //if you failed to read contets of the directory read content one by one
                getFiletype("file -b " + pathname,false);
            }
            //calculate sha256 for file   
           dup[sha256_from_file(pathname)].push_back(pathname);
           struct stat s;
           //if you failed to obtain struct for file return false
           if(stat(pathname.c_str(),&s) < 0){    
             return false;
           }
           all += s.st_size;
           //update largest file if needed
           if(s.st_size > largests){
               largests = s.st_size;
               largestp = pathname;             
           }
    }
   
    
  }
   closedir(dir);
  }
    //return data in the provided struct 
    res.largest_file_path = largestp;
    res.largest_file_size = largests;
    res.n_files = nf;
    res.n_dirs = nd;
    res.all_files_size = all;
    //sort common file types and duplicates
    std::vector<std::pair<int,std::string>> arr;
    for(auto & c : common)
       arr.emplace_back(-c.second, c.first);

    if(arr.size() > size_t(5)) {
    std::partial_sort(arr.begin(), arr.begin() + 5, arr.end());
    // drop all entries after the first n
    arr.resize(5);
  } else {
    std::sort(arr.begin(), arr.end());
  }
   for(auto & a : arr){
     res.most_common_types.push_back(a.second);
   }
   
    std::vector<std::pair<int,std::vector<std::string>>> arr2;
    for(auto & d : dup)
       arr2.emplace_back(-d.second.size(), d.second);

    if(arr2.size() > size_t(5)) {
    std::partial_sort(arr2.begin(), arr2.begin() + 5, arr2.end());
    // drop all entries after the first n
    arr2.resize(5);
  } else {
    std::sort(arr2.begin(), arr2.end());
  }
   for(auto & a : arr2){
      if(a.second.size()>=2)
        res.duplicate_files.push_back(a.second);
   }   
    
    
    
  
  return true;
}
