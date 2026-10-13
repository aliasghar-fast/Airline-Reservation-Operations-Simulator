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
    if (weight <= 20)
        return 0;
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


    printf("PNR Number is: %d", &baggage[baggagecount].pnr);
    
}