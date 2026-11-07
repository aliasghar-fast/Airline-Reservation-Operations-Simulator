#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define NAME_LEN 30
#define CONTACT_LEN 15
#define FLIGHT_LEN 10
#define MAX_BOOKINGS 200
#define FIRST_PNR 1001

/* TEMPORARY PLACE: struct to be moved into shared.h once Ali creates it */
struct Booking {
    int   pnr;
    int   passengerID;
    char  flightno[FLIGHT_LEN];
    int   row;            // 0-based: row 0 is shown as 1
    int   column;         // 0-based: column 0 is shown as A
    int   travelClass;    // 1 = Economy, 2 = Business
    float fare;
    int   active;         // 1 = current booking, 0 = cancelled
    int   checkedIn;      // 0 = not checked in yet
};

// Passenger functions
void registerPassenger(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int index);
void displayPassenger(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int index);
void displayAllPassengers(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int count);
int searchPassenger(int id[], int count, int searchID);

// Booking functions
int createBooking(struct Booking bookings[], int *bookingCount, int *nextPNR, int id[], int passengerCount);
int findBookingByPNR(struct Booking bookings[], int bookingCount, int pnr);
void displayBooking(struct Booking bookings[], int index);
int cancelBooking(struct Booking bookings[], int bookingCount);

// TEMPORARY stand-ins for Ali's flight/seat functions
int flightExists(char flightno[]);
int isSeatAvailable(char flightno[], int row, int column);
int reserveSeat(char flightno[], int row, int column);
void releaseSeat(char flightno[], int row, int column);

int main() {

    int  id[MAX];
    int  age[MAX];
    char name[MAX][NAME_LEN];
    char contact[MAX][CONTACT_LEN];
    int  passengerCount = 0;

    struct Booking bookings[MAX_BOOKINGS];
    int  bookingCount = 0;
    int  nextPNR = FIRST_PNR;

    int  choice;
    int  searchID;
    int  searchPNR;
    int  position;
    int  pnr;

    do {
        printf("\n===== TEST MENU =====\n");
        printf("1. Register passenger\n");
        printf("2. Display all passengers\n");
        printf("3. Search passenger by ID\n");
        printf("4. Create booking\n");
        printf("5. Find booking by PNR\n");
        printf("6. Cancel booking\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (passengerCount >= MAX) {
                    printf("Passenger limit reached.\n");
                } else {
                    registerPassenger(id, name, age, contact, passengerCount);
                    passengerCount = passengerCount + 1;
                }
                break;

            case 2:
                displayAllPassengers(id, name, age, contact, passengerCount);
                break;

            case 3:
                printf("Enter the Passenger ID to search: ");
                scanf("%d", &searchID);
                position = searchPassenger(id, passengerCount, searchID);
                if (position != -1) {
                    printf("\n--- Passenger Found ---\n");
                    displayPassenger(id, name, age, contact, position);
                } else {
                    printf("Passenger not found.\n");
                }
                break;

            case 4:
                pnr = createBooking(bookings, &bookingCount, &nextPNR, id, passengerCount);
                if (pnr != 0) {
                    printf("\nBooking successful! Your PNR is %d\n", pnr);
                }
                break;

            case 5:
                printf("Enter PNR: ");
                scanf("%d", &searchPNR);
                position = findBookingByPNR(bookings, bookingCount, searchPNR);
                if (position != -1) {
                    printf("\n--- Booking Found ---\n");
                    displayBooking(bookings, position);
                } else {
                    printf("Booking not found.\n");
                }
                break;

            case 6:
                if (cancelBooking(bookings, bookingCount) == 1) {
                    printf("\nBooking cancelled. The seat is free again.\n");
                }
                break;

            case 0:
                printf("Exiting.\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}

/* ---------- PASSENGER FUNCTIONS ---------- */

void registerPassenger(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int index) {
    int newID;

    printf("Enter Passenger ID: ");
    scanf("%d", &newID);

    while (searchPassenger(id, index, newID) != -1) {
        printf("That ID is already registered. Enter a different ID: ");
        scanf("%d", &newID);
    }

    id[index] = newID;
    getchar();

    printf("Enter Passenger Name: ");
    fgets(name[index], NAME_LEN, stdin);
    name[index][strcspn(name[index], "\n")] = '\0';

    printf("Enter Passenger Age: ");
    scanf("%d", &age[index]);

    printf("Enter Passenger Contact: ");
    scanf("%14s", contact[index]);   // 14 = CONTACT_LEN - 1
}

void displayPassenger(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int index) {
    printf("ID:      %d\n", id[index]);
    printf("Name:    %s\n", name[index]);
    printf("Age:     %d\n", age[index]);
    printf("Contact: %s\n", contact[index]);
}

void displayAllPassengers(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int count) {
    int i;

    for (i = 0; i < count; i++) {
        printf("\n--- Passenger %d ---\n", i + 1);
        displayPassenger(id, name, age, contact, i);
    }
}

int searchPassenger(int id[], int count, int searchID) {
    int x = 0;

    while (x < count) {
        if (id[x] == searchID) {
            return x;
        }
        x = x + 1;
    }

    return -1;
}

/* ---------- BOOKING FUNCTIONS ---------- */

// Returns the new PNR, or 0 if the booking failed (and nothing was changed).
int createBooking(struct Booking bookings[], int *bookingCount, int *nextPNR, int id[], int passengerCount) {
    int   passengerID;
    char  flightno[FLIGHT_LEN];
    int   seatRow;
    char  seatLetter;
    int   row;
    int   column;
    int   travelClass;
    float fare;
    int   slot;

    if (*bookingCount >= MAX_BOOKINGS) {
        printf("Booking limit reached.\n");
        return 0;
    }

    printf("Enter Passenger ID: ");
    scanf("%d", &passengerID);

    if (searchPassenger(id, passengerCount, passengerID) == -1) {
        printf("Passenger not found. Booking cancelled.\n");
        return 0;
    }

    printf("Enter Flight ID (e.g. PK301): ");
    scanf("%9s", flightno);

    if (flightExists(flightno) == 0) {
        printf("Flight not found. Booking cancelled.\n");
        return 0;
    }

    printf("Enter Seat (e.g. 12A): ");
    scanf("%d %c", &seatRow, &seatLetter);

    row = seatRow - 1;
    column = toupper(seatLetter) - 'A';

    if (isSeatAvailable(flightno, row, column) == 0) {
        printf("Seat is not available. Booking cancelled.\n");
        return 0;
    }

    printf("Enter Class (1 = Economy, 2 = Business): ");
    scanf("%d", &travelClass);

    if (travelClass != 1 && travelClass != 2) {
        printf("Invalid class. Booking cancelled.\n");
        return 0;
    }

    printf("Enter Fare: ");
    scanf("%f", &fare);

    // Every check has passed. Only now do we change anything.
    if (reserveSeat(flightno, row, column) == 0) {
        printf("Could not reserve the seat. Booking cancelled.\n");
        return 0;
    }

    slot = *bookingCount;
    bookings[slot].pnr = *nextPNR;
    bookings[slot].passengerID = passengerID;
    strcpy(bookings[slot].flightno, flightno);
    bookings[slot].row = row;
    bookings[slot].column = column;
    bookings[slot].travelClass = travelClass;
    bookings[slot].fare = fare;
    bookings[slot].active = 1;
    bookings[slot].checkedIn = 0;

    *bookingCount = *bookingCount + 1;
    *nextPNR = *nextPNR + 1;

    return bookings[slot].pnr;
}

int findBookingByPNR(struct Booking bookings[], int bookingCount, int pnr) {
    int x = 0;

    while (x < bookingCount) {
        if (bookings[x].pnr == pnr) {
            return x;
        }
        x = x + 1;
    }

    return -1;
}

void displayBooking(struct Booking bookings[], int index) {
    printf("PNR:        %d\n", bookings[index].pnr);
    printf("Passenger:  %d\n", bookings[index].passengerID);
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

// Returns 1 if a booking was cancelled, 0 otherwise (and nothing was changed).
int cancelBooking(struct Booking bookings[], int bookingCount) {
    int pnr;
    int position;

    printf("Enter PNR to cancel: ");
    scanf("%d", &pnr);

    position = findBookingByPNR(bookings, bookingCount, pnr);

    if (position == -1) {
        printf("Booking not found.\n");
        return 0;
    }

    if (bookings[position].active == 0) {
        printf("This booking is already cancelled.\n");
        return 0;
    }

    // The seat is released only here. Once active is 0, we never get this far again.
    releaseSeat(bookings[position].flightno, bookings[position].row, bookings[position].column);
    bookings[position].active = 0;

    return 1;
}

/* ---------- TEMPORARY STAND-INS FOR ALI'S FUNCTIONS ----------
   Whole section to be deleted when Ali's real flight/seat code is merged. */

int stubSeats[30][6];   // 0 = free, 1 = taken (one test flight, 30 rows x 6 seats)

int flightExists(char flightno[]) {
    if (strcmp(flightno, "PK301") == 0) {
        return 1;
    }
    return 0;
}

int isSeatAvailable(char flightno[], int row, int column) {
    if (row < 0 || row >= 30 || column < 0 || column >= 6) {
        return 0;
    }
    if (stubSeats[row][column] == 1) {
        return 0;
    }
    return 1;
}

int reserveSeat(char flightno[], int row, int column) {
    stubSeats[row][column] = 1;
    return 1;
}

void releaseSeat(char flightno[], int row, int column) {
    stubSeats[row][column] = 0;
}