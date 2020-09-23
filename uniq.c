#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

struct Options {
  int count;
  int duplicates;
  int ignore;
};
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
char buf[512];
const int baseSize = sizeof(buf);
struct String baseString;
struct Options options;

char* allocateCharMemory(int size){
  char* line = malloc(size);
  if (line == 0){
    printf(2, "could not allocate memory\n");
    exit();
  }
  return line;
}

struct String** allocateStringMemory(int size){
  struct String** strings = malloc(size * sizeof(&baseString));
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
    *target[j] = *source[j];
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
  struct StringArray stringArray = createStringArray(512);
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
    if (fileIndex == file->index && currentChar == splitTarget){
      stringArray.strings[stringArray.index++] = cloneString(&string);
    }
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

int ignoreCase(int code){
  if (options.ignore && code <= 122 && code >= 97) return code - 32;
  return code;
}

int compareStrings(struct String* string1, struct String* string2){
  int length = string1->index;
  if (string2->index > length){
    length = string2->index;
  }
  for (int i = 0; i < length; i++){
    int code1 = ignoreCase((int)string1->source[i]);
    int code2 = ignoreCase((int)string2->source[i]);
    if (code1 != code2) return 0;
  }
  return 1;
}

int getLengthOfInt(int num){
  int count = 0;
  while (num > 0){
    num -= num % 10;
    num /= 10;
    count += 1;
  }
  return count;
}

void writeToResults(struct String* results, struct String* word, int lineCount){
  if (options.duplicates && lineCount <= 1){
    return;
  }
  if (results->index) results->source[results->index++] = '\n';
  if (options.count){
    int lineSize = getLengthOfInt(lineCount);
    results->index += lineSize;
    for (int i = 1; i <= lineSize; i++){
      int toAdd = lineCount % 10;
      results->source[results->index - i] = (char)(toAdd + '0');
      lineCount = (lineCount - toAdd) / 10;
    }
    results->source[results->index++] = ' ';
  }
  for (int i = 0; i < word->index; i++){
    results->source[results->index++] = word->source[i];
  }
}

struct String compareLines(struct StringArray* stringArray, int fileLength){
  struct String results = createString(fileLength);
  struct String* base = stringArray->strings[0];
  int lineCount = 0;
  for (int i = 1; i < stringArray->index; i++){
    lineCount += 1;
    struct String* toCompare = stringArray->strings[i];
    if (!compareStrings(base, toCompare)){
      writeToResults(&results, base, lineCount);
      base = toCompare;
      lineCount = 0;
    }
  }
  writeToResults(&results, base, lineCount);
  return results;
}

int main(int argc, char* argv[]){
  options.count = options.duplicates = options.ignore = 0;
  int fd = 0;
  for (int i = 1; i < argc; i++){
    char* arg = argv[i];
    if (arg[0] == '-'){
      for (int j = 1; arg[j]; j++){
        if (arg[j] == 'c') options.count = 1;
        else if (arg[j] == 'd') options.duplicates = 1;
        else if (arg[j] == 'i') options.ignore = 1;
      }
    } else if (i + 1 == argc) fd = open(argv[i], 0);
  }
  struct String file = readFile(fd);
  struct StringArray stringArray = split(&file, '\n');
  free(file.source);
  struct String results = compareLines(&stringArray, file.length * 2);
  for (int i = 0; i < results.index; i++) printf(1, "%c", results.source[i]);
  printf(1, "\n");
  close(fd);
  exit();
}