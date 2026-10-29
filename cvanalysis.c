
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define MAX_SKILLS 15

typedef struct Candidate {
    int id;
    char name[MAX];
    char education[MAX];
    char objective[MAX];
    char project[MAX];
    int experience;
    int skillCount;
    char skills[MAX_SKILLS][MAX];
    struct Candidate *next;
} Candidate;

typedef struct QueueNode {
    int candidateId;
    struct QueueNode *next;
} QueueNode;

typedef struct History {
    int candidateId;
    char name[MAX];
    char role[MAX];
    float score;
    struct History *next;
} History;

/* Global data structure pointers */
Candidate *head = NULL;
QueueNode *front = NULL, *rear = NULL;
History *top = NULL;

int nextId = 1;

/* Remove newline from input */
void removeNewline(char str[]) {
    str[strcspn(str, "\n")] = '\0';
}

/* Safe string input */
void readString(const char *message, char str[], int size) {
    printf("%s", message);

    if (fgets(str, size, stdin) != NULL) {
        removeNewline(str);

        /* Clear remaining input if too long */
        if (str[strlen(str)] == '\0') {
            int ch;
            if (strlen(str) == size - 1) {
                while ((ch = getchar()) != '\n' && ch != EOF);
            }
        }
    } else {
        str[0] = '\0';
    }
}

/* Read an integer */
int readInt(const char *message) {
    char line[MAX];
    int value;
    char extra;

    while (1) {
        readString(message, line, MAX);

        if (sscanf(line, "%d %c", &value, &extra) == 1)
            return value;

        printf("Enter a valid whole number.\n");
    }
}

/* Case-insensitive string comparison */
int sameString(const char a[], const char b[]) {
    int i = 0;

    while (a[i] && b[i]) {
        if (tolower((unsigned char)a[i]) !=
            tolower((unsigned char)b[i]))
            return 0;
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

/* Search candidate by ID in linked list */
Candidate *findCandidate(int id) {
    Candidate *temp = head;

    while (temp != NULL) {
        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}

/* Add a candidate CV */
void addCandidate(void) {
    Candidate *newNode, *temp;
    int i;

    newNode = (Candidate *)malloc(sizeof(Candidate));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newNode->id = nextId++;

    printf("\n--- ENTER CANDIDATE DETAILS ---\n");

    readString("Candidate name: ",
               newNode->name, MAX);

    readString("Education / degree: ",
               newNode->education, MAX);

    readString("Career objective: ",
               newNode->objective, MAX);

    newNode->experience = readInt(
        "Years of experience (0 for fresher): ");

    if (newNode->experience < 0) {
        printf("Experience cannot be negative.\n");
        free(newNode);
        return;
    }

    readString("Project description: ",
               newNode->project, MAX);

    newNode->skillCount = readInt(
        "Number of skills (0 to 15): ");

    if (newNode->skillCount < 0 ||
        newNode->skillCount > MAX_SKILLS) {
        printf("Invalid number of skills.\n");
        free(newNode);
        return;
    }

    for (i = 0; i < newNode->skillCount; i++) {
        char prompt[60];

        snprintf(prompt, sizeof(prompt),
                 "Enter skill %d: ", i + 1);

        readString(prompt, newNode->skills[i], MAX);
    }

    newNode->next = NULL;

    /* Insert at end of linked list */
    if (head == NULL) {
        head = newNode;
    } else {
        temp = head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newNode;
    }

    printf("\nCV added successfully! Candidate ID: %d\n",
           newNode->id);
}

/* Display all candidate CVs */
void displayCandidates(void) {
    Candidate *temp = head;
    int i;

    if (head == NULL) {
        printf("\nNo CVs available.\n");
        return;
    }

    printf("\n========== CANDIDATE CV LIST ==========\n");

    while (temp != NULL) {
        printf("\nCandidate ID : %d\n", temp->id);
        printf("Name         : %s\n", temp->name);
        printf("Education    : %s\n", temp->education);
        printf("Experience   : %d years\n", temp->experience);
        printf("Objective    : %s\n",
               strlen(temp->objective) ?
               temp->objective : "Not provided");
        printf("Project      : %s\n",
               strlen(temp->project) ?
               temp->project : "Not provided");

        printf("Skills       : ");

        if (temp->skillCount == 0)
            printf("None");

        for (i = 0; i < temp->skillCount; i++) {
            printf("%s", temp->skills[i]);

            if (i < temp->skillCount - 1)
                printf(", ");
        }

        printf("\n---------------------------------------\n");
        temp = temp->next;
    }
}

/* Add a candidate ID to the analysis queue */
void enqueueCV(void) {
    int id;
    QueueNode *newNode;

    if (head == NULL) {
        printf("\nAdd a CV before using the queue.\n");
        return;
    }

    displayCandidates();
    id = readInt("\nEnter candidate ID to queue: ");

    if (findCandidate(id) == NULL) {
        printf("Candidate ID not found.\n");
        return;
    }

    newNode = (QueueNode *)malloc(sizeof(QueueNode));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newNode->candidateId = id;
    newNode->next = NULL;

    if (rear == NULL) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }

    printf("CV added to analysis queue.\n");
}

/* Display queued CVs */
void displayQueue(void) {
    QueueNode *temp = front;

    if (temp == NULL) {
        printf("\nAnalysis queue is empty.\n");
        return;
    }

    printf("\n--- CV ANALYSIS QUEUE (FIFO) ---\n");

    while (temp != NULL) {
        Candidate *c = findCandidate(temp->candidateId);

        if (c != NULL)
            printf("ID: %d | Name: %s\n",
                   c->id, c->name);

        temp = temp->next;
    }
}

/* Push an analysis result onto the history stack */
void pushHistory(int id, const char name[],
                 const char role[], float score) {
    History *newNode;

    newNode = (History *)malloc(sizeof(History));

    if (newNode == NULL) {
        printf("Could not save analysis history.\n");
        return;
    }

    newNode->candidateId = id;
    strcpy(newNode->name, name);
    strcpy(newNode->role, role);
    newNode->score = score;

    newNode->next = top;
    top = newNode;
}

/* Perform analysis for the next queued CV */
void analyseNextCV(void) {
    QueueNode *q;
    Candidate *c;
    int requiredCount, i, j;
    int matched = 0;
    int minExperience, requireProject;
    int educationMatch, experienceMatch, projectMatch;
    float skillScore, totalScore;
    char role[MAX], requiredEducation[MAX];
    char requiredSkills[MAX_SKILLS][MAX];

    if (front == NULL) {
        printf("\nNo CVs in the analysis queue.\n");
        return;
    }

    /* Remove the first queue node (FIFO) */
    q = front;
    front = front->next;

    if (front == NULL)
        rear = NULL;

    c = findCandidate(q->candidateId);
    free(q);

    if (c == NULL) {
        printf("Candidate no longer exists.\n");
        return;
    }

    printf("\n========== SMART CV ANALYSIS ==========\n");
    printf("Candidate: %s (ID: %d)\n", c->name, c->id);

    readString("Enter job role: ", role, MAX);

    readString("Required education keyword "
               "(or type ANY): ",
               requiredEducation, MAX);

    minExperience = readInt(
        "Minimum required experience in years: ");

    if (minExperience < 0) {
        printf("Experience requirement cannot be negative.\n");
        return;
    }

    requiredCount = readInt(
        "Number of required skills (0 to 15): ");

    if (requiredCount < 0 ||
        requiredCount > MAX_SKILLS) {
        printf("Invalid number of required skills.\n");
        return;
    }

    for (i = 0; i < requiredCount; i++) {
        char prompt[60];

        snprintf(prompt, sizeof(prompt),
                 "Required skill %d: ", i + 1);

        readString(prompt, requiredSkills[i], MAX);
    }

    requireProject = readInt(
        "Is a project required? (1=Yes, 0=No): ");

    if (requireProject != 0 && requireProject != 1) {
        printf("Enter only 1 or 0.\n");
        return;
    }

    /* Match candidate skills with job skills */
    for (i = 0; i < requiredCount; i++) {
        for (j = 0; j < c->skillCount; j++) {
            if (sameString(requiredSkills[i],
                           c->skills[j])) {
                matched++;
                break;
            }
        }
    }

    /* Weighted scoring */
    if (requiredCount == 0)
        skillScore = 70.0f;
    else
        skillScore = (70.0f * matched) / requiredCount;

    educationMatch =
        sameString(requiredEducation, "ANY") ||
        strstr(c->education, requiredEducation) != NULL;

    /* Retry with case-insensitive matching for education */
    if (!educationMatch &&
        strlen(requiredEducation) > 0) {
        char educationCopy[MAX];
        char keywordCopy[MAX];

        strcpy(educationCopy, c->education);
        strcpy(keywordCopy, requiredEducation);

        for (i = 0; educationCopy[i]; i++)
            educationCopy[i] =
                (char)tolower((unsigned char)educationCopy[i]);

        for (i = 0; keywordCopy[i]; i++)
            keywordCopy[i] =
                (char)tolower((unsigned char)keywordCopy[i]);

        educationMatch =
            strstr(educationCopy, keywordCopy) != NULL;
    }

    experienceMatch = c->experience >= minExperience;
    projectMatch = !requireProject ||
                   strlen(c->project) > 0;

    totalScore = skillScore;

    if (educationMatch)
        totalScore += 10.0f;

    if (experienceMatch)
        totalScore += 10.0f;

    if (projectMatch)
        totalScore += 10.0f;

    printf("\n========== ANALYSIS REPORT ==========\n");
    printf("Candidate       : %s\n", c->name);
    printf("Job role        : %s\n", role);
    printf("Required skills : %d\n", requiredCount);
    printf("Matched skills  : %d\n", matched);
    printf("Skill score     : %.2f / 70\n", skillScore);
    printf("Education       : %s\n",
           educationMatch ? "MATCH" : "NOT MATCHED");
    printf("Experience      : %s\n",
           experienceMatch ? "MATCH" : "NOT MATCHED");
    printf("Project         : %s\n",
           projectMatch ? "MATCH" : "MISSING");

    printf("Overall score   : %.2f / 100\n", totalScore);

    if (totalScore >= 80)
        printf("Result          : STRONG MATCH\n");
    else if (totalScore >= 60)
        printf("Result          : MODERATE MATCH\n");
    else
        printf("Result          : LOW MATCH\n");

    printf("\n--- IMPROVEMENT SUGGESTIONS ---\n");

    if (requiredCount == 0)
        printf("- Add job-specific skills for meaningful skill scoring.\n");

    for (i = 0; i < requiredCount; i++) {
        int found = 0;

        for (j = 0; j < c->skillCount; j++) {
            if (sameString(requiredSkills[i], c->skills[j])) {
                found = 1;
                break;
            }
        }

        if (!found)
            printf("- Consider developing the skill: %s\n",
                   requiredSkills[i]);
    }

    if (!educationMatch)
        printf("- Review the education requirement for this role.\n");

    if (!experienceMatch)
        printf("- Gain relevant experience or practical exposure.\n");

    if (!strlen(c->objective))
        printf("- Add a clear career objective to the CV.\n");

    if (!strlen(c->project))
        printf("- Add a relevant academic or personal project.\n");

    if (educationMatch && experienceMatch && projectMatch &&
        matched == requiredCount)
        printf("- Your profile meets all entered criteria.\n");

    pushHistory(c->id, c->name, role, totalScore);
}

/* Display recent analysis history (LIFO stack) */
void displayHistory(void) {
    History *temp = top;

    if (temp == NULL) {
        printf("\nNo analysis history available.\n");
        return;
    }

    printf("\n--- RECENT ANALYSIS HISTORY (LIFO) ---\n");

    while (temp != NULL) {
        printf("Candidate ID: %d | Name: %s | "
               "Role: %s | Score: %.2f%%\n",
               temp->candidateId, temp->name,
               temp->role, temp->score);

        temp = temp->next;
    }
}

/* Release dynamically allocated memory */
void freeMemory(void) {
    Candidate *c;
    QueueNode *q;
    History *h;

    while (head != NULL) {
        c = head;
        head = head->next;
        free(c);
    }

    while (front != NULL) {
        q = front;
        front = front->next;
        free(q);
    }

    rear = NULL;

    while (top != NULL) {
        h = top;
        top = top->next;
        free(h);
    }
}

/* Main menu */
int main(void) {
    int choice;

    do {
        printf("\n\n====================================\n");
        printf("       SMART CV ANALYSER IN C\n");
        printf("====================================\n");
        printf("1. Add Candidate CV\n");
        printf("2. Display All CVs\n");
        printf("3. Add CV to Analysis Queue\n");
        printf("4. Display Analysis Queue\n");
        printf("5. Analyse Next CV\n");
        printf("6. Display Analysis History\n");
        printf("7. Exit\n");
        printf("====================================\n");

        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1:
                addCandidate();
                break;

            case 2:
                displayCandidates();
                break;

            case 3:
                enqueueCV();
                break;

            case 4:
                displayQueue();
                break;

            case 5:
                analyseNextCV();
                break;

            case 6:
                displayHistory();
                break;

            case 7:
                freeMemory();
                printf("Thank you for using Smart CV Analyser!\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 7);

    return 0;
}
