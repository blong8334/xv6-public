#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int loadFirstWord(char *file, char *cache)
{
  int fileIdx = 0;
  while (1)
  {
    char value = file[fileIdx];
    if (value == '\0')
    {
      return fileIdx;
    }
    *cache = value;
    cache += 1;
    fileIdx += 1;
    *cache = '\0';
    if (value == '\n')
      return fileIdx;
  }
}

void writeCacheToResults(char *cache, char *results)
{
  int length = strlen(results);
  while (*cache != '\0')
  {
    results[length++] = *cache;
    results[length] = '\0';
    cache += 1;
  }
}

void generateUniq(char *file, char *cache, char *results)
{
  results[0] = '\0';
  int cacheIdx = 0, uniqCount = 1;
  int fileIdx = loadFirstWord(file, cache);
  file = &file[fileIdx];
  while (*file != '\0')
  {
    if (cacheIdx >= strlen(cache) || *file != cache[cacheIdx])
    {
      writeCacheToResults(cache, results);
      while (1)
      {
        cache[cacheIdx++] = *file;
        cache[cacheIdx] = '\0';
        if (*file == '\0')
        {
          break;
        }
        if (*file == '\n')
        { // starting new word
          uniqCount = 1;
          file += 1;
          cacheIdx = 0;
          break;
        }
        file += 1;
      }
    }
    else if (*file == '\n')
    {
      uniqCount += 1;
      file += 1;
      cacheIdx = 0;
    }
    else
    {
      cacheIdx += 1;
      file += 1;
    }
  }
  writeCacheToResults(cache, results);
}

int main(int argc, char *argv[])
{
  char *fileName = argv[1];
  int fd = open(fileName, 0);
  struct stat statBuf;
  fstat(fd, &statBuf);
  int fileSize = statBuf.size + 1;
  char file[fileSize];
  file[fileSize - 1] = '\0';
  read(fd, file, fileSize);
  char results[fileSize];
  char cache[fileSize];
  generateUniq(file, cache, results);
  exit();
}