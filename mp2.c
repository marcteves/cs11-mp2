#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SMS 200 //Arbitrary max size of record fields

/*TODO:
 -documentation of code
*/

typedef struct Record {
  int id; //UNIQUE id of the song in the library
  char title[SMS]; //title of the song. cannot be empty
  char artist[SMS]; //artist of the song.
  char composer[SMS]; //composer of the song.
  char album[SMS]; //album of the song. if empty, album is PRINTED as a single
  char genre[SMS]; //genre of the song.
  int rating; //0-5 only
  char remarks[SMS]; //comments about the song.
  struct Record *next;
  //if not NULL, this Record is a part of a list.
  //if NULL, this Record is a single record or the head of a list with
  //one element
  struct Record *ref;
  //if this is not NULL, then the record is a reference to another record
} Record;

Record* parseInputFile(char*, Record*);
Record* parseUserInput(Record*);
Record* checkIfUniqueInList(Record *,int);
Record* findSongList(Record*, char*, char*); //this returns a list based on the query char*
Record* userSelectFromList(Record*);
Record* createNodeRef(Record*);
void copyNodes(Record*, Record*);

//Print class functions.
//Take a list, and print it in some manner.
void printListSort(Record *);
void printList(Record *);
void printListNoID(Record *);

//Update class functions.
void updateSong(Record*);
void replaceField(Record*, Record*, char*, char*);

//Some of the functions used by printListSort
void artistSort(Record *);
void titleSort(Record *);
void ratingSort(Record *);
void normalSort(Record *);

//misc functions
void writeToFile(Record *, char *);
char* lowCase(char *);
int charToInt(char);

void main(){
  Record *list = NULL;
  Record *tail = NULL;
  char input[1200];
  char path[200] = "m.input";

  //load file to a linked list
  FILE *fp = fopen(path, "r+");
  while (fgets(input, 1200, fp) != NULL){
    if (list == NULL){
      list = parseInputFile(input, list);
      tail = list;
    } else {
      tail -> next = parseInputFile(input, list);
      tail = tail -> next;
    }
  }
  fclose(fp);
  int option;
  while (1){
    printf("Options:\n1-Add song\n2-Update song\n3-List by Query\n4-Exit\n");
    scanf("%d", &option);
    getchar();
    if (option == 1){
      if (list == NULL){
        list = parseUserInput(list);
        tail = list;
      } else {
        tail -> next = parseUserInput(list);
        tail = tail -> next;
      }
    } else if (option == 2){
      updateSong(list);
    } else if (option == 3){
      printListSort(list);
    } else if (option == 4){
      printf("Goodbye. Saving list to \\%s", path);
      writeToFile(list, path);
      break;
    }
    printf("\n");
  }
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
  add -> id = 0;
  add -> ref = NULL;
  char *end = input + strlen(input) - 1;
  int ix = 0; //index for inputting the fields onto the Record node
  int field = 0;
  while (1){
      if (*input != ',' && *input != '\n'){
        switch (field){
          case 0: //ID
          add -> id *= 10;
          add -> id += charToInt(*input);
          break;
          case 1:
          add -> title[ix] = *input;
          break;
          case 2:
          add -> artist[ix] = *input;
          break;
          case 3:
          add -> composer[ix] = *input;
          break;
          case 4:
          add -> album[ix] = *input;
          break;
          case 5:
          add -> genre[ix] = *input;
          break;
          case 6:
          if (charToInt(*input) > 0) add -> rating = charToInt(*input);
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
      add -> remarks[ix] = *input;
      break;
    }
  }
  return add;
}

Record* parseUserInput(Record *list){
  Record *add = malloc(sizeof(Record));
  add -> next = NULL;
  add -> ref = NULL;
  int id;
  while (1) {
    printf("ID: ");
    scanf("%d", &id);
    getchar();
    if (checkIfUniqueInList(list, id) == NULL){ //no similar id found
      add -> id = id;
      break;
    } else {
      printf("Error: ID already exists.\n");
    }
  }
  printf("Title: ");
  fgets(&(add -> title[0]), SMS, stdin);
  while (add -> title[0] == '\n'){
    printf("Title cannot be empty!\n");
    fgets(&(add -> title[0]), SMS, stdin);
  }
  printf("Artist: ");
  fgets(&(add -> artist[0]), SMS, stdin);
  printf("Composer: ");
  fgets(&(add -> composer[0]), SMS, stdin);
  printf("Album: ");
  fgets(&(add -> album[0]), SMS, stdin);
  printf("Genre: ");
  fgets(&(add -> genre[0]), SMS, stdin);
  printf("Rating: ");
  scanf("%d", &(add -> rating));
  getchar();
  printf("Remarks: ");
  fgets(&(add -> remarks[0]), SMS, stdin);
  add -> title[strcspn(add -> title, "\n")] = 0;
  add -> artist[strcspn(add -> artist, "\n")] = 0;
  add -> composer[strcspn(add -> composer, "\n")] = 0;
  add -> album[strcspn(add -> album, "\n")] = 0;
  add -> genre[strcspn(add -> genre, "\n")] = 0;
  add -> remarks[strcspn(add -> remarks, "\n")] = 0;
  printf("Record added!\n");
  printList(add);
  return add;
}

Record* userSelectFromList(Record * list){
  printList(list);
  printf("What song did you need? Specify by ID\n");
  int input;
  Record *select = NULL;
  while (select == NULL){
    scanf("%d", &input);
    getchar();
    select = createNodeRef(checkIfUniqueInList(list, input));
    if (select == NULL) printf("Error: id not found.\n");
  }
  return select;
}

Record* findSongList(Record* list, char* query, char* input){
  Record* newlist = NULL;
  Record* current = NULL;
  while (list != NULL){
    if (strcmp(query, "title") == 0){
      if (strcmp(list -> title, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "artist") == 0){
      if (strcmp(list -> artist, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "composer") == 0){
      if (strcmp(list -> composer, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "album") == 0){
      if (strcmp(list -> album, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "genre") == 0){
      if (strcmp(list -> genre, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "rating") == 0){
      int rating;
      sscanf(input, "%d", &rating);
      if (list -> rating >= rating){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    }
    list = list -> next;
  }
  return newlist;
}

Record* createNodeRef(Record *ref){
  if (ref == NULL) return NULL;
  Record *new = malloc(sizeof(Record));
  new -> ref = ref;
  new -> next = NULL;
  copyNodes(new, ref);
  return new;
}

//copies n2 to n1
void copyNodes(Record *n1, Record *n2){
  n1 -> id = n2 -> id;
  strcpy(n1 -> title, n2 -> title);
  strcpy(n1 -> artist, n2 -> artist);
  strcpy(n1 -> composer, n2 -> composer);
  strcpy(n1 -> album, n2 -> album);
  strcpy(n1 -> genre, n2 -> genre);
  n1 -> rating = n2 -> rating;
  strcpy(n1 -> remarks, n2 -> remarks);
}

void printListSort(Record *list){
  Record* refList;
  char query[30];
  char input[200];
  char bigin[230];
  printf("What song did you want to print? Specify with <Category>_<Value>\n");
  fgets(&bigin[0], 230, stdin);
  sscanf(&bigin[0], "%s %s", query, input);
  printf("%s %s\n", query, input);
  if (strcmp(lowCase(query), "all") != 0){
    refList = findSongList(list, lowCase(query), lowCase(input));
  }
  if (strcmp(lowCase(query), "artist") == 0){
    artistSort(refList);
    printListNoID(refList);
  } else if (strcmp(lowCase(query), "title") == 0) {
    titleSort(refList);
    printListNoID(refList);
  } else if (strcmp(lowCase(query), "all") == 0) {
    printList(list);
  } else if (strcmp(lowCase(query), "rating") == 0){
    int rating;
    sscanf(input, "%d", &rating);
    printf("%d", rating);
    while (rating < 1 || rating > 5){
      printf("Error: Rating should not exceed from 1 to 5.\n");
      scanf("%d", &rating);
      getchar();
    }
    normalSort(refList);
    ratingSort(refList);
    printListNoID(refList);
  } else {
    normalSort(refList);
    printListNoID(refList);
  }
}

//step 1: sort the list, ignoring similar fields
//step 2: find subsets with similar fields, and sort that list.
void artistSort(Record* list){
  Record* sortednode = NULL;
  Record* now;
  Record* swapper = malloc(sizeof(Record));
  while (sortednode != list){
    now = list;
    while (now -> next != sortednode){
      if (strcmp(now -> title, (now -> next) -> title) > 0 ){
        copyNodes(swapper, now);
        copyNodes(now, now -> next);
        copyNodes(now -> next, swapper);
      }
      now = now -> next;
    }
    sortednode = now;
  }
}

void titleSort(Record* list){
  Record* sortednode = NULL;
  Record* now;
  Record* swapper = malloc(sizeof(Record));
  while (sortednode != list){
    now = list;
    while (now -> next != sortednode){
      if (strcmp(now -> artist, (now -> next) -> artist) > 0 ){
        copyNodes(swapper, now);
        copyNodes(now, now -> next);
        copyNodes(now -> next, swapper);
      }
      now = now -> next;
    }
    sortednode = now;
  }
}

void ratingSort(Record* list){
  Record* sortednode = NULL;
  Record* now;
  Record* swapper = malloc(sizeof(Record));
  while (sortednode != list){
    now = list;
    while (now -> next != sortednode){
      if (now -> rating < (now -> next) -> rating){
        copyNodes(swapper, now);
        copyNodes(now, now -> next);
        copyNodes(now -> next, swapper);
      }
      now = now -> next;
    }
    sortednode = now;
  }
}

void normalSort(Record* list){
  artistSort(list);
  titleSort(list);
}

//updates the selected song in list
void updateSong(Record * list){
  if (list != NULL){
    char input[200];
    char query[30];
    Record* searchlist;
    Record* update = NULL;
    printf("Input title of song to be updated: ");
    fgets(input, 200, stdin);
    input[strcspn(input, "\n")] = '\0';
    searchlist = findSongList(list, "title", lowCase(input));
    if (searchList != NULL){
      if (searchlist -> next != NULL){ //if next is not null, there is more than one result
        printf("Multiple song titles detected.\n");
        update = userSelectFromList(searchlist);
      } else {
        update = createNodeRef(searchlist);
      }
      printList(update);
      printf("What did you want to update?");
      fgets(query, 30, stdin);
      query[strcspn(query, "\n")] = 0;
      printf("What did you want to replace it with?");
      fgets(input, 200, stdin);
      input[strcspn(input, "\n")] = 0;
      replaceField(list, update -> ref, lowCase(query), input);
    } else {
      printf("Song record does not exist!\n");
    }
  } else {
    printf("List empty.\n");
  }
}

void replaceField(Record* list, Record *update, char *query, char *input){
  //remember that records passed here are nodes with
  //non-NULL ref values
  printf("s\n");
  printf("%p", update -> ref);
  if (strcmp(query, "id") == 0){
    int digits;
    sscanf(input, "%d", &digits);
    printf("%d", digits);
    if ((checkIfUniqueInList(list, digits)) == NULL){
      (update -> ref) -> id = digits;
    } else {
      printf("ID already taken. ID not updated.\n");
    }
  } else if (strcmp(query, "title") == 0){
    strcpy((update -> ref) -> title, input);
  } else if (strcmp(query, "artist") == 0){
    strcpy((update -> ref) -> artist, input);
  } else if (strcmp(query, "composer") == 0){
    strcpy((update -> ref) -> composer, input);
  } else if (strcmp(query, "album") == 0){
    strcpy((update -> ref) -> album, input);
  } else if (strcmp(query, "genre") == 0){
    strcpy((update -> ref) -> genre, input);
  } else if (strcmp(query, "rating") == 0){
    int digits;
    sscanf(input, "%d", &digits);
    (update -> ref) -> rating = digits;
  } else if (strcmp(query, "title") == 0){
    strcpy((update -> ref) -> title, input);
  } else  {
    printf("Query invalid. Error unhandled. Nothing changed.\n");
  }
}

//returns the address of the specified node with the id
//null if it is not in the list
Record* checkIfUniqueInList(Record *list,int id){
  while (list != NULL){
    if (id == list -> id) return list;
    list = list -> next;
  }
  return list;
}

void printListNoID(Record* list){
  printf("TITLE\tARTIST\tCOMPOSER\t\tALBUM\tGENRE\tRATING\tREMARKS\n");
  while (list != NULL){
    printf("%s\t", list -> title);
    printf("%s\t", list -> artist);
    printf("%s\t\t", list -> composer);
    if (list -> album[0] == 0){
      printf("Single\t");
    } else {
      printf("%s\t", list -> album);
    }
    printf("%s\t", list -> genre);
    printf("%d\t", list -> rating);
    printf("%s\n", list -> remarks);
  //   printf("%d\t%s\t%s\t%s\t%s\t%s\t%d\t%s\n", list -> id, list -> title,
  //  list -> artist, list -> composer, list -> album, list -> genre,
  //  list -> rating, list -> remarks);
   list = list -> next;
  }
}

void printList(Record* list){
  printf("ID\tTITLE\tARTIST\tCOMPOSER\t\tALBUM\tGENRE\tRATING\tREMARKS\n");
  while (list != NULL){
    printf("%d\t", list -> id);
    printf("%s\t", list -> title);
    printf("%s\t", list -> artist);
    printf("%s\t\t", list -> composer);
    if (list -> album[0] == 0){
      printf("Single\t");
    } else {
      printf("%s\t", list -> album);
    }
    printf("%s\t", list -> genre);
    printf("%d\t", list -> rating);
    printf("%s\n", list -> remarks);
  //   printf("%d\t%s\t%s\t%s\t%s\t%s\t%d\t%s\n", list -> id, list -> title,
  //  list -> artist, list -> composer, list -> album, list -> genre,
  //  list -> rating, list -> remarks);
   list = list -> next;
  }
}

void writeToFile(Record *list, char *path){
  FILE *fp = fopen(path, "w+");
  char digits[3];
  while (list != NULL){
    snprintf(digits, 3, "%d", list -> id);
    fputs(digits, fp);
    fputs(",", fp);
    fputs(list -> title, fp);
    fputs(",", fp);
    fputs(list -> artist, fp);
    fputs(",", fp);
    fputs(list -> composer, fp);
    fputs(",", fp);
    fputs(list -> album, fp);
    fputs(",", fp);
    fputs(list -> genre, fp);
    fputs(",", fp);
    snprintf(digits, 3, "%d", list -> rating);
    fputs(digits, fp);
    fputs(",", fp);
    fputs(list -> remarks, fp);
    list = list -> next;
  }
  fclose(fp);
}

int charToInt(char c){
  if (c >= 48 ||c <= 57){
    return c - 48;
  }
  return -1;
}

/*
  Converts a string to lower-case, for CASE-INSENSITIVE
  purposes.
*/
char* lowCase(char *string){
  char *point = string;
  //WHY MUST IT BE CASE-INSENSITIVE T_T
  //I have to spam this function in many places now.
  while (string < point + strlen(point)){
    *string = tolower(*string);
    *string++;
  }
  return point;
}
