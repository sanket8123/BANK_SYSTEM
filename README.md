# BankSystem in C

This is a console-based C version of the original Java Swing banking system.

## Features

- Open a new account
- Deposit money
- Withdraw money
- Delete a customer
- Search by account number
- Search by customer name
- View all customers
- Print a customer balance report to the console

## Build

```sh
mingw32-make
```

If `make` is not installed, compile directly:

```sh
gcc -Wall -Wextra -std=c99 -pedantic main.c -o bank_system
```

## Run

```sh
bank_system.exe
```

The program stores records in `Bank.dat` in this folder using a simple text format:

```text
account_no|name|month|day|year|balance
```

Note: the original Java project used Java's binary `DataOutputStream.writeUTF` format. This C version uses a readable text file instead.
