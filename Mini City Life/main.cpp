/*
 * ============================================================
 *  VILLAGE SCENIC LANDSCAPE — OpenGL / GLUT
 * ============================================================
 *  Controls:
 *    N / n  → Night mode
 *    D / d  → Day mode
 *    Q / q  → Quit
 *
 *  How to compile (Linux / macOS):
 *    g++ village_scene.cpp -o village -lGL -lGLU -lglut
 *  Windows (MinGW):
 *    g++ village_scene.cpp -o village -lopengl32 -lglu32 -lfreeglut
 * ============================================================
 *
 *  HOW TO CUSTOMISE THIS FILE (quick guide)
 *  -----------------------------------------
 *  Sky colours    → see "DAY / NIGHT PALETTES" section below
 *  Hills          → drawHills() — change hill count, heights, colours
 *  River          → drawRiver() — adjust x / y coordinates
 *  Houses         → drawHouse() — change roof colour, wall colour, size
 *  Trees          → drawTree()  — change trunk / foliage colours
 *  Cars           → drawCar()   — same as your original code
 *  Road           → drawRoadAndFootpath()
 *  Cloud speed    → cloudSpeedX / cloudSpeedY constants at top
 *  Car speeds     → carSpeed1 / carSpeed2 / carSpeed3
 *  Window size    → glutInitWindowSize(WIDTH, HEIGHT)
 *  Ortho range    → gluOrtho2D(0, 100, 0, 100)  — all coords 0-100
 * ============================================================
 */

#ifdef _WIN32
  #include <windows.h>
  #include <GL/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>

// ─────────────────────────────────────────────
//  GLOBAL STATE
// ─────────────────────────────────────────────
bool isNight     = false;   // true = night palette
bool nightLights = false;   // true = car headlight cones visible

// Moving object positions
float cloudX1 = 10.0f,  cloudX2 = 40.0f,  cloudX3 = 70.0f;
float cloudY1 = 88.0f,  cloudY2 = 83.0f,  cloudY3 = 91.0f;

float carX1 = -20.0f;   // red car  (moves right on lower lane)
float carX2 = 115.0f;   // green car (moves left, oncoming)
float carX3 = -35.0f;   // blue car (moves right on upper lane)

// ─────────────────────────────────────────────
//  SPEEDS  — change these to adjust animation
// ─────────────────────────────────────────────
const float cloudSpeed1 = 0.06f;
const float cloudSpeed2 = 0.04f;
const float cloudSpeed3 = 0.05f;

const float carSpeed1   = 0.22f;
const float carSpeed2   = 0.18f;
const float carSpeed3   = 0.16f;

// ─────────────────────────────────────────────
//  DAY / NIGHT PALETTES
//  Edit these RGB triplets to change sky/star colours
// ─────────────────────────────────────────────
struct RGB { float r, g, b; };

const RGB SKY_TOP_DAY    = {0.40f, 0.70f, 1.00f};
const RGB SKY_BOT_DAY    = {0.75f, 0.90f, 1.00f};
const RGB SKY_TOP_NIGHT  = {0.02f, 0.02f, 0.12f};
const RGB SKY_BOT_NIGHT  = {0.05f, 0.05f, 0.20f};
const RGB CLOUD_DAY      = {1.00f, 1.00f, 1.00f};
const RGB CLOUD_NIGHT    = {0.55f, 0.55f, 0.65f};


// =========================================================
//  UTILITY: draw a filled circle
// =========================================================
void drawCircle(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + radius * cosf(angle),
                   cy + radius * sinf(angle));
    }
    glEnd();
}

// =========================================================
//  SKY  (gradient quad)
// =========================================================
void drawSky()
{
    RGB top = isNight ? SKY_TOP_NIGHT : SKY_TOP_DAY;
    RGB bot = isNight ? SKY_BOT_NIGHT : SKY_BOT_DAY;

    glBegin(GL_QUADS);
        glColor3f(bot.r, bot.g, bot.b); glVertex2f(  0,  0);
        glColor3f(bot.r, bot.g, bot.b); glVertex2f(100,  0);
        glColor3f(top.r, top.g, top.b); glVertex2f(100, 100);
        glColor3f(top.r, top.g, top.b); glVertex2f(  0, 100);
    glEnd();
}

// =========================================================
//  STARS  (night only, simple dots)
// =========================================================
void drawStars()
{
    if (!isNight) return;

    // Hardcoded simple star field — add/remove rows as you like
    static const float stars[][2] = {
        {5,95},{12,98},{22,93},{30,97},{38,94},{50,99},{60,96},
        {70,95},{78,98},{88,93},{95,97},{15,87},{28,90},{42,88},
        {55,85},{65,89},{80,87},{92,91},{8,82},{35,84},{72,82},
        {18,75},{48,78},{83,76},{3,70},{62,72},{90,68}
    };
    const int N = sizeof(stars) / sizeof(stars[0]);

    glColor3f(1.0f, 1.0f, 0.9f);
    glPointSize(1.8f);
    glBegin(GL_POINTS);
    for (int i = 0; i < N; ++i)
        glVertex2f(stars[i][0], stars[i][1]);
    glEnd();
    glPointSize(1.0f);
}

// =========================================================
//  SUN
// =========================================================
void drawSun()
{
    // Glow halo
    glColor4f(1.0f, 0.95f, 0.60f, 0.20f);
    drawCircle(85, 88, 5.5f, 40);

    // Sun disc
    glColor3f(1.0f, 0.90f, 0.20f);
    drawCircle(85, 88, 3.8f, 40);
}

// =========================================================
//  MOON
// =========================================================
void drawMoon()
{
    // Full moon disc
    glColor3f(0.95f, 0.95f, 0.80f);
    drawCircle(80, 88, 4.0f, 40);

    // Bite out of moon (same as sky colour) → crescent effect
    glColor3f(SKY_TOP_NIGHT.r, SKY_TOP_NIGHT.g, SKY_TOP_NIGHT.b);
    drawCircle(82.5f, 89.5f, 3.2f, 40);
}

// =========================================================
//  CLOUD  (three overlapping circles)
// =========================================================
void drawCloud(float cx, float cy, float scale)
{
    RGB c = isNight ? CLOUD_NIGHT : CLOUD_DAY;
    glColor3f(c.r, c.g, c.b);

    drawCircle(cx,          cy,          3.0f * scale, 20);
    drawCircle(cx + 3.5f*scale, cy + 0.5f*scale, 2.5f * scale, 20);
    drawCircle(cx - 3.0f*scale, cy + 0.3f*scale, 2.2f * scale, 20);
    drawCircle(cx + 1.0f*scale, cy + 2.0f*scale, 2.0f * scale, 20);
}

// =========================================================
//  HILLS  (background scenery)
//  Change positions / colours freely here
// =========================================================
void drawHills()
{
    // Back hills (lighter = farther away)
    glColor3f(0.45f, 0.68f, 0.38f);
    drawCircle(20, 58, 18, 50);
    drawCircle(55, 55, 22, 50);
    drawCircle(85, 57, 16, 50);

    // Cover bottom half of the circles so they look like hills
    glColor3f(isNight ? 0.35f : 0.55f,
              isNight ? 0.55f : 0.80f,
              isNight ? 0.25f : 0.35f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f(100, 0);
        glVertex2f(100, 55); glVertex2f(0, 55);
    glEnd();

    // Front hills (darker / greener)
    glColor3f(0.30f, 0.58f, 0.26f);
    drawCircle(10, 55, 14, 50);
    drawCircle(38, 52, 18, 50);
    drawCircle(70, 53, 20, 50);
    drawCircle(96, 54, 12, 50);
}

// =========================================================
//  RIVER
// =========================================================
void drawRiver()
{
    // River body
    glColor3f(isNight ? 0.10f : 0.30f,
              isNight ? 0.20f : 0.55f,
              isNight ? 0.45f : 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(30, 37);
        glVertex2f(60, 37);
        glVertex2f(62, 44);
        glVertex2f(28, 44);
    glEnd();

    // River shimmer lines
    glColor4f(1.0f, 1.0f, 1.0f, isNight ? 0.08f : 0.18f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(32, 39); glVertex2f(42, 39);
        glVertex2f(45, 41); glVertex2f(56, 41);
        glVertex2f(35, 43); glVertex2f(48, 43);
    glEnd();
}

// =========================================================
//  GRASS / GROUND
// =========================================================
void drawGrass()
{
    // Main ground band
    glColor3f(isNight ? 0.15f : 0.30f,
              isNight ? 0.35f : 0.65f,
              isNight ? 0.12f : 0.22f);
    glBegin(GL_QUADS);
        glVertex2f(0,  0); glVertex2f(100,  0);
        glVertex2f(100, 35); glVertex2f(0, 35);
    glEnd();
}

// =========================================================
//  ROAD AND FOOTPATH
// =========================================================
void drawRoadAndFootpath()
{
    // Main road (dark asphalt)
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS);
        glVertex2f(0, 10); glVertex2f(100, 10);
        glVertex2f(100, 30); glVertex2f(0, 30);
    glEnd();

    // Centre line (dashed white)
    glColor3f(0.95f, 0.95f, 0.80f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (float x = 0; x < 100; x += 7) {
        glVertex2f(x,     20); glVertex2f(x + 4, 20);
    }
    glEnd();

    // Footpath / kerb (left side)
    glColor3f(0.70f, 0.70f, 0.65f);
    glBegin(GL_QUADS);
        glVertex2f(0, 8); glVertex2f(100, 8);
        glVertex2f(100, 10); glVertex2f(0, 10);
    glEnd();

    // Footpath / kerb (right side)
    glBegin(GL_QUADS);
        glVertex2f(0, 30); glVertex2f(100, 30);
        glVertex2f(100, 32); glVertex2f(0, 32);
    glEnd();
}

// =========================================================
//  TREE  (positioned by base centre)
// =========================================================
void drawTree(float x, float y, float scale)
{
    // Trunk
    glColor3f(0.45f, 0.28f, 0.10f);
    glBegin(GL_QUADS);
        float tw = 0.8f * scale;
        glVertex2f(x - tw, y);
        glVertex2f(x + tw, y);
        glVertex2f(x + tw, y + 4.0f * scale);
        glVertex2f(x - tw, y + 4.0f * scale);
    glEnd();

    // Foliage layers (bottom → top)
    glColor3f(0.18f, 0.58f, 0.18f);
    // Bottom triangle
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 3.5f*scale, y + 3.0f*scale);
        glVertex2f(x + 3.5f*scale, y + 3.0f*scale);
        glVertex2f(x,              y + 8.0f*scale);
    glEnd();

    glColor3f(0.15f, 0.52f, 0.15f);
    // Middle triangle
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 2.8f*scale, y + 5.5f*scale);
        glVertex2f(x + 2.8f*scale, y + 5.5f*scale);
        glVertex2f(x,              y + 9.5f*scale);
    glEnd();

    glColor3f(0.12f, 0.46f, 0.12f);
    // Top triangle
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 2.0f*scale, y + 7.5f*scale);
        glVertex2f(x + 2.0f*scale, y + 7.5f*scale);
        glVertex2f(x,              y + 11.0f*scale);
    glEnd();
}

// =========================================================
//  HOUSE  (positioned by bottom-left corner)
//  r,g,b  = wall colour
//  rr,rg,rb = roof colour
// =========================================================
void drawHouse(float x, float y, float w, float h,
               float r,  float g,  float b,
               float rr, float rg, float rb)
{
    // Wall
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x,   y);
        glVertex2f(x+w, y);
        glVertex2f(x+w, y+h);
        glVertex2f(x,   y+h);
    glEnd();

    // Roof (triangle)
    glColor3f(rr, rg, rb);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.5f, y + h);
        glVertex2f(x + w + 0.5f, y + h);
        glVertex2f(x + w * 0.5f, y + h + w * 0.45f);
    glEnd();

    // Door
    glColor3f(0.35f, 0.20f, 0.08f);
    float dw = w * 0.20f, dh = h * 0.30f;
    float dx = x + w * 0.40f;
    glBegin(GL_QUADS);
        glVertex2f(dx,      y);
        glVertex2f(dx + dw, y);
        glVertex2f(dx + dw, y + dh);
        glVertex2f(dx,      y + dh);
    glEnd();

    // Windows (two per house)
    float windowGlow = isNight ? 0.90f : 1.00f;
    glColor3f(windowGlow, windowGlow * 0.90f, isNight ? 0.30f : 0.85f);
    float ww = w * 0.16f, wh = h * 0.22f;
    float wy = y + h * 0.45f;
    // Left window
    float lx = x + w * 0.12f;
    glBegin(GL_QUADS);
        glVertex2f(lx,      wy);
        glVertex2f(lx + ww, wy);
        glVertex2f(lx + ww, wy + wh);
        glVertex2f(lx,      wy + wh);
    glEnd();
    // Right window
    float rx2 = x + w * 0.62f;
    glBegin(GL_QUADS);
        glVertex2f(rx2,      wy);
        glVertex2f(rx2 + ww, wy);
        glVertex2f(rx2 + ww, wy + wh);
        glVertex2f(rx2,      wy + wh);
    glEnd();

    // Night: warm window glow (halo)
    if (isNight) {
        glColor4f(1.0f, 0.85f, 0.30f, 0.10f);
        drawCircle(lx  + ww*0.5f, wy + wh*0.5f, ww * 2.0f, 16);
        drawCircle(rx2 + ww*0.5f, wy + wh*0.5f, ww * 2.0f, 16);
    }

    // Chimney
    glColor3f(0.55f, 0.30f, 0.18f);
    float chx = x + w * 0.70f;
    float chy = y + h;
    float chw = w * 0.08f, chh = h * 0.28f;
    glBegin(GL_QUADS);
        glVertex2f(chx,      chy);
        glVertex2f(chx+chw,  chy);
        glVertex2f(chx+chw,  chy+chh);
        glVertex2f(chx,      chy+chh);
    glEnd();
}

// =========================================================
//  VILLAGE  — place houses and trees in the scene
//  Adjust x, y, w, h and colours to rearrange the village
// =========================================================
void drawVillage()
{
    // House 1 — left side, red-brick
    drawHouse(2, 34, 12, 10,
              0.80f, 0.55f, 0.40f,   // wall: warm brick
              0.55f, 0.22f, 0.12f);  // roof: dark red

    // House 2 — centre-left, cream/beige
    drawHouse(17, 34, 10, 9,
              0.92f, 0.88f, 0.75f,   // wall: cream
              0.65f, 0.40f, 0.20f);  // roof: brown

    // House 3 — centre, blue-grey
    drawHouse(62, 34, 13, 11,
              0.72f, 0.78f, 0.85f,   // wall: pale blue
              0.28f, 0.35f, 0.55f);  // roof: navy

    // House 4 — right, white farmhouse
    drawHouse(80, 34, 14, 10,
              0.95f, 0.95f, 0.90f,   // wall: white
              0.50f, 0.30f, 0.18f);  // roof: terracotta

    // Trees scattered around the village
    drawTree( 1, 34, 0.90f);
    drawTree(14, 34, 0.80f);
    drawTree(29, 34, 1.00f);
    drawTree(35, 34, 0.75f);
    drawTree(55, 34, 0.85f);
    drawTree(76, 34, 0.90f);
    drawTree(97, 34, 0.80f);
}

// =========================================================
//  STREET LAMP  (visible at night)
// =========================================================
void drawLamp(float x, float y)
{
    // Pole
    glColor3f(0.60f, 0.60f, 0.60f);
    glBegin(GL_QUADS);
        glVertex2f(x - 0.2f, y);
        glVertex2f(x + 0.2f, y);
        glVertex2f(x + 0.2f, y + 6.0f);
        glVertex2f(x - 0.2f, y + 6.0f);
    glEnd();

    // Arm
    glBegin(GL_QUADS);
        glVertex2f(x - 0.2f, y + 5.6f);
        glVertex2f(x + 1.5f, y + 5.6f);
        glVertex2f(x + 1.5f, y + 5.9f);
        glVertex2f(x - 0.2f, y + 5.9f);
    glEnd();

    // Bulb
    if (isNight) {
        glColor3f(1.0f, 0.95f, 0.70f);
        drawCircle(x + 1.5f, y + 5.7f, 0.5f, 12);

        // Glow halo
        glColor4f(1.0f, 0.95f, 0.60f, 0.08f);
        drawCircle(x + 1.5f, y + 5.7f, 3.5f, 20);
    } else {
        glColor3f(0.85f, 0.85f, 0.75f);
        drawCircle(x + 1.5f, y + 5.7f, 0.5f, 12);
    }
}

// =========================================================
//  CAR  (same logic as your original, kept compatible)
//  facingRight = true → headlights on right side
// =========================================================
void drawCar(float x, float y, float r, float g, float b, bool facingRight)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    // Headlight cone (night only)
    if (nightLights) {
        glColor4f(1.0f, 1.0f, 0.6f, 0.15f);
        if (facingRight) {
            glBegin(GL_TRIANGLES);
                glVertex2f( 5.5f,  1.0f);
                glVertex2f(22.0f, -1.0f);
                glVertex2f(22.0f,  3.0f);
            glEnd();
        } else {
            glBegin(GL_TRIANGLES);
                glVertex2f(-5.5f,  1.0f);
                glVertex2f(-22.0f,-1.0f);
                glVertex2f(-22.0f, 3.0f);
            glEnd();
        }
    }

    // Car body
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(-5.0f, 0.0f);
        glVertex2f( 5.0f, 0.0f);
        glVertex2f( 5.0f, 2.5f);
        glVertex2f(-5.0f, 2.5f);
    glEnd();

    // Roof
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, 2.5f);
        glVertex2f( 2.5f, 2.5f);
        glVertex2f( 2.0f, 4.2f);
        glVertex2f(-2.0f, 4.2f);
    glEnd();

    // Windows
    glColor3ub(180, 220, 240);
    glBegin(GL_QUADS);
        glVertex2f(-2.3f, 2.7f); glVertex2f(-0.2f, 2.7f);
        glVertex2f(-0.2f, 4.0f); glVertex2f(-2.3f, 4.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f( 0.2f, 2.7f); glVertex2f( 2.3f, 2.7f);
        glVertex2f( 2.3f, 4.0f); glVertex2f( 0.2f, 4.0f);
    glEnd();

    // Wheels
    glColor3ub(30, 30, 30);
    drawCircle(-3.0f, 0, 1.0f, 20);
    drawCircle( 3.0f, 0, 1.0f, 20);

    // Hubcaps
    glColor3ub(180, 180, 180);
    drawCircle(-3.0f, 0, 0.4f, 20);
    drawCircle( 3.0f, 0, 0.4f, 20);

    // Headlights / tail-lights
    if (facingRight) {
        glColor3ub(255, 255, 150); drawCircle( 5.0f, 1.2f, 0.5f, 15); // front
        glColor3ub(255,  80,  80); drawCircle(-5.0f, 1.2f, 0.5f, 15); // rear
    } else {
        glColor3ub(255, 255, 150); drawCircle(-5.0f, 1.2f, 0.5f, 15); // front
        glColor3ub(255,  80,  80); drawCircle( 5.0f, 1.2f, 0.5f, 15); // rear
    }

    glPopMatrix();
}

// =========================================================
//  MAIN DISPLAY
// =========================================================
void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();
    drawStars();

    if (!isNight) drawSun();
    else          drawMoon();

    drawCloud(cloudX1, cloudY1, 0.70f);
    drawCloud(cloudX2, cloudY2, 0.60f);
    drawCloud(cloudX3, cloudY3, 0.55f);

    drawHills();
    drawRiver();
    drawVillage();      // houses + trees
    drawGrass();
    drawRoadAndFootpath();

    // Street lamps
    drawLamp( 8, 10);
    drawLamp(30, 10);
    drawLamp(55, 10);
    drawLamp(78, 10);

    // Cars  (y=12.5 = lower lane, y=22 = upper lane)
    drawCar(carX1,       12.5f, 0.85f, 0.10f, 0.10f, true);   // red
    drawCar(carX3 + 20,  22.0f, 0.10f, 0.40f, 0.90f, true);   // blue
    drawCar(carX2,       25.5f, 0.15f, 0.70f, 0.20f, false);  // green (oncoming)

    glutSwapBuffers();
}

// =========================================================
//  ANIMATION TIMER  (called every ~16 ms ≈ 60 fps)
// =========================================================
void update(int /*value*/)
{
    // Move clouds (wrap around)
    cloudX1 += cloudSpeed1; if (cloudX1 > 115) cloudX1 = -25;
    cloudX2 += cloudSpeed2; if (cloudX2 > 115) cloudX2 = -25;
    cloudX3 += cloudSpeed3; if (cloudX3 > 115) cloudX3 = -25;

    // Move cars (wrap around)
    carX1 += carSpeed1;  if (carX1  >  115) carX1  = -12;
    carX2 -= carSpeed2;  if (carX2  <  -15) carX2  =  115;
    carX3 += carSpeed3;  if (carX3  >  115) carX3  = -35;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// =========================================================
//  KEYBOARD
// =========================================================
void handleKey(unsigned char key, int /*x*/, int /*y*/)
{
    switch (key) {
        case 'n': case 'N':
            isNight = true;
            nightLights = true;
            break;
        case 'd': case 'D':
            isNight = false;
            nightLights = false;
            break;
        case 'q': case 'Q':
            exit(0);
    }
    glutPostRedisplay();
}

// =========================================================
//  INIT
// =========================================================
void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);    // ← world coordinate range
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// =========================================================
//  MAIN
// =========================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Village Scenic Landscape — press N (night) / D (day)");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKey);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
