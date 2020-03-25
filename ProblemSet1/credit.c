#include <stdio.h>
#include <cs50.h>

string getProvider(long card)
{

    // While the card number is more than 2 digits long
    while (card > 99)
    {
        // Divide it by 10
        card /= 10;
    }

    // Get the card's first number
    int firstNumber = (card / 10) % 10;

    // Get the card's second number
    int secondNumber = card % 10;

    // If the first number is 3...
    if (firstNumber == 3)
    {
        // ... and the second number is 4 or 7...
        if (secondNumber == 4 || secondNumber == 7)
        {
            // it's an AMEX card
            return "AMEX";
        }
    }

    // If the first number is 5...
    if (firstNumber == 5)
    {
        // ... and the second number is between 1 and 5...
        if (secondNumber > 0 && secondNumber < 6)
        {
            // ... it's a MASTERCARD card
            return "MASTERCARD";
        }
    }

    // If the first number is 4...
    if (firstNumber == 4)
    {
        // ... it's a VISA card
        return "VISA";
    }

    // If it's none of the above, it's an invalid card
    return "INVALID";
}

// Return the last digit from a given number
int getLastDigit(long number)
{
    return number % 10;
}

bool getValidity(long card)
{
    bool multiply = false;
    int sum = 0;
    int length = 0;

    // While there are digits in the card number...
    while (card > 0)
    {
        // ... get the last digit from the card.
        int lastDigit = getLastDigit(card);

        // If
        if (multiply)
        {
            int product = lastDigit * 2;

            while (product > 0)
            {
                int lastProductDigit = getLastDigit(product);
                sum += lastProductDigit;
                product /= 10;
            }
        }
        else
        {
            sum += lastDigit;
        }

        multiply = !multiply;
        card /= 10;
        length++;
    }

    if (length < 13)
    {
        return false;
    }

    return sum % 10 == 0;
}

int main(void)
{
    // Request card number from user
    long card = get_long("Number: ");

    // Get the card provider by its first 2 digits
    string provider = getProvider(card);

    // Validate the card number
    bool valid = getValidity(card);

    // Return result
    if (valid)
    {
        printf("%s\n", provider);
    }
    else
    {
        printf("INVALID\n");
    }
}
