// #baggage operation and data report
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"
#define MAX_BAGGAGE MAX_RECORDS
#define MAX_FLIGHTS 100
#define Null 0
// baggage structure
struct Baggage
{
    char pnr[10];
    char passengername[100];
    int bags;
    float weight;
    float charges;
};
struct Baggage baggage[MAX_BAGGAGE];

int baggagecount = 0;

float calculatebaggagecharge(float weight){
    if (weight <= 20){
        return 0;
    }
    return (weight -20)*10;
}
void addBaggage() {
    int pnr = readInt("Enter PNR: ", 1001, 9999999);
    struct Booking *b = getBooking(pnr);
    if (b == NULL || !b->active || b->checkedIn) {
        puts("Enter an active PNR before check-in.");
        return;
    }
    char key[10];
    sprintf(key, "%d", pnr);
    int i = 0;
    while (i < baggagecount && strcmp(baggage[i].pnr, key) != 0) i++;
    if (i == MAX_BAGGAGE) {
        puts("Baggage list is full.");
        return;
    }
    int bags = readInt("Number of bags: ", 0, 3);
    float weight = 0;
    if (bags > 0) weight = readFloat("Total weight in kg: ", 0.01f, bags * 32);
    strcpy(baggage[i].pnr, key);
    strcpy(baggage[i].passengername, getPassenger(b->passengerID)->name);
    baggage[i].bags = bags;
    baggage[i].weight = weight;
    baggage[i].charges = calculatebaggagecharge(weight);
    if (i == baggagecount) baggagecount++;
    saveData();
    printf("Baggage charges: Rs. %.2f\n", baggage[i].charges);
}
// searching for the baggage fount == 0 or found ==1
void searchBaggage()
{
    char searchPNR[20];
    int found = 0;
    printf("\nEnter PNR to search: ");
    sprintf(searchPNR, "%d", readInt("", 1001, 9999999));
    for (int i = 0; i < baggagecount; i++) {
        if (strcmp(baggage[i].pnr, searchPNR) == 0) {
            printf("\n--- BAGGAGE DETAILS ---\n");
            printf("PNR : %s\n", baggage[i].pnr);
            printf("Passenger : %s\n", baggage[i].passengername);
            printf("Weight : %.2f KG\n", baggage[i].weight);
            printf("Pieces : %d\n", baggage[i].bags);
            printf("Charges : Rs. %.2f\n", baggage[i].charges);
            found = 1;
            break;
        }
    }
    
    if (found == 0) {
        printf("\nBaggage record not found.\n");
    }
}
void displayBaggage() {
    if (baggagecount == 0) {
        printf("\nNo baggage records available.\n");
        return;
    }
    printf("\n--- BAGGAGE REPORT ---\n");
    for (int i = 0; i < baggagecount; i++) {
        printf("\nRecord %d\n", i + 1);
        printf("PNR : %s\n", baggage[i].pnr);
        printf("Passenger : %s\n", baggage[i].passengername);
        printf("Weight : %.2f KG\n", baggage[i].weight);
        printf("Pieces : %d\n", baggage[i].bags);
        printf("Charges : Rs. %.2f\n", baggage[i].charges);
    }
}
float calculate_Occupancy(int totalSeats, int occupiedSeats) {
    return ((float)occupiedSeats / totalSeats) * 100;
}

static float flightRevenue(int f) {
    float total = 0;
    for (int i = 0; i < getBookingCount(); i++) {
        struct Booking *b = getBookingAt(i);
        if (b->active && b->date == departureDate && strcmp(b->flightno, flights[f].id) == 0)
            total += b->fare;
    }
    return total;
}

static int occupied(int f) {
    int count = 0;
    for (int i = 0; i < getBookingCount(); i++) {
        struct Booking *b = getBookingAt(i);
        if (b->active && b->date == departureDate && strcmp(b->flightno, flights[f].id) == 0)
            count++;
    }
    return count;
}

void occupancyReport() {
    if (FLIGHTS == 0) {
        printf("\nNo flight records available.\n");
        return;
    }
    
    printf("\n--- OCCUPANCY REPORT ---\n");
    for (int i = 0; i < FLIGHTS; i++) {
        int availableSeats = (flights[i].rows * 6) - occupied(i);
        printf("\nFlight : %s", flights[i].id);
        printf("\nTotal Seats : %d", (flights[i].rows * 6));
        printf("\nOccupied : %d", occupied(i));
        printf("\nAvailable : %d", availableSeats);
        printf("\nOccupancy : %.2f%%\n", calculate_Occupancy((flights[i].rows * 6), occupied(i)));
    }
}
void revenueReport() {
    float totalRevenue = 0;
    if (FLIGHTS == 0) {
        printf("\nNo flight records available.\n");
        return;
    }
    
    printf("\n--- REVENUE REPORT ---\n");
    for (int i = 0; i < FLIGHTS; i++) {
        float revenue = flightRevenue(i);
        printf("\nFlight : %s", flights[i].id);
        printf("\nTickets Sold : %d", occupied(i));
        printf("\nEconomy base fare : Rs. %d", flights[i].base);
        printf("\nRevenue : Rs. %.2f\n", revenue);
        totalRevenue += revenue;
    }
    printf("\nTOTAL REVENUE: Rs. %.2f\n", totalRevenue);
}
void sortFlightsByRevenue() {
    int order[FLIGHTS];
    for (int i = 0; i < FLIGHTS; i++) order[i] = i;
    int temp;
    for (int i = 0; i < FLIGHTS - 1; i++) {
        for (int j = 0; j < FLIGHTS - i - 1; j++) {
            float rev_1 = flightRevenue(order[j]);
            float rev_2 = flightRevenue(order[j + 1]);
            if (rev_1 < rev_2) {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < FLIGHTS; i++)
        printf("%s: Rs. %.2f\n", flights[order[i]].id, flightRevenue(order[i]));
}
void baggageMenu() {
    int choice;
    do {
        printf("\n*** BAGGAGE MANAGEMENT ***\n");
        printf("1. Add Baggage\n");
        printf("2. Search Baggage\n");
        printf("3. Display Baggage\n");
        printf("4. Return to Main Menu\n");
        printf("Enter choice: ");
        choice = readInt("", 1, 4);
        
        switch (choice) {
            case 1: addBaggage(); break;
            case 2: searchBaggage(); break;
            case 3: displayBaggage(); break;
            case 4: break;
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 4);
}
void reportMenu() {
    int choice;
    do {
        printf("\n*** REPORTS ***\n");
        printf("1. Baggage Report\n");
        printf("2. Occupancy Report\n");
        printf("3. Revenue Report\n");
        printf("4. Sort Flights by Revenue\n");
        printf("5. Return to Main Menu\n");
        printf("Enter choice: ");
        choice = readInt("", 1, 5);
        
        switch (choice) {
            case 1: displayBaggage(); break;
            case 2: occupancyReport(); break;
            case 3: revenueReport(); break;
            case 4: sortFlightsByRevenue(); break;
            case 5: break;
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 5);
}

float ticketFare(char flightno[], int travelClass, int age) {
    float fare = flights[flightIndex(flightno)].base;
    if (travelClass == 2) fare *= 1.8f;
    if (age < 12) fare *= 0.75f;
    return fare * 1.05f;
}

void checkIn(void) {
    int pnr = readInt("Enter PNR: ", 1001, 9999999);
    struct Booking *b = getBooking(pnr);
    if (b == NULL || !b->active || b->checkedIn) {
        puts("Check-in rejected.");
        return;
    }
    b->checkedIn = 1;
    saveData();
    puts("BOARDING PASS");
    displayBooking(findBookingByPNR(pnr));
}

void writeBaggage(FILE *file) {
    fprintf(file, "Baggage %d\n", baggagecount);
    for (int i = 0; i < baggagecount; i++)
        fprintf(file, "%s %d %.9g\n", baggage[i].pnr, baggage[i].bags, baggage[i].weight);
}

int loadBaggage(FILE *file) {
    int count;
    if (fscanf(file, " Baggage %8d", &count) != 1 || count < 0 || count > getBookingCount()) return 0;
    for (int i = 0; i < count; i++) {
        int pnr, bags;
        float weight;
        if (fscanf(file, " %8d %2d %f", &pnr, &bags, &weight) != 3 ||
            bags < 0 || bags > 3 || !(weight >= 0 && weight <= bags * 32)) return 0;
        struct Booking *b = getBooking(pnr);
        if (!b) return 0;
        sprintf(baggage[i].pnr, "%d", pnr);
        strcpy(baggage[i].passengername, getPassenger(b->passengerID)->name);
        baggage[i].bags = bags;
        baggage[i].weight = weight;
        baggage[i].charges = calculatebaggagecharge(weight);
        baggagecount++;
    }
    return 1;
}
