#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[12];
const int baseSize = sizeof(buf);
/*
  read file.
  split file.
  compare lines.
*/

struct String {
  int length;
  int index;
  char* source;
};

char* allocateCharMemory(int size){
  char* line = malloc(size);
  if (line == 0){
    printf(2, "could not allocate memory\n");
    exit();
  }
  line[0] = '\0';
  return line;
}

// char** allocateCharPointerMemory(int size){
//   char** line = malloc(size);
//   if (line == 0){
//     printf(2, "could not allocate memory\n");
//     exit();
//   }
//   line[0] = '\0';
//   return line;
// }

char* copyChars(int length, char* source, char* target){
  for (int j = 0; j < length; j++){
    target[j] = source[j];
  }
  return target;
}

// char** copyChars(char** source, char** target){
//   for (int j = 0; source[j]; j++){
//     target[j] = source[j];
//     target[j + 1] = '\0';
//   }
//   return target;
// }

// char** copyCharPointers(char** source, char** target){
//   for (int j = 0; source[j]; j++){
//     target[j] = source[j];
//     target[j + 1] = '\0';
//   }
//   return target;
// }

struct String checkSize(struct String str){
  int length = str.length;
  int index = str.index;
  if (index < length){
    return str;
  }
  int newLength = length * 2 + 1;
  char* newSource;
  newSource = copyChars(length, str.source, malloc(newLength));
  struct String newStr;
  newStr.source = newSource;
  newStr.length = newLength;
  newStr.index = index;
  free(str.source);
  return newStr;
}

// char** checkSize(int index, char** source){
//   int sourceSize = sizeof(source);
//   if (index < sourceSize / sizeof(source[0])){
//     return source;
//   }
//   char** temp = malloc(sourceSize * 2 + 1);
//   temp = copyChars(source, temp);
//   free(source);
//   return temp;
// }

// char** split(char* str, char splitTarget){
//   char* line = allocateCharMemory(baseSize);
//   char** results = allocateCharPointerMemory(baseSize);
//   int lineIdx = 0, resultsIdx = 0;
//   while (*str != '\0'){
//     if (*str == splitTarget){
//       results[resultsIdx++] = line;
//       lineIdx = 0;
//       if (resultsIdx >= sizeof(results) / sizeof(results[0])){
//         // we need to make a bigger results array and then copy it over. 
//       }
//     }
//     line[lineIdx++] = *str;
//     str += 1;
//   }
//   return results;
// }

char* readFile(int fd){
  int n;
  struct String string1;
  string1.source = allocateCharMemory(baseSize);
  string1.length = baseSize;
  string1.index = 0;
  while ((n = read(fd, buf, sizeof(buf))) > 0){
    for (int i = 0; i < n; i++){
      string1.source[string1.index++] = buf[i];
      string1 = checkSize(string1);
    }
  }
  if (n < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
  return string1.source;
}

void writeCacheToResults(char* cache, char* results){
  int length = strlen(results);
  while (*cache != '\0'){
    results[length++] = *cache;
    results[length] = '\0';
    cache += 1;
  }
}

void generateUniq(char* file, char* cache, char* results){
  results[0] = '\0';
  cache[0] = '\0';
  int cacheIdx = 0, uniqCount = 1;
  while (*file != '\0'){
    if (cacheIdx >= strlen(cache) || *file != cache[cacheIdx]){
      writeCacheToResults(cache, results);
      while (1){
        cache[cacheIdx++] = *file;
        cache[cacheIdx] = '\0';
        if (*file == '\0'){
          break;
        }
        if (*file == '\n'){ // starting new word
          uniqCount = 1;
          file += 1;
          cacheIdx = 0;
          break;
        }
        file += 1;
      }
    } else if (*file == '\n'){
      uniqCount += 1;
      file += 1;
      cacheIdx = 0;
    } else{
      cacheIdx += 1;
      file += 1;
    }
  }
  writeCacheToResults(cache, results);
}

int main(int argc, char* argv[]){
  int fd;
  if (argc <= 1){
    fd = 0;
  } else{
    fd = open(argv[1], 0);
  }
  char* results = readFile(fd);
  printf(1, "%s\n", results);
  close(fd);
  exit();
}