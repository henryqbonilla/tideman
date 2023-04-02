#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool checkForCycle(pair p, int candidateCycleCheck, int pairNumber);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        if(strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        int candidate_i_index = 0;
        int candidate_j_index = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            //makes sure candidate does compare against itself
            if(i != j)
            {
                //search for index of candidate i and j
                for(int k = 0; k < candidate_count; k++)
                {
                    //save index of candidate i
                    if(ranks[k] == i){candidate_i_index = k;}
                    //save index of candidate j
                    if(ranks[k] == j){candidate_j_index = k;}
                }//end k loop
                //add 1 to 2D array index if index i greater than j
                if(candidate_i_index < candidate_j_index)
                {
                    preferences[i][j]++;
                }
            }
        }//end j loop
    }//end i loop

}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i+1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pair p;
                p.winner = i;
                p.loser = j;
                pairs[pair_count] = p;
                pair_count++;
            }
            else if(preferences[i][j] < preferences[j][i])
            {
                pair p;
                p.winner = j;
                p.loser = i;
                pairs[pair_count] = p;
                pair_count++;
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // TODO
    //bubble sort
    for (int i = 0; i < pair_count - 1; i++)
    {
        //Last i elements are already in place
        for (int j = 0; j < pair_count - i - 1; j++)
        {
            //change pairs[j] and j+1 to preference winner count
            if (preferences[pairs[j].winner][pairs[j].loser] < preferences[pairs[j+1].winner][pairs[j+1].loser])
            {
                pair temp = pairs[j];
                pairs[j] = pairs[j+1];
                pairs[j+1] = temp;
            }
        }
    }
}

// Lock pairs into the candidate graph in order, without creating cycles

void lock_pairs(void)
{
    //goes through each pair to figure out whether we should lock pair or not
    for (int i = 0; i < pair_count; i++)
    {
        //calls recursive function
        bool result = checkForCycle(pairs[i], pairs[i].winner, i);
        //if false is returned, pair wil be locked
        if (!result)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        bool checkAllFalse = false;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                checkAllFalse = true;
            }
        }

        if(checkAllFalse == false)
        {
            printf("%s\n", candidates[i]);
        }
    }
}

//this recursive method checks for and cycles existing within locked array
bool checkForCycle(pair p, int candidateCycleCheck, int pairNumber)
{
    bool result = false;
    //checks if next p loser points to start of pair check for cycle
    if(p.loser == candidateCycleCheck)
    {
        result = true;
    }
    else
    {
        //this for loop goes questioning candidate to see if it is locked
        //on any locked[i][...]. if so, then jump into function again and question
        //the next pair in question until all need pairs were searched
        for (int i = 0; i < candidate_count; i++)
        {
            //if locked pair was found then,...
            if(locked[p.loser][i] == true)
            {
                //...find where in pairs, this lock exists
                for(int j = 0; j < pair_count; j++)
                {
                    //once found, check this pair for winner pointing to any other pair
                    if (pairs[j].winner == p.loser)
                    {
                        int previousPair = pairNumber;
                        p = pairs[j];
                        result = checkForCycle(p, candidateCycleCheck, j);

                        //if no locked found, revert to previous questioning pair
                        if (result == false)
                        {
                            p = pairs[previousPair];
                        }

                        //if results come back true, break away from for loop j
                        if(result == true)
                        {
                            break;
                        }
                    }//end if match
                }//end search for pair[i] winner
            }
        }//end for loop locked[][] check
    }

    return result;
}//end bool checkForCycle(pair p, int candidateCycleCheck)
