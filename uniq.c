#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

typedef enum {true, false} bool;

int main(int argc, char *argv[])
{
  printf(1, "Stub for uniq fn\n");
  printf(2, "argc %d\n", argc);
  char* fileName = argv[1];
  printf(1, "fileName: %s\n", fileName);
  int fd = open(fileName, 0);
  printf(1, "fd: %d\n", fd);
  struct stat statBuf;
  int status = fstat(fd, &statBuf);
  printf(1, "Status: %d\n", status);
  int fileSize = statBuf.size;
  char buf[statBuf.size];
  int n = read(fd, buf, sizeof(buf));
  printf(1, "n: %d\n", n);
  char wordCache[fileSize];
  int wordIndex = 0;
  int wordCount = 1;
  bool newWord = false;
  // we need to read a line. check if it mataches the previous line. 
  for (int i = 0; i < n; i++) {
    char currentChar = buf[i];
    if (currentChar == '\n') {
      // do we have a new word?
      continue;
    } if (currentChar == wordCache[wordIndex++]) {

    }
  }
  printf(1, "\n");
  exit();
}

/*
  Read line
  compare to last line
  update unique count. 
  update last word. 
*/