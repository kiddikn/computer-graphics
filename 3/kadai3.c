/* kadai3.c 2Dの動画プログラム */
#include <stdlib.h>
#include <GLUT/glut.h> 
#include <math.h>

#define   rocket      1  //ロケット 
#define   star        2  //星
#define r2 2
#define r1 1
float rote = 0.;  
float speed = 0.001; // 図形の移動速度 
float accel = 0.001; // 移動速度を調整するパラメータ 
unsigned char moveFlag = GL_TRUE;// 星の運動(1)か(0)かのフラグ 
float distance;

void resetview(void);

void myinit(void) 
{ 
    int i;
    float quads[][2]={1.,1.,-1.,1.,-1.,-5.,1.,-5.};
    float triangle[][2]={-1.,1.,1.,1.,0.,3.};
    float engine[][2]={-0.3,-5.4,0.3,-5.4,0.,-4.8};
    float leftwing[][2]={-1.,-3.0,-2.,-4.2,-2.,-5.5,-1.,-5.};
    float rightwing[][2]={1.,-3.0,1.,-5.,2.,-5.5,2.,-4.2};
    float Ax,Ay,Bx,By,Cx,Cy;
    float tx = 0.0,ty=0.0;

    glClearColor(0.,0.,0.,1.);    // 消去色 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 消去 

    glNewList( rocket, GL_COMPILE );  // ロケットの表示リスト作成    
        glPushMatrix();
        glRotatef(45.0,1.0,1.0,0.0);
        glTranslatef(0.0, 1.0, -10.0);
        glBegin(GL_QUADS);
        for( i=0; i<4; i++ )glVertex2fv(quads[i]); //本体     
        glEnd();
        glBegin(GL_TRIANGLES);
        for( i=0; i<3; i++ )glVertex2fv(triangle[i]); //一段目 
        glEnd();
        glBegin(GL_TRIANGLES);
        for( i=0; i<3; i++ )glVertex2fv(engine[i]); //engine 
        glEnd();
        glBegin(GL_QUADS);
        for( i=0; i<4; i++ )glVertex2fv(leftwing[i]); //左翼 
        glEnd();
        glBegin(GL_QUADS);
        for( i=0; i<4; i++ )glVertex2fv(rightwing[i]); //右翼 
        glEnd();
        glPopMatrix();
   glEndList(); 

   glNewList( star, GL_COMPILE);     //星の作成
       glPushMatrix();
       glTranslatef(0.0, 1.0, -10.0);
       glScalef(0.1,0.1,0.1);  
       glBegin(GL_TRIANGLE_STRIP);
       for (i=0; i<5; i++){
           Ax = tx + r2*sin(2*M_PI*2*i/10);
           Ay = ty + r2*cos(2*M_PI*2*i/10);
           Bx = tx + r1*sin(2*M_PI*(2*i+1)/10);
           By = ty + r1*cos(2*M_PI*(2*i+1)/10);
           Cx = tx + r2*sin(2*M_PI*(2*i+2)/10);
           Cy = ty + r2*cos(2*M_PI*(2*i+2)/10);
           glVertex2d(tx , ty);
           glVertex2f(Ax, Ay);
           glVertex2f(Bx, By);
           glVertex2d(tx , ty);
           glVertex2f(Bx, By);
           glVertex2f(Cx, Cy);
       }
       glEnd();
       glPopMatrix();
   glEndList();
} 

void display(void) 
{   
    int i;
    float xDisp[]={1.0,3.0,5.0,6.0,7.0};
    float yDisp[]={2.0,8.0,5.0,0.3,4.4};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(0.0,0.0,0.0);
    glTranslatef(3.0, 0.0, -distance);    
    if( moveFlag ) rote += speed; 
    glEnable(GL_DEPTH_TEST); //物体がかぶらないように計算    
    
    glPushMatrix();
    glRotatef(rote,1.0,0.0,0.0);
    glColor3f( 1.,0.,0. ); 
    glCallList(rocket); 
    glPopMatrix();
    glColor3f(1.0,1.0,1.0);
    glCallList(star);          
    glPushMatrix();
    glTranslatef(-6.0,-6.0,5.0);
    glCallList(star);
    glPopMatrix();   

    glPushMatrix();
    glTranslatef(-6.0,6.0,0.0);
    glCallList(star);
    glPopMatrix();   

   glDisable(GL_DEPTH_TEST);
    glutSwapBuffers(); 
} 



void reshape(int w, int h) 
{ 
    h = (h == 0) ? 1 : h; 
    float aspect = (float) w / (float) h;   
    glViewport(0, 0, w, h); 
    glMatrixMode(GL_PROJECTION); // 以降投影用変換行列を設定 
    glLoadIdentity(); 
    gluPerspective( 45.0, aspect, 1.0, 50.0 ); //orthoからperspective/
    glMatrixMode(GL_MODELVIEW);  // 以降モデル表示用行列を設定 
} 

void resetview(void)
{
    distance = 10.0;
}  

void key_input(unsigned char key, int x, int y) //キー振り分け登録 
{ 
    switch(key){ 
        case 27:  // ESC  
        case 13:  exit(0);  // Enter  
        case 'R': rote = 0.0;
                  resetview();
                  break;
        case ' ': moveFlag = !moveFlag; 
                  if( moveFlag ) glutIdleFunc(display); 
                  else  glutIdleFunc(0);   //動かさない
        default:   break; 
    } 
} 

void special_input(int key, int x, int y) //特別キー 
{ 
    switch(key){ 
        case GLUT_KEY_UP:   speed += accel; moveFlag=GL_TRUE; 
                            glutIdleFunc(display); break; 
        case GLUT_KEY_DOWN: speed -= accel; moveFlag=GL_TRUE; 
                            glutIdleFunc(display); break; 
        default: return; 
    } 
} 

int main(int argc, char **argv) 
{ 
    glutInitWindowPosition(0,0);
    glutInitWindowSize (600, 600); 
    glutInit(&argc, argv); 
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); 
    glutCreateWindow(argv[0]); 
    myinit(); 
    glutKeyboardFunc(key_input);  // 通常キー入力による割込関数 
    glutSpecialFunc(special_input); //　矢印キー入力で呼ばれる関数を指定 
    resetview();
    glutDisplayFunc(display); // 描画用関数を指定 
    glutReshapeFunc(reshape); // 再描画時に呼ばれる関数を指定 
    glutIdleFunc(display); // 常時呼ばれる関数を指定 
    glutMainLoop(); 
    return 0; 
} 


