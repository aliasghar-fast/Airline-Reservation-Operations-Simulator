#include <stdio.h>
#include <string.h>

#define MAX 100

int main() {

    int  id[MAX];
    int  age[MAX];
    char name[MAX][30];
    char contact[MAX][15];
    int  count;
    int  i;
    int  found;
    int  searchID;
    int  x;

    printf("Enter the number of passengers: ");
    scanf("%d", &count);

    if (count < 1 || count > MAX) {
        printf("Invalid number of passengers.\n");
        return 0;
    }

    // Input
    for (i = 0; i < count; i++) {
        printf("\n--- Passenger %d ---\n", i + 1);

        printf("Enter Passenger ID: ");
        scanf("%d", &id[i]);
        getchar();

        printf("Enter Passenger Name: ");
        fgets(name[i], sizeof(name[i]), stdin);
        name[i][strcspn(name[i], "\n")] = '\0';

        printf("Enter Passenger Age: ");
        scanf("%d", &age[i]);

        printf("Enter Passenger Contact: ");
        scanf("%14s", contact[i]);
    }

    // Display
    for (i = 0; i < count; i++) {
        printf("\n--- Passenger %d ---\n", i + 1);
        printf("ID:      %d\n", id[i]);
        printf("Name:    %s\n", name[i]);
        printf("Age:     %d\n", age[i]);
        printf("Contact: %s\n", contact[i]);
    }

    // Search by ID
    printf("\nEnter the Passenger ID that you want to search: ");
    scanf("%d", &searchID);

    found = 0;
    x = 0;

    while (found == 0 && x < count) {
        if (searchID == id[x]) {
            found = 1;
        } else {
            x = x + 1;
        }
    }

    if (found == 1) {
        printf("\n--- Passenger Found ---\n");
        printf("ID:      %d\n", id[x]);
        printf("Name:    %s\n", name[x]);
        printf("Age:     %d\n", age[x]);
        printf("Contact: %s\n", contact[x]);
    } else {
        printf("Passenger not found.\n");
    }

    return 0;
}