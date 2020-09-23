#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

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
char buf[12];
const int baseSize = sizeof(buf);

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

struct String* cloneString(struct String* string){
  struct String* newStringP = malloc(sizeof(string));
  *newStringP = createString(string->index);
  newStringP->index = string->index;
  int i = 0;
  while (i < newStringP->index){
    newStringP->source[i] = string->source[i];
    i++;
  }
  return newStringP;
}

struct String** copyStrings(int length, struct String** source, struct String** target){
  for (int j = 0; j < length; j++){
    target[j] = source[j];
  }
  return target;
}

struct String* ensureStringSize(struct String* str){
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

struct StringArray* ensureStringArraySize(struct StringArray* str){
  int length = str->length;
  int index = str->index;
  if (index < length){
    return str;
  }
  int newLength = length * 2 + 1;
  struct String** newStrings = allocateStringMemory(newLength);
  newStrings = copyStrings(length, str->strings, newStrings);
  struct String** oldStrings = str->strings;
  str->strings = newStrings;
  str->length = newLength;
  free(oldStrings);
  return str;
}

struct StringArray split(struct String* file, char splitTarget){
  struct String string = createString(file->length);
  struct StringArray stringArray = createStringArray(12);
  struct StringArray* stringArrayPointer = &stringArray;
  int fileIndex = 0;
  while (fileIndex < file->index){
    char currentChar = file->source[fileIndex++];
    if (currentChar != splitTarget){
      string.source[string.index++] = currentChar;
      if (fileIndex != file->index) continue;
    }
    stringArray.strings[stringArray.index++] = cloneString(&string);
    stringArrayPointer = ensureStringArraySize(stringArrayPointer);
    string.index = 0;
  }
  return stringArray;
}

struct String readFile(int fd){
  int n;
  struct String string1 = createString(baseSize);
  struct String* stringP = &string1;
  while ((n = read(fd, buf, sizeof(buf))) > 0){
    for (int i = 0; i < n; i++){
      string1.source[string1.index++] = buf[i];
      stringP = ensureStringSize(stringP);
    }
  }
  if (n < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
  return string1;
}

int main(int argc, char* argv[]){
  int fd;
  if (argc <= 1){
    fd = 0;
  } else{
    fd = open(argv[1], 0);
  }
  struct String string = readFile(fd);
  printf(1, "string source %s\n", string.source);
  struct StringArray stringArray = split(&string, '\n');
  printf(1, "%s\n", stringArray.strings[0]->source);
  close(fd);
  exit();
}