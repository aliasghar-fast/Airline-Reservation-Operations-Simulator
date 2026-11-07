// #baggage operation and data report
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BAGGAGE 100
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
// records
struct FlightAvailaibulity
{
    char pnr[10];
    char flightno[10];
    char class;
    int cancel;
    float fare;
};
struct Flight
{
    flightno[10];
    char orgin[30], dest[30];
    int totalseats , occupied_seats;
    float ticketprice;

};
struct Baggage baggage[MAX_BAGGAGE];
struct Flight flights[MAX_FLIGHTS];
int baggagecount = 0;
int flightcount = 0;
float calculatebaggagecharge(float weight){
    if (weight <= 20){
        return 0;
    }
    return (weight -20)*10;
}
void addBaggage() {
    if (baggagecount >= MAX_BAGGAGE )
    {
        printf("\n Baggge is full!\n");
        return;
    }
    printf("\n =============ADD BAGAGE ==========\n");
    printf("enter pnr:");
    scanf("%19s",baggage[baggagecount].pnr);
    printf("enter name of passenger:");
    scanf("%[^\n]",baggage[baggagecount].passengername);
    printf("enter baggae weight(kg):");
    scanf("%f", &baggage[baggagecount].weight);
    printf("enter the no. of pieces of bag:");
    scanf("%d", &baggage[baggagecount].bags);
    baggage[baggagecount].charges = calculatebaggagecharge(baggage[baggagecount].weight);
    printf("\n baggage added succesfully!");
    printf("\n baggage charges: RS. %.2f\n",baggage[baggagecount].charges);
    baggagecount++;   
    
}
// searching for the baggage fount == 0 or found ==1
void searchBaggage()
{
    char searchPNR[20];
    int found = 0;
    printf("\nEnter PNR to search: ");
    scanf("%19s", searchPNR);
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

void addFlight() {
    if (flightcount >= MAX_FLIGHTS) {
        printf("\nFlight storage is full!\n");
        return;
    }
    printf("\n--- ADD FLIGHT ---\n");
    printf("Enter Flight Number: ");
    scanf("%s", flights[flightcount].flightno);
    printf("Enter Total Seats: ");
    scanf("%d", &flights[flightcount].totalseats);
    printf("Enter Occupied Seats: ");
    scanf("%d", &flights[flightcount].occupied_seats);
    printf("Enter Ticket Fare: ");
    scanf("%f", &flights[flightcount].ticketprice);
    
    flightcount++;
    printf("\nFlight added successfully!\n");
}
void occupancyReport() {
    if (flightcount == 0) {
        printf("\nNo flight records available.\n");
        return;
    }
    
    printf("\n--- OCCUPANCY REPORT ---\n");
    for (int i = 0; i < flightcount; i++) {
        int availableSeats = flights[i].totalseats - flights[i].occupied_seats;
        printf("\nFlight : %s", flights[i].flightno);
        printf("\nTotal Seats : %d", flights[i].totalseats);
        printf("\nOccupied : %d", flights[i].occupied_seats);
        printf("\nAvailable : %d", availableSeats);
        printf("\nOccupancy : %.2f%%\n", calculateOccupancy(flights[i].totalseats, flights[i].occupied_seats));
    }
}
void revenueReport() {
    float totalRevenue = 0;
    if (flightcount == 0) {
        printf("\nNo flight records available.\n");
        return;
    }
    
    printf("\n--- REVENUE REPORT ---\n");
    for (int i = 0; i < flightcount; i++) {
        float revenue = flights[i].occupied_seats * flights[i].ticketprice;
        printf("\nFlight : %s", flights[i].flightno);
        printf("\nTickets Sold : %d", flights[i].occupied_seats);
        printf("\nTicket Fare : Rs. %.2f", flights[i].ticketprice);
        printf("\nRevenue : Rs. %.2f\n", revenue);
        totalRevenue += revenue;
    }
    printf("\nTOTAL REVENUE: Rs. %.2f\n", totalRevenue);
}