// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void transferFunds(FILE *fPtr);
void calculateSIP(void);
void calculateTax(void);
void loanEligibility(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            transferFunds(cfPtr);
            break;
        case 6:
            calculateSIP();
            break;
        case 7:
            calculateTax();
            break;
        case 8:
            loanEligibility(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write single record to text file
            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("%s", "Enter account to update ( 1 - 100 ): ");
    scanf("%d", &account);

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    { // update record
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // request transaction amount from user
        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        scanf("%lf", &transaction);
        client.balance += transaction; // update record balance

        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0}; // blank client
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("%s", "Enter new account number ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;
        // move file pointer to correct record in file
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    // display available options
    printf("%s", "\nEnter your choice:\n"
                 "1 - Store a formatted text file of accounts for printing\n"
                 "2 - Update an account (Deposit/Withdraw)\n"
                 "3 - Add a new account\n"
                 "4 - Delete an account\n"
                 "5 - Transfer funds\n"
                 "6 - Systematic Investment Plan (SIP) Calculator\n"
                 "7 - Tax Calculator\n"
                 "8 - Check Loan Eligibility\n"
                 "9 - End program\n? ");

    scanf("%u", &menuChoice); // receive choice from user
    return menuChoice;
} // end function enterChoice

// transfer funds between two accounts
void transferFunds(FILE *fPtr)
{
    unsigned int sender, receiver;
    double amount;
    struct clientData senderClient = {0, "", "", 0.0};
    struct clientData receiverClient = {0, "", "", 0.0};

    printf("Enter sender account number (1-100): ");
    scanf("%u", &sender);
    printf("Enter receiver account number (1-100): ");
    scanf("%u", &receiver);

    if (sender == receiver) {
        printf("Sender and receiver cannot be the same.\n");
        return;
    }

    // Read sender
    fseek(fPtr, (sender - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&senderClient, sizeof(struct clientData), 1, fPtr);
    if (senderClient.acctNum == 0) {
        printf("Sender account #%u does not exist.\n", sender);
        return;
    }

    // Read receiver
    fseek(fPtr, (receiver - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&receiverClient, sizeof(struct clientData), 1, fPtr);
    if (receiverClient.acctNum == 0) {
        printf("Receiver account #%u does not exist.\n", receiver);
        return;
    }

    printf("Enter amount to transfer: ");
    scanf("%lf", &amount);

    if (amount <= 0) {
        printf("Invalid transfer amount.\n");
        return;
    }
    if (senderClient.balance < amount) {
        printf("Insufficient balance in sender account.\n");
        return;
    }

    // Perform transfer
    senderClient.balance -= amount;
    receiverClient.balance += amount;

    // Update sender
    fseek(fPtr, (sender - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&senderClient, sizeof(struct clientData), 1, fPtr);

    // Update receiver
    fseek(fPtr, (receiver - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&receiverClient, sizeof(struct clientData), 1, fPtr);

    printf("Transfer successful...\n");
    printf("Sender New Balance: %.2f\n", senderClient.balance);
    printf("Receiver New Balance: %.2f\n", receiverClient.balance);
}

// simulate SIP using compound interest
void calculateSIP(void)
{
    double monthlyInvestment, interestRate, expectedReturn, totalInvestment;
    int durationYears, months;

    printf("Enter monthly investment amount: ");
    scanf("%lf", &monthlyInvestment);
    printf("Enter expected annual return rate (%%): ");
    scanf("%lf", &interestRate);
    printf("Enter duration in years: ");
    scanf("%d", &durationYears);

    if (monthlyInvestment <= 0 || interestRate < 0 || durationYears <= 0) {
        printf("Invalid input for SIP calculation.\n");
        return;
    }

    months = durationYears * 12;
    // Monthly interest rate
    double i = (interestRate / 100) / 12;
    // SIP Formula: M = P x ({[1 + i]^n - 1} / i) x (1 + i)
    expectedReturn = monthlyInvestment * (pow(1 + i, months) - 1) / i * (1 + i);
    totalInvestment = monthlyInvestment * months;

    printf("\n--- SIP Calculation Results ---\n");
    printf("Total Amount Invested : %.2f\n", totalInvestment);
    printf("Expected Return Rate  : %.2f%%\n", interestRate);
    printf("Estimated Total Value : %.2f\n\n", expectedReturn);
}

// tax calculator based on slabs
void calculateTax(void)
{
    double income, tax = 0.0;

    printf("Enter annual income: ");
    scanf("%lf", &income);

    if (income < 0) {
        printf("Invalid income amount.\n");
        return;
    }

    if (income <= 250000) {
        tax = 0.0;
    } else if (income <= 500000) {
        tax = (income - 250000) * 0.05;
    } else if (income <= 1000000) {
        tax = (250000 * 0.05) + ((income - 500000) * 0.20);
    } else {
        tax = (250000 * 0.05) + (500000 * 0.20) + ((income - 1000000) * 0.30);
    }

    printf("\n--- Tax Calculation ---\n");
    printf("Annual Income : %.2f\n", income);
    printf("Total Tax Payable : %.2f\n\n", tax);
}

// check loan eligibility based on account balance
void loanEligibility(FILE *fPtr)
{
    unsigned int accountNum;
    struct clientData client = {0, "", "", 0.0};
    double minBalanceThreshold = 10000.0;
    int loanMultiple = 5;

    printf("Enter account number to check loan eligibility (1 - 100): ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account #%u does not exist.\n", accountNum);
        return;
    }

    printf("\n--- Loan Eligibility Check ---\n");
    printf("Account: %s %s\n", client.firstName, client.lastName);
    printf("Account Balance: %.2f\n", client.balance);

    if (client.balance >= minBalanceThreshold) {
        double maxLoan = client.balance * loanMultiple;
        printf("Status: ELIGIBLE\n");
        printf("Maximum Eligible Loan Amount: %.2f\n\n", maxLoan);
    } else {
        printf("Status: NOT ELIGIBLE\n");
        printf("Reason: Balance is below the minimum threshold of %.2f\n\n", minBalanceThreshold);
    }
}