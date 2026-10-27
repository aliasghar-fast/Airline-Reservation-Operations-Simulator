#include <stdio.h>
#include <string.h>

#define MAX 100
#define NAME_LEN 30
#define CONTACT_LEN 15

void registerPassenger(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int index);
void displayPassenger(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int index);
void displayAllPassengers(int id[], char name[][NAME_LEN], int age[], char contact[][CONTACT_LEN], int count);
int searchPassenger(int id[], int count, int searchID);

int main() {

    int  id[MAX];
    int  age[MAX];
    char name[MAX][NAME_LEN];
    char contact[MAX][CONTACT_LEN];
    int  count;
    int  i;
    int  searchID;
    int  position;

    printf("Enter the number of passengers: ");
    scanf("%d", &count);

    if (count < 1 || count > MAX) {
        printf("Invalid number of passengers.\n");
        return 0;
    }

    // Input
    for (i = 0; i < count; i++) {
        printf("\n--- Passenger %d ---\n", i + 1);
        registerPassenger(id, name, age, contact, i);
    }

    // Display
    displayAllPassengers(id, name, age, contact, count);

    // Search by ID
    printf("\nEnter the Passenger ID that you want to search: ");
    scanf("%d", &searchID);

    position = searchPassenger(id, count, searchID);

    if (position != -1) {
        printf("\n--- Passenger Found ---\n");
        displayPassenger(id, name, age, contact, position);
    } else {
        printf("Passenger not found.\n");
    }

    return 0;
}

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