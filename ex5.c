/***********
 ID:
 NAME:
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

char *getString();
int getInt();

int validLength(char *s);
int countShows();

void shrinkDB();
void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void addShow();
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

// reading a string from input
char *getString(){
    char buffer[1024];
    if (fgets(buffer,sizeof(buffer),stdin)==NULL){
        return NULL;
    }
    // to remove newline
    buffer[strcspn(buffer,"\n")]='\0';
    char *str=(char*)malloc(strlen(buffer)+1);
    strcpy(str,buffer);
    return str;
}

// to read an integer
int getInt(){
    int num;
    scanf("%d",&num);
    getchar();
    return num;
}

// to validate the length of format XX:XX:XX (0-99:0-59:0-59)
int validLength(char*s){
    if(strlen(s)!=8) return 0;
    if(s[2]!=':'||s[5]!=':') return 0;

    // checking all digits
    for(int i=0;i<8;i++){
        if(i==2||i==5) continue;
        if(s[i]<'0'||s[i]>'9') return 0;
    }

    // parsing hours, minutes, seconds
    int hours=(s[0]-'0')*10+(s[1]-'0');
    int minutes=(s[3]-'0')*10+(s[4]-'0');
    int seconds=(s[6]-'0')*10+(s[7]-'0');

    // validating ranges
    if(hours<0||hours>99) return 0;
    if(minutes<0||minutes>59) return 0;
    if(seconds<0||seconds>59) return 0;
    return 1;
}
// to count the number of shows inside database
int countShows(){
    int count=0;
    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]!=NULL){
                count++;
            }
        }
    }
    return count;
}
// Expand database to (n+1)x(n+1)
void expandDB(){
    int newSize=dbSize+1;
    TVShow ***newDB=(TVShow ***)malloc(newSize*sizeof(TVShow**));
    for(int i=0;i<newSize;i++){
        newDB[i]=(TVShow **)malloc(newSize*sizeof(TVShow*));
        for(int j=0;j<newSize;j++){
            newDB[i][j]=NULL;
        }
    }

    // Copy linearly to maintain order
    int idx=0;
    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]!=NULL){
                newDB[idx/newSize][idx%newSize]=database[i][j];
                idx++;
            }
        }
    }

    if(database!=NULL){
        for(int i=0;i<dbSize;i++){
            free(database[i]);
        }
        free(database);
    }
    database=newDB;
    dbSize=newSize;
}

// to shrink database to (n-1)x(n-1)
void shrinkDB(){
    int newSize=dbSize-1;
    if(newSize==0){
        for(int i=0;i<dbSize;i++){
            free(database[i]);
        }
        free(database);
        database=NULL;
        dbSize=0;
        return;
    }
    TVShow ***newDB=(TVShow***)malloc(newSize*sizeof(TVShow**));
    for(int i=0;i<newSize;i++){
        newDB[i]=(TVShow**)malloc(newSize*sizeof(TVShow*));
        for(int j=0;j<newSize;j++){
            newDB[i][j]=NULL;
        }
    }

    // Copy linearly to maintain order
    int idx = 0;
    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]!=NULL){
                newDB[idx/newSize][idx%newSize] = database[i][j];
                idx++;
            }
        }
    }

    for(int i=0;i<dbSize;i++){
        free(database[i]);
    }
    free(database);
    database=newDB;
    dbSize=newSize;
}

// Free an episode
void freeEpisode(Episode *e){
    if(e==NULL) return;
    free(e->name);
    free(e->length);
    free(e);
}

// Free a season and all its episodes
void freeSeason(Season *s){
    if (s==NULL) return;
    Episode *ep = s->episodes;
    while (ep != NULL) {
        Episode *next=ep->next;
        freeEpisode(ep);
        ep=next;
    }

    free(s->name);
    free(s);
}

// to free a show and all its seasons
void freeShow(TVShow *show){
    if (show==NULL) return;

    Season *season=show->seasons;
    while (season!=NULL){
        Season *next=season->next;
        freeSeason(season);
        season=next;
    }

    free(show->name);
    free(show);
}

// to free all memory
void freeAll(){
    if(database==NULL) return;
    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]!=NULL){
                freeShow(database[i][j]);
            }
        }
        free(database[i]);
    }
    free(database);
    database=NULL;
    dbSize=0;
}
// to find a show by name
TVShow *findShow(char *name){
    if(database==NULL) return NULL;

    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]!=NULL&&strcmp(database[i][j]->name,name)==0){
                return database[i][j];
            }
        }
    }
    return NULL;
}

// to find a season in a show
Season *findSeason(TVShow *show,char *name){
    if(show==NULL) return NULL;

    Season *season=show->seasons;
    while(season!=NULL) {
        if(strcmp(season->name,name)==0){
            return season;
        }
        season=season->next;
    }
    return NULL;
}
// to find an episode in a season
Episode *findEpisode(Season *season,char *name){
    if(season==NULL) return NULL;

    Episode *ep=season->episodes;
    while(ep!=NULL) {
        if(strcmp(ep->name,name)==0){
            return ep;
        }
        ep=ep->next;
    }
    return NULL;
}
// to add a TV show
void addShow(){
    printf("Enter the name of the show:\n");
    char *name=getString();

    if(findShow(name)!=NULL){
        printf("Show already exists.\n");
        free(name);
        return;
    }

    TVShow *show=(TVShow *)malloc(sizeof(TVShow));
    show->name=name;
    show->seasons=NULL;

    int numShows=countShows();
    if(dbSize==0||numShows>=dbSize*dbSize){
        expandDB();
    }

    TVShow *toInsert=show;
    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]==NULL){
                database[i][j]=toInsert;
                return;
            } else if (strcmp(toInsert->name,database[i][j]->name)<0){
                TVShow *temp=database[i][j];
                database[i][j]=toInsert;
                toInsert=temp;
            }
        }
    }
}
// adding a season
void addSeason(){
    printf("Enter the name of the show:\n");
    char *showName=getString();
    TVShow *show=findShow(showName);
    if(show==NULL){
        printf("Show not found.\n");
        free(showName);
        return;
    }
    free(showName);
    printf("Enter the name of the season:\n");
    char *seasonName=getString();
    // checking if there is a season already exists
    if (findSeason(show, seasonName) != NULL) {
        printf("Season already exists.\n");
        free(seasonName);
        return;
    }
    printf("Enter the position:\n");
    int position=getInt();

    // creating a new season
    Season *newSeason=(Season *)malloc(sizeof(Season));
    newSeason->name=seasonName;
    newSeason->episodes=NULL;
    newSeason->next=NULL;

    // inserting at position
    if(position<=0||show->seasons==NULL){
        newSeason->next=show->seasons;
        show->seasons=newSeason;
    } else{
        Season *current=show->seasons;
        int pos=0;
        while(current->next!=NULL&&pos<position-1){
            current=current->next;
            pos++;
        }
        newSeason->next=current->next;
        current->next=newSeason;
    }
}
// adding an episode
void addEpisode(){
    printf("Enter the name of the show:\n");
    char *showName=getString();
    TVShow *show=findShow(showName);
    if(show==NULL){
        printf("Show not found.\n");
        free(showName);
        return;
    }
    free(showName);

    printf("Enter the name of the season:\n");
    char *seasonName=getString();

    Season *season=findSeason(show,seasonName);
    if (season==NULL) {
        printf("Season not found.\n");
        free(seasonName);
        return;
    }
    free(seasonName);

    printf("Enter the name of the episode:\n");
    char *episodeName=getString();

    // checking if there is an episode already exists
    if(findEpisode(season,episodeName)!=NULL){
        printf("Episode already exists.\n");
        free(episodeName);
        return;
    }
    printf("Enter the length (xx:xx:xx):\n");
    char *length=getString();
    // to validate length
    while(!validLength(length)){
        printf("Invalid length, enter again:\n");
        free(length);
        length=getString();
    }

    printf("Enter the position:\n");
    int position=getInt();

    // creating a new episode
    Episode *newEpisode=(Episode *)malloc(sizeof(Episode));
    newEpisode->name=episodeName;
    newEpisode->length=length;
    newEpisode->next=NULL;

    // inserting at position
    if(position<=0||season->episodes==NULL){
        newEpisode->next=season->episodes;
        season->episodes=newEpisode;
    } else{
        Episode *current=season->episodes;
        int pos=0;
        while(current->next!=NULL&&pos<position-1){
            current=current->next;
            pos++;
        }
        newEpisode->next=current->next;
        current->next=newEpisode;
    }
}
// to delete a TV show
void deleteShow(){
    printf("Enter the name of the show:\n");
    char *name=getString();

    // finding the show
    int found=0;
    int foundI=-1,foundJ=-1;
    for(int i=0;i<dbSize&&!found;i++){
        for(int j=0;j<dbSize&&!found;j++){
            if(database[i][j]!=NULL&&strcmp(database[i][j]->name,name)==0){
                found=1;
                foundI=i;
                foundJ=j;
            }
        }
    }
    if(!found){
        printf("Show not found.\n");
        free(name);
        return;
    }
    // free the show
    freeShow(database[foundI][foundJ]);
    database[foundI][foundJ] = NULL;

    // shiftting the remaining shows backwards
    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]==NULL){
                // finding the next non-null
                int nextI=i,nextJ=j+1;
                if(nextJ>=dbSize){
                    nextI++;
                    nextJ=0;
                }
                while(nextI<dbSize){
                    while(nextJ<dbSize){
                        if(database[nextI][nextJ]!=NULL){
                            database[i][j]=database[nextI][nextJ];
                            database[nextI][nextJ]=NULL;
                            goto next_shift;
                        }
                        nextJ++;
                    }
                    nextI++;
                    nextJ=0;
                }
                goto done_shifting;
                next_shift:;
            }
        }
    }
    done_shifting:
    // checking if we can shrink
    int numShows=countShows();
    if(dbSize>1&&numShows<=(dbSize-1)*(dbSize-1)){
        shrinkDB();
    }
    free(name);
}

// to delete a season
void deleteSeason(){
    printf("Enter the name of the show:\n");
    char *showName=getString();

    TVShow *show=findShow(showName);
    if(show==NULL){
        printf("Show not found.\n");
        free(showName);
        return;
    }
    free(showName);

    printf("Enter the name of the season:\n");
    char *seasonName=getString();

    // find and remove season
    Season *current=show->seasons;
    Season *prev=NULL;

    while(current!=NULL){
        if(strcmp(current->name,seasonName)==0){
            if(prev==NULL){
                show->seasons=current->next;
            } else{
                prev->next=current->next;
            }
            freeSeason(current);
            free(seasonName);
            return;
        }
        prev=current;
        current=current->next;
    }

    printf("Season not found.\n");
    free(seasonName);
}

// to delete an episode
void deleteEpisode(){
    printf("Enter the name of the show:\n");
    char *showName=getString();

    TVShow *show=findShow(showName);
    if (show==NULL){
        printf("Show not found.\n");
        free(showName);
        return;
    }
    free(showName);

    printf("Enter the name of the season:\n");
    char *seasonName=getString();

    Season *season = findSeason(show, seasonName);
    if(season==NULL){
        printf("Season not found.\n");
        free(seasonName);
        return;
    }
    free(seasonName);

    printf("Enter the name of the episode:\n");
    char *episodeName=getString();

    // find and remove episode
    Episode *current=season->episodes;
    Episode *prev=NULL;

    while(current!=NULL){
        if(strcmp(current->name,episodeName)==0){
            if(prev==NULL){
                season->episodes=current->next;
            } else{
                prev->next=current->next;
            }
            freeEpisode(current);
            free(episodeName);
            return;
        }
        prev=current;
        current=current->next;
    }

    printf("Episode not found.\n");
    free(episodeName);
}

// to print a TV show
void printShow(){
    printf("Enter the name of the show:\n");
    char *name=getString();

    TVShow *show=findShow(name);
    if(show==NULL) {
        printf("Show not found.\n");
        free(name);
        return;
    }

    printf("Name: %s\n", show->name);
    printf("Seasons:\n");

    Season *season=show->seasons;
    int seasonNum=0;
    while(season!=NULL){
        printf("\tSeason %d: %s\n", seasonNum, season->name);

        Episode *ep=season->episodes;
        int epNum=0;
        while(ep!=NULL){
            printf("\t\tEpisode %d: %s (%s)\n", epNum, ep->name, ep->length);
            ep=ep->next;
            epNum++;
        }

        season=season->next;
        seasonNum++;
    }

    free(name);
}

// to print an episode
void printEpisode(){
    printf("Enter the name of the show:\n");
    char *showName=getString();

    TVShow *show=findShow(showName);
    if(show==NULL){
        printf("Show not found.\n");
        free(showName);
        return;
    }
    free(showName);

    printf("Enter the name of the season:\n");
    char *seasonName=getString();

    Season *season=findSeason(show,seasonName);
    if(season==NULL){
        printf("Season not found.\n");
        free(seasonName);
        return;
    }
    free(seasonName);

    printf("Enter the name of the episode:\n");
    char *episodeName=getString();

    Episode *ep=findEpisode(season,episodeName);
    if(ep==NULL) {
        printf("Episode not found.\n");
        free(episodeName);
        return;
    }

    printf("Name: %s\n",ep->name);
    printf("Length: %s\n",ep->length);

    free(episodeName);
}

// to print the array
void printArray(){
    if(database==NULL||countShows()==0){
        return;
    }

    for(int i=0;i<dbSize;i++){
        for(int j=0;j<dbSize;j++){
            if(database[i][j]!=NULL){
                printf("[%s] ",database[i][j]->name);
            } else{
                printf("[NULL] ");
            }
        }
        printf("\n");
    }
}

void addMenu(){
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}
