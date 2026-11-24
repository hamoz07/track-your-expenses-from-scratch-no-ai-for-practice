#include <stdio.h>
#include <stdlib.h>

int main(void) {
    float remainingAllowance = 0.0;
    FILE *file = fopen("wallet.txt", "r");
    if (file) {
        fscanf(file, "%f", &remainingAllowance);
        fclose(file);
        printf("Your remaining allowance is: %.2f\n", remainingAllowance);
    } else {
        printf("No allowance file found.\n");
    }
    return 0;
}
