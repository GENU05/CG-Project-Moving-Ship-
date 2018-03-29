#include<windows.h>
#include<GL/glut.h>
#include <stdio.h>
#include <math.h>
#include<cmath>
#include<stdlib.h>
#include<string.h>
#define maxHt 1024
#define maxWd 1024
#define maxVer 10000

int flag=0;
float pos=0,y=0;


typedef struct edgebucket{
    int ymax;
    float xofymin;
    float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup{
    int countEdgeBucket;
    EdgeBucket buckets[maxVer];
}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;

void initEdgeTable(){
    int i;
    for (i=0; i<maxHt; i++){
        EdgeTable[i].countEdgeBucket = 0;
    }
    ActiveEdgeTuple.countEdgeBucket = 0;
}

void midpoint(int X1,int Y1,int X2,int Y2)
{
  int dx=(X2-X1);
  int dy=(Y2-Y1);
  int d=dy-(dx/2);
  int x;
  int y;
  x=X1;
  y=Y1;
  glBegin(GL_POINTS);
  glVertex2d(x,y);
  glPointSize(10);
  while(x<X2){

        x++;

        if(d<0)
            d=d+dy;
        else{
            d+=(dy-dx);
            y++;
        }
            glVertex2d(x,y);
    }
  glEnd();
}

void insertionSort(EdgeTableTuple *ett){
    int i,j;
    EdgeBucket temp;

    for (i = 1; i < ett->countEdgeBucket; i++){
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

    while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0)){
        ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
        ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
        ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
        j = j - 1;
    }
    ett->buckets[j + 1].ymax = temp.ymax;
    ett->buckets[j + 1].xofymin = temp.xofymin;
    ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}


void storeEdgeInTuple (EdgeTableTuple *receiver,int ym,int xm,float slopInv){
    (receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;
    insertionSort(receiver);
    (receiver->countEdgeBucket)++;
}


void storeEdgeInTable (int x1,int y1, int x2, int y2){
    float m,minv;
    int ymaxTS,xwithyminTS, scanline; //ts stands for to store

    if (x2==x1){
        minv=0.000000;
    }
    else{
    m = ((float)(y2-y1))/((float)(x2-x1));
    if (y2==y1)
        return;

    minv = (float)1.0/m;
    }

    if (y1>y2){
        scanline=y2;
        ymaxTS=y1;
        xwithyminTS=x2;
    }
    else{
        scanline=y1;
        ymaxTS=y2;
        xwithyminTS=x1;
    }
    storeEdgeInTuple(&EdgeTable[scanline],ymaxTS,xwithyminTS,minv);
}


void removeEdgeByYmax(EdgeTableTuple *Tup,int yy){
    int i,c;
    for (i=0; i< Tup->countEdgeBucket; i++){
        if (Tup->buckets[i].ymax == yy){
            for ( c = i ; c < Tup->countEdgeBucket -1 ; c++ ){
                Tup->buckets[c].ymax =Tup->buckets[c+1].ymax;
                Tup->buckets[c].xofymin =Tup->buckets[c+1].xofymin;
                Tup->buckets[c].slopeinverse = Tup->buckets[c+1].slopeinverse;
                }
                Tup->countEdgeBucket--;
            i--;
        }
    }
}


void updatexbyslopeinv(EdgeTableTuple *Tup){
    int i;
    for (i=0; i<Tup->countEdgeBucket; i++){
        (Tup->buckets[i]).xofymin =(Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
    }
}


void ScanlineFill(float r,float g,float b)
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
    for (i=0; i<maxHt; i++){
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++){
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                     ymax,EdgeTable[i].buckets[j].xofymin,
                    EdgeTable[i].buckets[j].slopeinverse);
        }
        removeEdgeByYmax(&ActiveEdgeTuple, i);
        insertionSort(&ActiveEdgeTuple);
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = -1;
        x2 = -1;
        ymax1 = -1;
        ymax2 = -1;
        while (j<ActiveEdgeTuple.countEdgeBucket){
            if (coordCount%2==0){
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2)
                {
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else{
                        coordCount++;
                    }
                }
                else{
                        coordCount++;
                }
            }
            else{
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;
                FillFlag = 0;
                if (x1==x2){
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else{
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else{
                        coordCount++;
                        FillFlag = 1;
                }
            if(FillFlag){
                glColor3f(r,g,b);
                midpoint(x1,i,x2,i);
                }
        }
        j++;
    }
    updatexbyslopeinv(&ActiveEdgeTuple);
}
}


void ScanlineFill(float r,float g,float b,float extra)
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
    for (i=0; i<maxHt; i++){
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++){
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                     ymax,EdgeTable[i].buckets[j].xofymin,
                    EdgeTable[i].buckets[j].slopeinverse);
        }
        removeEdgeByYmax(&ActiveEdgeTuple, i);
        insertionSort(&ActiveEdgeTuple);
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = -1;
        x2 = -1;
        ymax1 = -1;
        ymax2 = -1;
        while (j<ActiveEdgeTuple.countEdgeBucket){
            if (coordCount%2==0){
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2)
                {
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else{
                        coordCount++;
                    }
                }
                else{
                        coordCount++;
                }
            }
            else{
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;
                FillFlag = 0;
                if (x1==x2){
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else{
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else{
                        coordCount++;
                        FillFlag = 1;
                }
            if(FillFlag){
                glColor4f(r,g,b,extra);
                midpoint(x1,i,x2,i);
                }
        }
        j++;
    }
    updatexbyslopeinv(&ActiveEdgeTuple);
}
}


void drawPolyDino(int a[],int sizei)
{
    int count = 0,x1,y1,x2,y2;
    int n=0;
    while(n!=sizei){
        count++;
        if (count>2){
            x1 = x2;
            y1 = y2;
            count=2;
        }
        if (count==1){
            x1=a[n];
            n++;
            y1=a[n];
            n++;
        }
        else{
            x2=a[n];
            n++;
            y2=a[n];
            n++;
            storeEdgeInTable(x1, y1, x2, y2);
        }
    }
}


void drawDino(int a[],int n,float r,float g , float b){
    initEdgeTable();
    drawPolyDino(a,n);
    ScanlineFill(r,g,b);
}


void drawDino(int a[],int n,float r,float g , float b,float extra){
    initEdgeTable();
    drawPolyDino(a,n);
    ScanlineFill(r,g,b,extra);
}


void plot(int x,int y,int X1,int Y1,float r,float g,float b,float alpha){
    glColor4f(r,g,b,alpha);
    //glColor3f( 0.80, 0.80,0.80);
    glBegin(GL_POINTS);
    glVertex2i(x+X1,y+Y1);
    glEnd();
}


void midpointcircle(int X1,int Y1,int r,float ra, float g, float b,float xtra){

    int x=0;
    int y=r;
    int decision=1-r;
    plot(x,y,X1,Y1,ra,g,b,xtra);
    plot(-y,x,X1,Y1,ra,g,b,xtra);
    plot(-x,-y,X1,Y1,ra,g,b,xtra);
    plot(y,-x,X1,Y1,ra,g,b,xtra);
    while(y>x){
        if(decision<0){
            x++;
            decision+=2*x+1;
        }
        else{
            y--;
            x++;
            decision+=2*(x-y)+1;
        }
        plot(x,y,X1,Y1,ra,g,b,xtra);
        plot(x,-y,X1,Y1,ra,g,b,xtra);
        plot(-x, y,X1,Y1,ra,g,b,xtra);
		plot(-x, -y,X1,Y1,ra,g,b,xtra);
		plot(y, x,X1,Y1,ra,g,b,xtra);
		plot(-y, x,X1,Y1,ra,g,b,xtra);
		plot(y, -x,X1,Y1,ra,g,b,xtra);
		plot(-y, -x,X1,Y1,ra,g,b,xtra);
    }
}


void drawcloud(){
        int i,j;
        for(j=0;j<2;j++)
        for(i=50;i>=0;i--){
            midpointcircle(150+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(190+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(110+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(140+j,820,i,0.80,0.80,0.80,1.0);
            midpointcircle(140+j,780,i,0.80,0.80,0.80,1.0);
            midpointcircle(160+j,820,i,0.80,0.80,0.80,1.0);
            midpointcircle(160+j,780,i,0.80,0.80,0.80,1.0);
        }

    for(j=0;j<2;j++)
    for(i=50;i>=0;i--){
            midpointcircle(550+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(590+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(510+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(540+j,820,i,0.80,0.80,0.80,1.0);
            midpointcircle(540+j,780,i,0.80,0.80,0.80,1.0);
            midpointcircle(560+j,820,i,0.80,0.80,0.80,1.0);
            midpointcircle(560+j,780,i,0.80,0.80,0.80,1.0);
        }

    for(j=0;j<2;j++)
    for(i=50;i>=0;i--){
            midpointcircle(850+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(890+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(810+j,800,i,0.80,0.80,0.80,1.0);
            midpointcircle(840+j,820,i,0.80,0.80,0.80,1.0);
            midpointcircle(840+j,780,i,0.80,0.80,0.80,1.0);
            midpointcircle(860+j,820,i,0.80,0.80,0.80,1.0);
            midpointcircle(860+j,780,i,0.80,0.80,0.80,1.0);
    }
}


void drawsun(){
    int i,j;
    for(j=0;j<2;j++)
        for(i=70;i>=0;i--)
            midpointcircle(450+j,850,i,1.0,1.0,0.2,1.0);
}


void island(){

    int i,j;
    for(j=0;j<2;j++)
        for(i=70;i>0;i--){
            midpointcircle(650+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(575+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(725+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(805+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(885+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(980+j,520,i,0.0,1.0,0.0,1.0);
    }

    int base[] ={500,550,530,560,1024,560,1024,550,500,550};
    drawDino(base,10,0.91,0.76,0.65);

    for(j=0;j<2;j++)
        for(i=70;i>0;i--){
            midpointcircle(25+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(150+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(75+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(225+j,520,i,0.0,1.0,0.0,1.0);
            midpointcircle(275+j,520,i,0.0,1.0,0.0,1.0);
    }
    int base2[] ={0,550,0,560,320,560,350,550,100,550};
    drawDino(base2,10,0.91,0.76,0.65);
}


void ship(int x,int y)
{
		int b[] = {20+x,240+y,20+x,220+y,82+x,140+y,438+x,84+y,584+x,196+y,600+x,220+y,20+x,240+y};
		drawDino(b,14,0.30f,0.06f,0.42f);

        int c[] = {70+x,234+y,70+x,256+y,478+x,262+y,478+x,262+y,478+x,222+y,70+x,238+y,70+x,234+y};
        drawDino(c,12,0.70f,0.0f,0.0f);


        int m[] = {70+x,235+y,70+x,256+y,478+x,262+y,478+x,262+y,478+x,222+y,70+x,238+y};
        drawDino(m,10,0.70f,0.0f,0.0f);

		int d[] = {90+x,258+y,90+x,280+y,466+x,298+y,466+x,262+y};
		drawDino(d,8,1.0f,1.0f,1.0f);

		int e[] = {90+x,258+y,90+x,280+y,466+x,298+y,466+x,262+y,90+x,258+y};
		drawDino(e,10,1.0f,1.0f,1.0f);

        int f[] = {102+x,302+y,102+x,280+y,442+x,298+y,442+x,330+y,102+x,302+y};
        drawDino(f,10,0.3f,0.3f,0.3f);

        int n[] = {102+x,302+y,102+x,280+y,442+x,298+y,442+x,330+y};
        drawDino(n,8,0.3f,0.3f,0.3f);

        int h[] ={442+x,328+y,442+x,298+y,494+x,294+y,494+x,324+y,442+x,328+y};
        drawDino(h,10,0.4f,0.4f,0.4f);

        int i[]={466+x,298+y,466+x,262+y,508+x,256+y,508+x,290+y,466+x,298+y};
        drawDino(i,10,0.8f,0.8f,0.8f);

        int j[]={478+x,262+y,478+x,222+y,514+x,220+y,514+x,254+y,478+x,262+y};
        drawDino(j,10,0.5,0.0,0.0);

        int k[] ={224+x,312+y,224+x,396+y,254+x,396+y,254+x,315+y,224+x,312+y};
        drawDino(k,10,0.2,0.0,0.0);

        int l[]={318+x,322+y,318+x,406+y,358+x,406+y,358+x,320+y,318+x,322+y};
        drawDino(l,10,0.2,0.0,0.0);

        int g[]={158+x,304+y,158+x,388+y,188+x,388+y,188+x,310+y,158+x,304+y};
        drawDino(g,10,0.2f,0.0f,0.0f);
}


void water1(){
        int b[]= {0,160,80,160,480,100,1024,100,1024,0,0,0,0,160};
    drawDino(b,14,0.196078f,0.196078f,0.8f,1.0f);
}


void water()
{
        int b[] = {80,160,1024,160,1024,100,480,100,80,160};
    drawDino(b,10,0.196078f,0.196078f,0.8f,1.0f);
}


void water2(){
    int b[]={0,150,0,550,1024,550,1024,150,0,150};
    drawDino(b,10,0.196078f,0.196078f,0.8f,1.0f);
}


void init()
{
	glClearColor( 0.196078,0.6,0.8,0.0);
	glColor3f(1.0,0.0,0.0);
	glLineWidth(3.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,1024,0,1024);

}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawsun();
	island();
	water();
	water2();
	ship(pos,y);
	drawcloud();
	water1();
    glutPostRedisplay();
	glFlush();
    glutSwapBuffers();
}


void myKeyboardFunc( unsigned char key, int x, int y )
{
	if(key==37)
	{
		pos-=0.1;
        water();
        ship(pos,y);
        water1();
        glutPostRedisplay();
        glFlush();
	};
	if(key==39)
	{
		pos+=0.1;
        water();
        ship(pos,y);
        water1();
        glutPostRedisplay();
        glFlush();
	};
	/*
	left arrow: 37
up arrow: 38
right arrow: 39
down arrow: 40
	*/

}


void processSpecialKeys (int key, int mx, int my) {
    switch(key){
    case GLUT_KEY_LEFT :
        pos-=1.0;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT :
        pos+=1.0;
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP :
        break;
    case GLUT_KEY_DOWN :
        break;
    default:
        break;
    }
}

int main(int argc,char ** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(1024,1024);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Moving Ship");
	glutSwapBuffers();
	glutDisplayFunc(display);
	glutKeyboardFunc(myKeyboardFunc);
	glutSpecialFunc(processSpecialKeys);
	glEnable(GL_SMOOTH);
	init();
	glutMainLoop();
    return 0;
}
