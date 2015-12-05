#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SMS 200

typedef struct Record {
  int id;
  char title[SMS];
  char artist[SMS];
  char composer[SMS];
  char album[SMS];
  char genre[SMS]; //0-1-2
  int rating; //0-5
  char remarks[SMS];
  struct Record *next;
} Record;

int charToInt(char);
Record* parseInputFile(char*, Record*);
Record* parseUserInput(Record*);
Record* checkIfUniqueInList(Record *,int);
void printList(Record *);


void main(){
  Record *list = NULL;
  Record *tail;
  int i;
  for (i = 0; i < 2; i++){
    if (list == NULL){
      list = parseUserInput(list);
      tail = list;
    } else {
      tail -> next = parseUserInput(list);
      tail = tail -> next;
    }
  }
  printList(list);
}

/*
  ID, Title, Artist[e], Composer[e], Album[e], Genre*, Rating, Remarks
  *Art Music, Popular Music, or Traditional Music (store as int 1-2 maybe)
*/

/*
  *mode 1 - string input
  *mode 2 - file input (looks for an ID)
  Returns the address of a record node containing the values specified by
  the string input.
*/
Record* parseInputFile(char* input, Record* list){
  Record *add = malloc(sizeof(Record));
  add -> next = NULL;
  add -> id = -1;
  char *end = input + strlen(input);
  int ix = 0; //index for inputting the fields onto the Record node
  int field = 0;
  while (input < end){
      if (*input != ','){
        switch (field){
          case 0: //ID
          add -> id *= 10;
          add -> id += charToInt(*input);
          break;
          case 1: //title
          add -> title[ix] = *input;
          break;
          case 2: //artist
          add -> artist[ix] = *input;
          break;
          case 3: //composer
          add -> composer[ix] = *input;
          break;
          case 4:
          add -> album[ix] = *input;
          break;
          case 5:
          add -> genre[ix] = *input;
          break;
          case 6:
          add -> rating = charToInt(*input);
          break;
          case 7:
          add -> remarks[ix] = *input;
          break;
          default:
          break;
        }
        ix++;
      } else {
        field++;
        ix = 0;
      }
    input++;
    if (input >= end){
      switch (field){
        case 0: //ID
        add -> id *= 10;
        add -> id += charToInt(*input);
        break;
        case 1: //title
        add -> title[ix] = *input;
        break;
        case 2: //artist
        add -> artist[ix] = *input;
        break;
        case 3: //composer
        add -> composer[ix] = *input;
        break;
        case 4:
        add -> album[ix] = *input;
        break;
        case 5:
        add -> genre[ix] = *input;
        break;
        case 6:
        add -> rating = charToInt(*input);
        break;
        case 7:
        add -> remarks[ix] = *input;
        break;
        default:
        break;
      }
      break;
      }
  }
  return add;
}

Record* parseUserInput(Record *list){
  Record *add = malloc(sizeof(Record));
  add -> next = NULL;
  int id;
  while (1) {
    printf("ID: ");
    scanf("%d", &id);
    if (checkIfUniqueInList(list, id) == NULL){ //no similar id found
      add -> id = id;
      break;
    } else {
      printf("Error: ID already exists.\n");
    }
  }
  getchar();
  printf("Title: ");
  fgets(&(add -> title[0]), SMS + 1, stdin);
  printf("Artist: ");
  fgets(&(add -> artist[0]), SMS + 1, stdin);
  printf("Composer: ");
  fgets(&(add -> composer[0]), SMS + 1, stdin);
  printf("Album: ");
  fgets(&(add -> album[0]), SMS + 1, stdin);
  printf("Genre: ");
  fgets(&(add -> genre[0]), SMS + 1, stdin);
  printf("Rating: ");
  scanf("%d", &(add -> rating));
  getchar();
  printf("Remarks: ");
  fgets(&(add -> remarks[0]), SMS + 1, stdin);
  add -> title[strcspn(add -> title, "\n")] = 0;
  add -> artist[strcspn(add -> artist, "\n")] = 0;
  add -> composer[strcspn(add -> composer, "\n")] = 0;
  add -> album[strcspn(add -> album, "\n")] = 0;
  add -> genre[strcspn(add -> genre, "\n")] = 0;
  add -> remarks[strcspn(add -> remarks, "\n")] = 0;
  printf("Record added!\n");
  // printRecord(add);
  return add;
}

Record* checkIfUniqueInList(Record *list,int id){
  while (list != NULL){
    if (id == list -> id) return list;
    list = list -> next;
  }
  return list;
}

void printList(Record* list){
  printf("ID\tTITLE\tARTIST\tCOMPOSER\tALBUM\tGENRE\tRATING\tREMARKS\t\n\n");
  while (list != NULL){
    printf("%d\t%s\t%s\t%s\t%s\t%s\t%d\t%s\n", list -> id, list -> title,
   list -> artist, list -> composer, list -> album, list -> genre,
   list -> rating, list -> remarks);
   list = list -> next;
  }
}

int charToInt(char c){
  if (c >= 48 ||c <= 57){
    return c - 48;
  }
  return -1;
}
