#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME "Pabualan Airlines"
#define BORDER "---------------------------------\n"

char available[11][7];


int mainMenu();
void registerPassenger();
int reserveSeat();
void registerSeat();

void initSeats();
void savePassengerFile(char*, int, char*);
void showPassengerDetails();
void showSeats();
void saveRegisteredSeat(int, int, char);
void saveReservedSeat(int, char);
void removeReservedSeat();
void showPassengersList();

int askInput();
void menuInput(int);
int exists(char*);



int main()
{ 
    if(!exists("seat_arrangement.txt")) initSeats();
    mainMenu();
}


int mainMenu()
{
    printf("%sWelcome to %s\n%s", BORDER, NAME, BORDER);
    
    int registered;
    do {
        if(!exists("passenger_details.txt")) {
            registered = 0;
            printf("[1] Register profile info\n");
        }
        else {
            registered = 1;
            printf("\nThis is your profile.\n");
            showPassengerDetails();
            printf("[1] Register new profile\n");
        }
        printf("[2] Reserve a seat\n[3] See list of reserved passengers\n[4] End program\n");
        menuInput(registered);

        printf("Back to menu? (y/n) ");
    } while(askInput());

    printf("Thank you for using our service!");
    exit(0);
}

void registerPassenger()
{
    char name[100]; int age; char address[100];
    do {
        printf("\nEnter your name: ");
        fflush(stdin);
        gets(name);
        printf("Enter your age: ");
        fflush(stdin);
        scanf("%d", &age);
        printf("Enter your city address: ");
        fflush(stdin);
        gets(address);
        savePassengerFile(name, age, address);

        showPassengerDetails();
        printf("Is this correct? (y/n) ");
    } while(!askInput());
}
int reserveSeat()
{
    printf("\n\nThese are the seats. The ones marked in X are currently reserved.\n");
    showSeats();

    FILE *passengerDetails = fopen("passenger_details.txt", "r");
    
    char line[100]; int reserved = 0;
    for(int i = 0; i < 3; i++)
        fgets(line, sizeof line, passengerDetails);
    if(fgets(line, sizeof line, passengerDetails) == NULL) {
        printf("You currently have no reservations. Would you like to book one? (y/n) ");
    }
    else {
        reserved = 1;
        printf("%s\n\nWould you like to change your reserved seat? (y/n) ", line);
    }

    fclose(passengerDetails);
    if(!askInput()) return 0;
    if(reserved) removeReservedSeat();

    registerSeat();
}
void registerSeat() {
    char columns[] = "ABCDEF";

    int row; char col; int c;
    do {
        while(1) {
            printf("\nChoose row (1-10) ");
            fflush(stdin);
            while(scanf("%d", &row) == 1) {
                if(row >= 1 && row <= 10) break;
                printf("Try again. Choose row (1-10) ");
                fflush(stdin);
            }
            printf("Choose column (A-F) ");
            fflush(stdin);
            while(scanf("%c", &col) == 1) {
                int found = 0;
                for(c = 0; c < 6; c++) {
                    if(columns[c] == toupper(col)) {
                        found = 1;
                        break;
                    }
                }
                if(found) break;
                printf("Try again. Choose column (A-F) ");
                fflush(stdin);
            }
            if(available[row][c+1] == ' ') break;
            printf("\nThe seat in row %d column %c is already reserved. Try again\n", row, toupper(col));
        }
        printf("\nYou chose row %d column %c. Is that correct? (y/n) ", row, toupper(col));
    } while(!askInput());

    saveRegisteredSeat(row, c+1, toupper(col));
    reserveSeat();
}

void initSeats()
{
    remove("seat_arrangement");
    char columns[] = "ABCDEF";

    FILE *seats = fopen("seat_arrangement.txt", "w");

    for(int r = 1; r <= 10; r++) {
        for(int c = 0; c < 6; c++)
            fprintf(seats, "%02d%c\n", r, columns[c]);
    }

    fclose(seats);
}
void savePassengerFile(char *name, int age, char *address)
{
    FILE *passengerDetails = fopen("passenger_details.txt", "w");
    
    fprintf(passengerDetails, "Name: %s\nAge: %d\nCity Address: %s", name, age, address);

    fclose(passengerDetails);
}
void showPassengerDetails()
{
    printf(BORDER);
    char details[100];
    FILE *passengerDetails = fopen("passenger_details.txt", "r");

    while(fgets(details, sizeof details, passengerDetails))
        printf("%s", details);
    printf("\n");

    fclose(passengerDetails);
    printf(BORDER);
}
void showSeats()
{
    printf(BORDER);
    FILE *seats = fopen("seat_arrangement.txt", "r");

    char line[255];

    printf("    [A][B][C]   [D][E][F]\n");
    for(int i = 1; i <= 10; i++) {
        for(int s = 1; s <= 6; s++) {
            fgets(line, sizeof line, seats);
            if(strlen(line) > 8)
                available[i][s] = 'x';
            else available[i][s] = ' ';
        }
        printf("[%02d][%c][%c][%c]   [%c][%c][%c]\n", i, available[i][1], available[i][2], available[i][3], available[i][4], available[i][5], available[i][6]);
    }

    fclose(seats);
    printf(BORDER);
}
void saveRegisteredSeat(int row, int col, char column)
{
    FILE *seats = fopen("seat_arrangement.txt", "r");
    FILE *temp = fopen("temp__seat_arrangement.txt", "w");

    char line[255];
    int pos = (row - 1) * 6 + col - 1;
    while (fgets(line, sizeof line, seats) != NULL) {
        if(!pos) {
            line[3] = ' ';
            FILE *passengerDetails = fopen("passenger_details.txt", "r");

            char details[100];
            for(int i = 0; i < 3; i++) {
                fgets(details, sizeof details, passengerDetails);
                if(details[strlen(details) - 1] == '\n') details[strlen(details) - 1] = ' ';
                strcat(line, "/ ");
                strcat(line, details);
            }
            strcat(line, "\n");

            fclose(passengerDetails);
        }
        fputs(line, temp);
        pos--;
    }

    fclose(seats);
    fclose(temp);

    remove("seat_arrangement.txt");
    rename("temp__seat_arrangement.txt", "seat_arrangement.txt");
    remove("temp__seat_arrangement.txt");

    saveReservedSeat(row, column);
}
void saveReservedSeat(int row, char col)
{
    FILE *passenger_details = fopen("passenger_details.txt", "r");
    FILE *temp = fopen("temp__passenger_details.txt", "w");

    char line[100];
    for(int i = 0; i < 3; i++) {
        fgets(line, sizeof line, passenger_details);
        fputs(line, temp);
    }
    fprintf(temp, "\nReserved Seat: %02d%c", row, col);

    fclose(passenger_details);
    fclose(temp);

    remove("passenger_details.txt");
    rename("temp__passenger_details.txt", "passenger_details.txt");
    remove("temp__passenger_details.txt");
}
void removeReservedSeat()
{
    char columns[] = "ABCDEF";
    int row; int col;

    FILE *passenger_details = fopen("passenger_details.txt", "r");
    FILE *temp = fopen("temp__passenger_details.txt", "w");

    char line[255];
    for(int i = 0; i < 3; i++) {
        fgets(line, sizeof line, passenger_details);
        if(i == 2) line[strlen(line)-1] = ' ';
        fputs(line, temp);
    }

    fgets(line, sizeof line, passenger_details);
    char r[2]; r[0] = line[15]; r[1] = line[16];
    char c = line[17];

    row = atoi(r);
    for(col = 0; col < 6; col++)
        if(columns[col] == c) break;

    fclose(passenger_details);
    fclose(temp);

    remove("passenger_details.txt");
    rename("temp__passenger_details.txt", "passenger_details.txt");
    remove("temp__passenger_details.txt");


    FILE *seats = fopen("seat_arrangement.txt", "r");
    temp = fopen("temp__seat_arrangement.txt", "w");

    int pos = (row - 1) * 6 + col;
    while (fgets(line, sizeof line, seats) != NULL) {
        if(!pos) {
            fprintf(temp, "%02d%c\n", row, c);
        }
        else fputs(line, temp);
        pos--;
    }

    fclose(seats);
    fclose(temp);

    remove("seat_arrangement.txt");
    rename("temp__seat_arrangement.txt", "seat_arrangement.txt");
    remove("temp__seat_arrangement.txt");
}
void showPassengersList()
{
    printf("\n\nList of passengers\n%s", BORDER);
    FILE *seats = fopen("seat_arrangement.txt", "r");

    int noReservations = 0; char line[255];
    while(fgets(line, sizeof line, seats) != NULL) {
        if(strlen(line) > 8) {
            noReservations++;
            printf(line);
        }
    }
    
    if(!noReservations) printf("There are still no reservations.");
    printf("\n");

    fclose(seats);
}

int askInput()
{
    fflush(stdin);

    char input;
    while(scanf("%c", &input) == 1) {
        if(input == 'y' || input == 'Y') return 1;
        else if(input == 'n' || input == 'N') return 0;
        fflush(stdin);
        printf("Enter with (y/n) ");
    }
}
void menuInput(int profileCheck)
{
    fflush(stdin);

    int retry = 0; int input;
    while(scanf("%d", &input) == 1) {
        switch(input) {
            case 1:
                registerPassenger();
                break;
            case 2:
                if(profileCheck)
                    reserveSeat();
                else
                    printf("\nYou need to register first to reserve a seat!\n");
                break;
            case 3:
                showPassengersList();
                break;
            case 4:
                printf("\nThank you for using our service!");
                exit(0);
            default:
                retry = 1;
                printf("\nTry again. Enter the option number. ");
        }
        if(!retry) break;
        fflush(stdin);
    }
}
int exists(char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}