// libraries
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>  // for printing in the screen
#include <stdlib.h> // for expecting an input from the user from his/her screen
#include <string.h> // for getchar()
#include <unistd.h>
// variables
double remainingAllowance = 0.0;
char currentWalletName[100];
typedef struct {
    char gender[3];
    char specified[10];
    char walletName[100];
    char username[100];
    double allowance;
} WalletData;
// declared promised functions
int finalPrompts(char *buffer, size_t size, char *userName, char *userPrompt);
void saveAllowance(void);
void loadAllowance(void);
bool saveWalletData(WalletData *data);
bool loadWalletData(WalletData *data);
void command(char *buffer, size_t size, const char *format, ...);
int spend(void);
int add(void);
double getDoubleFromInput(const char *prompt);
void getGenderandConfirmWalletName(char *gender, char *specified, char *funcWalletName);
void confirmWalletName(char *specified, char *currentWalletName);
void confirmtryagain(char *confirmChar, char *funcWalletName, bool *tryAgain, char *tempName);
double getWalletAmount(char *currentWalletName);
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
// main wallet program function
int main(void)
{

    loadAllowance(); // load saved allowance at program start
    char gender[3];
    char specified[10] = "";
    char begin[20];
    char last[20];
    char fullName[50];

    static bool entryState = true;
   WalletData user;  // holds gender, specified, walletName, allowance

if (entryState == true)
{
    if (loadWalletData(&user)) {
        // file found: returning user
        printf("\033[1;32mWelcome back, %s%s! Your wallet \"%s\" has %.2f left.\033[0m\n",
               user.specified,user.username, user.walletName, user.allowance);
        remainingAllowance = user.allowance;
    } else {
        // first-time user setup
        getGenderandConfirmWalletName(user.gender, user.specified, user.walletName);
        user.allowance = getWalletAmount(user.walletName);
        saveWalletData(&user);
        sleep(1);
    command(begin, sizeof(begin),
            "\033[1;33mplease enter your first "
            "name:\033[0m ");
    command(last, sizeof(last),
            "\033[1;33mand your last "
            "name:\033[0m ");
    snprintf(fullName, sizeof(fullName), "%s %s", begin, last);
    if (strlen(begin) == 0 || strlen(last) == 0)
    {
        sleep(1);
        strcpy(fullName, "user");
        printf("you didnt enter your full name, try again\n");
        entryState = false;
        return main();
    }
    else
    {
        printf("\033[1;32mHello, %s%s!\033[0m\n", specified, fullName);
        sleep(1);
    }
    }

    clearInputBuffer();
}


    while (1)
    {
        char prompt[50];
        command(prompt, sizeof(prompt),
                "\033[1;33myou could now manage your wallet's \"%s\" expenses by either: "
                "\033[1;33mTyping \"spend\" to add spending, \"add\" to increase your allowance, "
                "\"review\" to "
                "see allowance, or \"exit\" to quit: \033[0m",
                currentWalletName);
        if (strcmp(prompt, "exit") == 0)
        {
            printf("\033[1;32mSee you later %s%s!\033[0m\n", specified, begin);
            saveAllowance();
            sleep(1);
            break;
        }
        else if (strcmp(prompt, "review") == 0)
        {
            printf("Your remaining allowance is: \033[1;32m%.2f\033[0m\n", remainingAllowance);
            char specialInternalStr[20];
            finalPrompts(specialInternalStr, sizeof(specialInternalStr), begin, prompt);
            break;
        }
        else if (strcmp(prompt, "add") == 0)
        {
            add();
            char specialInternalStr[20];
            finalPrompts(specialInternalStr, sizeof(specialInternalStr), begin, prompt);
            break;
        }
        else if (strcmp(prompt, "spend") == 0)

        {
            spend();
        }
        else
        {
            printf("\033[1;31mCommand: %s is not valid\033[0m\n", prompt);
        }
    }

    return 0;
}

// DRY principle functions

void saveAllowance()
{
    FILE *file = fopen("wallet.txt", "w");
    if (file)
    {
        fprintf(file, "%lf", remainingAllowance);
        fclose(file);
    }
}

void loadAllowance()
{
    FILE *file = fopen("wallet.txt", "r");
    if (file)
    {
        fscanf(file, "%lf", &remainingAllowance);
        fclose(file);
    }
}

bool saveWalletData(WalletData *data) {
    FILE *file = fopen("wallet_data.txt", "w");
    if (!file) return false;

    fprintf(file, "%s\n%s\n%s\n%s\n%lf\n",
            data->gender,
            data->specified,
            data->walletName,
            data->username,
            data->allowance);
    fclose(file);
    return true;
}

bool loadWalletData(WalletData *data) {
    FILE *file = fopen("wallet_data.txt", "r");
    if (!file) return false;

    if (fscanf(file, "%2s\n%9[^\n]\n%99[^\n]\n%99[^\n]\n%lf\n",
               data->gender,
               data->specified,
               data->walletName,
               data->username,
               &data->allowance) != 5) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

void command(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    if (format != NULL)
    {
        vprintf(format, args); // like printf but takes va_list
        fflush(stdout);
    }

    va_end(args);

    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // remove newline
}

double getDoubleFromInput(const char *prompt)
{
    char buffer[50];
    command(buffer, sizeof(buffer), prompt);

    char *endptr;
    double value = strtod(buffer, &endptr);

    if (endptr == buffer || *endptr != '\0')
    {
        printf("\033[1;31mInvalid number, please try again.\033[0m\n");
        return getDoubleFromInput(prompt);
    }
    else if (value == 0.0)
    {
        printf("\033[1;33mYou entered 0 — please enter a number greater than 0.\033[0m\n");
        return getDoubleFromInput(prompt);
    }

    return value;
}

int spend(void)
{
    double f = getDoubleFromInput("\033[1;33mEnter today's spending:\033[0m ");
    remainingAllowance -= f;
    printf("\033[1;32mYou spent %.2f from your wallet\033[0m\n", f);
    saveAllowance();
    return 0;
}

int add(void)
{
    double f = getDoubleFromInput(
        "\033[1;33mEnter how much you want to increase your allowance by:\033[0m ");
    remainingAllowance += f;
    printf("\033[1;32maddition successful\033[0m\n");
    sleep(1);
    printf("\033[1;32mYou added %.1f from your wallet, your allowance is %.2f now\033[0m\n", f,
           remainingAllowance);
    saveAllowance();
    return 0;
}

int finalPrompts(char *buffer, size_t size, char *userName, char *userPrompt)
{
    sleep(1);
    command(buffer, size,
            "\033[1;31mfinally: would you like to spend or add or exit ? and after what "
            "you choose the program will end\033[0m,\n\033[1;33mType \"spend\" to add "
            "spending, \"add\" to "
            "increase your allowance, or \"exit\" to quit: \033[0m");
    if (strcmp(buffer, "spend") == 0)
    {
        spend();
        saveAllowance();
    }
    else if (strcmp(buffer, "add") == 0)
    {
        add();
        saveAllowance();
    }
    else if (strcmp(buffer, "exit") == 0)
    {
        printf("\033[1;32mSee you later, %s!\033[0m\n", userName);
        saveAllowance();
    }
    else
    {
        printf("\033[1;31mCommand: %s is not valid\033[0m\n", userPrompt);
        sleep(1);
    }
    return 0;
}

void getGenderandConfirmWalletName(char *gender, char *specified, char *funcWalletName)
{
    command(gender, sizeof(gender), "\033[1;33mFirst specify your gender (M/F):\033[0m ");
    gender[0] = toupper(gender[0]);

    if (gender[0] == 'F')
    {
        printf("Gender entered: female\n");
        strcpy(specified, "Mrs. ");
    }
    else if (gender[0] == 'M')
    {
        printf("Gender entered: male\n");
        strcpy(specified, "Mr. ");
    }
    else
    {
        printf("\033[1;31mInvalid input, please try again.\033[0m\n");
        return getGenderandConfirmWalletName(gender, specified, funcWalletName);
    }

    confirmWalletName(specified, funcWalletName);
}

void confirmWalletName(char *specified, char *funcWalletName)
{
    static bool checkValidation = false;
    bool tryAgain = false;
    char tempName[100];
    char confirmChar[2];

    if (!checkValidation)
    {
        printf("\033[1;31mNow you need to specify a name for your wallet.\033[0m\n");
        command(tempName, sizeof(tempName),
                "\033[1;33mEnter a name for your wallet please:\033[0m ");
        checkValidation = true;
    }
    else
    {
        strcpy(tempName, funcWalletName); // in case it's already set
    }

    command(confirmChar, sizeof(confirmChar),
            "\033[1;31mIf you are sure you want your wallet's name to be '%s', type 'y', if "
            "you're not type 'n':\033[0m ",
            tempName);

    confirmChar[0] = tolower(confirmChar[0]);

    if (confirmChar[0] != 'y' && confirmChar[0] != 'n')
    {
        printf("\033[1;31mCommand is not valid, try again.\033[0m\n");
        tryAgain = true;
        confirmWalletName(specified, funcWalletName);
        return;
    }

    confirmtryagain(confirmChar, funcWalletName, &tryAgain, tempName);
}

void confirmtryagain(char *confirmChar, char *funcWalletName, bool *tryAgain, char *tempName)
{
    if (confirmChar[0] == 'y')
    {
        *tryAgain = false;
        strcpy(currentWalletName, tempName);
        printf("\033[1;32mYour current wallet name is '%s'\033[0m\n", currentWalletName);
    }
    else if (confirmChar[0] == 'n')
    {
        *tryAgain = false;
        printf("\033[1;33mLet's choose another wallet name.\033[0m\n");
        confirmWalletName(NULL, funcWalletName); // re-run naming
    }
}

double getWalletAmount(char *funcWalletName)
{
    double walletAllowanceVal = getDoubleFromInput("\033[1;33mspecify your wallet allowance please:\033[0m ");

    if (walletAllowanceVal > 0.0)
    {
        printf("\033[1;32mYour wallet '%s' now starts with %.2f allowance.\033[0m\n",
               currentWalletName, walletAllowanceVal);
        remainingAllowance = walletAllowanceVal; // store globally
        saveAllowance();
    }
    else
    {
        printf("\033[1;31mInvalid amount entered.\033[0m\n");
    }

    return walletAllowanceVal;
}
