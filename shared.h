#ifndef SHARED_H
#define SHARED_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define MAX_RECORDS 10000
#define FLIGHTS 8
#define FIRST_DAY 20261111
#define LAST_DAY 20271231
typedef struct { char id[8], from[4], to[4], departure[6], arrival[6], model[16]; int rows, business_rows, base; } Flight;
typedef struct { int id, age; char name[64], contact[32]; } Passenger;
typedef struct { int pnr, passenger, flight, date, row, col, active, checked, bags; double kg, fare, bag_fee; } Booking;
typedef struct { Passenger *passengers; Booking *bookings; int np, nb, pc, bc, next_pnr; } Airline;
extern const Flight flights[FLIGHTS];
bool reserve_memory(void **ptr, int *capacity, int needed, size_t item);
void init_airline(Airline *a);
void free_airline(Airline *a);
bool valid_date(int date);
int flight_index(const char *id);
int passenger_index(const Airline *a, int id);
int booking_index(const Airline *a, int pnr);
bool add_passenger(Airline *a, int id, const char *name, int age, const char *contact);
int seat_owner(const Airline *a, int flight, int date, int row, int col);
int availability(const Airline *a, int flight, int date);
void show_seats(const Airline *a, int flight, int date);
void list_flights(const Airline *a, const char *from, const char *to, int date);
void route_map(void);
double ticket_fare(int flight, int row, int age);
double baggage_fee(int flight, int row, double kg);
int make_booking(Airline *a, int passenger, int flight, int date, int row, int col);
bool cancel_booking(Airline *a, int pnr);
bool set_baggage(Airline *a, int pnr, int count, double kg);
bool check_in(Airline *a, int pnr);
void show_booking(const Airline *a, int pnr);
void reports(const Airline *a, int date);
bool save_airline(const Airline *a, const char *path);
bool load_airline(Airline *a, const char *path);
bool read_text(const char *prompt, char *out, int capacity);
int read_number(const char *prompt, int low, int high);
double read_weight(void);
void run_menu(Airline *a);
#endif