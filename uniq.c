#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[12];
/*
  Create a fn that reads lines.
  use malloc to write the chunk to a line until you don't have anymore room.
    if you run out of space with the current line, then create some new, bigger memory, free
    the old and keep on rocking in the free world.
*/

char* allocateMemory(int size){
  char* line = malloc(size);
  if (line == 0){
    printf(2, "could not allocate memory\n");
    exit();
  }
  line[0] = '\0';
  return line;
}

void readFile(int fd){
  int n, mx = 1;
  int lineIndex = 0;
  const int baseSize = sizeof(buf);
  char* line = allocateMemory(baseSize);
  while ((n = read(fd, buf, sizeof(buf))) > 0){
    for (int i = 0; i < n; i++){
      line[lineIndex++] = buf[i];
      line[lineIndex] = '\0';
      if (lineIndex > baseSize * mx){
        mx *= 2;
        char* newLine = allocateMemory(baseSize * mx + 1);
        for (int j = 0; line[j]; j++){
          newLine[j] = line[j];
          newLine[j + 1] = '\0';
        }
        char* toDelete = line;
        line = newLine;
        free(toDelete);
      }
    }
  }
  if (n < 0){
    printf(1, "wc: read error\n");
    exit();
  }
  printf(1, "line %s\n", line);
  free(line);
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
  readFile(fd);
  // printf(1, "%s\n", results);
  close(fd);
  exit();
}