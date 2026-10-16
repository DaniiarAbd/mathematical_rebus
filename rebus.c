#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_WORDS 8
#define MAX_LEN 33
#define NOT_USED -2
#define NOT_ASSIGNED -1

typedef struct rebus{
    char words[MAX_WORDS][MAX_LEN]; // 1-й элемент в слове - длина
    unsigned char word_count;
    char digit_of[26];
    char used[10];
} REBUS;

int parse(REBUS *r, const char *s){
    memset(r, 0, sizeof(REBUS));

    if(sizeof(char) == 1){
        memset(r->digit_of, -2, 26);
    }
    else{
        for(int i = 0; i < 26; i++){
            r->digit_of[i] = -2;
        }
    }

    int letter_count = 0, pos = 1, max_len = 0;

    for(const char *c = s; ; c++){
        if('A' <= *c && *c <= 'Z'){
            if(r->word_count >= MAX_WORDS || pos >= MAX_LEN - 1) return 0;

            r->words[r->word_count][pos++] = *c;

            if(r->digit_of[*c - 'A'] == NOT_USED){
                if(++letter_count == 11) return 0;
                r->digit_of[*c - 'A'] = NOT_ASSIGNED;
            }
        }
        else{
            if(pos > 1){
                r->words[r->word_count++][0] = pos - 1;
                max_len = max_len >= pos - 1 ? max_len : pos - 1;
            }
            if(*c == 0) break;
            pos = 1;
        }
    }

    return r->word_count >= 3 && max_len == r->words[r->word_count - 1][0];
}

// idx_d - номер разряда
// idx_w - номер слова
int search(REBUS *r, int idx_d, int idx_w, int sum){
    int len = r->words[idx_w][0];

    if(idx_w == r->word_count - 1){   
        char pass, ost = sum % 10, c = r->words[idx_w][len - idx_d];

        if(r->digit_of[c - 'A'] == NOT_ASSIGNED){
            if(r->used[ost]) return 0;

            r->digit_of[c - 'A'] = ost;
            r->used[ost] = 1;

            if(len - 1 == idx_d){
                pass = (sum / 10 == 0) && (r->digit_of[r->words[idx_w][1] - 'A'] != 0 || len == 1);
            }
            else{
                pass = search(r, idx_d + 1, 0, sum / 10);
            }

            if(pass) return 1;

            r->digit_of[c - 'A'] = NOT_ASSIGNED;
            r->used[ost] = 0;
            return 0;
        }
        if(r->digit_of[c - 'A'] != ost) return 0;

        if(len - 1 == idx_d){
            return (sum / 10 == 0) && (r->digit_of[r->words[idx_w][1] - 'A'] != 0 || len == 1);
        }
        return search(r, idx_d + 1, 0, sum / 10);
    }
    if(len <= idx_d){
        return search(r, idx_d, idx_w + 1, sum);
    }

    char c = r->words[idx_w][len - idx_d];

    if(r->digit_of[c - 'A'] == NOT_ASSIGNED){
        for(int d = 0; d < 10; d++){
            if(r->used[d] || (len != 1 && len - 1 == idx_d && d == 0)) continue;

            r->used[d] = 1;
            r->digit_of[c - 'A'] = d;

            if(search(r, idx_d, idx_w + 1, sum + d)) return 1;

            r->used[d] = 0;
            r->digit_of[c - 'A'] = NOT_ASSIGNED;
        }
        return 0;
    }
    if(len != 1 && len - 1 == idx_d && r->digit_of[c - 'A'] == 0){
        return 0;
    }
    return search(r, idx_d, idx_w + 1, sum + r->digit_of[c - 'A']);
}

char *solve_rebus(const char *text){
    REBUS r;
    int i;

    if (!parse(&r, text)) return NULL;
    if (!search(&r, 0, 0, 0)) return NULL;

    char *result = (char*)malloc(strlen(text) + 1);
    if(result == NULL){
        return NULL;
    }

    for(i = 0; text[i]; i++){
        if('A' <= text[i] && text[i] <= 'Z'){
            result[i] = '0' + r.digit_of[text[i] - 'A'];
        }
        else{
            result[i] = text[i];
        }
    }
    result[i] = 0;
    return result;
}

int main(){
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    
    char text[512];

    printf("Введите ребус: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0;

    char *answer = solve_rebus(text);

    if(answer == NULL){
        printf("Не удалось получить решение!\n");
    }
    else{
        printf("Решение: %s\n", answer);
    }
    free(answer);
    return 0;
}
