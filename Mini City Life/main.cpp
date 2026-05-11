#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

bool isNight = false;
bool nightLight = false;

float cloudX1 = 10, cloudX2 = 50, cloudX3 = 80;
float carX1 = -30.0f, carX2 = 130.0f, carX3 = -60.0f;
float carSpeed1 = 0.3f, carSpeed2 = 0.8f, carSpeed3 = 0.2f;

void drawCircle(float cx, float cy, float r, int seg) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < seg; i++) {
        float theta = 2.0f * 3.14159265f * i / seg;
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

//  SKY

void drawSky() {
    if (!isNight) {
        ///  sky using two quads
        glBegin(GL_QUADS);
            glColor3ub(100, 180, 240);   // top
            glVertex2f(0, 100);
            glVertex2f(100, 100);
            glColor3ub(180, 225, 250);   // horizon
            glVertex2f(100, 50);
            glVertex2f(0, 50);
        glEnd();
    } else {
        /// navy to dark blue
        glBegin(GL_QUADS);
            glColor3ub(5, 5, 30);
            glVertex2f(0, 100);
            glVertex2f(100, 100);
            glColor3ub(10, 10, 50);
            glVertex2f(100, 50);
            glVertex2f(0, 50);
        glEnd();
        // Stars
        glColor3ub(255, 255, 220);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
            glVertex2f(10, 95);
            glVertex2f(25, 90);
            glVertex2f(40, 96);
            glVertex2f(55, 88);
            glVertex2f(70, 94);
            glVertex2f(85, 91);
            glVertex2f(15, 82);
            glVertex2f(33, 85);
            glVertex2f(60, 80);
            glVertex2f(78, 86);
            glVertex2f(90, 78);
            glVertex2f(5, 75);
            glVertex2f(48, 92);
            glVertex2f(63, 97);
            glVertex2f(20, 98);
        glEnd();
    }
}

void drawSun() {
    glColor3ub(255, 215, 0);
    drawCircle(85, 88, 5, 100);
    glColor4f(1.0f, 0.9f, 0.3f, 0.15f);
    drawCircle(85, 88, 7, 100);
}

void drawMoon() {
    glColor3ub(230, 230, 200);
    drawCircle(85, 88, 4.5f, 60);
    // Crescent cutout
    glColor3ub(10, 10, 50);
    drawCircle(87, 89, 3.8f, 60);
}

void drawCloud(float x, float y, float s) {
    if (isNight)
        glColor3ub(50, 55, 75);
    else
        glColor3ub(255, 255, 255);

    drawCircle(x,       y,       3.5*s, 40);
    drawCircle(x+3.5*s, y+1.2*s, 4.0*s, 40);
    drawCircle(x+7*s,   y,       3.5*s, 40);
    drawCircle(x+3.5*s, y-0.8*s, 3.0*s, 40);
}

void drawHills() {
    // mountains
    glColor3ub(154, 176, 204);
    glBegin(GL_POLYGON);
        glVertex2f(-3,63);
        glVertex2f(12,45);
        glVertex2f(26,52);
        glVertex2f(32,63);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(18,63);
        glVertex2f(34,41);
        glVertex2f(50,50);
        glVertex2f(59,63);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(47,63);
        glVertex2f(63,43);
        glVertex2f(79,52);
        glVertex2f(88,63);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(76,63);
        glVertex2f(91,44);
        glVertex2f(103,52);
        glVertex2f(106,63);
    glEnd();

    // --- MID mountains
    glColor3ub(110, 140, 170);
    glBegin(GL_POLYGON);
        glVertex2f(-1,65);
        glVertex2f(12,51);
        glVertex2f(28,58);
        glVertex2f(41,65);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(23,65);
        glVertex2f(42,46);
        glVertex2f(60,56);
        glVertex2f(73,65);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(59,65);
        glVertex2f(75,49);
        glVertex2f(91,58);
        glVertex2f(103,65);
    glEnd();

    ///mid mountains
    glColor3ub(240, 245, 255);
    glBegin(GL_POLYGON);
        glVertex2f(38,51);
        glVertex2f(42,46);
        glVertex2f(46,51);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(71,54);
        glVertex2f(75,49);
        glVertex2f(79,54);
    glEnd();

    /// Treeline at mountain base
    glColor3ub(45, 75, 45);
    glBegin(GL_QUADS);
        glVertex2f(0,65);
        glVertex2f(100,65);
        glVertex2f(100,67);
        glVertex2f(0,67);
    glEnd();

    ///FRONT HILLS
    /// Hill 1
    glBegin(GL_POLYGON);
        glColor3ub(58, 170, 58);
        glVertex2f(-3,67);
        glVertex2f(10,55);
        glColor3ub(30, 122, 30);
        glVertex2f(20,60);
        glVertex2f(29,67);
    glEnd();
    /// Hill 2
    glBegin(GL_POLYGON);
        glColor3ub(58, 170, 58);
        glVertex2f(12,67);
        glVertex2f(24,53);
        glColor3ub(30, 122, 30);
        glVertex2f(38,59);
        glVertex2f(55,67);
    glEnd();
    /// Hill 3
    glBegin(GL_POLYGON);
        glColor3ub(58, 170, 58);
        glVertex2f(38,67);
        glVertex2f(50,50);
        glColor3ub(30, 122, 30);
        glVertex2f(64,58);
        glVertex2f(80,67);
    glEnd();
    /// Hill 4
    glBegin(GL_POLYGON);
        glColor3ub(58, 170, 58);
        glVertex2f(67,67);
        glVertex2f(79,52);
        glColor3ub(30, 122, 30);
        glVertex2f(92,59);
        glVertex2f(103,67);
    glEnd();

    glColor3ub(34, 110, 34);
    glBegin(GL_QUADS);
        glVertex2f(0,50);
        glVertex2f(100,50);
        glVertex2f(100,67);
        glVertex2f(0,67);
    glEnd();
}

///  RIVER
void drawRiver() {
    /// River body
    if (!isNight)
        glColor3ub(70, 160, 210);
    else
        glColor3ub(20, 50, 100);

    glBegin(GL_QUADS);
        glVertex2f(0,  44);
        glVertex2f(100, 44);
        glVertex2f(100, 50);
        glVertex2f(0,  50);
    glEnd();

    /// Shimmer lines
    if (!isNight)
        glColor3ub(180, 220, 245);
    else
        glColor3ub(40,  80, 130);
    glLineWidth(1.0f);

    for (int i = 5; i < 100; i += 12) {
        glBegin(GL_LINES);
            glVertex2f(i,     47.5f);
            glVertex2f(i + 5, 47.5f);
        glEnd();
        glBegin(GL_LINES);
            glVertex2f(i + 2, 46.0f);
            glVertex2f(i + 6, 46.0f);
        glEnd();
    }

    /// River side
    glColor3ub(180, 155, 100);
    glBegin(GL_QUADS);
        glVertex2f(0, 43.2f);
        glVertex2f(100, 43.2f);
        glVertex2f(100, 44.0f);
        glVertex2f(0, 44.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(0, 50.0f);
        glVertex2f(100, 50.0f);
        glVertex2f(100, 50.8f);
        glVertex2f(0, 50.8f);
    glEnd();
}

///  GROUND
void drawGrass() {
    glColor3ub(60, 140, 60);
    glBegin(GL_QUADS);
        glVertex2f(0, 33);
        glVertex2f(100, 33);
        glVertex2f(100, 43.2f);
        glVertex2f(0, 43.2f);
    glEnd();
}


///  ROAD + FOOTPATH
void drawRoadAndFootpath() {
    glColor3ub(190, 185, 175);   /// top footpath
    glBegin(GL_QUADS);
        glVertex2f(0, 30);
        glVertex2f(100, 30);
        glVertex2f(100, 33);
        glVertex2f(0, 33);
    glEnd();

    glColor3ub(190, 185, 175);   /// bottom footpath
    glBegin(GL_QUADS);
        glVertex2f(0, 5);
        glVertex2f(100, 5);
        glVertex2f(100, 8);
        glVertex2f(0, 8);
    glEnd();

    /// Footpath tiles
    glColor3ub(160, 155, 145);
    for (int i = 0; i < 100; i += 8) {
        glBegin(GL_LINES);
            glVertex2f(i, 30); glVertex2f(i, 33);
        glEnd();
    }
    /// Footpath tiles
    for (int i = 0; i < 100; i += 8) {
        glBegin(GL_LINES);
            glVertex2f(i, 5); glVertex2f(i, 8);
        glEnd();
    }

    // Kerb lines
    glColor3ub(220, 215, 200);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(0, 30);
        glVertex2f(100, 30);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(0,  8);
        glVertex2f(100,  8);
    glEnd();

    ///rOAD SURFACE
    glColor3ub(45, 45, 45);
    glBegin(GL_QUADS);
        glVertex2f(0,  8);
        glVertex2f(100,  8);
        glVertex2f(100, 30);
        glVertex2f(0, 30);
    glEnd();

    // Road edge lines yellow
    glColor3ub(255, 200, 0);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(0, 9.5f); glVertex2f(100, 9.5f);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(0, 28.5f); glVertex2f(100, 28.5f);
    glEnd();

    // Center dashed white line
    glColor3ub(255, 255, 255);
    glLineWidth(1.5f);
    for (int i = 0; i < 100; i += 10) {
        glBegin(GL_LINES);
            glVertex2f(i,     19);
            glVertex2f(i + 6, 19);
        glEnd();
    }

    // Lane divider dashes
    glColor3ub(200, 200, 200);
    for (int i = 2; i < 100; i += 10) {
        glBegin(GL_LINES);
            glVertex2f(i,     24.5f);
            glVertex2f(i + 5, 24.5f);
        glEnd();
    }
    // Lane divider dashes
    for (int i = 2; i < 100; i += 10) {
        glBegin(GL_LINES);
            glVertex2f(i,     13.5f);
            glVertex2f(i + 5, 13.5f);
        glEnd();
    }

    //lamp posts on footpath
    glColor3ub(80, 80, 80);
    int lampX[] = {15, 40, 65, 90};
    for (int i = 0; i < 4; i++) {
        // Post
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(lampX[i], 30);
            glVertex2f(lampX[i], 36);
        glEnd();
        // Arm
        glBegin(GL_LINES);
            glVertex2f(lampX[i],     36);
            glVertex2f(lampX[i] + 2, 36);
        glEnd();

        /// Lamp head
        if (!isNight)
            glColor3ub(100, 100, 100);
        else
            glColor3ub(255, 240, 150);
        drawCircle(lampX[i] + 2, 36, 0.8f, 20);

        /// Night
        if (nightLight) {
            glColor4f(1.0f, 0.95f, 0.5f, 0.12f);
            drawCircle(lampX[i] + 2, 36, 4.0f, 30);
        }
        glColor3ub(80, 80, 80);
    }
}

void drawCar(float x, float y, float r, float g, float b, bool facingRight) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    /// night only
    if (nightLight) {
        glColor4f(1.0f, 1.0f, 0.6f, 0.2f);
        if (facingRight) {
            glBegin(GL_TRIANGLES);
                glVertex2f(5.5f, 1.0f);
                glVertex2f(18, -1);
                glVertex2f(18, 3);
            glEnd();
        } else {
            glBegin(GL_TRIANGLES);
                glVertex2f(-5.5f, 1.0f);
                glVertex2f(-18, -1);
                glVertex2f(-18, 3);
            glEnd();
        }
    }

    /// Car body
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(-5, 0);
        glVertex2f(5, 0);
        glVertex2f(5, 2.5f);
        glVertex2f(-5, 2.5f);
    glEnd();

    /// Roof
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, 2.5f);
        glVertex2f(2.5f, 2.5f);
        glVertex2f(2.0f,  4.2f);
        glVertex2f(-2.0f, 4.2f);
    glEnd();

    /// Windows
    glColor3ub(180, 220, 240);
    glBegin(GL_QUADS);
        glVertex2f(-2.3f, 2.7f);
        glVertex2f(-0.2f, 2.7f);
        glVertex2f(-0.2f, 4.0f);
        glVertex2f(-2.3f, 4.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(0.2f, 2.7f);
        glVertex2f(2.3f, 2.7f);
        glVertex2f(2.3f, 4.0f);
        glVertex2f(0.2f, 4.0f);
    glEnd();

    /// Wheels
    glColor3ub(30, 30, 30);
    drawCircle(-3.0f, 0, 1.0f, 20);
    drawCircle( 3.0f, 0, 1.0f, 20);
    /// Hubcaps
    glColor3ub(180, 180, 180);
    drawCircle(-3.0f, 0, 0.4f, 20);
    drawCircle( 3.0f, 0, 0.4f, 20);

    /// Headlights
    if (facingRight) {
        glColor3ub(255, 255, 150);
        drawCircle(5.0f, 1.2f, 0.5f, 15);
        glColor3ub(255, 80, 80);
        drawCircle(-5.0f, 1.2f, 0.5f, 15);
    } else {
        glColor3ub(255, 80, 80);
        drawCircle(5.0f, 1.2f, 0.5f, 15);
        glColor3ub(255, 255, 150);
        drawCircle(-5.0f, 1.2f, 0.5f, 15);
    }

    glPopMatrix();
}

void update(int v) {
    cloudX1 += 0.08f; if (cloudX1 > 115) cloudX1 = -25;
    cloudX2 += 0.05f; if (cloudX2 > 115) cloudX2 = -25;
    cloudX3 += 0.06f; if (cloudX3 > 115) cloudX3 = -25;

    carX1 += carSpeed1;  if (carX1  >  115) carX1  = -20;
    carX2 -= carSpeed2;  if (carX2  < -15)  carX2  = 115;
    carX3 += carSpeed3;  if (carX3  >  115) carX3  = -30;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void handleKey(unsigned char key, int x, int y) {
    if (key == 'n' || key == 'N')
        { isNight = true;  nightLight = true;  }
    if (key == 'd' || key == 'D')
        { isNight = false; nightLight = false; }
    glutPostRedisplay();
}


void display() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();
    if (!isNight)
        drawSun();
    else
        drawMoon();

    drawCloud(cloudX1, 88, 0.7f);
    drawCloud(cloudX2, 82, 0.6f);
    drawCloud(cloudX3, 92, 0.5f);

    drawHills();
    drawRiver();
    drawGrass();
    drawRoadAndFootpath();


    drawCar(carX1,       12.5f, 0.8f, 0.0f, 0.0f, true);   /// red, lower right lane
    drawCar(carX3 + 20,  22.0f, 0.0f, 0.5f, 0.9f, true);   /// blue, upper right lane
    drawCar(carX2,       25.5f, 0.2f, 0.7f, 0.2f, false);  /// green, oncoming

    glutSwapBuffers();
}

void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Scenic Landscape ");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKey);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
