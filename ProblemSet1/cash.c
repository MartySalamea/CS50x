#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{
    int intQuarter;
    int intDime;
    int intNickel;
    int intPenny;
    int changeOutput = 0;

    float changeInput;
    //Prompt user for change input
    do
    {
        changeInput = get_float("Change owed: ");
    }
    while (changeInput < 0);
    //Round to nearest cent
    //Get cents from the dollar input
    int cents = round(changeInput * 100);
    //Get amount of quarters
    for (intQuarter = 1; cents >= 25; intQuarter++)
    {
        cents -= 25;
        changeOutput += 1;
    }
    //Get amount of dimes
    for (intDime = 1; cents >= 10; intDime++)
    {
        cents -= 10;
        changeOutput += 1;
    }
    //Get amount oof nickels
    for (intNickel = 1; cents >= 5; intNickel++)
    {
        cents -= 5;
        changeOutput += 1;
    }
    //Get amount of pennies
    for (intPenny = 1; cents >= 1; intPenny++)
    {
        cents -= 1;
        changeOutput += 1;
    }
    //Print output
    printf("%i\n", changeOutput);


}
