#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 24
#define EMPTY '_'
#define PIXEL '*'
#define MAX_OBJECTS 100

char picture[HEIGHT][WIDTH];

// Shape types
#define SHAPE_LINE      1
#define SHAPE_RECTANGLE 2
#define SHAPE_CIRCLE    3
#define SHAPE_TRIANGLE  4

typedef struct {
    int type;
    int params[6]; // up to 6 params: x1,y1,x2,y2,x3,y3 or cx,cy,r
    int active;
} Shape;

Shape objects[MAX_OBJECTS];
int objectCount = 0;

void clearPicture() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            picture[y][x] = EMPTY;
}

void displayPicture() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            printf("%c", picture[y][x]);
        printf("\n");
    }
}

void setPixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        picture[y][x] = PIXEL;
}

void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void drawRectangle(int x1, int y1, int x2, int y2) {
    drawLine(x1, y1, x2, y1); // top
    drawLine(x1, y2, x2, y2); // bottom
    drawLine(x1, y1, x1, y2); // left
    drawLine(x2, y1, x2, y2); // right
}

void drawCircle(int cx, int cy, int radius) {
    int x = 0, y = radius;
    int d = 1 - radius;

    while (x <= y) {
        setPixel(cx + x, cy - y);
        setPixel(cx - x, cy - y);
        setPixel(cx + x, cy + y);
        setPixel(cx - x, cy + y);
        setPixel(cx + y, cy - x);
        setPixel(cx - y, cy - x);
        setPixel(cx + y, cy + x);
        setPixel(cx - y, cy + x);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

void renderAll() {
    clearPicture();
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (!objects[i].active) continue;
        int *p = objects[i].params;
        switch (objects[i].type) {
            case SHAPE_LINE:      drawLine(p[0],p[1],p[2],p[3]); break;
            case SHAPE_RECTANGLE: drawRectangle(p[0],p[1],p[2],p[3]); break;
            case SHAPE_CIRCLE:    drawCircle(p[0],p[1],p[2]); break;
            case SHAPE_TRIANGLE:  drawTriangle(p[0],p[1],p[2],p[3],p[4],p[5]); break;
        }
    }
}

void printMenu() {
    printf("\n2D Graphics Editor\n");
    printf("Canvas size: %d x %d\n", WIDTH, HEIGHT);
    printf("1. Add object\n");
    printf("2. Delete object\n");
    printf("3. Modify object\n");
    printf("4. Display picture\n");
    printf("5. List objects\n");
    printf("0. Exit\n");
    printf("Enter choice: ");
}

int addObject() {
    // Find next free slot
    int idx = -1;
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (!objects[i].active) { idx = i; break; }
    }
    if (idx == -1) { printf("No space for more objects.\n"); return -1; }

    printf("\nChoose shape type:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("Enter shape type: ");

    int type;
    scanf("%d", &type);

    int *p = objects[idx].params;
    memset(p, 0, sizeof(objects[idx].params));

    if (type == SHAPE_LINE) {
        printf("Enter x1 y1 x2 y2: ");
        scanf("%d %d %d %d", &p[0],&p[1],&p[2],&p[3]);
    } else if (type == SHAPE_RECTANGLE) {
        printf("Enter top-left x y and bottom-right x y: ");
        scanf("%d %d %d %d", &p[0],&p[1],&p[2],&p[3]);
    } else if (type == SHAPE_CIRCLE) {
        printf("Enter center x y and radius: ");
        scanf("%d %d %d", &p[0],&p[1],&p[2]);
    } else if (type == SHAPE_TRIANGLE) {
        printf("Enter x1 y1 x2 y2 x3 y3: ");
        scanf("%d %d %d %d %d %d", &p[0],&p[1],&p[2],&p[3],&p[4],&p[5]);
    } else {
        printf("Invalid shape type.\n");
        return -1;
    }

    objects[idx].type = type;
    objects[idx].active = 1;
    printf("Object added with index %d.\n", idx);
    return idx;
}

void deleteObject() {
    printf("Enter index to delete: ");
    int idx;
    scanf("%d", &idx);
    if (idx < 0 || idx >= MAX_OBJECTS || !objects[idx].active) {
        printf("Invalid index.\n");
        return;
    }
    objects[idx].active = 0;
    printf("Object %d deleted.\n", idx);
}

void modifyObject() {
    printf("Enter index to modify: ");
    int idx;
    scanf("%d", &idx);
    if (idx < 0 || idx >= MAX_OBJECTS || !objects[idx].active) {
        printf("Invalid index.\n");
        return;
    }
    objects[idx].active = 0;
    // Re-add at same slot
    int *p = objects[idx].params;
    int type = objects[idx].type;

    printf("Choose new shape type:\n");
    printf("1. Line\n2. Rectangle\n3. Circle\n4. Triangle\n");
    printf("Enter shape type: ");
    scanf("%d", &type);

    memset(p, 0, sizeof(objects[idx].params));
    if (type == SHAPE_LINE) {
        printf("Enter x1 y1 x2 y2: ");
        scanf("%d %d %d %d", &p[0],&p[1],&p[2],&p[3]);
    } else if (type == SHAPE_RECTANGLE) {
        printf("Enter top-left x y and bottom-right x y: ");
        scanf("%d %d %d %d", &p[0],&p[1],&p[2],&p[3]);
    } else if (type == SHAPE_CIRCLE) {
        printf("Enter center x y and radius: ");
        scanf("%d %d %d", &p[0],&p[1],&p[2]);
    } else if (type == SHAPE_TRIANGLE) {
        printf("Enter x1 y1 x2 y2 x3 y3: ");
        scanf("%d %d %d %d %d %d", &p[0],&p[1],&p[2],&p[3],&p[4],&p[5]);
    } else {
        printf("Invalid shape type.\n");
        return;
    }
    objects[idx].type = type;
    objects[idx].active = 1;
    printf("Object %d modified.\n", idx);
}

void listObjects() {
    const char *names[] = {"", "Line", "Rectangle", "Circle", "Triangle"};
    int found = 0;
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (!objects[i].active) continue;
        found = 1;
        int *p = objects[i].params;
        printf("Index %d: %s", i, names[objects[i].type]);
        if (objects[i].type == SHAPE_LINE)
            printf(" (%d,%d)-(%d,%d)", p[0],p[1],p[2],p[3]);
        else if (objects[i].type == SHAPE_RECTANGLE)
            printf(" (%d,%d)-(%d,%d)", p[0],p[1],p[2],p[3]);
        else if (objects[i].type == SHAPE_CIRCLE)
            printf(" center(%d,%d) r=%d", p[0],p[1],p[2]);
        else if (objects[i].type == SHAPE_TRIANGLE)
            printf(" (%d,%d)-(%d,%d)-(%d,%d)", p[0],p[1],p[2],p[3],p[4],p[5]);
        printf("\n");
    }
    if (!found) printf("No objects.\n");
}

int main() {
    memset(objects, 0, sizeof(objects));
    clearPicture();

    int choice;
    while (1) {
        printMenu();
        scanf("%d", &choice);

        if (choice == 1) {
            addObject();
        } else if (choice == 2) {
            deleteObject();
        } else if (choice == 3) {
            modifyObject();
        } else if (choice == 4) {
            renderAll();
            printf("\n");
            displayPicture();
        } else if (choice == 5) {
            listObjects();
        } else if (choice == 0) {
            printf("Goodbye.\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}