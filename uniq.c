#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

/*
  How would you do this in node?
    read the file.
    split at the new lines.
    set the first sentence to base.
    compare the second sentence. 
    move on. 
*/

typedef enum
{
  true,
  false
} bool;

void cacheWord(int *wordIdx, char buf[], char wordCache[])
{
  int cacheIdx = 0;
  char value;
  while ((value = buf[*wordIdx]))
  {
    *wordIdx += 1;
    if (value == '\n')
    {
      wordCache[cacheIdx] = '\0';
      return;
    }
    wordCache[cacheIdx++] = value;
  }
}

int main(int argc, char *argv[])
{
  char *fileName = argv[1];
  int fd = open(fileName, 0);
  struct stat statBuf;
  int status = fstat(fd, &statBuf);
  int fileSize = statBuf.size + 1;
  char fileString[fileSize];
  read(fd, fileString, fileSize);
  char wordCache[fileSize];
  int wordIndex = 0;
  cacheWord(&wordIndex, fileString, wordCache);
  cacheWord(&wordIndex, fileString, wordCache);
  exit();
}