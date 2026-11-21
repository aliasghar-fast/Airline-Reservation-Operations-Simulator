/* inputs.c - Passenger and booking records */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LEN 50
#define CONTACT_LEN 15
#define FLIGHT_LEN 10
#define INITIAL_CAPACITY 4
#define FIRST_PNR 1001

// [SHARED] move to shared.h
struct Passenger {
    int  id;
    char name[NAME_LEN];
    int  age;
    char contact[CONTACT_LEN];
};

// [SHARED] move to shared.h
struct Booking {
    int   pnr;
    int   passengerID;
    char  flightno[FLIGHT_LEN];
    int   row;            // 0-based
    int   column;         // 0-based
    int   travelClass;    // 1 = Economy, 2 = Business
    float fare;
    int   active;         // 1 = active, 0 = cancelled
    int   checkedIn;      // 1 = checked in, 0 = not
};

// Record storage
static struct Passenger *passengers = NULL;
static int passengerCount = 0;
static int passengerCapacity = 0;

static struct Booking *bookings = NULL;
static int bookingCount = 0;
static int bookingCapacity = 0;
static int nextPNR = FIRST_PNR;

// Input helpers
static int   readLine(char buffer[], int size);
static int   isOnlySpaces(char text[]);
static int   readInt(char prompt[], int min, int max);
static float readFloat(char prompt[], float min, float max);
static void  readName(char prompt[], char name[]);
static void  readContact(char prompt[], char contact[]);
static void  readFlightNo(char prompt[], char flightno[]);
static void  readSeat(char prompt[], int *row, int *column);

// Memory and record helpers
static int growPassengers(void);
static int growBookings(void);
static int seatIsBooked(char flightno[], int row, int column);

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
void freeRecords(void);

// [ALI] flight and seat functions
int  flightExists(char flightno[]);
int  isSeatAvailable(char flightno[], int row, int column);
int  seatMatchesClass(char flightno[], int row, int column, int travelClass);
int  reserveSeat(char flightno[], int row, int column);
void releaseSeat(char flightno[], int row, int column);

// [MENU] test driver; replaced by menu.c
int main() {

    int choice;

    do {
        printf("\n===== TEST MENU =====\n");
        printf("1. Register passenger\n");
        printf("2. Display all passengers\n");
        printf("3. Search passenger by ID\n");
        printf("4. Create booking\n");
        printf("5. Find booking by PNR\n");
        printf("6. Cancel booking\n");
        printf("0. Exit\n");
        choice = readInt("Enter choice: ", 0, 6);

        switch (choice) {
            case 1:
                registerPassenger();
                break;

            case 2:
                displayAllPassengers();
                break;

            case 3:
                searchPassengerMenu();
                break;

            case 4:
                createBooking();
                break;

            case 5:
                searchBookingMenu();
                break;

            case 6:
                cancelBooking();
                break;

            case 0:
                printf("Exiting.\n");
                break;
        }
    } while (choice != 0);

    freeRecords();

    return 0;
}

/* Input helpers */

static int readLine(char buffer[], int size) {
    int ch;

    if (fgets(buffer, size, stdin) == NULL) {
        printf("\nInput ended. Exiting.\n");
        freeRecords();
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

static int readInt(char prompt[], int min, int max) {
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

static float readFloat(char prompt[], float min, float max) {
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

static void readFlightNo(char prompt[], char flightno[]) {
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
        printf("Invalid flight ID. Letters and digits only, 2 to %d characters (e.g. PK301).\n", FLIGHT_LEN - 1);
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

/* Memory helpers */

static int growPassengers(void) {
    struct Passenger *bigger;
    int newCapacity;

    if (passengerCount < passengerCapacity) {
        return 1;
    }

    if (passengers == NULL) {
        newCapacity = INITIAL_CAPACITY;
        bigger = malloc(newCapacity * sizeof(struct Passenger));
    } else {
        newCapacity = passengerCapacity * 2;
        bigger = realloc(passengers, newCapacity * sizeof(struct Passenger));
    }

    if (bigger == NULL) {
        printf("Out of memory. Could not store more passengers.\n");
        return 0;
    }

    passengers = bigger;
    passengerCapacity = newCapacity;
    return 1;
}

static int growBookings(void) {
    struct Booking *bigger;
    int newCapacity;

    if (bookingCount < bookingCapacity) {
        return 1;
    }

    if (bookings == NULL) {
        newCapacity = INITIAL_CAPACITY;
        bigger = malloc(newCapacity * sizeof(struct Booking));
    } else {
        newCapacity = bookingCapacity * 2;
        bigger = realloc(bookings, newCapacity * sizeof(struct Booking));
    }

    if (bigger == NULL) {
        printf("Out of memory. Could not store more bookings.\n");
        return 0;
    }

    bookings = bigger;
    bookingCapacity = newCapacity;
    return 1;
}

static int seatIsBooked(char flightno[], int row, int column) {
    int x = 0;

    while (x < bookingCount) {
        if (bookings[x].active == 1 && bookings[x].row == row && bookings[x].column == column
                && strcmp(bookings[x].flightno, flightno) == 0) {
            return 1;
        }
        x = x + 1;
    }

    return 0;
}

// Releases all records; call once before exit
void freeRecords(void) {
    free(passengers);
    free(bookings);

    passengers = NULL;
    bookings = NULL;
    passengerCount = 0;
    passengerCapacity = 0;
    bookingCount = 0;
    bookingCapacity = 0;
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

    if (growPassengers() == 0) {
        return;
    }

    passengers[passengerCount] = p;
    passengerCount = passengerCount + 1;

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

    readFlightNo("Enter Flight ID (e.g. PK301): ", flightno);

    // [ALI] flight lookup
    if (flightExists(flightno) == 0) {
        printf("Flight not found. Booking not created.\n");
        return 0;
    }

    readSeat("Enter Seat (e.g. 12A): ", &row, &column);

    // [ALI] seat availability
    if (seatIsBooked(flightno, row, column) == 1 || isSeatAvailable(flightno, row, column) == 0) {
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
    fare = readFloat("Enter Fare: ", 1, 1000000);

    if (growBookings() == 0) {
        return 0;
    }

    // [ALI] seat reservation
    if (reserveSeat(flightno, row, column) == 0) {
        printf("Could not reserve the seat. Booking not created.\n");
        return 0;
    }

    slot = bookingCount;
    bookings[slot].pnr = nextPNR;
    bookings[slot].passengerID = passengerID;
    strcpy(bookings[slot].flightno, flightno);
    bookings[slot].row = row;
    bookings[slot].column = column;
    bookings[slot].travelClass = travelClass;
    bookings[slot].fare = fare;
    bookings[slot].active = 1;
    bookings[slot].checkedIn = 0;

    bookingCount = bookingCount + 1;
    nextPNR = nextPNR + 1;

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

    // [ALI] seat release
    releaseSeat(bookings[position].flightno, bookings[position].row, bookings[position].column);
    bookings[position].active = 0;

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