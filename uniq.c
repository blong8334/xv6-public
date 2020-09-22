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

struct StringArray {
  int length;
  int index;
  struct String** strings;
};

char* allocateCharMemory(int size){
  char* line = malloc(size);
  if (line == 0){
    printf(2, "could not allocate memory\n");
    exit();
  }
  return line;
}

struct String** allocateStringMemory(int size){
  struct String** strings = malloc(size);
  if (strings == 0){
    printf(2, "could not allocate memory\n");
    exit();
  }
  return strings;
}

struct String createString(int baseSize){
  struct String string1;
  string1.source = allocateCharMemory(baseSize);
  string1.length = baseSize;
  string1.index = 0;
  return string1;
}

struct StringArray createStringArray(int baseSize){
  struct StringArray stringArray;
  stringArray.strings = allocateStringMemory(baseSize);
  stringArray.length = baseSize;
  stringArray.index = 0;
  return stringArray;
}

char* copyChars(int length, char* source, char* target){
  for (int j = 0; j < length; j++){
    target[j] = source[j];
  }
  return target;
}

struct String* checkSize(struct String* str){
  int length = str->length;
  int index = str->index;
  if (index < length){
    return str;
  }
  int newLength = length * 2 + 1;
  char* newSource = allocateCharMemory(newLength);
  newSource = copyChars(length, str->source, newSource);
  char* oldSource = str->source;
  str->source = newSource;
  str->length = newLength;
  free(oldSource);
  return str;
}

struct StringArray split(struct String* file, char splitTarget){
  // struct String string = createString(file->length);
  // struct String* stringPointer = &string;

  struct StringArray stringArray = createStringArray(12);
  // struct StringArray* stringArrayPointer = &stringArray;
  int fileIndex = 0;
  while (fileIndex < file->index){
    printf(1, "file char: %c\n", file->source[fileIndex]);
    fileIndex += 1;
  }
  return stringArray;
}

struct String readFile(int fd){
  int n;
  struct String string1 = createString(baseSize);
  struct String* stringP = &string1;
  while ((n = read(fd, buf, sizeof(buf))) > 0){
    for (int i = 0; i < n; i++){
      stringP->source[stringP->index++] = buf[i];
      stringP = checkSize(stringP);
    }
  }
  if (n < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
  return string1;
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
  struct String string = readFile(fd);
  split(&string, '\n');
  // printf(1, "%s\n", results);
  close(fd);
  exit();
}