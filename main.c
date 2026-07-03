#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TRACE_CREATE
#define TRACE_STEP(label) fprintf(stderr, "TRACE: %s\n", label)
#else
#define TRACE_STEP(label) ((void)0)
#endif

#define DATA_FILE "Bank.dat"
#define MAX_ACCOUNTS 500
#define ACCOUNT_NO_LEN 32
#define NAME_LEN 80
#define MONTH_LEN 16
#define LINE_LEN 256

typedef struct {
    char account_no[ACCOUNT_NO_LEN];
    char name[NAME_LEN];
    char month[MONTH_LEN];
    int day;
    int year;
    long balance;
} Account;

static const char *MONTHS[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static void copy_text(char *destination, size_t size, const char *source) {
    if (size == 0) {
        return;
    }
    strncpy(destination, source, size - 1);
    destination[size - 1] = '\0';
}

static void strip_newline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

static void read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        putchar('\n');
        exit(EXIT_SUCCESS);
    }
    strip_newline(buffer);
}

static int is_digits(const char *text) {
    if (text[0] == '\0') {
        return 0;
    }
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (!isdigit((unsigned char)text[i])) {
            return 0;
        }
    }
    return 1;
}

static long read_positive_amount(const char *prompt) {
    char input[64];
    char *end = NULL;
    long value;

    while (1) {
        read_line(prompt, input, sizeof(input));
        value = strtol(input, &end, 10);
        if (input[0] != '\0' && *end == '\0' && value > 0) {
            return value;
        }
        printf("Please enter a positive whole number.\n");
    }
}

static void read_account_number(char *account_no, size_t size) {
    while (1) {
        read_line("Account No: ", account_no, size);
        if (is_digits(account_no)) {
            return;
        }
        printf("Account number must contain digits only.\n");
    }
}

static void read_name(char *name, size_t size) {
    while (1) {
        read_line("Person Name: ", name, size);
        if (name[0] != '\0') {
            return;
        }
        printf("Please provide the customer name.\n");
    }
}

static void read_date(char *month, size_t month_size, int *day, int *year) {
    char input[64];
    int choice;

    puts("Month:");
    for (int i = 0; i < 12; i++) {
        printf("  %2d. %s\n", i + 1, MONTHS[i]);
    }

    while (1) {
        read_line("Choose month (1-12): ", input, sizeof(input));
        choice = atoi(input);
        if (choice >= 1 && choice <= 12) {
            copy_text(month, month_size, MONTHS[choice - 1]);
            break;
        }
        printf("Please choose a month from 1 to 12.\n");
    }

    while (1) {
        read_line("Day (1-31): ", input, sizeof(input));
        *day = atoi(input);
        if (*day >= 1 && *day <= 31) {
            break;
        }
        printf("Please enter a day from 1 to 31.\n");
    }

    while (1) {
        read_line("Year (2000-2099): ", input, sizeof(input));
        *year = atoi(input);
        if (*year >= 2000 && *year <= 2099) {
            break;
        }
        printf("Please enter a year from 2000 to 2099.\n");
    }
}

static int load_accounts(Account accounts[], int *count) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[LINE_LEN];

    *count = 0;
    if (file == NULL) {
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL && *count < MAX_ACCOUNTS) {
        Account current;
        char *token;

        strip_newline(line);
        token = strtok(line, "|");
        if (token == NULL) {
            continue;
        }
        copy_text(current.account_no, sizeof(current.account_no), token);

        token = strtok(NULL, "|");
        if (token == NULL) {
            continue;
        }
        copy_text(current.name, sizeof(current.name), token);

        token = strtok(NULL, "|");
        if (token == NULL) {
            continue;
        }
        copy_text(current.month, sizeof(current.month), token);

        token = strtok(NULL, "|");
        if (token == NULL) {
            continue;
        }
        current.day = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) {
            continue;
        }
        current.year = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) {
            continue;
        }
        current.balance = atol(token);

        accounts[*count] = current;
        (*count)++;
    }

    fclose(file);
    return 1;
}

static int save_accounts(const Account accounts[], int count) {
    FILE *file = fopen(DATA_FILE, "w");
    if (file == NULL) {
        printf("Could not open %s for writing.\n", DATA_FILE);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s|%s|%s|%d|%d|%ld\n",
                accounts[i].account_no,
                accounts[i].name,
                accounts[i].month,
                accounts[i].day,
                accounts[i].year,
                accounts[i].balance);
    }

    fclose(file);
    return 1;
}

static int find_by_account_no(const Account accounts[], int count, const char *account_no) {
    for (int i = 0; i < count; i++) {
        if (strcmp(accounts[i].account_no, account_no) == 0) {
            return i;
        }
    }
    return -1;
}

static void print_account(const Account *account) {
    printf("\nAccount No.       : %s\n", account->account_no);
    printf("Customer Name     : %s\n", account->name);
    printf("Last Transaction  : %s, %d, %d\n", account->month, account->day, account->year);
    printf("Current Balance   : %ld\n\n", account->balance);
}

static void create_account(Account accounts[], int *count) {
    Account *account = &accounts[*count];

    TRACE_STEP("create:start");
    if (*count >= MAX_ACCOUNTS) {
        printf("Account limit reached.\n");
        return;
    }

    read_account_number(account->account_no, sizeof(account->account_no));
    TRACE_STEP("create:account_no");
    if (find_by_account_no(accounts, *count, account->account_no) != -1) {
        printf("Account No. %s already exists.\n", account->account_no);
        return;
    }

    read_name(account->name, sizeof(account->name));
    TRACE_STEP("create:name");
    read_date(account->month, sizeof(account->month), &account->day, &account->year);
    TRACE_STEP("create:date");
    account->balance = read_positive_amount("Opening Deposit Amount: ");
    TRACE_STEP("create:amount");

    (*count)++;
    TRACE_STEP("create:before_save");
    if (save_accounts(accounts, *count)) {
        TRACE_STEP("create:after_save");
        printf("The record has been saved successfully.\n");
    }
    TRACE_STEP("create:end");
}

static void search_account(const Account accounts[], int count) {
    char account_no[ACCOUNT_NO_LEN];
    int index;

    if (count == 0) {
        printf("Records file is empty. Enter records first.\n");
        return;
    }

    read_account_number(account_no, sizeof(account_no));
    index = find_by_account_no(accounts, count, account_no);
    if (index == -1) {
        printf("Account No. %s doesn't exist.\n", account_no);
        return;
    }
    print_account(&accounts[index]);
}

static void search_by_name(const Account accounts[], int count) {
    char query[NAME_LEN];
    int found = 0;

    if (count == 0) {
        printf("Records file is empty. Enter records first.\n");
        return;
    }

    read_line("Customer name to search: ", query, sizeof(query));
    for (int i = 0; i < count; i++) {
        if (strstr(accounts[i].name, query) != NULL) {
            print_account(&accounts[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("No customer found with name containing \"%s\".\n", query);
    }
}

static void deposit_money(Account accounts[], int count) {
    char account_no[ACCOUNT_NO_LEN];
    long deposit;
    int index;

    if (count == 0) {
        printf("Records file is empty. Enter records first.\n");
        return;
    }

    read_account_number(account_no, sizeof(account_no));
    index = find_by_account_no(accounts, count, account_no);
    if (index == -1) {
        printf("Account No. %s doesn't exist.\n", account_no);
        return;
    }

    printf("Customer: %s\n", accounts[index].name);
    read_date(accounts[index].month, sizeof(accounts[index].month),
              &accounts[index].day, &accounts[index].year);
    deposit = read_positive_amount("Deposit Amount: ");
    accounts[index].balance += deposit;

    if (save_accounts(accounts, count)) {
        printf("The file is updated successfully.\n");
        print_account(&accounts[index]);
    }
}

static void withdraw_money(Account accounts[], int count) {
    char account_no[ACCOUNT_NO_LEN];
    long withdraw;
    int index;

    if (count == 0) {
        printf("Records file is empty. Enter records first.\n");
        return;
    }

    read_account_number(account_no, sizeof(account_no));
    index = find_by_account_no(accounts, count, account_no);
    if (index == -1) {
        printf("Account No. %s doesn't exist.\n", account_no);
        return;
    }

    printf("Customer: %s\n", accounts[index].name);
    if (accounts[index].balance == 0) {
        printf("%s doesn't have any amount in balance.\n", accounts[index].name);
        return;
    }

    withdraw = read_positive_amount("Withdraw Amount: ");
    if (withdraw > accounts[index].balance) {
        printf("Withdraw amount can't be greater than actual balance.\n");
        return;
    }

    read_date(accounts[index].month, sizeof(accounts[index].month),
              &accounts[index].day, &accounts[index].year);
    accounts[index].balance -= withdraw;

    if (save_accounts(accounts, count)) {
        printf("The file is updated successfully.\n");
        print_account(&accounts[index]);
    }
}

static void delete_customer(Account accounts[], int *count) {
    char account_no[ACCOUNT_NO_LEN];
    char confirmation[8];
    int index;

    if (*count == 0) {
        printf("Records file is empty. Enter records first.\n");
        return;
    }

    read_account_number(account_no, sizeof(account_no));
    index = find_by_account_no(accounts, *count, account_no);
    if (index == -1) {
        printf("Account No. %s doesn't exist.\n", account_no);
        return;
    }

    print_account(&accounts[index]);
    read_line("Delete this record? (y/n): ", confirmation, sizeof(confirmation));
    if (tolower((unsigned char)confirmation[0]) != 'y') {
        printf("Deletion cancelled.\n");
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        accounts[i] = accounts[i + 1];
    }
    (*count)--;

    if (save_accounts(accounts, *count)) {
        printf("Record has been deleted successfully.\n");
    }
}

static void view_all_customers(const Account accounts[], int count) {
    if (count == 0) {
        printf("Records file is empty. Enter records to display.\n");
        return;
    }

    printf("\n%-15s %-25s %-24s %12s\n", "Account No.", "Customer Name", "Last Transaction", "Balance");
    printf("-------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        char date[48];
        snprintf(date, sizeof(date), "%s, %d, %d", accounts[i].month, accounts[i].day, accounts[i].year);
        printf("%-15s %-25s %-24s %12ld\n",
               accounts[i].account_no,
               accounts[i].name,
               date,
               accounts[i].balance);
    }
    putchar('\n');
}

static void print_balance_report(const Account accounts[], int count) {
    char account_no[ACCOUNT_NO_LEN];
    int index;

    if (count == 0) {
        printf("Records file is empty. Enter records first.\n");
        return;
    }

    read_account_number(account_no, sizeof(account_no));
    index = find_by_account_no(accounts, count, account_no);
    if (index == -1) {
        printf("Account No. %s doesn't exist.\n", account_no);
        return;
    }

    puts("\n               BankSystem [Pvt] Limited.");
    puts(" -----------------------------------------------------------");
    puts("               Customer Balance Report.\n");
    printf("  Account No.:       %s\n", accounts[index].account_no);
    puts(" -----------------------------------------------------------");
    printf("  Customer Name:     %s\n", accounts[index].name);
    puts(" -----------------------------------------------------------");
    printf("  Last Transaction:  %s, %d, %d\n",
           accounts[index].month, accounts[index].day, accounts[index].year);
    puts(" -----------------------------------------------------------");
    printf("  Current Balance:   %ld\n", accounts[index].balance);
    puts(" -----------------------------------------------------------\n");
}

static int menu_choice(void) {
    char input[32];
    char *end = NULL;
    long choice;

    puts("BankSystem [Pvt] Limited.");
    puts("1. Open New Account");
    puts("2. Deposit Money");
    puts("3. Withdraw Money");
    puts("4. Delete Customer");
    puts("5. Search By Account No.");
    puts("6. Search By Name");
    puts("7. View All Customers");
    puts("8. Print Customer Balance Report");
    puts("0. Quit");
    read_line("Choose an option: ", input, sizeof(input));
    choice = strtol(input, &end, 10);
    if (input[0] == '\0' || *end != '\0') {
        return -1;
    }
    return (int)choice;
}

int main(void) {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    int running = 1;

    if (!load_accounts(accounts, &count)) {
        return EXIT_FAILURE;
    }

    while (running) {
        int choice;
        putchar('\n');
        choice = menu_choice();
        putchar('\n');

        switch (choice) {
            case 1:
                create_account(accounts, &count);
                break;
            case 2:
                deposit_money(accounts, count);
                break;
            case 3:
                withdraw_money(accounts, count);
                break;
            case 4:
                delete_customer(accounts, &count);
                break;
            case 5:
                search_account(accounts, count);
                break;
            case 6:
                search_by_name(accounts, count);
                break;
            case 7:
                view_all_customers(accounts, count);
                break;
            case 8:
                print_balance_report(accounts, count);
                break;
            case 0:
                running = 0;
                puts("Thanks for using BankSystem.");
                break;
            default:
                puts("Please choose a valid option.");
                break;
        }
    }

    return EXIT_SUCCESS;
}
