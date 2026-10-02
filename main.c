// #baggage operation and data report
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max 100
#define Null 0
// baggage structure
struct Baggage
{
    char pnr[10];
    char passengername[100];
    int bags;
    float weight;
    float fee;
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
struct Baggage * baggage = Null;
struct Flight flights[max];
int baggagecount = 0;
int flightcount = 0;
float calculatebaggagecharge(float weight){
    if (weight <= 20)
        return 0;
    return (weight -20)*10;
}
