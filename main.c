/**
 * @file poker.c
 * @author Benjamin Foreman (bennyforeman1@gmail.com)
 * @date 2021-04-17
 * 
 * This program determine the winnner from a file of poker hands and
 * play poker with computers who learn through simulation.
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POKER_FILE_PATH "poker.txt"
#define OUTPUT_FILE_PATH "csis.txt"
#define STR_BUF_SIZE 128

const int RANK_COUNT = 15;

// print nicely
// documentation
// cleanup

/**
 * Prints a formatted message to both stdout and a given file.
 * 
 * @param fp file pointer for printing out to
 * @param formatted_message formatted print message
 * @param ... variadic arglist for fromatted printing
 */
void csis_printf(FILE *fp, const char *formatted_message, ...)
{
    va_list arglist;

    va_start(arglist, formatted_message);
    vprintf(formatted_message, arglist);
    va_end(arglist);

    va_start(arglist, formatted_message);
    vfprintf(fp, formatted_message, arglist);
    va_end(arglist);
}

void counting_sort(int *vals, int size, int el_max, bool reverse)
{
    int *counts = calloc(el_max, sizeof(int));

    for (int i = 0; i < size; i++)
        counts[vals[i]]++;

    int val_idx = 0;

    if (reverse)
        for (int i = el_max - 1; i > 0; i--)
            for (int j = 0; j < counts[i]; j++)
                vals[val_idx++] = i;
    else
        for (int i = 0; i < el_max; i++)
            for (int j = 0; j < counts[i]; j++)
                vals[val_idx++] = i;
}

int rank_to_value(char rank)
{
    switch (rank)
    {
    case '2':
        return 0;
    case '3':
        return 1;
    case '4':
        return 2;
    case '5':
        return 3;
    case '6':
        return 4;
    case '7':
        return 5;
    case '8':
        return 6;
    case '9':
        return 7;
    case 'T':
        return 8;
    case 'J':
        return 9;
    case 'Q':
        return 10;
    case 'K':
        return 11;
    case 'A':
        return 12;
    default:
        return -1;
    }
}

char value_to_rank(int value)
{
    switch (value)
    {
    case 0:
        return '2';
    case 1:
        return '3';
    case 2:
        return '4';
    case 3:
        return '5';
    case 4:
        return '6';
    case 5:
        return '7';
    case 6:
        return '8';
    case 7:
        return '9';
    case 8:
        return 'T';
    case 9:
        return 'J';
    case 10:
        return 'Q';
    case 11:
        return 'K';
    case 12:
        return 'A';
    default:
        return '\0';
    }
}

int count_to_n_pairs(int count)
{
    switch (count)
    {
    case 0:
        return 0;
    case 1:
        return 0;
    case 2:
        return 1;
    case 3:
        return 3;
    case 4:
        return 6;
    default:
        return -1;
    }
}

int n_pairs_to_score(int n_pairs)
{
    switch (n_pairs)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 6;
    case 6:
        return 7;
    default:
        return -1;
    }
}

char *score_to_play_string(int score)
{
    switch (score)
    {
    case 0:
        return "High Card";
    case 1:
        return "Pair";
    case 2:
        return "Two Pair";
    case 3:
        return "Three of a Kind";
    case 4:
        return "Straight";
    case 5:
        return "Flush";
    case 6:
        return "Full House";
    case 7:
        return "Four of a Kind";
    case 8:
        return "Straight Flush";
    case 9:
        return "Royal Flush";
    default:
        return NULL;
    }
}

typedef struct
{
    char rank;
    int value;
    char suit;
} Card;

Card card_make(char rank, char suit)
{
    return (Card){
        .rank = rank,
        .value = rank_to_value(rank),
        .suit = suit,
    };
}

int *cards_get_values(size_t card_count, Card *cards)
{
    int *values = malloc(sizeof(int) * card_count);

    for (size_t i = 0; i < card_count; i++)
        values[i] = cards[i].value;

    return values;
}

char *cards_get_suits(size_t card_count, Card *cards)
{
    char *suits = malloc(sizeof(char) * card_count);

    for (size_t i = 0; i < card_count; i++)
        suits[i] = cards[i].suit;

    return suits;
}

int *cards_get_value_counts(size_t card_count, Card *cards)
{
    int *value_counts = calloc(RANK_COUNT, sizeof(int));

    for (size_t i = 0; i < card_count; i++)
        value_counts[cards[i].value]++;

    return value_counts;
}

typedef struct
{
    int score;
    size_t play_val_count;
    int *play_vals;
    size_t high_val_count;
    int *high_vals;
} Play;

Play calc_pairs(size_t card_count, Card *cards)
{
    int *counts = cards_get_value_counts(card_count, cards);

    int n_pairs = 0;
    for (size_t i = 0; i < RANK_COUNT; i++)
        n_pairs += count_to_n_pairs(counts[i]);

    int play_val_count = 0, high_val_count = 0;
    for (size_t i = 0; i < RANK_COUNT; i++)
        if (counts[i] > 1)
            play_val_count += counts[i];
        else
            high_val_count += counts[i];

    size_t play_val_idx = 0;
    int *play_vals = malloc(play_val_count * sizeof(int));
    for (size_t i = 0; i < RANK_COUNT; i++)
        if (counts[i] > 1)
            for (size_t j = 0; j < counts[i]; j++)
                play_vals[play_val_idx++] = i;

    size_t high_val_idx = 0;
    int *high_vals = malloc(high_val_count * sizeof(int));
    for (size_t i = 0; i < RANK_COUNT; i++)
        if (counts[i] == 1)
            for (size_t j = 0; j < counts[i]; j++)
                high_vals[high_val_idx++] = i;

    free(counts);

    return (Play){
        .score = n_pairs_to_score(n_pairs),
        .play_vals = play_vals,
        .play_val_count = play_val_count,
        .high_vals = high_vals,
        .high_val_count = high_val_count,
    };
}

bool is_straight(size_t card_count, Card *cards)
{
    int *values = cards_get_values(card_count, cards);

    counting_sort(values, card_count, RANK_COUNT, false);

    int prev_value = values[0];
    for (size_t i = 1; i < card_count; i++)
    {
        if (prev_value + 1 != values[i])
        {
            free(values);
            return false;
        }

        prev_value = values[i];
    }

    free(values);
    return true;
}

bool is_flush(size_t card_count, Card *cards)
{
    char *suits = cards_get_suits(card_count, cards);

    char prev_suit = suits[0];
    for (size_t i = 1; i < card_count; i++)
        if (prev_suit != suits[i])
        {
            free(suits);
            return false;
        }

    free(suits);
    return true;
}

bool is_royal(size_t card_count, Card *cards)
{
    int *values = cards_get_values(card_count, cards);

    counting_sort(values, card_count, RANK_COUNT, false);

    char royal_ranks[] = {'T', 'J', 'Q', 'K', 'A'};

    for (size_t i = 0; i < card_count; i++)
        if (values[i] != rank_to_value(royal_ranks[i]))
        {
            free(values);
            return false;
        }

    free(values);
    return true;
}

Play calculate_play(size_t card_count, Card *cards)
{
    int *values = cards_get_values(card_count, cards);
    char *suits = cards_get_suits(card_count, cards);

    Play curr_play = calc_pairs(card_count, cards); // TODO fix

    bool curr_is_straight = is_straight(card_count, cards);
    if (curr_is_straight && curr_play.score < 4)
        curr_play = (Play){
            .score = 4,
            .play_val_count = card_count,
            .play_vals = values,
        };

    bool curr_is_flush = is_flush(card_count, cards);
    if (curr_is_flush && curr_play.score < 5)
        curr_play = (Play){
            .score = 5,
            .play_val_count = card_count,
            .play_vals = values,
        };

    if (curr_is_straight && curr_is_flush)
    {
        curr_play = (Play){
            .score = 8,
            .play_val_count = card_count,
            .play_vals = values,
        };

        if (is_royal(card_count, cards))
            curr_play = (Play){
                .score = 9,
            };
    }

    free(values); // may be an issue because return play with values
    free(suits);

    return curr_play;
}

int high_vals_cmp(Play *a, Play *b)
{
    counting_sort(a->high_vals, a->high_val_count, RANK_COUNT, true);
    counting_sort(b->high_vals, b->high_val_count, RANK_COUNT, true);

    for (size_t i = 0; i < a->high_val_count; i++)
        printf("%c, ", value_to_rank(a->high_vals[i]));
    printf("\n");

    size_t i = 0;
    while (i < a->high_val_count && i < b->high_val_count)
    {
        if (a->high_vals[i] != b->high_vals[i])
            return a->high_vals[i] - b->high_vals[i];

        i++;
    }

    return a->high_val_count - b->high_val_count;
}

int play_vals_cmp(Play *a, Play *b)
{
    counting_sort(a->play_vals, a->play_val_count, RANK_COUNT, true);
    counting_sort(b->play_vals, b->play_val_count, RANK_COUNT, true);

    size_t i = 0;
    while (i < a->play_val_count && i < b->play_val_count)
    {
        if (a->play_vals[i] != b->play_vals[i])
            return a->play_vals[i] - b->play_vals[i];

        i++;
    }

    return a->play_val_count - b->play_val_count;
}

int play_cmp(Play *a, Play *b) // TODO flesh out
{
    if (a->score != b->score)
        return a->score - b->score;

    int play_vals_cmp_result = play_vals_cmp(a, b);
    if (play_vals_cmp_result != 0)
        return play_vals_cmp_result;

    return high_vals_cmp(a, b);
}

int main(int argc, char const *argv[])
{
    FILE *fp_in = fopen(POKER_FILE_PATH, "r");
    if (fp_in == NULL)
    {
        printf("Could not open %s for input.", POKER_FILE_PATH);
        exit(EXIT_FAILURE);
    }

    FILE *fp_out = fopen(OUTPUT_FILE_PATH, "w");
    if (fp_out == NULL)
    {
        printf("Could not open %s for input.", OUTPUT_FILE_PATH);
        exit(EXIT_FAILURE);
    }

    int result = 0;

    char line[STR_BUF_SIZE];
    while (fgets(line, STR_BUF_SIZE, fp_in))
    {
        Card line_cards[10];

        size_t card_idx = 0, char_idx = 0;
        while (char_idx < strlen(line))
        {
            line_cards[card_idx++] = card_make(line[char_idx], line[char_idx + 1]);
            char_idx += 3;
        }

        Play player_play = calculate_play(5, &line_cards[0]);
        Play other_play = calculate_play(5, &line_cards[5]);

        bool player_won = play_cmp(&player_play, &other_play) > 0;

        for (size_t i = 0; i < 5; i++)
            printf("%c, ", value_to_rank(line_cards[i].value));

        printf("Score = %d, %s", player_play.score, score_to_play_string(player_play.score));

        for (size_t i = 0; i < player_play.play_val_count; i++)
            printf("%c, ", value_to_rank(player_play.play_vals[i]));
        for (size_t i = 0; i < player_play.high_val_count; i++)
            printf("%c, ", value_to_rank(player_play.high_vals[i]));

        printf(" | ");

        for (size_t i = 5; i < 10; i++)
            printf("%c, ", value_to_rank(line_cards[i].value));

        printf("Score = %d, %s", other_play.score, score_to_play_string(other_play.score));

        for (size_t i = 0; i < other_play.play_val_count; i++)
            printf("%c, ", value_to_rank(other_play.play_vals[i]));
        for (size_t i = 0; i < other_play.high_val_count; i++)
            printf("%c, ", value_to_rank(other_play.high_vals[i]));

        printf("\n");

        result += player_won;
    }

    csis_printf(fp_out, "Player won %d times!\n", result);

    fclose(fp_out);
    fclose(fp_in);
}
