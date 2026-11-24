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
bool saveWalletData(WalletData *data);
bool loadWalletData(WalletData *data);
void command(char *buffer, size_t size, const char *format, ...);
int spend(void);
int add(void);
double getDoubleFromInput(const char *prompt, bool trying);
void getGenderandConfirmWalletName(char *gender, size_t genderSize,
                                   char *specified, size_t specifiedSize,
                                   char *funcWalletName, size_t walletSize);
void confirmWalletName(char *specified, char *funcWalletName, bool isRetry);
double getWalletAmount(char *currentWalletName);
// ==============================================================
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
// main wallet program function
int main(void)
{


     char begin[20] = "";
    char last[20]  = "";
    char fullName[50] = "";

    WalletData user = {0}; // zero-initialize — no garbage memory

    if (loadWalletData(&user))
    {
        // — Returning user —
        printf("\033[1;32mWelcome back, %s%s! Your wallet \"%s\" has %.2f left.\033[0m\n",
               user.specified, user.username, user.walletName, user.allowance);

        remainingAllowance = user.allowance;

        // populate locals so exit/review messages work correctly
        strncpy(begin, user.username, sizeof(begin) - 1);
    }
    else
    {
        // — First-time setup —
        getGenderandConfirmWalletName(user.gender,    sizeof(user.gender),
                                      user.specified, sizeof(user.specified),
                                      user.walletName, sizeof(user.walletName));

        user.allowance = getWalletAmount(user.walletName);
        sleep(1);

        // name input loop — replaces return main()
        while (1)
        {
            command(begin, sizeof(begin),
                    "\033[1;33mplease enter your first name:\033[0m ");
            command(last,  sizeof(last),
                    "\033[1;33mand your last name:\033[0m ");

            if (strlen(begin) == 0 || strlen(last) == 0)
            {
                printf("\033[1;31mYou didn't enter your full name, try again.\033[0m\n");
                sleep(1);
                continue;
            }
            break;
        }

        // build fullName and store in user.username BEFORE the single save
        snprintf(fullName,      sizeof(fullName),      "%s %s", begin, last);
        snprintf(user.username, sizeof(user.username), "%s %s", begin, last);

        saveWalletData(&user); // ← one save, after everything is populated

        printf("\033[1;32mHello, %s%s!\033[0m\n", user.specified, fullName);
        sleep(1);
    }



    // — Main command loop —
    while (1)
    {
        char prompt[50];
        command(prompt, sizeof(prompt),
                "\033[1;33mManage your wallet \"%s\": type \"spend\", \"add\", "
                "\"review\", or \"exit\":\033[0m ",
                user.walletName); // ← user.walletName, not currentWalletName

        if (strcmp(prompt, "exit") == 0)
        {
            printf("\033[1;32mSee you later, %s%s!\033[0m\n", user.specified, begin);
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
        }
        else if (strcmp(prompt, "spend") == 0)
        {
            spend();
        }
        else
        {
            printf("\033[1;31mCommand: \"%s\" is not valid.\033[0m\n", prompt);
        }
    }

    return 0;
}

// DRY principle functions

void saveAllowance(void)
{
    WalletData data;
    if (loadWalletData(&data)) {
        data.allowance = remainingAllowance;
        saveWalletData(&data);
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

    if (fgets(buffer, size, stdin) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
    }
    else
    {
        buffer[0] = '\0';
    }
}

double getDoubleFromInput(const char *prompt, bool trying)
{
    char buffer[50];
    command(buffer, sizeof(buffer), !trying ? prompt : "\033[1;31mPlease try again: \033[0m");

    char *endptr;
    double value = strtod(buffer, &endptr);
    if (endptr == buffer)
    {
        printf("\033[1;31mInvalid number, \033[0m");
        return getDoubleFromInput(prompt,true);
    }
    else if (value == 0.0)
    {
        printf("\033[1;31You entered 0 — enter a number greater than 0, \033[0m");

        return getDoubleFromInput(prompt, true);
    }else if (value < 0.0)
    {
        printf("\033[1;31mNegative numbers are not allowed, \033[0m");

        return getDoubleFromInput(prompt, true);
    }

    return value;
}

int spend(void)
{
    // need to not display the prompt if the user is trying again after an invalid input
    // and instead let the try again message be displayed with ": " and wait for user to enter a new value
    double f = getDoubleFromInput("\033[1;33mEnter today's spending:\033[0m ", false);
    if(remainingAllowance < f){
        printf("you cant spend that much, you have exceeded your limit.");
        printf("Your remaining allowance is: \033[1;32m%.2f\033[0m\n", remainingAllowance);
    }else{
        remainingAllowance -= f;
        printf("\033[1;32mYou spent %.2f from your wallet\033[0m\n", f);
            saveAllowance();
    }
    return 0;
}

int add(void)
{
    double f = getDoubleFromInput(
        "\033[1;33mEnter how much you want to increase your allowance by:\033[0m ", false);
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
    }
    else if (strcmp(buffer, "add") == 0)
    {
        add();
    }
    else if (strcmp(buffer, "exit") == 0)
    {
        printf("\033[1;32mSee you later, %s!\033[0m\n", userName);
    }
    else
    {
        printf("\033[1;31mCommand: %s is not valid\033[0m\n", userPrompt);
        sleep(1);
    }
        saveAllowance();

    return 0;
}

void getGenderandConfirmWalletName(char *gender, size_t genderSize,
                                   char *specified, size_t specifiedSize,
                                   char *funcWalletName, size_t walletSize)
{
    command(gender, genderSize, "\033[1;33mFirst specify your gender (M/F):\033[0m ");
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
        return getGenderandConfirmWalletName(gender,genderSize, specified,specifiedSize, funcWalletName,walletSize);
    }

   confirmWalletName(specified, funcWalletName, false);
}

void confirmWalletName(char *specified, char *funcWalletName, bool isRetry)
{
    char tempName[100];
    char confirmChar[3]; // ✅ big enough: 'y'/'n' + '\n' + '\0'

    if (!isRetry) {
        printf("\033[1;31mNow you need to specify a name for your wallet.\033[0m\n");
    }

    // ✅ always ask for name — prompt changes based on isRetry
    command(tempName, sizeof(tempName),
            isRetry
                ? "\033[1;33mEnter a different wallet name: \033[0m"
                : "\033[1;33mEnter a name for your wallet: \033[0m");

    // ✅ correct empty check
    if (strlen(tempName) == 0)
    {
        printf("\033[1;31mWallet name cannot be empty, try again.\033[0m\n");
        confirmWalletName(specified, funcWalletName, isRetry);
        return;
    }

    command(confirmChar, sizeof(confirmChar),
            "\033[1;31mConfirm '%s' as your wallet name? (y/n): \033[0m", tempName);

    confirmChar[0] = tolower(confirmChar[0]);

    if (confirmChar[0] == 'y')
    {
        strcpy(currentWalletName, tempName);
        strncpy(funcWalletName, tempName, 99);
        funcWalletName[99] = '\0';
    }
    else if (confirmChar[0] == 'n')
    {
        confirmWalletName(specified, funcWalletName, true);
    }
    else
    {
        printf("\033[1;31mInvalid input, try again.\033[0m\n");
        confirmWalletName(specified, funcWalletName, isRetry);
    }
}

double getWalletAmount(char *funcWalletName)
{
    double walletAllowanceVal = getDoubleFromInput("\033[1;33mspecify your wallet allowance please:\033[0m ", false);

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
