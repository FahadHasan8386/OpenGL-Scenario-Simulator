// ============================================================
//  MERGED SCENIC LANDSCAPE  —  OpenGL / GLUT
//
//  Press  1  →  Scene 1 (Road + River + Hills)
//  Press  2  →  Scene 2 (Village + Houses + Trees)
//  Press  N  →  Night mode
//  Press  D  →  Day  mode
//  Press  Q  →  Quit
// ============================================================

#ifdef _WIN32
  #include <windows.h>
  #include <GL/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>

// ============================================================
//  GLOBAL STATE
// ============================================================

int  currentScene = 1;   // 1 = Scene 1,  2 = Scene 2
bool isNight      = false;
bool nightLights  = false;

// ---- Moving cloud positions (shared by both scenes) --------
float cloudX1 = 10.0f,  cloudY1 = 88.0f;
float cloudX2 = 45.0f,  cloudY2 = 83.0f;
float cloudX3 = 75.0f,  cloudY3 = 91.0f;

// ---- Car positions (shared by both scenes) -----------------
float carX1 = -20.0f;   // red  car  → moves right, lower lane
float carX2 = 115.0f;   // green car → moves left,  oncoming
float carX3 = -35.0f;   // blue car  → moves right, upper lane

// ---- Animation speeds  (edit these freely) -----------------
const float CLOUD_SPEED1 = 0.07f;
const float CLOUD_SPEED2 = 0.05f;
const float CLOUD_SPEED3 = 0.06f;

const float CAR_SPEED1   = 0.25f;
const float CAR_SPEED2   = 0.20f;
const float CAR_SPEED3   = 0.18f;


// ============================================================
//  UTILITY:  draw a filled circle
//  cx, cy = centre    radius = size    seg = smoothness
// ============================================================
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


// ============================================================
//  REUSABLE DRAWING FUNCTIONS
//  These are called by BOTH scenes where appropriate
// ============================================================

// ------------------------------------------------------------
//  SKY  – full-screen gradient quad
// ------------------------------------------------------------
void drawSky()
{
    float tr, tg, tb;   // top colour
    float br, bg, bb;   // bottom colour

    if (!isNight) {
        tr = 0.39f; tg = 0.71f; tb = 0.94f;   // day top  (bright blue)
        br = 0.71f; bg = 0.88f; bb = 0.98f;   // day bot  (pale horizon)
    } else {
        tr = 0.02f; tg = 0.02f; tb = 0.12f;   // night top (deep navy)
        br = 0.04f; bg = 0.04f; bb = 0.20f;   // night bot (slightly lighter)
    }

    glBegin(GL_QUADS);
        glColor3f(br, bg, bb); glVertex2f(  0,   0);
        glColor3f(br, bg, bb); glVertex2f(100,   0);
        glColor3f(tr, tg, tb); glVertex2f(100, 100);
        glColor3f(tr, tg, tb); glVertex2f(  0, 100);
    glEnd();
}

// ------------------------------------------------------------
//  STARS  (night only)
// ------------------------------------------------------------
void drawStars()
{
    if (!isNight) return;

    static const float stars[][2] = {
        { 5,95},{12,98},{22,93},{30,97},{38,94},{50,99},
        {60,96},{70,95},{78,98},{88,93},{95,97},{15,87},
        {28,90},{42,88},{55,85},{65,89},{80,87},{92,91},
        { 8,82},{35,84},{72,82},{18,75},{48,78},{83,76},
        { 3,70},{62,72},{90,68}
    };
    const int N = (int)(sizeof(stars) / sizeof(stars[0]));

    glColor3f(1.0f, 1.0f, 0.9f);
    glPointSize(1.8f);
    glBegin(GL_POINTS);
    for (int i = 0; i < N; ++i)
        glVertex2f(stars[i][0], stars[i][1]);
    glEnd();
    glPointSize(1.0f);
}

// ------------------------------------------------------------
//  SUN
// ------------------------------------------------------------
void drawSun()
{
    // soft glow halo
    glColor4f(1.0f, 0.95f, 0.60f, 0.20f);
    drawCircle(85, 88, 5.5f, 40);
    // solid disc
    glColor3f(1.0f, 0.90f, 0.20f);
    drawCircle(85, 88, 3.8f, 40);
}

// ------------------------------------------------------------
//  MOON  (crescent via overlapping circles)
// ------------------------------------------------------------
void drawMoon()
{
    // full disc
    glColor3f(0.95f, 0.95f, 0.80f);
    drawCircle(80, 88, 4.0f, 40);
    // bite-out using sky colour → crescent
    glColor3f(0.02f, 0.02f, 0.12f);
    drawCircle(82.5f, 89.5f, 3.2f, 40);
}

// ------------------------------------------------------------
//  CLOUD  —  three overlapping circles
//  cx, cy = left-centre anchor    scale = overall size
// ------------------------------------------------------------
void drawCloud(float cx, float cy, float scale)
{
    if (!isNight)
        glColor3f(1.00f, 1.00f, 1.00f);   // white in day
    else
        glColor3f(0.50f, 0.52f, 0.62f);   // dark grey at night

    drawCircle(cx,              cy,              3.0f * scale, 24);
    drawCircle(cx + 3.5f*scale, cy + 0.5f*scale, 2.5f * scale, 24);
    drawCircle(cx - 3.0f*scale, cy + 0.3f*scale, 2.2f * scale, 24);
    drawCircle(cx + 1.0f*scale, cy + 2.0f*scale, 2.0f * scale, 24);
}

// ------------------------------------------------------------
//  ROAD AND FOOTPATH
//  Road occupies y = 8 → 30   (world coords)
// ------------------------------------------------------------
void drawRoad()
{
    // ---- top footpath / kerb -----------------------------------
    glColor3f(0.75f, 0.73f, 0.68f);
    glBegin(GL_QUADS);
        glVertex2f(  0, 30); glVertex2f(100, 30);
        glVertex2f(100, 33); glVertex2f(  0, 33);
    glEnd();

    // ---- bottom footpath / kerb --------------------------------
    glBegin(GL_QUADS);
        glVertex2f(  0,  5); glVertex2f(100,  5);
        glVertex2f(100,  8); glVertex2f(  0,  8);
    glEnd();

    // Tile grooves on footpaths
    glColor3f(0.60f, 0.58f, 0.54f);
    glLineWidth(1.0f);
    for (int i = 0; i < 100; i += 8) {
        glBegin(GL_LINES);
            glVertex2f(i, 30); glVertex2f(i, 33);
            glVertex2f(i,  5); glVertex2f(i,  8);
        glEnd();
    }

    // ---- asphalt road surface ----------------------------------
    glColor3f(0.18f, 0.18f, 0.18f);
    glBegin(GL_QUADS);
        glVertex2f(  0,  8); glVertex2f(100,  8);
        glVertex2f(100, 30); glVertex2f(  0, 30);
    glEnd();

    // Yellow edge lines
    glColor3f(1.0f, 0.78f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(0,  9.5f); glVertex2f(100,  9.5f);
        glVertex2f(0, 28.5f); glVertex2f(100, 28.5f);
    glEnd();

    // White centre dashed line
    glColor3f(1.0f, 1.0f, 1.0f);
    for (float x = 0; x < 100; x += 10) {
        glBegin(GL_LINES);
            glVertex2f(x,     19); glVertex2f(x + 6, 19);
        glEnd();
    }

    // Secondary lane dashes
    glColor3f(0.75f, 0.75f, 0.75f);
    for (float x = 2; x < 100; x += 10) {
        glBegin(GL_LINES);
            glVertex2f(x,     24.5f); glVertex2f(x + 5, 24.5f);
            glVertex2f(x,     13.5f); glVertex2f(x + 5, 13.5f);
        glEnd();
    }
}

// ------------------------------------------------------------
//  STREET LAMP  (pole + arm + bulb)
//  x, y = base of pole
// ------------------------------------------------------------
void drawLamp(float x, float y)
{
    // Pole
    glColor3f(0.55f, 0.55f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(x - 0.25f, y);
        glVertex2f(x + 0.25f, y);
        glVertex2f(x + 0.25f, y + 6.0f);
        glVertex2f(x - 0.25f, y + 6.0f);
    glEnd();

    // Arm
    glBegin(GL_QUADS);
        glVertex2f(x - 0.25f, y + 5.6f);
        glVertex2f(x + 1.8f,  y + 5.6f);
        glVertex2f(x + 1.8f,  y + 5.9f);
        glVertex2f(x - 0.25f, y + 5.9f);
    glEnd();

    // Bulb
    if (isNight) {
        glColor3f(1.0f, 0.95f, 0.70f);
        drawCircle(x + 1.8f, y + 5.7f, 0.55f, 12);
        // Soft glow halo
        glColor4f(1.0f, 0.95f, 0.55f, 0.09f);
        drawCircle(x + 1.8f, y + 5.7f, 4.0f, 20);
    } else {
        glColor3f(0.85f, 0.85f, 0.75f);
        drawCircle(x + 1.8f, y + 5.7f, 0.55f, 12);
    }
}

// ------------------------------------------------------------
//  CAR
//  x, y       = centre-bottom of car body
//  r, g, b    = body colour
//  facingRight= true → headlights on right end
// ------------------------------------------------------------
void drawCar(float x, float y, float r, float g, float b, bool facingRight)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    // Headlight cone  (night only)
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
        glVertex2f(-5.0f, 0.0f); glVertex2f( 5.0f, 0.0f);
        glVertex2f( 5.0f, 2.5f); glVertex2f(-5.0f, 2.5f);
    glEnd();

    // Roof (slightly darker)
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, 2.5f); glVertex2f( 2.5f, 2.5f);
        glVertex2f( 2.0f, 4.2f); glVertex2f(-2.0f, 4.2f);
    glEnd();

    // Windows
    glColor3ub(180, 220, 240);
    glBegin(GL_QUADS);   // left
        glVertex2f(-2.3f, 2.7f); glVertex2f(-0.2f, 2.7f);
        glVertex2f(-0.2f, 4.0f); glVertex2f(-2.3f, 4.0f);
    glEnd();
    glBegin(GL_QUADS);   // right
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

// ------------------------------------------------------------
//  TREE  — trunk + three layered triangles
//  x, y  = base centre    scale = overall size
// ------------------------------------------------------------
void drawTree(float x, float y, float scale)
{
    // Trunk
    float tw = 0.8f * scale;
    glColor3f(0.42f, 0.26f, 0.10f);
    glBegin(GL_QUADS);
        glVertex2f(x - tw, y);
        glVertex2f(x + tw, y);
        glVertex2f(x + tw, y + 4.0f * scale);
        glVertex2f(x - tw, y + 4.0f * scale);
    glEnd();

    // Bottom foliage (widest / lightest)
    glColor3f(0.18f, 0.58f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 3.5f*scale, y + 3.0f*scale);
        glVertex2f(x + 3.5f*scale, y + 3.0f*scale);
        glVertex2f(x,              y + 8.0f*scale);
    glEnd();

    // Middle foliage
    glColor3f(0.15f, 0.52f, 0.15f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 2.8f*scale, y + 5.5f*scale);
        glVertex2f(x + 2.8f*scale, y + 5.5f*scale);
        glVertex2f(x,              y + 9.5f*scale);
    glEnd();

    // Top foliage (darkest)
    glColor3f(0.12f, 0.46f, 0.12f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 2.0f*scale, y + 7.5f*scale);
        glVertex2f(x + 2.0f*scale, y + 7.5f*scale);
        glVertex2f(x,              y + 11.0f*scale);
    glEnd();
}

// ------------------------------------------------------------
//  HOUSE
//  x, y        = bottom-left corner
//  w, h        = width, wall height
//  r/g/b       = wall colour
//  rr/rg/rb    = roof colour
// ------------------------------------------------------------
void drawHouse(float x, float y, float w, float h,
               float r,  float g,  float b,
               float rr, float rg, float rb)
{
    // Wall
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x,   y);   glVertex2f(x+w, y);
        glVertex2f(x+w, y+h); glVertex2f(x,   y+h);
    glEnd();

    // Roof triangle
    glColor3f(rr, rg, rb);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.5f,       y + h);
        glVertex2f(x + w + 0.5f,   y + h);
        glVertex2f(x + w * 0.5f,   y + h + w * 0.45f);
    glEnd();

    // Door
    glColor3f(0.35f, 0.20f, 0.08f);
    float dx = x + w * 0.40f,  dw = w * 0.20f,  dh = h * 0.30f;
    glBegin(GL_QUADS);
        glVertex2f(dx,      y);       glVertex2f(dx + dw, y);
        glVertex2f(dx + dw, y + dh);  glVertex2f(dx,      y + dh);
    glEnd();

    // Windows
    float wx = x + w * 0.12f,  wx2 = x + w * 0.62f;
    float wy = y + h * 0.45f,  ww  = w * 0.16f,    wh = h * 0.22f;

    // Lit warm colour at night, bright by day
    float wc = isNight ? 0.95f : 1.0f;
    glColor3f(wc, wc * 0.90f, isNight ? 0.30f : 0.85f);

    glBegin(GL_QUADS);   // left window
        glVertex2f(wx,      wy);       glVertex2f(wx + ww, wy);
        glVertex2f(wx + ww, wy + wh);  glVertex2f(wx,      wy + wh);
    glEnd();
    glBegin(GL_QUADS);   // right window
        glVertex2f(wx2,      wy);      glVertex2f(wx2 + ww, wy);
        glVertex2f(wx2 + ww, wy + wh); glVertex2f(wx2,      wy + wh);
    glEnd();

    // Night: warm glow halos around windows
    if (isNight) {
        glColor4f(1.0f, 0.85f, 0.30f, 0.10f);
        drawCircle(wx  + ww*0.5f, wy + wh*0.5f, ww * 2.2f, 16);
        drawCircle(wx2 + ww*0.5f, wy + wh*0.5f, ww * 2.2f, 16);
    }

    // Chimney
    glColor3f(0.55f, 0.30f, 0.18f);
    float chx = x + w * 0.70f,  chw = w * 0.08f,  chh = h * 0.28f;
    glBegin(GL_QUADS);
        glVertex2f(chx,      y + h);       glVertex2f(chx + chw, y + h);
        glVertex2f(chx + chw, y + h + chh); glVertex2f(chx,       y + h + chh);
    glEnd();
}

// ------------------------------------------------------------
//  RIVER  —  used in both scenes (slightly different palette)
// ------------------------------------------------------------
void drawRiver()
{
    // River body
    if (!isNight)
        glColor3f(0.27f, 0.62f, 0.82f);
    else
        glColor3f(0.08f, 0.18f, 0.40f);

    glBegin(GL_QUADS);
        glVertex2f( 0, 44); glVertex2f(100, 44);
        glVertex2f(100, 50); glVertex2f(  0, 50);
    glEnd();

    // Sandy banks
    glColor3f(0.70f, 0.60f, 0.40f);
    glBegin(GL_QUADS);
        glVertex2f(0, 43.2f); glVertex2f(100, 43.2f);
        glVertex2f(100, 44.0f); glVertex2f(0, 44.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(0, 50.0f); glVertex2f(100, 50.0f);
        glVertex2f(100, 50.8f); glVertex2f(0, 50.8f);
    glEnd();

    // Shimmer lines on water
    if (!isNight)
        glColor4f(1.0f, 1.0f, 1.0f, 0.20f);
    else
        glColor4f(1.0f, 1.0f, 1.0f, 0.07f);
    glLineWidth(1.0f);
    for (float xi = 5; xi < 100; xi += 12) {
        glBegin(GL_LINES);
            glVertex2f(xi,     47.5f); glVertex2f(xi + 5, 47.5f);
            glVertex2f(xi + 2, 46.0f); glVertex2f(xi + 6, 46.0f);
        glEnd();
    }
}

// ------------------------------------------------------------
//  RIVER  —  Scene 2 style  (diagonal, partial width)
//  Matches original Project 2 river shape exactly
// ------------------------------------------------------------
void drawRiver2()
{
    // River body (angled quad, flows diagonally across mid-scene)
    if (!isNight)
        glColor3f(0.30f, 0.55f, 0.85f);
    else
        glColor3f(0.10f, 0.20f, 0.45f);

    glBegin(GL_QUADS);
        glVertex2f(28, 37);
        glVertex2f(62, 37);
        glVertex2f(60, 44);
        glVertex2f(30, 44);
    glEnd();

    // Shimmer lines on water
    glColor4f(1.0f, 1.0f, 1.0f, isNight ? 0.08f : 0.18f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(32, 39); glVertex2f(42, 39);
        glVertex2f(45, 41); glVertex2f(56, 41);
        glVertex2f(35, 43); glVertex2f(48, 43);
    glEnd();
}

// ------------------------------------------------------------
//  GRASS / GROUND band  (sits below river / above road)
// ------------------------------------------------------------
void drawGrass(float y0, float y1)
{
    // Colour slightly darker at night
    if (!isNight)
        glColor3f(0.24f, 0.60f, 0.18f);
    else
        glColor3f(0.10f, 0.28f, 0.08f);

    glBegin(GL_QUADS);
        glVertex2f(  0, y0); glVertex2f(100, y0);
        glVertex2f(100, y1); glVertex2f(  0, y1);
    glEnd();
}


// ============================================================
//  SCENE 1  —  Road, River, Hills  (original project 1 look)
// ============================================================

// Back mountains + foreground hills specific to Scene 1
void drawScene1Hills()
{
    // ---- Far mountains (bluish) --------------------------------
    glColor3ub(154, 176, 204);
    glBegin(GL_POLYGON);
        glVertex2f(-3,63); glVertex2f(12,45); glVertex2f(26,52); glVertex2f(32,63);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(18,63); glVertex2f(34,41); glVertex2f(50,50); glVertex2f(59,63);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(47,63); glVertex2f(63,43); glVertex2f(79,52); glVertex2f(88,63);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(76,63); glVertex2f(91,44); glVertex2f(103,52); glVertex2f(106,63);
    glEnd();

    // ---- Mid mountains (darker) --------------------------------
    glColor3ub(110, 140, 170);
    glBegin(GL_POLYGON);
        glVertex2f(-1,65); glVertex2f(12,51); glVertex2f(28,58); glVertex2f(41,65);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(23,65); glVertex2f(42,46); glVertex2f(60,56); glVertex2f(73,65);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(59,65); glVertex2f(75,49); glVertex2f(91,58); glVertex2f(103,65);
    glEnd();

    // Snow caps on mid peaks
    glColor3ub(240, 245, 255);
    glBegin(GL_POLYGON);
        glVertex2f(38,51); glVertex2f(42,46); glVertex2f(46,51);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(71,54); glVertex2f(75,49); glVertex2f(79,54);
    glEnd();

    // ---- Treeline strip at mountain base -----------------------
    glColor3ub(45, 75, 45);
    glBegin(GL_QUADS);
        glVertex2f(0,65); glVertex2f(100,65);
        glVertex2f(100,67); glVertex2f(0,67);
    glEnd();

    // ---- Front hills (green, gradient shading) -----------------
    glBegin(GL_POLYGON);
        glColor3ub(58,170,58);  glVertex2f(-3,67);  glVertex2f(10,55);
        glColor3ub(30,122,30);  glVertex2f(20,60);  glVertex2f(29,67);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(58,170,58);  glVertex2f(12,67);  glVertex2f(24,53);
        glColor3ub(30,122,30);  glVertex2f(38,59);  glVertex2f(55,67);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(58,170,58);  glVertex2f(38,67);  glVertex2f(50,50);
        glColor3ub(30,122,30);  glVertex2f(64,58);  glVertex2f(80,67);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(58,170,58);  glVertex2f(67,67);  glVertex2f(79,52);
        glColor3ub(30,122,30);  glVertex2f(92,59);  glVertex2f(103,67);
    glEnd();

    // Green fill below hills
    glColor3ub(34, 110, 34);
    glBegin(GL_QUADS);
        glVertex2f(0,50);  glVertex2f(100,50);
        glVertex2f(100,67); glVertex2f(0,67);
    glEnd();
}

// Main display function for Scene 1
void displayScene1()
{
    drawSky();
    drawStars();

    if (!isNight) drawSun(); else drawMoon();

    // Clouds
    drawCloud(cloudX1, cloudY1, 0.70f);
    drawCloud(cloudX2, cloudY2, 0.60f);
    drawCloud(cloudX3, cloudY3, 0.55f);

    // Scenery (back to front)
    drawScene1Hills();
    drawRiver();
    drawGrass(33.0f, 43.2f);   // grass strip between river and road
    drawRoad();

    // Lamp posts along top footpath
    drawLamp(15, 30);
    drawLamp(40, 30);
    drawLamp(65, 30);
    drawLamp(90, 30);

    // Cars on road
    drawCar(carX1,      12.5f, 0.85f, 0.10f, 0.10f, true);    // red
    drawCar(carX3 + 20, 22.0f, 0.10f, 0.40f, 0.90f, true);    // blue
    drawCar(carX2,      25.5f, 0.15f, 0.70f, 0.20f, false);   // green (oncoming)
}


// ============================================================
//  SCENE 2  —  Village with Houses and Trees
// ============================================================

// Rounded hill shapes using circles (Scene 2 style)
void drawScene2Hills()
{
    // Back hills (lighter)
    glColor3f(0.45f, 0.68f, 0.38f);
    drawCircle(20, 58, 18, 50);
    drawCircle(55, 55, 22, 50);
    drawCircle(85, 57, 16, 50);

    // Fill ground below back hills
    if (!isNight)
        glColor3f(0.55f, 0.80f, 0.35f);
    else
        glColor3f(0.25f, 0.45f, 0.18f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f(100, 0);
        glVertex2f(100, 55); glVertex2f(0, 55);
    glEnd();

    // Front hills (darker / closer)
    glColor3f(0.30f, 0.58f, 0.26f);
    drawCircle(10, 55, 14, 50);
    drawCircle(38, 52, 18, 50);
    drawCircle(70, 53, 20, 50);
    drawCircle(96, 54, 12, 50);
}

// Houses + trees arranged as a small village
void drawVillage()
{
    // --- House 1: left side, red-brick ---
    drawHouse(2,  34, 12, 10,
              0.80f, 0.55f, 0.40f,    // wall: warm brick
              0.55f, 0.22f, 0.12f);   // roof: dark red

    // --- House 2: centre-left, cream ---
    drawHouse(17, 34, 10,  9,
              0.92f, 0.88f, 0.75f,    // wall: cream
              0.65f, 0.40f, 0.20f);   // roof: brown

    // --- House 3: centre, pale blue ---
    drawHouse(62, 34, 13, 11,
              0.72f, 0.78f, 0.85f,    // wall: pale blue-grey
              0.28f, 0.35f, 0.55f);   // roof: navy

    // --- House 4: right, white farmhouse ---
    drawHouse(80, 34, 14, 10,
              0.95f, 0.95f, 0.90f,    // wall: near-white
              0.50f, 0.30f, 0.18f);   // roof: terracotta

    // --- Trees scattered around the village ---
    drawTree( 1, 34, 0.90f);
    drawTree(14, 34, 0.80f);
    drawTree(29, 34, 1.00f);
    drawTree(35, 34, 0.75f);
    drawTree(55, 34, 0.85f);
    drawTree(76, 34, 0.90f);
    drawTree(97, 34, 0.80f);
}

// Main display function for Scene 2
void displayScene2()
{
    drawSky();
    drawStars();

    if (!isNight) drawSun(); else drawMoon();

    // Clouds
    drawCloud(cloudX1, cloudY1, 0.70f);
    drawCloud(cloudX2, cloudY2, 0.60f);
    drawCloud(cloudX3, cloudY3, 0.55f);

    // Scenery (back to front)
    drawScene2Hills();
    drawRiver2();          // Scene 2 diagonal river
    drawVillage();         // houses and trees sit on the hills
    drawGrass(32.0f, 37.0f);   // grass strip above road
    drawRoad();

    // Street lamps on TOP footpath of road (y=30)
    drawLamp( 8, 30);
    drawLamp(30, 30);
    drawLamp(55, 30);
    drawLamp(78, 30);

    // Cars on road
    drawCar(carX1,      12.5f, 0.85f, 0.10f, 0.10f, true);    // red
    drawCar(carX3 + 20, 22.0f, 0.10f, 0.40f, 0.90f, true);    // blue
    drawCar(carX2,      25.5f, 0.15f, 0.70f, 0.20f, false);   // green (oncoming)
}


// ============================================================
//  MAIN DISPLAY  —  picks which scene to render
// ============================================================
void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (currentScene == 1)
        displayScene1();
    else
        displayScene2();

    glutSwapBuffers();
}


// ============================================================
//  ANIMATION TIMER  (~60 fps)
// ============================================================
void update(int /*value*/)
{
    // Clouds wrap around the screen
    cloudX1 += CLOUD_SPEED1; if (cloudX1 > 115) cloudX1 = -25;
    cloudX2 += CLOUD_SPEED2; if (cloudX2 > 115) cloudX2 = -25;
    cloudX3 += CLOUD_SPEED3; if (cloudX3 > 115) cloudX3 = -25;

    // Cars wrap around
    carX1 += CAR_SPEED1;  if (carX1  >  115) carX1  = -12;
    carX2 -= CAR_SPEED2;  if (carX2  <  -15) carX2  =  115;
    carX3 += CAR_SPEED3;  if (carX3  >  115) carX3  = -35;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


// ============================================================
//  KEYBOARD INPUT
// ============================================================
void handleKey(unsigned char key, int /*x*/, int /*y*/)
{
    switch (key)
    {
        case '1':                       // Switch to Scene 1
            currentScene = 1;
            break;

        case '2':                       // Switch to Scene 2
            currentScene = 2;
            break;

        case 'n': case 'N':             // Night mode
            isNight     = true;
            nightLights = true;
            break;

        case 'd': case 'D':             // Day mode
            isNight     = false;
            nightLights = false;
            break;

        case 'q': case 'Q':             // Quit
            exit(0);
    }
    glutPostRedisplay();
}


// ============================================================
//  INIT  —  projection + blending setup
// ============================================================
void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);   // world space: 0-100 in both axes
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// ============================================================
//  MAIN
// ============================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Scenic Landscape  |  1=Scene1  2=Scene2  N=Night  D=Day  Q=Quit");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKey);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
