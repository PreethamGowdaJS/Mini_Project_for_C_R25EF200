#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 80
#define HEIGHT 24
#define MAX_OBJECTS 100

typedef struct {
int type;
int p[6];
} Object;

Object objects[MAX_OBJECTS];
int objectCount = 0;

char canvas[HEIGHT][WIDTH];

void clearCanvas() {
for(int y=0;y<HEIGHT;y++)
for(int x=0;x<WIDTH;x++)
canvas[y][x]='_';
}

void plot(int x,int y){
if(x>=0 && x<WIDTH && y>=0 && y<HEIGHT)
canvas[y][x]='*';
}

void drawLineInternal(int x1,int y1,int x2,int y2){
int dx=abs(x2-x1);
int dy=abs(y2-y1);
int sx=(x1<x2)?1:-1;
int sy=(y1<y2)?1:-1;
int err=dx-dy;

while(1){
    plot(x1,y1);

    if(x1==x2 && y1==y2)
        break;

    int e2=2*err;

    if(e2>-dy){
        err-=dy;
        x1+=sx;
    }

    if(e2<dx){
        err+=dx;
        y1+=sy;
    }
}

}

void drawRectangleInternal(int x1,int y1,int x2,int y2){
for(int x=x1;x<=x2;x++){
plot(x,y1);
plot(x,y2);
}

for(int y=y1;y<=y2;y++){
    plot(x1,y);
    plot(x2,y);
}

}

void drawCircleInternal(int cx,int cy,int r){
for(int y=cy-r;y<=cy+r;y++){
for(int x=cx-r;x<=cx+r;x++){
int dx=x-cx;
int dy=y-cy;
int d=dx*dx+dy*dy;

        if(abs(d-r*r)<=r)
            plot(x,y);
    }
}

}

void drawTriangleInternal(
int x1,int y1,
int x2,int y2,
int x3,int y3)
{
drawLineInternal(x1,y1,x2,y2);
drawLineInternal(x2,y2,x3,y3);
drawLineInternal(x3,y3,x1,y1);
}

void redrawCanvas(){
clearCanvas();

for(int i=0;i<objectCount;i++){

    if(objects[i].type==1){
        drawLineInternal(
            objects[i].p[0],
            objects[i].p[1],
            objects[i].p[2],
            objects[i].p[3]
        );
    }

    else if(objects[i].type==2){
        drawRectangleInternal(
            objects[i].p[0],
            objects[i].p[1],
            objects[i].p[2],
            objects[i].p[3]
        );
    }

    else if(objects[i].type==3){
        drawCircleInternal(
            objects[i].p[0],
            objects[i].p[1],
            objects[i].p[2]
        );
    }

    else if(objects[i].type==4){
        drawTriangleInternal(
            objects[i].p[0],
            objects[i].p[1],
            objects[i].p[2],
            objects[i].p[3],
            objects[i].p[4],
            objects[i].p[5]
        );
    }
}

}

void displayPicture(){
redrawCanvas();

for(int y=0;y<HEIGHT;y++){
    for(int x=0;x<WIDTH;x++)
        printf("%c",canvas[y][x]);

    printf("\n");
}

}

void listObjects(){
printf("\nObjects:\n");

for(int i=0;i<objectCount;i++){
    printf("Index %d -> Type %d\n",i,objects[i].type);
}

}

void addObject(){

int type;

printf("\nChoose shape type:\n");
printf("1. Line\n");
printf("2. Rectangle\n");
printf("3. Circle\n");
printf("4. Triangle\n");
printf("Enter shape type: ");

scanf("%d",&type);

objects[objectCount].type=type;

if(type==1){
    printf("Enter x1 y1 x2 y2: ");
    scanf("%d%d%d%d",
          &objects[objectCount].p[0],
          &objects[objectCount].p[1],
          &objects[objectCount].p[2],
          &objects[objectCount].p[3]);
}

else if(type==2){
    printf("Enter top-left x y and bottom-right x y: ");
    scanf("%d%d%d%d",
          &objects[objectCount].p[0],
          &objects[objectCount].p[1],
          &objects[objectCount].p[2],
          &objects[objectCount].p[3]);
}

else if(type==3){
    printf("Enter center x y and radius: ");
    scanf("%d%d%d",
          &objects[objectCount].p[0],
          &objects[objectCount].p[1],
          &objects[objectCount].p[2]);
}

else if(type==4){
    printf("Enter x1 y1 x2 y2 x3 y3: ");
    scanf("%d%d%d%d%d%d",
          &objects[objectCount].p[0],
          &objects[objectCount].p[1],
          &objects[objectCount].p[2],
          &objects[objectCount].p[3],
          &objects[objectCount].p[4],
          &objects[objectCount].p[5]);
}

printf("Object added with index %d.\n",objectCount);

objectCount++;

}

void deleteObject(){

int index;

printf("Enter object index: ");
scanf("%d",&index);

if(index<0 || index>=objectCount){
    printf("Invalid index.\n");
    return;
}

for(int i=index;i<objectCount-1;i++)
    objects[i]=objects[i+1];

objectCount--;

}

void modifyObject(){

int index;

printf("Enter object index: ");
scanf("%d",&index);

if(index<0 || index>=objectCount){
    printf("Invalid index.\n");
    return;
}

printf("Modify values again:\n");

int type=objects[index].type;

if(type==1){
    printf("Enter x1 y1 x2 y2: ");
    scanf("%d%d%d%d",
          &objects[index].p[0],
          &objects[index].p[1],
          &objects[index].p[2],
          &objects[index].p[3]);
}

else if(type==2){
    printf("Enter top-left x y and bottom-right x y: ");
    scanf("%d%d%d%d",
          &objects[index].p[0],
          &objects[index].p[1],
          &objects[index].p[2],
          &objects[index].p[3]);
}

else if(type==3){
    printf("Enter center x y radius: ");
    scanf("%d%d%d",
          &objects[index].p[0],
          &objects[index].p[1],
          &objects[index].p[2]);
}

else if(type==4){
    printf("Enter x1 y1 x2 y2 x3 y3: ");
    scanf("%d%d%d%d%d%d",
          &objects[index].p[0],
          &objects[index].p[1],
          &objects[index].p[2],
          &objects[index].p[3],
          &objects[index].p[4],
          &objects[index].p[5]);
}

}

int main(){

int choice;

while(1){

    printf("\n2D Graphics Editor\n");
    printf("Canvas size: 80 x 24\n");
    printf("1. Add object\n");
    printf("2. Delete object\n");
    printf("3. Modify object\n");
    printf("4. Display picture\n");
    printf("5. List objects\n");
    printf("0. Exit\n");
    printf("Enter choice: ");

    scanf("%d",&choice);

    switch(choice){

        case 1:
            addObject();
            break;

        case 2:
            deleteObject();
            break;

        case 3:
            modifyObject();
            break;

        case 4:
            displayPicture();
            break;

        case 5:
            listObjects();
            break;

        case 0:
            printf("Goodbye.\n");
            return 0;

        default:
            printf("Invalid choice.\n");
    }
}
}

