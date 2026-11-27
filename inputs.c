/* inputs.c - Passenger and booking records */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "shared.h"

// Record storage
static struct Passenger passengers[MAX_RECORDS];
static int passengerCount = 0;

static struct Booking bookings[MAX_RECORDS];
static int bookingCount = 0;
static int nextPNR = FIRST_PNR;

// Input helpers
static int   readLine(char buffer[], int size);
static int   isOnlySpaces(char text[]);
int   readInt(char prompt[], int min, int max);
float readFloat(char prompt[], float min, float max);
static void  readName(char prompt[], char name[]);
static void  readContact(char prompt[], char contact[]);
void  readFlightNo(char prompt[], char flightno[]);
static void  readSeat(char prompt[], int *row, int *column);

// Memory and record helpers
int seatIsBooked(char flightno[], int row, int column);

// Passenger functions
void registerPassenger(void);
void displayPassenger(int index);
void displayAllPassengers(void);
int  searchPassenger(int searchID);
void searchPassengerMenu(void);

// Booking functions
int  createBooking(void);
int  findBookingByPNR(int pnr);
void displayBooking(int index);
void searchBookingMenu(void);
int  cancelBooking(void);

// Accessors
struct Booking   *getBooking(int pnr);
struct Passenger *getPassenger(int id);
int               getBookingCount(void);
struct Booking   *getBookingAt(int index);

// Cleanup

// [ALI] flight and seat functions
int  flightExists(char flightno[]);
int  isSeatAvailable(char flightno[], int row, int column);
int  seatMatchesClass(char flightno[], int row, int column, int travelClass);

/* Input helpers */

static int readLine(char buffer[], int size) {
    int ch;

    if (fgets(buffer, size, stdin) == NULL) {
        printf("\nInput ended. Exiting.\n");
        exit(0);
    }

    if (strchr(buffer, '\n') == NULL) {
        ch = getchar();
        if (ch == '\n' || ch == EOF) {
            return 1;
        }
        while (ch != '\n' && ch != EOF) {
            ch = getchar();
        }
        return 0;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

static int isOnlySpaces(char text[]) {
    int i = 0;

    while (text[i] != '\0') {
        if (!isspace((unsigned char)text[i])) {
            return 0;
        }
        i = i + 1;
    }

    return 1;
}

int readInt(char prompt[], int min, int max) {
    char line[50];
    char *end;
    long value;

    while (1) {
        printf("%s", prompt);

        if (readLine(line, sizeof(line)) == 1) {
            value = strtol(line, &end, 10);

            if (end != line && isOnlySpaces(end) == 1 && value >= min && value <= max) {
                return (int)value;
            }
        }
        printf("Invalid input. Enter a whole number from %d to %d.\n", min, max);
    }
}

float readFloat(char prompt[], float min, float max) {
    char line[50];
    char *end;
    double value;

    while (1) {
        printf("%s", prompt);

        if (readLine(line, sizeof(line)) == 1) {
            value = strtod(line, &end);

            if (end != line && isOnlySpaces(end) == 1 && value >= min && value <= max) {
                return (float)value;
            }
        }
        printf("Invalid input. Enter a number from %.0f to %.0f.\n", min, max);
    }
}

static void readName(char prompt[], char name[]) {
    char line[100];
    int  i;
    int  fitted;

    while (1) {
        printf("%s", prompt);
        fitted = readLine(line, sizeof(line));

        i = (int)strlen(line);
        while (i > 0 && line[i - 1] == ' ') {
            line[i - 1] = '\0';
            i = i - 1;
        }

        if (fitted == 1 && isalpha((unsigned char)line[0]) && strlen(line) < NAME_LEN) {
            i = 0;
            while (line[i] != '\0' && (isalpha((unsigned char)line[i]) || line[i] == ' '
                    || line[i] == '.' || line[i] == '-' || line[i] == '\'')) {
                i = i + 1;
            }

            if (line[i] == '\0') {
                strcpy(name, line);
                return;
            }
        }
        printf("Invalid name. Start with a letter; letters, spaces, . - ' only; up to %d characters.\n", NAME_LEN - 1);
    }
}

static void readContact(char prompt[], char contact[]) {
    char line[100];
    int  i;
    int  fitted;

    while (1) {
        printf("%s", prompt);
        fitted = readLine(line, sizeof(line));

        i = 0;
        while (isdigit((unsigned char)line[i])) {
            i = i + 1;
        }

        if (fitted == 1 && line[i] == '\0' && i >= 10 && i < CONTACT_LEN) {
            strcpy(contact, line);
            return;
        }
        printf("Invalid contact. Digits only, 10 to %d digits, no spaces or dashes.\n", CONTACT_LEN - 1);
    }
}

void readFlightNo(char prompt[], char flightno[]) {
    char line[100];
    int  i;
    int  fitted;

    while (1) {
        printf("%s", prompt);
        fitted = readLine(line, sizeof(line));

        i = 0;
        while (isalnum((unsigned char)line[i])) {
            i = i + 1;
        }

        if (fitted == 1 && line[i] == '\0' && i >= 2 && i < FLIGHT_LEN) {
            i = 0;
            while (line[i] != '\0') {
                flightno[i] = (char)toupper((unsigned char)line[i]);
                i = i + 1;
            }
            flightno[i] = '\0';
            return;
        }
        printf("Invalid flight ID. Letters and digits only, 2 to %d characters (e.g. MA101).\n", FLIGHT_LEN - 1);
    }
}

static void readSeat(char prompt[], int *row, int *column) {
    char line[50];
    char *end;
    long number;

    while (1) {
        printf("%s", prompt);

        if (readLine(line, sizeof(line)) == 1) {
            number = strtol(line, &end, 10);

            if (end != line && number >= 1 && number <= 99 && isalpha((unsigned char)*end)
                    && isOnlySpaces(end + 1) == 1) {
                *row = (int)number - 1;
                *column = toupper((unsigned char)*end) - 'A';
                return;
            }
        }
        printf("Invalid seat. Type a row number then a letter, like 12A.\n");
    }
}

int seatIsBooked(char flightno[], int row, int column) {
    int x = 0;

    while (x < bookingCount) {
        if (bookings[x].date == departureDate && bookings[x].active == 1 && bookings[x].row == row && bookings[x].column == column
                && strcmp(bookings[x].flightno, flightno) == 0) {
            return 1;
        }
        x = x + 1;
    }

    return 0;
}

/* Passenger functions */

void registerPassenger(void) {
    struct Passenger p;

    p.id = readInt("Enter Passenger ID (1 to 999999): ", 1, 999999);

    while (searchPassenger(p.id) != -1) {
        printf("That ID is already registered.\n");
        p.id = readInt("Enter a different ID (1 to 999999): ", 1, 999999);
    }

    readName("Enter Passenger Name: ", p.name);
    p.age = readInt("Enter Passenger Age (1 to 120): ", 1, 120);
    readContact("Enter Passenger Contact (digits only): ", p.contact);

    if (passengerCount == MAX_RECORDS) {
        puts("Passenger list is full.");
        return;
    }

    passengers[passengerCount] = p;
    passengerCount = passengerCount + 1;

    saveData();
    printf("\nPassenger registered.\n");
}

void displayPassenger(int index) {
    printf("ID:      %d\n", passengers[index].id);
    printf("Name:    %s\n", passengers[index].name);
    printf("Age:     %d\n", passengers[index].age);
    printf("Contact: %s\n", passengers[index].contact);
}

void displayAllPassengers(void) {
    int i;

    if (passengerCount == 0) {
        printf("No passengers registered yet.\n");
        return;
    }

    for (i = 0; i < passengerCount; i++) {
        printf("\n--- Passenger %d ---\n", i + 1);
        displayPassenger(i);
    }
}

// Returns the index of the passenger with this ID, or -1
int searchPassenger(int searchID) {
    int x = 0;

    while (x < passengerCount) {
        if (passengers[x].id == searchID) {
            return x;
        }
        x = x + 1;
    }

    return -1;
}

void searchPassengerMenu(void) {
    int searchID;
    int position;

    searchID = readInt("Enter the Passenger ID to search: ", 1, 999999);
    position = searchPassenger(searchID);

    if (position != -1) {
        printf("\n--- Passenger Found ---\n");
        displayPassenger(position);
    } else {
        printf("Passenger not found.\n");
    }
}

/* Booking functions */

// Returns the new PNR, or 0 if the booking failed
int createBooking(void) {
    int   passengerID;
    char  flightno[FLIGHT_LEN];
    int   row;
    int   column;
    int   travelClass;
    float fare;
    int   slot;

    passengerID = readInt("Enter Passenger ID: ", 1, 999999);

    if (searchPassenger(passengerID) == -1) {
        printf("Passenger not found. Booking not created.\n");
        return 0;
    }

    readFlightNo("Enter Flight ID (e.g. MA101): ", flightno);

    // [ALI] flight lookup
    if (flightExists(flightno) == 0) {
        printf("Flight not found. Booking not created.\n");
        return 0;
    }

    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].active && bookings[i].passengerID == passengerID &&
            bookings[i].date == departureDate && strcmp(bookings[i].flightno, flightno) == 0) {
            puts("Passenger already booked on this departure.");
            return 0;
        }
    }
    readSeat("Enter Seat (e.g. 12A): ", &row, &column);

    // [ALI] seat availability
    if (isSeatAvailable(flightno, row, column) == 0) {
        printf("Seat is not available. Booking not created.\n");
        return 0;
    }

    travelClass = readInt("Enter Class (1 = Economy, 2 = Business): ", 1, 2);

    // [ALI] seat-class rule
    if (seatMatchesClass(flightno, row, column, travelClass) == 0) {
        printf("That seat is not in the chosen class. Booking not created.\n");
        return 0;
    }

    // [ZAIN] fare calculation
    fare = ticketFare(flightno, travelClass, getPassenger(passengerID)->age);
    printf("Fare: Rs. %.2f\n", fare);
    if (readInt("Confirm (1 yes, 0 no): ", 0, 1) == 0) return 0;

    if (bookingCount == MAX_RECORDS) {
        puts("Booking list is full.");
        return 0;
    }

    slot = bookingCount;
    bookings[slot].pnr = nextPNR;
    bookings[slot].passengerID = passengerID;
    strcpy(bookings[slot].flightno, flightno);
    bookings[slot].date = departureDate;
    bookings[slot].row = row;
    bookings[slot].column = column;
    bookings[slot].travelClass = travelClass;
    bookings[slot].fare = fare;
    bookings[slot].active = 1;
    bookings[slot].checkedIn = 0;

    bookingCount = bookingCount + 1;
    nextPNR = nextPNR + 1;

    saveData();
    printf("\nBooking successful! Your PNR is %d\n", bookings[slot].pnr);
    return bookings[slot].pnr;
}

// Returns the index of the booking with this PNR, or -1
int findBookingByPNR(int pnr) {
    int x = 0;

    while (x < bookingCount) {
        if (bookings[x].pnr == pnr) {
            return x;
        }
        x = x + 1;
    }

    return -1;
}

void displayBooking(int index) {
    int who;

    printf("PNR:        %d\n", bookings[index].pnr);

    who = searchPassenger(bookings[index].passengerID);
    if (who != -1) {
        printf("Passenger:  %d (%s)\n", bookings[index].passengerID, passengers[who].name);
    } else {
        printf("Passenger:  %d\n", bookings[index].passengerID);
    }

    printf("Flight:     %s\n", bookings[index].flightno);
    printf("Date:       %d\n", bookings[index].date);
    printf("Seat:       %d%c\n", bookings[index].row + 1, 'A' + bookings[index].column);

    if (bookings[index].travelClass == 1) {
        printf("Class:      Economy\n");
    } else {
        printf("Class:      Business\n");
    }

    printf("Fare:       %.2f\n", bookings[index].fare);

    if (bookings[index].active == 1) {
        printf("Status:     Active\n");
    } else {
        printf("Status:     Cancelled\n");
    }

    if (bookings[index].checkedIn == 1) {
        printf("Checked in: Yes\n");
    } else {
        printf("Checked in: No\n");
    }
}

void searchBookingMenu(void) {
    int pnr;
    int position;

    pnr = readInt("Enter PNR: ", 1, 9999999);
    position = findBookingByPNR(pnr);

    if (position != -1) {
        printf("\n--- Booking Found ---\n");
        displayBooking(position);
    } else {
        printf("Booking not found.\n");
    }
}

// Returns 1 if the booking was cancelled, 0 otherwise
int cancelBooking(void) {
    int pnr;
    int position;

    pnr = readInt("Enter PNR to cancel: ", 1, 9999999);
    position = findBookingByPNR(pnr);

    if (position == -1) {
        printf("Booking not found.\n");
        return 0;
    }

    if (bookings[position].active == 0) {
        printf("This booking is already cancelled.\n");
        return 0;
    }

    if (bookings[position].checkedIn) {
        puts("Already checked in. Cannot cancel.");
        return 0;
    }
    bookings[position].active = 0;

    saveData();
    printf("\nBooking cancelled. The seat is free again.\n");
    return 1;
}

/* Accessors (read access for other modules, return NULL if not found) */

struct Booking *getBooking(int pnr) {
    int position = findBookingByPNR(pnr);

    if (position == -1) {
        return NULL;
    }
    return &bookings[position];
}

struct Passenger *getPassenger(int id) {
    int position = searchPassenger(id);

    if (position == -1) {
        return NULL;
    }
    return &passengers[position];
}

int getBookingCount(void) {
    return bookingCount;
}

struct Booking *getBookingAt(int index) {
    if (index < 0 || index >= bookingCount) {
        return NULL;
    }
    return &bookings[index];
}

void writeRecords(FILE *file) {
    fprintf(file, "Passengers %d\n", passengerCount);
    for (int i = 0; i < passengerCount; i++) {
        struct Passenger p = passengers[i];
        fprintf(file, "%d %d %s |%s\n", p.id, p.age, p.contact, p.name);
    }
    fprintf(file, "Bookings %d\n", bookingCount);
    for (int i = 0; i < bookingCount; i++) {
        struct Booking b = bookings[i];
        fprintf(file, "%d %d %s %d %d %d %d %.9g %d %d\n",
                b.pnr, b.passengerID, b.flightno, b.date, b.row, b.column,
                b.travelClass, b.fare, b.active, b.checkedIn);
    }
}

int loadRecords(FILE *file) {
    int count;
    if (fscanf(file, " Passengers %8d", &count) != 1 || count < 0 || count > MAX_RECORDS) return 0;
    for (int i = 0; i < count; i++) {
        struct Passenger p;
        if (fscanf(file, " %8d %3d %14s |%49[^\r\n]", &p.id, &p.age, p.contact, p.name) != 4 ||
            p.id < 1 || p.id > 999999 || p.age < 1 || p.age > 120 || searchPassenger(p.id) >= 0) return 0;
        passengers[passengerCount++] = p;
    }
    if (fscanf(file, " Bookings %8d", &count) != 1 || count < 0 || count > MAX_RECORDS) return 0;
    for (int i = 0; i < count; i++) {
        struct Booking b;
        if (fscanf(file, " %8d %8d %9s %8d %3d %2d %2d %f %2d %2d",
                   &b.pnr, &b.passengerID, b.flightno, &b.date, &b.row, &b.column,
                   &b.travelClass, &b.fare, &b.active, &b.checkedIn) != 10) return 0;
        int f = flightIndex(b.flightno);
        if (b.pnr != nextPNR || !getPassenger(b.passengerID) || f < 0 ||
            b.row < 0 || b.row >= flights[f].rows || b.column < 0 || b.column > 5 ||
            b.travelClass < 1 || b.travelClass > 2 || !(b.fare >= 0 && b.fare <= 1000000) ||
            b.active < 0 || b.active > 1 || b.checkedIn < 0 || b.checkedIn > b.active) return 0;
        bookings[bookingCount++] = b;
        nextPNR++;
    }
    return 1;
}
