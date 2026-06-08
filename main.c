#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH  60
#define HEIGHT 25
#define MAX_OBJECTS 100

/* ── Canvas ─────────────────────────────────────────────── */
char canvas[HEIGHT][WIDTH + 1];

void clear_canvas(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            canvas[y][x] = ' ';
        canvas[y][WIDTH] = '\0';
    }
}

void display(void) {
    printf("\n+");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    for (int y = 0; y < HEIGHT; y++)
        printf("|%s|\n", canvas[y]);
    printf("+");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
}

static void plot(int x, int y, char ch) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        canvas[y][x] = ch;
}

/* ── Draw functions ─────────────────────────────────────── */
void draw_line(int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        plot(x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void draw_rectangle(int x, int y, int w, int h, char ch) {
    for (int i = x; i <= x + w; i++) { plot(i, y, ch); plot(i, y + h, ch); }
    for (int j = y; j <= y + h; j++) { plot(x, j, ch); plot(x + w, j, ch); }
}

void draw_circle(int cx, int cy, int r, char ch) {
    /* Midpoint circle algorithm — compensate for char aspect ratio */
    for (int deg = 0; deg < 360; deg++) {
        double rad = deg * M_PI / 180.0;
        int x = (int)round(cx + r * 2.0 * cos(rad));
        int y = (int)round(cy + r * 0.5 * sin(rad));
        plot(x, y, ch);
    }
}

void draw_triangle(int x, int y, int w, int h, char ch) {
    int ax = x + w / 2, ay = y;          /* apex  */
    int bl_x = x,       bl_y = y + h;   /* bottom-left  */
    int br_x = x + w,   br_y = y + h;   /* bottom-right */
    draw_line(ax, ay, bl_x, bl_y, ch);
    draw_line(ax, ay, br_x, br_y, ch);
    draw_line(bl_x, bl_y, br_x, br_y, ch);
}

/* ── Object store ───────────────────────────────────────── */
typedef enum { SHAPE_CIRCLE, SHAPE_RECT, SHAPE_LINE, SHAPE_TRIANGLE } ShapeType;

typedef struct {
    ShapeType type;
    char ch;
    int x, y;          /* origin / x1,y1 */
    int w, h;          /* width, height, radius (w), or x2(w),y2(h) */
    int active;
} Object;

Object objects[MAX_OBJECTS];
int obj_count = 0;

static void render_all(void) {
    clear_canvas();
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        Object *o = &objects[i];
        switch (o->type) {
            case SHAPE_CIRCLE:   draw_circle(o->x, o->y, o->w, o->ch); break;
            case SHAPE_RECT:     draw_rectangle(o->x, o->y, o->w, o->h, o->ch); break;
            case SHAPE_LINE:     draw_line(o->x, o->y, o->w, o->h, o->ch); break;
            case SHAPE_TRIANGLE: draw_triangle(o->x, o->y, o->w, o->h, o->ch); break;
        }
    }
}

int add_object(ShapeType type, int x, int y, int w, int h, char ch) {
    if (obj_count >= MAX_OBJECTS) { printf("Object limit reached.\n"); return -1; }
    objects[obj_count] = (Object){ type, ch, x, y, w, h, 1 };
    printf("Object %d added.\n", obj_count);
    return obj_count++;
}

void delete_object(int id) {
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid object ID.\n"); return;
    }
    objects[id].active = 0;
    printf("Object %d deleted.\n", id);
    render_all();
}

void modify_object(int id, int x, int y, int w, int h, char ch) {
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid object ID.\n"); return;
    }
    objects[id].x = x; objects[id].y = y;
    objects[id].w = w; objects[id].h = h;
    objects[id].ch = ch;
    printf("Object %d modified.\n", id);
    render_all();
}

void list_objects(void) {
    const char *names[] = { "circle", "rectangle", "line", "triangle" };
    printf("\n%-4s %-10s %-4s %-4s %-4s %-4s %-4s\n",
           "ID", "Type", "x", "y", "w/r", "h/y2", "char");
    printf("---------------------------------------\n");
    int any = 0;
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        Object *o = &objects[i];
        printf("%-4d %-10s %-4d %-4d %-4d %-4d  %c\n",
               i, names[o->type], o->x, o->y, o->w, o->h, o->ch);
        any = 1;
    }
    if (!any) printf("  (no objects)\n");
}

/* ── Menu helpers ───────────────────────────────────────── */
static char pick_char(void) {
    char c;
    printf("Character (* or _): ");
    scanf(" %c", &c);
    return (c == '_') ? '_' : '*';
}

static void menu_add(void) {
    int choice, x, y, w, h;
    char ch;
    printf("\n1) Circle  2) Rectangle  3) Line  4) Triangle\nShape: ");
    scanf("%d", &choice);
    ch = pick_char();
    switch (choice) {
        case 1:
            printf("cx cy radius: "); scanf("%d %d %d", &x, &y, &w);
            add_object(SHAPE_CIRCLE, x, y, w, 0, ch); break;
        case 2:
            printf("x y width height: "); scanf("%d %d %d %d", &x, &y, &w, &h);
            add_object(SHAPE_RECT, x, y, w, h, ch); break;
        case 3:
            printf("x1 y1 x2 y2: "); scanf("%d %d %d %d", &x, &y, &w, &h);
            add_object(SHAPE_LINE, x, y, w, h, ch); break;
        case 4:
            printf("x y width height: "); scanf("%d %d %d %d", &x, &y, &w, &h);
            add_object(SHAPE_TRIANGLE, x, y, w, h, ch); break;
        default: printf("Unknown shape.\n"); return;
    }
    render_all();
}

static void menu_delete(void) {
    int id;
    list_objects();
    printf("Object ID to delete: "); scanf("%d", &id);
    delete_object(id);
}

static void menu_modify(void) {
    int id, x, y, w, h;
    char ch;
    list_objects();
    printf("Object ID to modify: "); scanf("%d", &id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid ID.\n"); return;
    }
    Object *o = &objects[id];
    const char *names[] = { "circle", "rectangle", "line", "triangle" };
    printf("Editing %s (current: x=%d y=%d w/r=%d h/y2=%d char=%c)\n",
           names[o->type], o->x, o->y, o->w, o->h, o->ch);
    if (o->type == SHAPE_CIRCLE)
        { printf("cx cy radius: "); scanf("%d %d %d", &x, &y, &w); h = 0; }
    else if (o->type == SHAPE_LINE)
        { printf("x1 y1 x2 y2: "); scanf("%d %d %d %d", &x, &y, &w, &h); }
    else
        { printf("x y width height: "); scanf("%d %d %d %d", &x, &y, &w, &h); }
    ch = pick_char();
    modify_object(id, x, y, w, h, ch);
}

/* ── Main ───────────────────────────────────────────────── */
int main(void) {
    clear_canvas();
    int choice;
    printf("=== ASCII 2D Graphics Editor (chars: * and _) ===\n");
    printf("Canvas: %d wide x %d tall\n", WIDTH, HEIGHT);

    while (1) {
        printf("\n1) Add shape\n2) Delete shape\n3) Modify shape\n"
               "4) List objects\n5) Display canvas\n0) Quit\nChoice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: menu_add();    display(); break;
            case 2: menu_delete(); display(); break;
            case 3: menu_modify(); display(); break;
            case 4: list_objects(); break;
            case 5: display(); break;
            case 0: printf("Bye!\n"); return 0;
            default: printf("Unknown option.\n");
        }
    }
}
/* Compile: gcc main.c -o graphics.exe -lm */
/* Run: .\graphics.exe */
/* the above line shows how to run the program */