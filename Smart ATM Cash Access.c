#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_ATTEMPTS 3
#define MAX_CARDS 10
#define DAILY_LIMIT 50000.0
#define MONTHLY_LIMIT 1000000.0
#define SINGLE_WITHDRAW_LIMIT 20000.0

typedef struct
{
    char accountNumber[20];
    char cardNumber[20];
    int pin;
    double balance;
    double dailyWithdrawn;
    double monthlyWithdrawn;
    int lastWithdrawMonth;
} Card;

void printSeparator()
{
    printf("\n-------------------------------------------\n");
}

void printHeader(const char *title)
{
    printf("\n===========================================\n");
    printf("         %s         \n", title);
    printf("===========================================\n");
}

bool confirmAction(const char *message)
{
    char choice;
    printf("%s (Y/N or y/n): ", message);
    scanf(" %c", &choice);
    return (choice == 'Y' || choice == 'y');
}

bool login(Card *card)
{
    int inputPin, attempts = 0;
    while (attempts < MAX_ATTEMPTS)
    {
        printf("\nEnter your 4-digit PIN: ");
        if (scanf("%d", &inputPin) != 1)
        {
            printf("Invalid input! Please enter a numeric PIN.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        if (inputPin == card->pin)
        {
            printf("\nLogin successful!\n");
            return true;
        }
        else
        {
            printf("Incorrect PIN. Attempts remaining: %d\n", MAX_ATTEMPTS - attempts - 1);
            attempts++;
        }
    }
    printf("Too many incorrect attempts. Access denied.\n");
    printf("Please, Contact to our nearest official branch for more details.\n");
    printf("Please, Contact to your nearest official branch for reactivate your card!\n");
    exit(1);
}

void resetPin(Card *card)
{
    printHeader("PIN Reset");

    int oldPin, newPin, confirmPin;
    int attempts = 0;
    const int maxAttempts = 3;

    while (attempts < maxAttempts)
    {
        printf("Enter current PIN: ");
        if (scanf("%d", &oldPin) == 1 && oldPin == card->pin)
        {
            printf("Enter new 4-digit PIN: ");
            if (scanf("%d", &newPin) != 1 || newPin < 1000 || newPin > 9999)
            {
                printf("Invalid PIN format. Reset cancelled.\n");
                while (getchar() != '\n');
                return;
            }

            printf("Confirm new PIN: ");
            if (scanf("%d", &confirmPin) != 1 || confirmPin != newPin)
            {
                printf("PIN confirmation failed. Reset cancelled.\n");
                while (getchar() != '\n');
                return;
            }

            card->pin = newPin;
            printf("PIN reset successful. Please log in again with your new PIN.\n");

            if (!login(card))
            {
                printf("Re-login failed. Exiting.\n");
                exit(1);
            }

            return;
        }
        else
        {
            attempts++;
            printf("Incorrect PIN. Attempts remaining: %d\n", maxAttempts - attempts);
            while (getchar() != '\n');
        }
    }

    printf("Too many incorrect attempts. Access denied.\n");
    exit(1);
}

void showCards(Card cards[], int cardCount)
{
    printHeader("Available Cards");
    for (int i = 0; i < cardCount; i++)
    {
        printf("[%d] Card Ending in %s\n", i + 1, &cards[i].cardNumber[strlen(cards[i].cardNumber) - 2]);
    }
    printSeparator();
}

int selectCardByLastTwoDigits(Card cards[], int cardCount)
{
    showCards(cards, cardCount);
    char lastTwoDigits[3];

    printf("Enter the last two digits of the card to activate: ");
    scanf("%2s", lastTwoDigits);

    for (int i = 0; i < cardCount; i++)
    {
        if (strcmp(&cards[i].cardNumber[strlen(cards[i].cardNumber) - 2], lastTwoDigits) == 0)
        {
            printf("Card ending in %s activated.\n", lastTwoDigits);
            return i;
        }
    }

    printf("No card found with the last two digits '%s'.\n", lastTwoDigits);
    return -1;
}

void atmMenu()
{
    printHeader("Main Menu");
    printf("1. Instant Deposit\n");
    printf("2. Withdraw\n");
    printf("3. Check Balance\n");
    printf("4. Show Card Details\n");
    printf("5. Switch Card\n");
    printf("6. Card to Mobile Transfer\n");
    printf("7. Add New Card\n");
    printf("8. Reset PIN\n");
    printf("9. Exit\n");
    printSeparator();
}

void checkBalance(Card *card)
{
    printHeader("Balance Information");
    printf("Account Number: %s\n", card->accountNumber);
    printf("Card Number: %s\n", card->cardNumber);
    printf("Current Balance: $%.2f\n", card->balance);
    printSeparator();
}

void deposit(Card *card)
{
    printHeader("Deposit");
    double amount;

    printf("Enter amount to deposit: $");
    if (scanf("%lf", &amount) != 1 || amount <= 0)
    {
        printf("Invalid amount entered.\n");
        return;
    }

    card->balance += amount;
    printf("Deposit of $%.2f successful! New balance: $%.2f\n", amount, card->balance);
}

void withdraw(Card *card)
{
    printHeader("Withdraw");
    double amount;

    printf("Enter amount to withdraw: $");
    if (scanf("%lf", &amount) != 1 || amount <= 0)
    {
        printf("Invalid amount entered.\n");
        return;
    }

    if (amount > SINGLE_WITHDRAW_LIMIT)
    {
        printf("Cannot withdraw more than $%.2f in a single transaction.\n", SINGLE_WITHDRAW_LIMIT);
        return;
    }

    if (amount > card->balance)
    {
        printf("Insufficient funds.\n");
        return;
    }

    card->balance -= amount;
    printf("Withdrawal of $%.2f successful! New balance: $%.2f\n", amount, card->balance);
}

void showCardDetails(Card *card)
{
    printHeader("Card Details");
    printf("Account Number: %s\n", card->accountNumber);
    printf("Card Number: %s\n", card->cardNumber);
    printf("Balance: $%.2f\n", card->balance);
    printSeparator();
}

void cardToMobileTransfer(Card *card)
{
    printHeader("Card to Mobile Transfer");
    char mobileNumber[15];
    double amount;

    printf("Enter Mobile Number: ");
    scanf("%14s", mobileNumber);

    printf("Enter amount to transfer: $");
    if (scanf("%lf", &amount) != 1 || amount <= 0 || amount > card->balance)
    {
        printf("Invalid amount or insufficient balance.\n");
        return;
    }

    card->balance -= amount;
    printf("Successfully transferred $%.2f to %s. \nNew balance: $%.2f\n", amount, mobileNumber, card->balance);
}

void addNewCard(Card cards[], int *cardCount)
{
    if (*cardCount >= MAX_CARDS)
    {
        printf("Maximum number of cards reached. Cannot add more.\n");
        return;
    }

    printHeader("Add New Card");
    Card newCard;

    printf("Enter Account Number: ");
    scanf("%19s", newCard.accountNumber);

    printf("Enter Card Number: ");
    scanf("%19s", newCard.cardNumber);

    printf("Set 4-digit PIN: ");
    scanf("%d", &newCard.pin);

    printf("Enter Initial Balance: $");
    scanf("%lf", &newCard.balance);

    newCard.dailyWithdrawn = 0;
    newCard.monthlyWithdrawn = 0;
    newCard.lastWithdrawMonth = -1;

    cards[(*cardCount)++] = newCard;
    printf("New card added successfully!\n");
}

int main()
{
    Card cards[MAX_CARDS] = {
        {"0322310105101042", "032231042", 1234, 100000.0, 0, 0, -1},
        {"0322310205101014", "032231014", 5678, 50000.0, 0, 0, -1},
        {"0322310105101025", "032231025", 9012, 99000.0, 0, 0, -1}};

    int cardCount = 3;
    int activeCardIndex = -1;

    printHeader("Welcome to the ATM Machine");

    while (true)
    {
        if (activeCardIndex == -1)
        {
            activeCardIndex = selectCardByLastTwoDigits(cards, cardCount);
            if (activeCardIndex == -1)
                continue;
            if (!login(&cards[activeCardIndex]))
                continue;
        }

        atmMenu();
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. \nPlease enter a number.\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choice)
        {
        case 1:
            deposit(&cards[activeCardIndex]);
            break;
        case 2:
            withdraw(&cards[activeCardIndex]);
            break;
        case 3:
            checkBalance(&cards[activeCardIndex]);
            break;
        case 4:
            showCardDetails(&cards[activeCardIndex]);
            break;
        case 5:
            printf("Switching card...\n");
            activeCardIndex = -1;
            break;
        case 6:
            cardToMobileTransfer(&cards[activeCardIndex]);
            break;
        case 7:
            addNewCard(cards, &cardCount);
            break;
        case 8:
            resetPin(&cards[activeCardIndex]);
            break;
        case 9:
            if (confirmAction("Are you sure you want to exit?"))
            {
                printf("Thank you for using the ATM. Goodbye!\n");
                return 0;
            }
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }

        if (choice != 5 && choice != 9 && !confirmAction("Do you want to perform another action?"))
        {
            printf("Thank you for using the ATM. Goodbye!\n");
            printf("Developed by team Syntax Error,PUB!\n");
            printf("Powered By Pundra University!\n");
            break;
        }
    }

    return 0;
}
