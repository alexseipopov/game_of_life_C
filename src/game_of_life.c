#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25
#define LIVE "#"
#define EMPTY "."
#define DELTA 5e-6

void print_field(int arr[HEIGHT][WIDTH], int counter, int speed_count);
void game();
void preset(int arr[HEIGHT][WIDTH], int *flag);
void game_loop(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
void calculate_new(int prev[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
int check_pos(int arr[HEIGHT][WIDTH], int x, int y);
void exchange(int prev[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
void set_view(FILE *file, int arr[HEIGHT][WIDTH]);
void show_txt(FILE *file);
void handler_time(double *speed, char command, int *speed_count);
void clear_field();
void pause_check(char c);
void clear_footer();
void genegate_random();


int main() {
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(FALSE);

    game();

    endwin();
    return 0;
}

void game() {
    int field[HEIGHT][WIDTH];
    int next_field[HEIGHT][WIDTH];
    int status = 0;
    while (status != 2) {
        status = 0;
        clear_field();
        FILE *file = fopen("start.txt", "r");
        show_txt(file);
        fclose(file);
        preset(field, &status);
        if (status == 1) {
            game_loop(field, next_field);
        }
    }
}

void preset(int arr[HEIGHT][WIDTH], int *flag) {
    nodelay(stdscr, FALSE);
    FILE * f;
    while (*flag == 0) {
        char state = getch();
        if (state == '1') {
            f = fopen("1.txt", "r");
            *flag = 1;
        } else if (state == '2') {
            f = fopen("2.txt", "r");
            *flag = 1;
        } else if (state == '3') {
            f = fopen("3.txt", "r");
            *flag = 1;
        } else if (state == '4') {
            f = fopen("4.txt", "r");
            *flag = 1;
        } else if (state == '5') {
            f = fopen("5.txt", "r");
            *flag = 1;
        } else if (state == '0') {
            genegate_random();
            f = fopen("random.txt", "r");
            *flag = 1;
        } else if (state == 'q' || state == 'Q') {
            *flag = 2;
        }
        if (*flag == 1) {
            set_view(f, arr);
            fclose(f);
            nodelay(stdscr, TRUE);
        }
    }
}

void genegate_random() {
    FILE *file = fopen("random.txt", "w");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (rand() % 100 > 75) {
                fprintf(file, "1");
            } else {
                fprintf(file, "0");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void show_txt(FILE *file) {
    char ch;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            fscanf(file, "%c", &ch);
            if (j < WIDTH) {
                mvprintw(i, j, "%c", ch);
            }
        }
    }
}

void set_view(FILE *file, int arr[HEIGHT][WIDTH]) {
    char ch = '.';
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            fscanf(file, "%c", &ch);
            if (j <= WIDTH) {
                if (ch == '1') {
                    arr[i][j] = 1;
                } else {
                    arr[i][j] = 0;
                }
            }
        }
    }
}

void game_loop(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
    char c = '0';
    double t = 0;
    double speed = 5e-6;
    int counter = 1;
    int speed_count = 1;
    while (c != 'q' && c != 'Q') {
        c = getch();
        handler_time(&speed, c, &speed_count);
        pause_check(c);
        t += speed;
        if (t > 1) {
            t = 0;
            print_field(current, counter, speed_count);
            calculate_new(current, next);
            exchange(current, next);
            counter += 1;
        }
    }
}

void pause_check(char c) {
    if (c == ' ') {
        c = '.';
        while (c != ' ' && c != 'q' && c != 'Q') {
            c = getch();
            mvprintw(HEIGHT + 3, 3, "Speed: Pause");
        }
    }
}

void clear_field() {
    for (int i = 0; i < HEIGHT + 10; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            mvprintw(i, j, " ");
        }
        refresh();
    }
    refresh();
}

void handler_time(double *speed, char command, int *speed_count) {
    if ((command == 'w' || command == 'W') && *speed_count <= 9) {
        *speed += DELTA;
        *speed_count += 1;
    } else if ((command == 's' || command == 'S') && *speed_count >= 2) {
        *speed -= DELTA;
        *speed_count -= 1;
    }
}

void print_field(int arr[HEIGHT][WIDTH], int counter, int speed_count) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (arr[i][j] == 1) {
                mvprintw(i, j, LIVE);
            } else {
                mvprintw(i, j, EMPTY);
            }
        }
        refresh();
    }
    clear_footer();
    mvprintw(HEIGHT + 2, 3, "Generation: %d", counter);
    mvprintw(HEIGHT + 3, 3, "Speed: %d", speed_count);
    mvprintw(HEIGHT + 2, 59, "QUIT - [ q | Q ]");
    mvprintw(HEIGHT + 2, 30, "SPEED+ : [ w | W ]");
    mvprintw(HEIGHT + 3, 30, "SPEED- : [ s | S ]");
    refresh();
}

void clear_footer() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = HEIGHT; j < HEIGHT + 10; j++) {
            mvprintw(j, i, " ");
        }
    }
}

void calculate_new(int prev[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
    int r;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            r = check_pos(prev, i, j);
            if (r == 0) {
                next[i][j] = 1;
            } else {
                next[i][j] = 0;
            }
        }
    }
}

int check_pos(int arr[HEIGHT][WIDTH], int x, int y) {
    int p, k, res = 0, req = 1;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            if (x + i < 0) {
                p = HEIGHT - 1;
            } else if (x + i >= HEIGHT) {
                p = 0;
            } else {
                p = x + i;
            }
            if (y + j < 0) {
                k = WIDTH - 1;
            } else if (y + j >= WIDTH) {
                k = 0;
            } else {
                k = y + j;
            }
            if (arr[p][k] == 1) {
                res += 1;
            }
        }
    }
    if ((arr[x][y] == 1 && res == 2) || res == 3) {
        req = 0;
    }
    return req;
}

void exchange(int prev[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            prev[i][j] = next[i][j];
        }
    }
}
