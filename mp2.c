#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SMS 200

/*TODO:
 -Make query non case sensitive
 -Free the nodes!
 -The sorting
*/

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
  struct Record *ref; //if this is not NULL, then the record is a reference to another record
} Record;

int charToInt(char);
Record* parseInputFile(char*, Record*);
Record* parseUserInput(Record*);
Record* checkIfUniqueInList(Record *,int);
Record* findSongList(Record*, char*, char*); //this returns a list based on the query char*
Record* userSelectFromList(Record*);
Record* createNodeRef(Record*);
Record* sortList(Record *);
void printList(Record *);
void updateSong(Record*);
void writeToFile(Record *, char *);
void copyNodes(Record*, Record*);
void replaceField(Record*, Record*, char*, char*);

void main(){
  Record *list = NULL;
  Record *tail = NULL;
  char input[1200];
  char path[200] = "m.input";
  FILE *fp = fopen(path, "r+");
  //load file to a linked list
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
    printList(list);
    printf("Options:\n1-Add song\n2-Update song\n3-List by Query\n4-Exit");
    scanf("%d", &option);
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
      list = sortList(list);
    } else if (option == 4){
      printf("Goodbye. Saving list to \\%s", path);
      writeToFile(list, path);
    }
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
    if (checkIfUniqueInList(list, id) == NULL){ //no similar id found
      add -> id = id;
      break;
    } else {
      printf("Error: ID already exists.\n");
    }
  }
  getchar();
  printf("Title: ");
  fgets(&(add -> title[0]), SMS, stdin);
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
    select = checkIfUniqueInList(list, input);
    if (select == NULL) printf("Error: id not found.\n");
  }
  return select;
}

Record* findSongList(Record* list, char* query, char* input){
  Record* newlist = NULL;
  Record* current = NULL;
  while (list != NULL){
    if (strcmp(query, "Title") == 0){
      if (strcmp(list -> title, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "Artist") == 0){
      if (strcmp(list -> artist, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } else if (strcmp(query, "Composer") == 0){
      if (strcmp(list -> composer, input) == 0){
        if (newlist == NULL){
          newlist = createNodeRef(list);
          current = newlist;
        } else {
          current -> next = createNodeRef(list);
          current = current -> next;
        }
      }
    } if (strcmp(query, "Album") == 0){
      if (strcmp(list -> album, input) == 0){
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

Record* sortList(Record* list){
  char query[30];
  if (strcmp(query, "id") == 0){
  } else if (strcmp(query, "title") == 0){
    list = titleSort(list);
  } else if (strcmp(query, "artist") == 0){
    list = artistSort(list);
  } else if (strcmp(query, "composer") == 0){
    list = normalSort(list, query);
  } else if (strcmp(query, "album") == 0){
    list = normalSort(list, query);
  } else if (strcmp(query, "genre") == 0){
    list = normalSort(list, query);
  } else if (strcmp(query, "rating") == 0){
    list = normalSort(list, query);
  } else if (strcmp(query, "title") == 0){
    list = normalSort(list, query);
  } else  {
    printf("Query invalid. Error unhandled. Nothing changed.\n");
  }
}

Record* titleSort(Record* list){
  
}

Record* artistSort(Record* list){

}

Record* normalSort(Record* list, char *query){

}

//updates the selected song in list
void updateSong(Record * list){
  char input[200];
  char query[30];
  Record* searchlist;
  Record* update = NULL;
  printf("Input title of song to be updated: ");
  fgets(input, 200, stdin);
  input[strcspn(input, "\n")] = 0;
  searchlist = findSongList(list, "Title", input);
  if (searchlist -> next != NULL){ //if next is not null, there is more than one result
    printf("Multiple song titles detected.\n");
    update = userSelectFromList(searchlist);
  } else {
    update = searchlist;
  }
  printList(update);
  printf("What did you want to update?");
  fgets(query, 30, stdin);
  query[strcspn(query, "\n")] = 0;
  printf("What did you want to replace it with?");
  fgets(input, 200, stdin);
  input[strcspn(input, "\n")] = 0;
  replaceField(list, update, query, input);
}

void replaceField(Record* list, Record *update, char *query, char *input){
  //remember that records passed here are nodes with
  //non-NULL ref values
  if (strcmp(query, "id") == 0){
    int digits;
    snprintf(input, SMS, "%d", digits);
    if ((checkIfUniqueInList(list, digits)) == NULL){
      snprintf(input, SMS, "%d", (update -> ref) -> id);
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
    snprintf(input, SMS, "%d", (update -> ref) -> rating);
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

void printList(Record* list){
  printf("ID\tTITLE\tARTIST\tCOMPOSER\tALBUM\tGENRE\tRATING\tREMARKS\n");
  while (list != NULL){
    printf("%d\t", list -> id);
    printf("%s\t", list -> title);
    printf("%s\t", list -> artist);
    printf("%s\t", list -> composer);
    printf("%s\t", list -> album);
    printf("%s\t", list -> genre);
    printf("%d\t", list -> rating);
    printf("%s", list -> remarks);
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
