#ifdef _WIN32
  #include <windows.h>
  #include <GL/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>


int  currentScene = 1;
bool isNight      = false;
bool nightLights  = false;

float cloudX1 = 10.0f,  cloudY1 = 88.0f;
float cloudX2 = 45.0f,  cloudY2 = 83.0f;
float cloudX3 = 75.0f,  cloudY3 = 91.0f;

float carX1 = -20.0f;
float carX2 = 115.0f;
float carX3 = -35.0f;

const float CLOUD_SPEED1 = 0.25f;
const float CLOUD_SPEED2 = 0.05f;
const float CLOUD_SPEED3 = 0.06f;
const float CAR_SPEED1   = 0.25f;
const float CAR_SPEED2   = 0.20f;
const float CAR_SPEED3   = 0.18f;


///circle
void drawCircle(float cx, float cy, float radius, int seg)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; ++i) {
        float a = 2.0f * 3.14159f * i / seg;
        glVertex2f(cx + radius * cosf(a), cy + radius * sinf(a));
    }
    glEnd();
}

///SKY
void drawSky()
{
    float tr, tg, tb, br, bg, bb;
    if (!isNight) {
        tr=0.39f; tg=0.71f; tb=0.94f;
        br=0.71f; bg=0.88f; bb=0.98f;
    } else {
        tr=0.02f; tg=0.02f; tb=0.12f;
        br=0.04f; bg=0.04f; bb=0.20f;
    }
    glBegin(GL_QUADS);
        glColor3f(br,bg,bb);
        glVertex2f(  0,  0);
        glColor3f(br,bg,bb);
        glVertex2f(100,  0);
        glColor3f(tr,tg,tb);
        glVertex2f(100,100);
        glColor3f(tr,tg,tb);
        glVertex2f(  0,100);
    glEnd();
}

void drawStars()
{
    if (!isNight) return;
    static const float stars[][2] = {
        {5,95},{12,98},{22,93},{30,97},{38,94},{50,99},
        {60,96},{70,95},{78,98},{88,93},{95,97},{15,87},
        {28,90},{42,88},{55,85},{65,89},{80,87},{92,91},
        {8,82},{35,84},{72,82},{18,75},{48,78},{83,76},
        {3,70},{62,72},{90,68}
    };
    const int N = (int)(sizeof(stars)/sizeof(stars[0]));
    glColor3f(1.0f,1.0f,0.9f);
    glPointSize(1.8f);
    glBegin(GL_POINTS);
    for (int i=0;i<N;++i) glVertex2f(stars[i][0],stars[i][1]);
    glEnd();
    glPointSize(1.0f);
}

void drawSun()
{
    glColor4f(1.0f,0.95f,0.60f,0.20f);
    drawCircle(85,88,5.5f,40);
    glColor3f(1.0f,0.90f,0.20f);
    drawCircle(85,88,3.8f,40);
}

void drawMoon()
{
    glColor3f(0.95f,0.95f,0.80f);
    drawCircle(80,88,4.0f,40);
    glColor3f(0.02f,0.02f,0.12f);
    drawCircle(82.5f,89.5f,3.2f,40);
}

void drawCloud(float cx, float cy, float scale)
{
    if (!isNight)
        glColor3f(1.00f,1.00f,1.00f);
    else
        glColor3f(0.50f,0.52f,0.62f);
    drawCircle(cx,              cy,              3.0f*scale,24);
    drawCircle(cx+3.5f*scale,  cy+0.5f*scale,   2.5f*scale,24);
    drawCircle(cx-3.0f*scale,  cy+0.3f*scale,   2.2f*scale,24);
    drawCircle(cx+1.0f*scale,  cy+2.0f*scale,   2.0f*scale,24);
}

///  ROAD
void drawRoad()
{
    glColor3f(0.75f,0.73f,0.68f);
    glBegin(GL_QUADS);
        glVertex2f(0,30);
        glVertex2f(100,30);
        glVertex2f(100,33);
        glVertex2f(0,33);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2f(0,5);
        glVertex2f(100,5);
        glVertex2f(100,8);
        glVertex2f(0,8);
    glEnd();
    glColor3f(0.60f,0.58f,0.54f);
    glLineWidth(1.0f);

    for (int i=0;i<100;i+=8) {
        glBegin(GL_LINES);
            glVertex2f(i,30);
            glVertex2f(i,33);
            glVertex2f(i,5);
            glVertex2f(i,8);
        glEnd();
    }
    glColor3f(0.18f,0.18f,0.18f);
    glBegin(GL_QUADS);
        glVertex2f(0,8);
        glVertex2f(100,8);
        glVertex2f(100,30);
        glVertex2f(0,30);
    glEnd();
    glColor3f(1.0f,0.78f,0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(0,9.5f);
        glVertex2f(100,9.5f);
        glVertex2f(0,28.5f);
        glVertex2f(100,28.5f);
    glEnd();
    glColor3f(1.0f,1.0f,1.0f);
    for (float x=0;x<100;x+=10) {
        glBegin(GL_LINES);
        glVertex2f(x,19);
        glVertex2f(x+6,19);
        glEnd();
    }
    glColor3f(0.75f,0.75f,0.75f);
    for (float x=2;x<100;x+=10) {
        glBegin(GL_LINES);
            glVertex2f(x,24.5f);
            glVertex2f(x+5,24.5f);
            glVertex2f(x,13.5f);
            glVertex2f(x+5,13.5f);
        glEnd();
    }
}

/// Lamp
void drawLamp(float x, float y)
{
    glColor3f(0.55f,0.55f,0.55f);
    glBegin(GL_QUADS);
        glVertex2f(x-0.25f,y);
        glVertex2f(x+0.25f,y);
        glVertex2f(x+0.25f,y+6.0f);
        glVertex2f(x-0.25f,y+6.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(x-0.25f,y+5.6f);
        glVertex2f(x+1.8f,y+5.6f);
        glVertex2f(x+1.8f,y+5.9f);
        glVertex2f(x-0.25f,y+5.9f);
    glEnd();
    if (isNight) {
        glColor3f(1.0f,0.95f,0.70f);
        drawCircle(x+1.8f,y+5.7f,0.55f,12);
        glColor4f(1.0f,0.95f,0.55f,0.09f);
        drawCircle(x+1.8f,y+5.7f,4.0f,20);
    } else {
        glColor3f(0.85f,0.85f,0.75f);
        drawCircle(x+1.8f,y+5.7f,0.55f,12);
    }
}


///  CAR
void drawCar(float x, float y, float r, float g, float b, bool facingRight)
{
    glPushMatrix();
    glTranslatef(x,y,0.0f);
    if (nightLights) {
        glColor4f(1.0f,1.0f,0.6f,0.15f);
        if (facingRight) {
            glBegin(GL_TRIANGLES);
                glVertex2f(5.5f,1.0f);
                glVertex2f(22.0f,-1.0f);
                glVertex2f(22.0f,3.0f);
            glEnd();
        } else {
            glBegin(GL_TRIANGLES);
                glVertex2f(-5.5f,1.0f);
                glVertex2f(-22.0f,-1.0f);
                glVertex2f(-22.0f,3.0f);
            glEnd();
        }
    }
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glVertex2f(-5.0f,0.0f);
        glVertex2f(5.0f,0.0f);
        glVertex2f(5.0f,2.5f);
        glVertex2f(-5.0f,2.5f);
    glEnd();

    glColor3f(r*0.75f,g*0.75f,b*0.75f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f,2.5f);
        glVertex2f(2.5f,2.5f);
        glVertex2f(2.0f,4.2f);
        glVertex2f(-2.0f,4.2f);
    glEnd();

    glColor3ub(180,220,240);
    glBegin(GL_QUADS);
        glVertex2f(-2.3f,2.7f);
        glVertex2f(-0.2f,2.7f);
        glVertex2f(-0.2f,4.0f);
        glVertex2f(-2.3f,4.0f);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2f(0.2f,2.7f);
        glVertex2f(2.3f,2.7f);
        glVertex2f(2.3f,4.0f);
        glVertex2f(0.2f,4.0f);
    glEnd();

    glColor3ub(30,30,30);
    drawCircle(-3.0f,0,1.0f,20);
    drawCircle(3.0f,0,1.0f,20);

    glColor3ub(180,180,180);
    drawCircle(-3.0f,0,0.4f,20);
    drawCircle(3.0f,0,0.4f,20);

    if (facingRight) {
        glColor3ub(255,255,150);
        drawCircle( 5.0f,1.2f,0.5f,15);
        glColor3ub(255, 80, 80);
        drawCircle(-5.0f,1.2f,0.5f,15);
    } else {
        glColor3ub(255,255,150);
        drawCircle(-5.0f,1.2f,0.5f,15);
        glColor3ub(255, 80, 80);
        drawCircle( 5.0f,1.2f,0.5f,15);
    }
    glPopMatrix();
}

///TREE
void drawTree(float x, float y, float scale)
{
    float tw = 0.8f*scale;
    glColor3f(0.42f,0.26f,0.10f);
    glBegin(GL_QUADS);
        glVertex2f(x-tw,y);
        glVertex2f(x+tw,y);
        glVertex2f(x+tw,y+4.0f*scale);
        glVertex2f(x-tw,y+4.0f*scale);
    glEnd();
    glColor3f(0.18f,0.58f,0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x-3.5f*scale,y+3.0f*scale);
        glVertex2f(x+3.5f*scale,y+3.0f*scale);
        glVertex2f(x,           y+8.0f*scale);
    glEnd();
    glColor3f(0.15f,0.52f,0.15f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x-2.8f*scale,y+5.5f*scale);
        glVertex2f(x+2.8f*scale,y+5.5f*scale);
        glVertex2f(x,           y+9.5f*scale);
    glEnd();
    glColor3f(0.12f,0.46f,0.12f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x-2.0f*scale,y+7.5f*scale);
        glVertex2f(x+2.0f*scale,y+7.5f*scale);
        glVertex2f(x,           y+11.0f*scale);
    glEnd();
}

///HOUSE
void drawHouse(float x, float y, float w, float h,
               float r, float g, float b,
               float rr, float rg, float rb)
{
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glVertex2f(x,y);
        glVertex2f(x+w,y);
        glVertex2f(x+w,y+h);
        glVertex2f(x,y+h);
    glEnd();
    glColor3f(rr,rg,rb);
    glBegin(GL_TRIANGLES);
        glVertex2f(x-0.5f,y+h);
        glVertex2f(x+w+0.5f,y+h);
        glVertex2f(x+w*0.5f,y+h+w*0.45f);
    glEnd();
    glColor3f(0.35f,0.20f,0.08f);
    float dx=x+w*0.40f, dw=w*0.20f, dh=h*0.30f;
    glBegin(GL_QUADS);
        glVertex2f(dx,y);
        glVertex2f(dx+dw,y);
        glVertex2f(dx+dw,y+dh);
        glVertex2f(dx,y+dh);
    glEnd();
    float wx=x+w*0.12f, wx2=x+w*0.62f;
    float wy=y+h*0.45f, ww=w*0.16f, wh=h*0.22f;
    float wc=isNight?0.95f:1.0f;
    glColor3f(wc,wc*0.90f,isNight?0.30f:0.85f);
    glBegin(GL_QUADS);
        glVertex2f(wx,wy);
        glVertex2f(wx+ww,wy);
        glVertex2f(wx+ww,wy+wh);
        glVertex2f(wx,wy+wh);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(wx2,wy);
        glVertex2f(wx2+ww,wy);
        glVertex2f(wx2+ww,wy+wh);
        glVertex2f(wx2,wy+wh);
    glEnd();
    if (isNight) {
        glColor4f(1.0f,0.85f,0.30f,0.10f);
        drawCircle(wx+ww*0.5f,wy+wh*0.5f,ww*2.2f,16);
        drawCircle(wx2+ww*0.5f,wy+wh*0.5f,ww*2.2f,16);
    }
    glColor3f(0.55f,0.30f,0.18f);
    float chx=x+w*0.70f, chw=w*0.08f, chh=h*0.28f;
    glBegin(GL_QUADS);
        glVertex2f(chx,y+h);
        glVertex2f(chx+chw,y+h);
        glVertex2f(chx+chw,y+h+chh);
        glVertex2f(chx,y+h+chh);
    glEnd();
}

/// RIVER
void drawRiver()
{
    if (!isNight) glColor3f(0.27f,0.62f,0.82f);
    else          glColor3f(0.08f,0.18f,0.40f);
    glBegin(GL_QUADS);
        glVertex2f(0,44);
        glVertex2f(100,44);
        glVertex2f(100,50);
        glVertex2f(0,50);
    glEnd();
    glColor3f(0.70f,0.60f,0.40f);
    glBegin(GL_QUADS);
        glVertex2f(0,43.2f);
        glVertex2f(100,43.2f);
        glVertex2f(100,44.0f);
        glVertex2f(0,44.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(0,50.0f);
        glVertex2f(100,50.0f);
        glVertex2f(100,50.8f);
        glVertex2f(0,50.8f);
    glEnd();
    if (!isNight)
        glColor4f(1.0f,1.0f,1.0f,0.20f);
    else
        glColor4f(1.0f,1.0f,1.0f,0.07f);
    glLineWidth(1.0f);
    for (float xi=5;xi<100;xi+=12) {
        glBegin(GL_LINES);
            glVertex2f(xi,47.5f);
            glVertex2f(xi+5,47.5f);
            glVertex2f(xi+2,46.0f);
             glVertex2f(xi+6,46.0f);
        glEnd();
    }
}

///  RIVER2
void drawRiver2()
{
    if (!isNight) glColor3f(0.30f,0.55f,0.85f);
    else          glColor3f(0.10f,0.20f,0.45f);
    glBegin(GL_QUADS);
        glVertex2f(28,37); glVertex2f(62,37);
        glVertex2f(60,44); glVertex2f(30,44);
    glEnd();
    glColor4f(1.0f,1.0f,1.0f,isNight?0.08f:0.18f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(32,39); glVertex2f(42,39);
        glVertex2f(45,41); glVertex2f(56,41);
        glVertex2f(35,43); glVertex2f(48,43);
    glEnd();
}
///  GRASS
void drawGrass(float y0, float y1)
{
    if (!isNight) glColor3f(0.24f,0.60f,0.18f);
    else          glColor3f(0.10f,0.28f,0.08f);
    glBegin(GL_QUADS);
        glVertex2f(0,y0); glVertex2f(100,y0);
        glVertex2f(100,y1); glVertex2f(0,y1);
    glEnd();
}

//  BENCH
void drawBench(float x, float y, float scale)
{
    float s = scale;

    ///Night
    float nt = isNight ? 0.55f : 1.0f;

    ///Legs
    glColor3ub((GLubyte)(90*nt),(GLubyte)(55*nt),(GLubyte)(20*nt));

    /// Left leg
    glBegin(GL_QUADS);
        glVertex2f(x+0.3f*s, y);
        glVertex2f(x+0.8f*s, y);
        glVertex2f(x+0.8f*s, y+1.8f*s);
        glVertex2f(x+0.3f*s, y+1.8f*s);
    glEnd();
    /// Right leg
    glBegin(GL_QUADS);
        glVertex2f(x+3.7f*s, y);
        glVertex2f(x+4.2f*s, y);
        glVertex2f(x+4.2f*s, y+1.8f*s);
        glVertex2f(x+3.7f*s, y+1.8f*s);
    glEnd();

    ///Seat plank
    glColor3ub((GLubyte)(160*nt),(GLubyte)(100*nt),(GLubyte)(45*nt));
    glBegin(GL_QUADS);
        glVertex2f(x,          y+1.6f*s);
        glVertex2f(x+4.5f*s,   y+1.6f*s);
        glVertex2f(x+4.5f*s,   y+2.1f*s);
        glVertex2f(x,          y+2.1f*s);
    glEnd();

    // Wood grain lines on seat
    glColor3ub((GLubyte)(120*nt),(GLubyte)(75*nt),(GLubyte)(28*nt));
    glLineWidth(0.6f);
    glBegin(GL_LINES);
        glVertex2f(x+1.5f*s, y+1.6f*s);
        glVertex2f(x+1.5f*s, y+2.1f*s);
        glVertex2f(x+3.0f*s, y+1.6f*s);
        glVertex2f(x+3.0f*s, y+2.1f*s);
    glEnd();

    /// Back-rest supports
    glColor3ub((GLubyte)(90*nt),(GLubyte)(55*nt),(GLubyte)(20*nt));
    // Left support
    glBegin(GL_QUADS);
        glVertex2f(x+0.3f*s, y+2.1f*s);
        glVertex2f(x+0.8f*s, y+2.1f*s);
        glVertex2f(x+0.8f*s, y+3.9f*s);
        glVertex2f(x+0.3f*s, y+3.9f*s);
    glEnd();
    // Right support
    glBegin(GL_QUADS);
        glVertex2f(x+3.7f*s, y+2.1f*s);
        glVertex2f(x+4.2f*s, y+2.1f*s);
        glVertex2f(x+4.2f*s, y+3.9f*s);
        glVertex2f(x+3.7f*s, y+3.9f*s);
    glEnd();

    // Back-rest rail
    glColor3ub((GLubyte)(160*nt),(GLubyte)(100*nt),(GLubyte)(45*nt));
    // Top rail
    glBegin(GL_QUADS);
        glVertex2f(x,          y+3.5f*s);
        glVertex2f(x+4.5f*s,   y+3.5f*s);
        glVertex2f(x+4.5f*s,   y+3.9f*s);
        glVertex2f(x,          y+3.9f*s);
    glEnd();
    // Mid rail
    glBegin(GL_QUADS);
        glVertex2f(x,          y+2.7f*s);
        glVertex2f(x+4.5f*s,   y+2.7f*s);
        glVertex2f(x+4.5f*s,   y+3.1f*s);
        glVertex2f(x,          y+3.1f*s);
    glEnd();
}


///  DISPLAY SCENE 1
void displayScene1()
{
    drawSky();
    drawStars();
    if (!isNight) drawSun(); else drawMoon();

    drawCloud(cloudX1,cloudY1,0.70f);
    drawCloud(cloudX2,cloudY2,0.60f);
    drawCloud(cloudX3,cloudY3,0.55f);


    drawRiver();

    drawGrass(33.0f, 43.2f);

    drawBench( 5.0f, 33.5f, 0.85f);
    drawBench(22.0f, 33.5f, 0.85f);
    drawBench(40.0f, 33.5f, 0.85f);
    drawBench(58.0f, 33.5f, 0.85f);
    drawBench(76.0f, 33.5f, 0.85f);

    drawRoad();

    drawLamp(15,30);
    drawLamp(40,30);
    drawLamp(65,30);
    drawLamp(90,30);

    drawCar(carX1,      12.5f, 0.85f,0.10f,0.10f, true);
    drawCar(carX3+20,   22.0f, 0.10f,0.40f,0.90f, true);
    drawCar(carX2,      25.5f, 0.15f,0.70f,0.20f, false);
}

///  SCENE 2 Hill
void drawScene2Hills()
{
    glColor3f(0.45f,0.68f,0.38f);
    drawCircle(20,58,18,50);
    drawCircle(55,55,22,50);
    drawCircle(85,57,16,50);
    if (!isNight)
        glColor3f(0.55f,0.80f,0.35f);
    else
        glColor3f(0.25f,0.45f,0.18f);
    glBegin(GL_QUADS);
        glVertex2f(0,0);
    glVertex2f(100,0);
        glVertex2f(100,55);
        glVertex2f(0,55);
    glEnd();
    glColor3f(0.30f,0.58f,0.26f);
    drawCircle(10,55,14,50);
    drawCircle(38,52,18,50);
    drawCircle(70,53,20,50);
    drawCircle(96,54,12,50);
}

//  GATE
void drawGate(float x, float y, float w)
{
    float nt = isNight ? 0.55f : 1.0f;

    // Gate posts
    glColor3ub((GLubyte)(155*nt),(GLubyte)(120*nt),(GLubyte)(70*nt));
    glBegin(GL_QUADS);  // left post
        glVertex2f(x,        y);
        glVertex2f(x+0.5f,   y);
        glVertex2f(x+0.5f,   y+3.5f);
        glVertex2f(x,  y+3.5f);
    glEnd();
    glBegin(GL_QUADS);  // right post
        glVertex2f(x+w-0.5f, y);
        glVertex2f(x+w,      y);
        glVertex2f(x+w,      y+3.5f);
        glVertex2f(x+w-0.5f, y+3.5f);
    glEnd();

    // Gate door planks (two halves)
    float hw = (w - 1.0f) * 0.48f;
    glColor3ub((GLubyte)(185*nt),(GLubyte)(148*nt),(GLubyte)(88*nt));
    glBegin(GL_QUADS);  // left half
        glVertex2f(x+0.5f,      y+0.2f);
        glVertex2f(x+0.5f+hw,  y+0.2f);
        glVertex2f(x+0.5f+hw,   y+3.0f);
        glVertex2f(x+0.5f,     y+3.0f);
    glEnd();
    glBegin(GL_QUADS);  // right half
        glVertex2f(x+w-0.5f-hw, y+0.2f);
        glVertex2f(x+w-0.5f,  y+0.2f);
        glVertex2f(x+w-0.5f,    y+3.0f);
glVertex2f(x+w-0.5f-hw,y+3.0f);
    glEnd();

    // Horizontal cross-bars
    glColor3ub((GLubyte)(130*nt),(GLubyte)(100*nt),(GLubyte)(55*nt));
    glLineWidth(0.7f);
    for (float yb = y+1.0f; yb < y+3.0f; yb += 0.95f) {
        glBegin(GL_LINES);
            glVertex2f(x+0.5f,          yb);
            glVertex2f(x+0.5f+hw,       yb);
            glVertex2f(x+w-0.5f-hw,     yb);
            glVertex2f(x+w-0.5f,        yb);
        glEnd();
    }

    /// Short fence to left and right of gate
    glColor3ub((GLubyte)(155*nt),(GLubyte)(120*nt),(GLubyte)(70*nt));
    float flen = 3.5f;
    /// Left fence pickets
    for (float fx = x-flen; fx < x-0.1f; fx += 1.1f) {
        glBegin(GL_QUADS);
            glVertex2f(fx, y); glVertex2f(fx+0.4f, y);
            glVertex2f(fx+0.4f, y+2.5f); glVertex2f(fx, y+2.5f);
        glEnd();
    }
    glBegin(GL_QUADS);  /// left rail
        glVertex2f(x-flen, y+1.8f); glVertex2f(x, y+1.8f);
        glVertex2f(x, y+2.1f); glVertex2f(x-flen, y+2.1f);
    glEnd();
    /// Right fence pickets
    for (float fx = x+w+0.1f; fx < x+w+flen; fx += 1.1f) {
        glBegin(GL_QUADS);
            glVertex2f(fx, y); glVertex2f(fx+0.4f, y);
            glVertex2f(fx+0.4f, y+2.5f); glVertex2f(fx, y+2.5f);
        glEnd();
    }
    glBegin(GL_QUADS);  /// right rail
        glVertex2f(x+w, y+1.8f); glVertex2f(x+w+flen, y+1.8f);
        glVertex2f(x+w+flen, y+2.1f); glVertex2f(x+w, y+2.1f);
    glEnd();
}

///  VILLAGE
void drawVillage()
{
    drawHouse( 2, 34,12,10, 0.80f,0.55f,0.40f, 0.55f,0.22f,0.12f);
    drawHouse(17, 34,10, 9, 0.92f,0.88f,0.75f, 0.65f,0.40f,0.20f);
    drawHouse(62, 34,13,11, 0.72f,0.78f,0.85f, 0.28f,0.35f,0.55f);
    drawHouse(80, 34,14,10, 0.95f,0.95f,0.90f, 0.50f,0.30f,0.18f);
    drawTree( 1,34,0.90f); drawTree(14,34,0.80f); drawTree(29,34,1.00f);
    drawTree(35,34,0.75f); drawTree(55,34,0.85f); drawTree(76,34,0.90f);
    drawTree(97,34,0.80f);
}

void displayScene2()
{
    drawSky();
    drawStars();
    if (!isNight) drawSun(); else drawMoon();

    drawCloud(cloudX1,cloudY1,0.70f);
    drawCloud(cloudX2,cloudY2,0.60f);
    drawCloud(cloudX3,cloudY3,0.55f);

    drawScene2Hills();
    drawRiver2();
    drawGrass(33.0f, 44.0f);
    drawVillage();
    drawRoad();

    drawLamp( 8,30); drawLamp(30,30);
    drawLamp(55,30); drawLamp(78,30);

    drawCar(carX1,      12.5f, 0.85f,0.10f,0.10f, true);
    drawCar(carX3+20,   22.0f, 0.10f,0.40f,0.90f, true);
    drawCar(carX2,      25.5f, 0.15f,0.70f,0.20f, false);
}

//  MAIN DISPLAY
void display()
{
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if (currentScene==1) displayScene1();
    else                  displayScene2();
    glutSwapBuffers();
}

///Update
float speedMul = 1.0f;
void update(int)
{
    cloudX1 += CLOUD_SPEED1 * speedMul; if(cloudX1>115) cloudX1=-25;
    cloudX2 += CLOUD_SPEED2 * speedMul; if(cloudX2>115) cloudX2=-25;
    cloudX3 += CLOUD_SPEED3 * speedMul; if(cloudX3>115) cloudX3=-25;
    carX1   += CAR_SPEED1   * speedMul; if(carX1> 115)  carX1=-12;
    carX2   -= CAR_SPEED2   * speedMul; if(carX2<-15)   carX2= 115;
    carX3   += CAR_SPEED3   * speedMul; if(carX3> 115)  carX3=-35;
    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

///  KEYBOARD
void handleKey(unsigned char key, int, int)
{
    switch(key) {
        case '1': currentScene=1; break;
        case '2': currentScene=2; break;
        case 'n': case 'N': isNight=true;  nightLights=true;  break;
        case 'd': case 'D': isNight=false; nightLights=false; break;
        case 'u': case 'U':                          /// speed up
            speedMul += 0.5f;
            if (speedMul > 5.0f) speedMul = 5.0f;   /// max 5x
            break;
        case 'l': case 'L':                          /// slow down
            speedMul -= 0.5f;
            if (speedMul < 0.5f) speedMul = 0.5f;   /// min 0.5x
            break;
        case 'q': case 'Q': exit(0);
    }
    glutPostRedisplay();
}

///  MOUSE CLICK
void handleMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isNight     = !isNight;
        nightLights = isNight;
        glutPostRedisplay();
    }
}

///MAIN
void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,100,0,100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(900,600);
    glutCreateWindow("Scenic Landscape  ");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKey);
    glutMouseFunc(handleMouse);
    glutTimerFunc(16,update,0);
    glutMainLoop();
    return 0;
}
