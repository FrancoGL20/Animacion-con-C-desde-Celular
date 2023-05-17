// ? (ChatGPT) Fuente: Expresiones regulares en C
// ah carajo!, no traje mis municiones
// Deje la estufa encendida!!!
#if WIN32
#include <windows.h>
#include <GL/glut.h>
#endif
#if __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

// Variables globales
GLfloat girax = 0, giray = 0;
GLboolean malla = GL_FALSE, ejes = GL_TRUE;
// variable que guarde un vector de 3 colores
GLfloat rojo[3] = {0.9, 0.0, 0.0};
GLfloat negro[3] = {0.0, 0.0, 0.0};
GLfloat azul[3] = {0, 0, 1};
GLboolean iniciando = GL_TRUE;
float giro = 0;
float gyroX;
float gyroY;
float gyroZ;
float posy_global = 0;

#define PS_GRAVITY -9.8
#define PS_WATERFALL 0
#define PS_FOUNTAIN 1

typedef struct
{
    float x, y, z;
    float radius;
} PSsphere;

typedef struct
{
    float position[3]; /* current position */
    float previous[3]; /* previous position */
    float velocity[3]; /* velocity (magnitude & direction) */
    float dampening;   /* % of energy lost on collision */
    int alive;         /* is this particle alive? */
} PSparticle;

float pub_tam_cab_ar, pub_dist_cab, pub_tam_cab_ab;
PSparticle *particles = NULL;
PSsphere sphere = {0.0, 5.85, 0.0, 1.2};
PSsphere sphere2;

int num_particles = 5000;
int type = PS_WATERFALL;
int points = 1;
int do_sphere = 0;
int frame_rate = 1;
float frame_time = 0;
float flow = 500;
float slow_down = 1;

float spin_x = 0;
float spin_y = 0;
int point_size = 3;

float timedelta(void)
{
    static long begin = 0;
    static long finish, difference;

#if defined(_WIN32)
#include <sys/timeb.h>
    static struct timeb tb;

    ftime(&tb);
    finish = tb.time * 1000 + tb.millitm;
#else
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
    static struct tms tb;

    finish = times(&tb);
#endif

    difference = finish - begin;
    begin = finish;

    return (float)difference / (float)100; /* CLK_TCK=1000 */
}

void text(int x, int y, char *s)
{
    int lines;
    char *p;

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH),
            0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3ub(0, 0, 0);
    glRasterPos2i(x + 1, y - 1);
    for (p = s, lines = 0; *p; p++)
    {
        if (*p == '\n')
        {
            lines++;
            glRasterPos2i(x + 1, y - 1 - (lines * 18));
        }
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }
    glColor3ub(128, 0, 255);
    glRasterPos2i(x, y);
    for (p = s, lines = 0; *p; p++)
    {
        if (*p == '\n')
        {
            lines++;
            glRasterPos2i(x, y - (lines * 18));
        }
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}

int fequal(float a, float b)
{
    float epsilon = 0.1;
    float f = a - b;

    if (f < epsilon && f > -epsilon)
        return 1;
    else
        return 0;
}

void psTimeStep(PSparticle *p, float dt)
{
    if (p->alive == 0)
        return;

    p->velocity[0] += 0;
    p->velocity[1] += PS_GRAVITY * dt;
    p->velocity[2] += 0;

    p->previous[0] = p->position[0];
    p->previous[1] = p->position[1];
    p->previous[2] = p->position[2];

    p->position[0] += p->velocity[0] * dt;
    p->position[1] += p->velocity[1] * dt;
    p->position[2] += p->velocity[2] * dt;
}

void psNewParticle(PSparticle *p, float dt, float x, float y)
{
    if (type == PS_WATERFALL)
    {
        p->velocity[0] = 1 * (drand48() - 0.5);
        p->velocity[1] = 0;
        p->velocity[2] = 0.5 * (drand48() - 0.0);
        p->position[0] = x;
        p->position[1] = y;
        p->position[2] = 0;
        p->previous[0] = p->position[0];
        p->previous[1] = p->position[1];
        p->previous[2] = p->position[2];
        p->dampening = 0.45 * drand48();
        p->alive = 1;
    }
    else if (type == PS_FOUNTAIN)
    {
        p->velocity[0] = 2 * (drand48() - 0.5);
        p->velocity[1] = 6;
        p->velocity[2] = 2 * (drand48() - 0.5);
        p->position[0] = 0;
        p->position[1] = 0;
        p->position[2] = 0;
        p->previous[0] = p->position[0];
        p->previous[1] = p->position[1];
        p->previous[2] = p->position[2];
        p->dampening = 0.35 * drand48();
        p->alive = 1;
    }

    psTimeStep(p, 2 * dt * drand48());
}

void psBounce(PSparticle *p, float dt)
{
    float s;

    if (p->alive == 0)
        return;

    /* since we know it is the ground plane, we only need to
       calculate s for a single dimension. */
    s = -p->previous[1] / p->velocity[1];

    p->position[0] = (p->previous[0] + p->velocity[0] * s +
                      p->velocity[0] * (dt - s) * p->dampening);
    p->position[1] = -p->velocity[1] * (dt - s) * p->dampening; /* reflect */
    p->position[2] = (p->previous[2] + p->velocity[2] * s +
                      p->velocity[2] * (dt - s) * p->dampening);

    /* damp the reflected velocity (since the particle hit something,
       it lost some energy) */
    p->velocity[0] *= p->dampening;
    p->velocity[1] *= -p->dampening; /* reflect */
    p->velocity[2] *= p->dampening;
}

void psCollide(PSparticle *p)
{
    float vx = p->position[0] - sphere.x;
    float vy = p->position[1] - sphere.y;
    float vz = p->position[2] - sphere.z;
    float distance;

    if (p->alive == 0)
        return;

    distance = sqrt(vx * vx + vy * vy + vz * vz);

    if (distance < sphere.radius)
    {
#if 0
	vx /= distance;  vy /= distance;  vz /= distance;
	d = 2*(-vx*p->velocity[0] + -vy*p->velocity[1] + -vz*p->velocity[2]);
	p->velocity[0] += vx*d*2;
	p->velocity[1] += vy*d*2;
	p->velocity[2] += vz*d*2;
	d = sqrt(p->velocity[0]*p->velocity[0] + 
		 p->velocity[1]*p->velocity[1] +
		 p->velocity[2]*p->velocity[2]);
	p->velocity[0] /= d;
	p->velocity[1] /= d;
	p->velocity[2] /= d;
#else
        p->position[0] = sphere.x + (vx / distance) * sphere.radius;
        p->position[1] = sphere.y + (vy / distance) * sphere.radius;
        p->position[2] = sphere.z + (vz / distance) * sphere.radius;
        p->previous[0] = p->position[0];
        p->previous[1] = p->position[1];
        p->previous[2] = p->position[2];
        p->velocity[0] = vx / distance;
        p->velocity[1] = vy / distance;
        p->velocity[2] = vz / distance;
#endif
    }
    if (distance < sphere2.radius)
    {
#if 0
	vx /= distance;  vy /= distance;  vz /= distance;
	d = 2*(-vx*p->velocity[0] + -vy*p->velocity[1] + -vz*p->velocity[2]);
	p->velocity[0] += vx*d*2;
	p->velocity[1] += vy*d*2;
	p->velocity[2] += vz*d*2;
	d = sqrt(p->velocity[0]*p->velocity[0] + 
		 p->velocity[1]*p->velocity[1] +
		 p->velocity[2]*p->velocity[2]);
	p->velocity[0] /= d;
	p->velocity[1] /= d;
	p->velocity[2] /= d;
#else
        p->position[0] = sphere2.x + (vx / distance) * sphere2.radius;
        p->position[1] = sphere2.y + (vy / distance) * sphere2.radius;
        p->position[2] = sphere2.z + (vz / distance) * sphere2.radius;
        p->previous[0] = p->position[0];
        p->previous[1] = p->position[1];
        p->previous[2] = p->position[2];
        p->velocity[0] = vx / distance;
        p->velocity[1] = vy / distance;
        p->velocity[2] = vz / distance;
#endif
    }
}

void reshape(int width, int height)
{
    float black[] = {0, 0, 0, 0};

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 1, 3, 0, 1, 0, 0, 1, 0);
    glFogfv(GL_FOG_COLOR, black);
    glFogf(GL_FOG_START, 2.5);
    glFogf(GL_FOG_END, 4);
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glPointSize(point_size);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);

    timedelta();
}

// Rotacion XY y Zoom
void mover(void)
{
    glRotatef(giray, 0.0, 1.0, 0.0);
    glRotatef(girax, 1.0, 0.0, 0.0);
}

// Crear malla
void creaMalla(int long_eje)
{
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    int i;
    for (i = -long_eje; i <= long_eje; i++)
    {
        glVertex3f(i, 0, -long_eje);
        glVertex3f(i, 0, long_eje);
        glVertex3f(-long_eje, 0, i);
        glVertex3f(long_eje, 0, i);
    }
    glEnd();
}

// Crear ejes
void creaEjes(void)
{
    glColor3fv(negro);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(11.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 11.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 11.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(11.0, 0.0, 0.0); /* eje x */
    glVertex3f(10.5, 0.0, -.50);
    glVertex3f(10.5, 0.0, .50);
    glColor3f(0.25, 1, 0.25); /* eje y */
    glVertex3f(0.0, 11.0, 0.0);
    glVertex3f(-.50, 10.5, 0.0);
    glVertex3f(.50, 10.5, 0.0);
    glColor3f(0.25, 0.25, 1.0); /* eje z */
    glVertex3f(0.0, 0.0, 11.0);
    glVertex3f(-.50, 0.0, 10.5);
    glVertex3f(.50, 0.0, 10.5);
    glEnd();
}

// Cambiar rango
float cambia_r(float x)
{
    return (float)((giro * 2) * x) / (180 * 2);
}

// Dibujar texto en pantalla
void dibujaTexto(void *tipo, char *s, float x, float y)
{
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(s); i++)
        glutBitmapCharacter(tipo, s[i]);
}

void dibuja(void)
{
    static int i;
    static float c;
    static char s[32];
    static int frames = 0;

    // limpiar frame buffer y Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    char mensaje[30] = "Deje la estufa encendida!!!";

    glPushMatrix();
    glRotatef(giray, 0.0, 1.0, 0.0);
    glRotatef(girax, 1.0, 0.0, 0.0);
    if (malla)
        creaMalla(10);
    if (ejes)
        creaEjes();

    if (points)
    {
        glBegin(GL_POINTS);

        for (i = 0; i < num_particles; i++)
        {
            if (particles[i].alive == 0)
                continue;
            c = particles[i].position[1] / 2.1 * 255;
            glColor3ub((GLubyte)c, (GLubyte)(128 + c * 0.5), 255);
            glVertex3f(particles[i].position[0], particles[i].position[1], particles[i].position[2]);
            glVertex3f((-1) * particles[i].position[0], particles[i].position[1], particles[i].position[2]);
        }
        glEnd();
    }
    else
    {
        glBegin(GL_LINES);
        for (i = 0; i < num_particles; i++)
        {
            if (particles[i].alive == 0)
                continue;
            c = particles[i].previous[1] / 2.1 * 255;
            glColor3ub((GLubyte)c, (GLubyte)(128 + c * 0.5), 255);
            glVertex3fv(particles[i].previous);
            c = particles[i].position[1] / 2.1 * 255;
            glColor3ub((GLubyte)c, (GLubyte)(128 + c * 0.5), 255);
            glVertex3fv(particles[i].position);
        }
        glEnd();
    }

    // Cuerpo
    glPushMatrix();
    posy_global = 2.1 - cambia_r(2.1);
    glTranslatef(0.0, 6.7 - cambia_r(3.4), 0.0); // 4.7 - 1.3 = 3.4

    // Texto
    glPushMatrix();
    dibujaTexto(GLUT_BITMAP_HELVETICA_18, mensaje, 1.6, 5.3);
    glPopMatrix();

    { // Torso
        glPushMatrix();

        // Cuello
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0.0, 4.0, 0.0);
        glScalef(0.4, 0.8, 0.4);
        glRotatef(-90, 1.0, 0.0, 0.0);
        gluCylinder(gluNewQuadric(), 1.5, 1.0, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Parte superior2 (torso)
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0.0, 4.0, 0.0);
        glScalef(1.85, 0.4, 1.1);
        glutSolidSphere(1.0, 20, 20);
        glPopMatrix();

        // Parte superior1 (torso)
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0.0, 1.5, 0.0);
        glScalef(1.3, 2.5, 0.8);
        glRotatef(-90, 1.0, 0.0, 0.0);
        gluCylinder(gluNewQuadric(), 1.0, 1.4, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Parte central (torso)
        glPushMatrix();
        glColor3fv(negro);
        glTranslatef(0.0, 1.2, 0.0);
        glScalef(1.0, 0.7, 0.6);
        glutSolidSphere(1.4, 20, 20);
        glPopMatrix();

        // Parte baja1 (torso)
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0.0, 0.3, 0.0);
        glScalef(1.0, 1.0, 0.6);
        glRotatef(-90, 1.0, 0.0, 0.0);
        gluCylinder(gluNewQuadric(), 1.3, 1.4, 0.7, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Parte baja2 (torso)
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0.0, 0.5, 0.0);
        glScalef(1.0, 0.5, 0.6);
        glutSolidSphere(1.4, 20, 20);
        glPopMatrix();
        glPopMatrix();
    }

    float esp_hombros = 1.75;
    float alt_hombros = 3.7;
    float largo_brazo = 2.5;
    float largo_antebrazo = 2.0;
    float tam_hombro = 0.7;
    float tam_muneca = 0.4;
    float giro_x_brazo = cambia_r(10);
    float giro_z_brazo = cambia_r(1);
    float giro_x_codo = cambia_r(40);
    float giro_z_codo = cambia_r(1);
    { // Brazo derecho
        glPushMatrix();
        // Posición inicial (brazo derecho)
        glTranslatef(-esp_hombros, alt_hombros, 0.0);
        // Giro en x (brazo derecho)
        glRotatef(-giro_x_brazo, 1, 0, 0);
        // Giro en y (brazo derecho)
        glRotatef(0, 0, 1, 0);
        // Giro en z (brazo derecho)
        glRotatef(-15 + giro_z_brazo, 0, 0, 1);

        // Hombro (brazo derecho)
        glPushMatrix();
        glColor3fv(negro);
        glutSolidSphere(tam_hombro, 20, 20);
        glPopMatrix();

        // brazo (brazo derecho)
        glPushMatrix();
        glScalef(1.0, largo_brazo, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), tam_hombro * 0.9, 0.5, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Codo con jerarquía (brazo derecho)
        glPushMatrix();
        glTranslatef(0.0, -largo_brazo, 0.0);
        // Giro en x
        glRotatef(-giro_x_codo, 1, 0, 0);
        // Giro en z
        glRotatef(giro_z_codo, 0, 0, 1);
        glRotatef(15, 0, 0, 1);

        // codo (brazo derecho)
        glPushMatrix();
        glColor3fv(rojo);
        glutSolidSphere(0.6, 20, 20);
        glPopMatrix();

        // antebrazo (brazo derecho)
        glPushMatrix();
        glColor3fv(rojo);
        glScalef(1.0, largo_antebrazo, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), 0.5, tam_muneca, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // muñeca (brazo derecho)
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0, -largo_antebrazo, 0);
        glutSolidSphere(tam_muneca, 20, 20);
        glPopMatrix();

        // mano (brazo derecho)

        glPopMatrix();
        glPopMatrix();
    }

    { // Brazo izquierdo
        glPushMatrix();
        // Posición inicial (brazo izquierdo)
        glTranslatef(esp_hombros, alt_hombros, 0.0);
        // Giro en x (brazo izquierdo)
        glRotatef(-giro_x_brazo, 1, 0, 0);
        // Giro en y (brazo izquierdo)
        glRotatef(0, 0, 1, 0);
        // Giro en z (brazo izquierdo)
        glRotatef(15 - giro_z_brazo, 0, 0, 1);

        // Hombro (brazo izquierdo)
        glPushMatrix();
        glColor3fv(negro);
        glutSolidSphere(tam_hombro, 20, 20);
        glPopMatrix();

        // brazo (brazo izquierdo)
        glPushMatrix();
        glScalef(1.0, largo_brazo, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), tam_hombro * 0.9, 0.5, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Codo con jerarquía (brazo izquierdo)
        glPushMatrix();
        glTranslatef(0.0, -largo_brazo, 0.0);
        // Giro en x
        glRotatef(-giro_x_codo, 1, 0, 0);
        // Giro en z
        glRotatef(-giro_z_codo, 0, 0, 1);
        glRotatef(-15, 0, 0, 1);

        // codo (brazo izquierdo)
        glPushMatrix();
        glColor3fv(rojo);
        glutSolidSphere(0.6, 20, 20);
        glPopMatrix();

        // antebrazo (brazo izquierdo)
        glPushMatrix();
        glColor3fv(rojo);
        glScalef(1.0, largo_antebrazo, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), 0.5, tam_muneca, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // muñeca (brazo izquierdo)
        glPushMatrix();
        glColor3fv(rojo);
        glTranslatef(0, -largo_antebrazo, 0);
        glutSolidSphere(tam_muneca, 20, 20);
        glPopMatrix();

        // mano (brazo izquierdo)

        glPopMatrix();
        glPopMatrix();
    }

    { // Cabeza
        glPushMatrix();

        glTranslatef(sphere.x, sphere.y, sphere.z);

        // Cabeza
        glPushMatrix();
        float tam_cab_ar = 1.2;
        float tam_cab_ab = tam_cab_ar * 0.9;
        float tam_cab_barba = 0.8;
        float dist_cab = 0.9;
        float dist_cab_ab = 0.5;
        sphere.radius = pub_tam_cab_ar;
        glColor3fv(rojo);
        glPushMatrix();
        glScalef(1.0, 0.5, 1.0);
        glutSolidSphere(sphere.radius, 20, 20);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.0, -1, 0.0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), tam_cab_ab, tam_cab_ar, dist_cab, 20, 20);
        glPopMatrix();
        glPushMatrix();
        pub_tam_cab_ar = tam_cab_ar;
        pub_tam_cab_ab = tam_cab_ab;
        pub_dist_cab = dist_cab;
        sphere2.x = 0.0;
        sphere2.y = -pub_dist_cab;
        sphere2.z = 0.0;
        sphere2.radius = pub_tam_cab_ab;
        glTranslatef(sphere2.x, sphere2.y, sphere2.z);
        glScalef(1.0, 0.5, 1.0);
        glutSolidSphere(sphere2.radius, 20, 20);
        glPopMatrix();
        glPopMatrix();

        float x = 5;               // Angulo de rotación en x
        float y = 26;              // Angulo de rotación en y
        float z = -15;             // Angulo de rotación en z
        float distancia = 1.348;   // Distancia a los ojos
        float dist_entre_ojos = 0; // Distancia entre los ojos

        // Ojo izquierdo
        glPushMatrix();
        glColor3fv(negro);
        glTranslatef(dist_entre_ojos, -0.9, distancia);
        float tam = 0.05;
        glScalef(tam + 0.01, tam, tam);
        { // Ojo negro
            glBegin(GL_POLYGON);
            glVertex3f(8, 1, -1.5 * 3);
            glVertex3f(10, 3, -1.5 * 3);
            glVertex3f(11, 4, -1.6 * 3);
            glVertex3f(12, 6, -2 * 3);
            glVertex3f(13, 8, -2.2 * 3);

            glVertex3f(14, 10, -2.3 * 3);
            glVertex3f(14, 12, -2.3 * 3);
            glVertex3f(14, 14, -2.3 * 3);
            glVertex3f(14, 16, -2.3 * 3);
            glVertex3f(14, 17, -2.3 * 3);
            glVertex3f(13, 20, -2.2 * 3);

            glVertex3f(11, 24, -1.8 * 3);
            glVertex3f(10, 24, -1.6 * 3);
            glVertex3f(9, 24, -1.4 * 3);

            glVertex3f(7, 23, -1.2 * 3);
            glVertex3f(5, 22, -1 * 3);
            glVertex3f(4, 21, -0.8 * 3);
            glVertex3f(3, 19, -0.8 * 3);
            glVertex3f(2, 15, -0.7 * 3);

            glVertex3f(1, 12, -0.3 * 3);
            glVertex3f(1, 10, -0.3 * 3);
            glVertex3f(2, 8, -0.3 * 3);
            glVertex3f(3, 6, -0.5 * 3);

            glVertex3f(5, 3, -0.7 * 3);
            glVertex3f(6, 2, -0.9 * 3);
            glVertex3f(7, 1, -0.9 * 3);
            glEnd();
        }
        { // Ojo negro base
            glBegin(GL_QUAD_STRIP);
            glVertex3f(8, 1, -1.5 * 3);
            glVertex3f(8, 1, -1.5 * 3 - 10);
            glVertex3f(10, 3, -1.5 * 3);
            glVertex3f(10, 3, -1.5 * 3 - 10);
            glVertex3f(11, 4, -1.6 * 3);
            glVertex3f(11, 4, -1.6 * 3 - 10);
            glVertex3f(12, 6, -2 * 3);
            glVertex3f(12, 6, -2 * 3 - 10);
            glVertex3f(13, 8, -2.2 * 3);
            glVertex3f(13, 8, -2.2 * 3 - 10);

            glVertex3f(14, 10, -2.3 * 3);
            glVertex3f(14, 10, -2.3 * 3 - 10);
            glVertex3f(14, 12, -2.3 * 3);
            glVertex3f(14, 12, -2.3 * 3 - 10);
            glVertex3f(14, 14, -2.3 * 3);
            glVertex3f(14, 14, -2.3 * 3 - 10);
            glVertex3f(14, 16, -2.3 * 3);
            glVertex3f(14, 16, -2.3 * 3 - 10);
            glVertex3f(14, 17, -2.3 * 3);
            glVertex3f(14, 17, -2.3 * 3 - 10);
            glVertex3f(13, 20, -2.2 * 3);
            glVertex3f(13, 20, -2.2 * 3 - 10);

            glVertex3f(11, 24, -1.8 * 3);
            glVertex3f(11, 24, -1.8 * 3 - 10);
            glVertex3f(10, 24, -1.6 * 3);
            glVertex3f(10, 24, -1.6 * 3 - 10);
            glVertex3f(9, 24, -1.4 * 3);
            glVertex3f(9, 24, -1.4 * 3 - 10);

            glVertex3f(7, 23, -1.2 * 3);
            glVertex3f(7, 23, -1.2 * 3 - 10);
            glVertex3f(5, 22, -1 * 3);
            glVertex3f(5, 22, -1 * 3 - 10);
            glVertex3f(4, 21, -0.8 * 3);
            glVertex3f(4, 21, -0.8 * 3 - 10);
            glVertex3f(3, 19, -0.8 * 3);
            glVertex3f(3, 19, -0.8 * 3 - 10);
            glVertex3f(2, 15, -0.7 * 3);
            glVertex3f(2, 15, -0.7 * 3 - 10);

            glVertex3f(1, 12, -0.3 * 3);
            glVertex3f(1, 12, -0.3 * 3 - 10);
            glVertex3f(1, 10, -0.3 * 3);
            glVertex3f(1, 10, -0.3 * 3 - 10);
            glVertex3f(2, 8, -0.3 * 3);
            glVertex3f(2, 8, -0.3 * 3 - 10);
            glVertex3f(3, 6, -0.5 * 3);
            glVertex3f(3, 6, -0.5 * 3 - 10);

            glVertex3f(5, 3, -0.7 * 3);
            glVertex3f(5, 3, -0.7 * 3 - 10);
            glVertex3f(6, 2, -0.9 * 3);
            glVertex3f(6, 2, -0.9 * 3 - 10);
            glVertex3f(7, 1, -0.9 * 3);
            glVertex3f(7, 1, -0.9 * 3 - 10);
            glEnd();
        }
        glPopMatrix();

        // Ojo izquierdo
        glPushMatrix();
        glTranslatef(-dist_entre_ojos, -0.9, distancia);
        glScalef(tam + 0.01, tam, tam);
        { // Ojo negro
            glBegin(GL_POLYGON);
            glVertex3f(-8, 1, -1.5 * 3);
            glVertex3f(-10, 3, -1.5 * 3);
            glVertex3f(-11, 4, -1.6 * 3);
            glVertex3f(-12, 6, -2 * 3);
            glVertex3f(-13, 8, -2.2 * 3);

            glVertex3f(-14, 10, -2.3 * 3);
            glVertex3f(-14, 12, -2.3 * 3);
            glVertex3f(-14, 14, -2.3 * 3);
            glVertex3f(-14, 16, -2.3 * 3);
            glVertex3f(-14, 17, -2.3 * 3);
            glVertex3f(-13, 20, -2.2 * 3);

            glVertex3f(-11, 24, -1.8 * 3);
            glVertex3f(-10, 24, -1.6 * 3);
            glVertex3f(-9, 24, -1.4 * 3);

            glVertex3f(-7, 23, -1.2 * 3);
            glVertex3f(-5, 22, -1 * 3);
            glVertex3f(-4, 21, -0.8 * 3);
            glVertex3f(-3, 19, -0.8 * 3);
            glVertex3f(-2, 15, -0.7 * 3);

            glVertex3f(-1, 12, -0.3 * 3);
            glVertex3f(-1, 10, -0.3 * 3);
            glVertex3f(-2, 8, -0.3 * 3);
            glVertex3f(-3, 6, -0.5 * 3);

            glVertex3f(-5, 3, -0.7 * 3);
            glVertex3f(-6, 2, -0.9 * 3);
            glVertex3f(-7, 1, -0.9 * 3);
            glEnd();
        }
        { // Ojo negro base
            glBegin(GL_QUAD_STRIP);
            glVertex3f(-8, 1, -1.5 * 3);
            glVertex3f(-8, 1, -1.5 * 3 - 10);
            glVertex3f(-10, 3, -1.5 * 3);
            glVertex3f(-10, 3, -1.5 * 3 - 10);
            glVertex3f(-11, 4, -1.6 * 3);
            glVertex3f(-11, 4, -1.6 * 3 - 10);
            glVertex3f(-12, 6, -2 * 3);
            glVertex3f(-12, 6, -2 * 3 - 10);
            glVertex3f(-13, 8, -2.2 * 3);
            glVertex3f(-13, 8, -2.2 * 3 - 10);

            glVertex3f(-14, 10, -2.3 * 3);
            glVertex3f(-14, 10, -2.3 * 3 - 10);
            glVertex3f(-14, 12, -2.3 * 3);
            glVertex3f(-14, 12, -2.3 * 3 - 10);
            glVertex3f(-14, 14, -2.3 * 3);
            glVertex3f(-14, 14, -2.3 * 3 - 10);
            glVertex3f(-14, 16, -2.3 * 3);
            glVertex3f(-14, 16, -2.3 * 3 - 10);
            glVertex3f(-14, 17, -2.3 * 3);
            glVertex3f(-14, 17, -2.3 * 3 - 10);
            glVertex3f(-13, 20, -2.2 * 3);
            glVertex3f(-13, 20, -2.2 * 3 - 10);

            glVertex3f(-11, 24, -1.8 * 3);
            glVertex3f(-11, 24, -1.8 * 3 - 10);
            glVertex3f(-10, 24, -1.6 * 3);
            glVertex3f(-10, 24, -1.6 * 3 - 10);
            glVertex3f(-9, 24, -1.4 * 3);
            glVertex3f(-9, 24, -1.4 * 3 - 10);

            glVertex3f(-7, 23, -1.2 * 3);
            glVertex3f(-7, 23, -1.2 * 3 - 10);
            glVertex3f(-5, 22, -1 * 3);
            glVertex3f(-5, 22, -1 * 3 - 10);
            glVertex3f(-4, 21, -0.8 * 3);
            glVertex3f(-4, 21, -0.8 * 3 - 10);
            glVertex3f(-3, 19, -0.8 * 3);
            glVertex3f(-3, 19, -0.8 * 3 - 10);
            glVertex3f(-2, 15, -0.7 * 3);
            glVertex3f(-2, 15, -0.7 * 3 - 10);

            glVertex3f(-1, 12, -0.3 * 3);
            glVertex3f(-1, 12, -0.3 * 3 - 10);
            glVertex3f(-1, 10, -0.3 * 3);
            glVertex3f(-1, 10, -0.3 * 3 - 10);
            glVertex3f(-2, 8, -0.3 * 3);
            glVertex3f(-2, 8, -0.3 * 3 - 10);
            glVertex3f(-3, 6, -0.5 * 3);
            glVertex3f(-3, 6, -0.5 * 3 - 10);

            glVertex3f(-5, 3, -0.7 * 3);
            glVertex3f(-5, 3, -0.7 * 3 - 10);
            glVertex3f(-6, 2, -0.9 * 3);
            glVertex3f(-6, 2, -0.9 * 3 - 10);
            glVertex3f(-7, 1, -0.9 * 3);
            glVertex3f(-7, 1, -0.9 * 3 - 10);
            glEnd();
        }
        glPopMatrix();

        // Pupila derecha
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(-0.5, -0.35, distancia - 0.2);
        glRotatef(-x, 1.0, 0.0, 0.0);
        glRotatef(-y, 0.0, 1.0, 0.0);
        glRotatef(1, 0.0, 0.0, 1.0);
        glScalef(0.4, 0.15 + cambia_r(0.15), 0.2);
        glutSolidSphere(0.5, 20, 20);
        glPopMatrix();

        // Pupila izquierda
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(0.5, -0.35, distancia - 0.2);
        glRotatef(-x, 1.0, 0.0, 0.0);
        glRotatef(-y, 0.0, 1.0, 0.0);
        glRotatef(1, 0.0, 0.0, 1.0);
        glScalef(0.4, 0.15 + cambia_r(0.15), 0.2);
        glutSolidSphere(0.5, 20, 20);
        glPopMatrix();
        glPopMatrix();
    }

    float largo_pierna = 2.0;
    float largo_tibia = 2.3;
    float ancho_pierna_arriba = 0.7;
    float ancho_pierna_abajo = 0.6;
    float ancho_union_pie = 0.5;
    float giro_z_pierna = cambia_r(1);
    float giro_x_pierna = cambia_r(50);
    float giro_x_rodilla = cambia_r(95);
    float giro_x_tobillo = cambia_r(40);
    { // pierna derecha
        glPushMatrix();
        // Posición inicial (pierna derecha)
        glTranslatef(-0.7, 0.1, 0.0);
        // Giro en x (pierna derecha)
        glRotatef(-giro_x_pierna, 1, 0, 0);
        // Giro en z (pierna derecha)
        glRotatef(0, 0, 0, 1);
        // // Giro en y (pierna derecha)
        // glRotatef(0,0,1,0);

        // Union (pierna derecha)
        glPushMatrix();
        glColor3fv(rojo);
        glutSolidSphere(ancho_pierna_arriba, 20, 20);
        glPopMatrix();

        // pierna (pierna derecha)
        glPushMatrix();
        glColor3fv(rojo);
        glScalef(1.0, largo_pierna, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), ancho_pierna_arriba, ancho_pierna_abajo, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Rodilla con jerarquía (pierna derecha)
        glPushMatrix();
        glTranslatef(0, -largo_pierna - 0.1, 0);
        // Giro en x (pierna derecha)
        glRotatef(giro_x_rodilla, 1, 0, 0);

        // Rodilla  (pierna derecha)
        glPushMatrix();
        glColor3fv(rojo);
        glutSolidSphere(ancho_pierna_abajo + 0.05, 20, 20);
        glPopMatrix();

        // Tibia (pierna derecha)
        glPushMatrix();
        glScalef(1.0, largo_tibia, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), ancho_pierna_abajo, ancho_union_pie, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Tobillo con jerarquía (pierna derecha)
        glPushMatrix();
        glTranslatef(0, -largo_tibia - 0.1, 0);
        // Giro en x (pierna derecha)
        glRotatef(-giro_x_tobillo, 1, 0, 0);

        // Tobillo (pierna derecha)
        glPushMatrix();
        glColor3fv(negro);
        glScalef(1.0, 0.7, 1.0);
        glutSolidSphere(ancho_union_pie + 0.1, 20, 20);
        glPopMatrix();

        // Pie (pierna derecha)
        glPushMatrix();
        glColor3fv(negro);
        // glTranslatef(0,-0.1,0);
        glScalef(1.0, 0.7, 1.0);
        gluCylinder(gluNewQuadric(), ancho_union_pie, ancho_union_pie, 1.0, 20, 20); // base, top, height, slices, stacks
        glRotatef(-90, 1, 0, 0);
        glTranslatef(0, -1.0, 0);
        glutSolidSphere(ancho_union_pie, 20, 20);
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
    }

    { // pierna izquierda
        glPushMatrix();
        // Posición inicial (pierna izquierda)
        glTranslatef(0.7, 0.1, 0.0);
        // Giro en x
        glRotatef(-giro_x_pierna, 1, 0, 0);
        // Giro en z (pierna izquierda)
        glRotatef(0, 0, 0, 1);
        // // Giro en y (pierna izquierda)
        // glRotatef(0,0,1,0);

        // Union (pierna izquierda)
        glPushMatrix();
        glColor3fv(rojo);
        glutSolidSphere(ancho_pierna_arriba, 20, 20);
        glPopMatrix();

        // pierna (pierna izquierda)
        glPushMatrix();
        glColor3fv(rojo);
        glScalef(1.0, largo_pierna, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), ancho_pierna_arriba, ancho_pierna_abajo, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Rodilla con jerarquía (pierna izquierda)
        glPushMatrix();
        glTranslatef(0, -largo_pierna - 0.1, 0);
        // Giro en x (pierna izquierda)
        glRotatef(giro_x_rodilla, 1, 0, 0);

        // Rodilla  (pierna izquierda)
        glPushMatrix();
        glColor3fv(rojo);
        glutSolidSphere(ancho_pierna_abajo + 0.05, 20, 20);
        glPopMatrix();

        // Tibia (pierna izquierda)
        glPushMatrix();
        glScalef(1.0, largo_tibia, 1.0);
        glRotatef(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), ancho_pierna_abajo, ancho_union_pie, 1.0, 20, 20); // base, top, height, slices, stacks
        glPopMatrix();

        // Tobillo con jerarquía (pierna izquierda)
        glPushMatrix();
        glTranslatef(0, -largo_tibia - 0.1, 0);
        // Giro en x (pierna izquierda)
        glRotatef(-giro_x_tobillo, 1, 0, 0);

        // Tobillo (pierna izquierda)
        glPushMatrix();
        glColor3fv(negro);
        glScalef(1.0, 0.7, 1.0);
        glutSolidSphere(ancho_union_pie + 0.1, 20, 20);
        glPopMatrix();

        // Pie (pierna izquierda)
        glPushMatrix();
        glColor3fv(negro);
        // glTranslatef(0,-0.1,0);
        glScalef(1.0, 0.7, 1.0);
        gluCylinder(gluNewQuadric(), ancho_union_pie, ancho_union_pie, 1.0, 20, 20); // base, top, height, slices, stacks
        glRotatef(-90, 1, 0, 0);
        glTranslatef(0, -1.0, 0);
        glutSolidSphere(ancho_union_pie, 20, 20);
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

// Función de animación
void anima(int v)
{

    if (iniciando)
    {
        if (giro < 180)
        {
            giro += 5;
        }
        else
        {
            iniciando = GL_FALSE;
        }
    }
    else
    {
        if (giro > 0)
        {
            giro -= 5;
        }
        else
        {
            iniciando = GL_TRUE;
        }
    }
    glutTimerFunc(70, anima, 0);
    glutPostRedisplay();
}

void idleFunc(void)
{
    static int i;
    static int living = 0; /* index to end of live particles */
    static float dt;

    dt = timedelta();
    frame_time += dt;

#if 1
    /* slow the simulation if we can't keep the frame rate up around
       10 fps */
    if (dt > 0.1)
    {
        slow_down = 0.75;
    }
    else if (dt < 0.1)
    {
        slow_down = 1;
    }
#endif

    dt *= slow_down;

    /* resurrect a few particles */
    for (i = 0; i < flow * dt; i++)
    {
        psNewParticle(&particles[living], dt, -0.8, posy_global);
        living++;
        if (living >= num_particles)
            living = 0;
    }

    for (i = 0; i < num_particles; i++)
    {
        psTimeStep(&particles[i], dt);

        /* collision with sphere? */
        if (do_sphere)
        {
            psCollide(&particles[i]);
        }

        /* collision with ground? */
        if (particles[i].position[1] <= 0)
        {
            psBounce(&particles[i], dt);
        }

        /* dead particle? */
        if (particles[i].position[1] < 0.1 &&
            fequal(particles[i].velocity[1], 0))
        {
            particles[i].alive = 0;
        }
    }

    glutPostRedisplay();
}

void visible(int state)
{
    if (state == GLUT_VISIBLE)
    {
        timedelta();
        glutIdleFunc(idleFunc);
    }
    else
    {
        glutIdleFunc(NULL);
    }
}

// Funciones con Teclas normales
void teclado(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
    case 'm': // activa/desactiva la malla
        malla = !malla;
        break;
    case 'e': // activa/desactiva los ejes
        ejes = !ejes;
        break;
    case 'w':
        type = PS_WATERFALL;
        break;
    case 'f':
        type = PS_FOUNTAIN;
        break;
    case 's':
        do_sphere = !do_sphere;
        break;
    case 'l':
        points = !points;
        break;
    case 'P':
        point_size++;
        glPointSize(point_size);
        break;
    case 'p':
        point_size--;
        if (point_size < 1)
            point_size = 1;
        glPointSize(point_size);
        break;
    case '+':
        flow += 100;
        if (flow > num_particles)
            flow = num_particles;
        printf("%g particles/second\n", flow);
        break;
    case '-':
        flow -= 100;
        if (flow < 0)
            flow = 0;
        printf("%g particles/second\n", flow);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// Funciones con Teclas Especiales
void rotar(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT: // rotacion en el eje Y
        giray -= 15;
        break;
    case GLUT_KEY_RIGHT: // rotacion en el eje Y
        giray += 15;
        break;
    case GLUT_KEY_UP: // rotacion en el eje X
        girax -= 15;
        break;
    case GLUT_KEY_DOWN: // rotacion en el eje X
        girax += 15;
        break;
    }
    glutPostRedisplay();
}

void ajusta(int ancho, int alto)
{
    glClearColor(1.0, 1.0, 1.0, 0.0); // color de fondo

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15, 15, -10 + 5, 10 + 5, -15, 15);

    // Habilitar iluminacion y color de los materiales
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_MODELVIEW); // matriz de modelado
    glLoadIdentity();           // matriz identidad
    glEnable(GL_DEPTH_TEST);    // activa el Z-buffer

    timedelta();
}

int main(int argc, char **argv)
{
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    float x = 800;
    glutInitWindowSize(x * (3 / 2), x); // Relacion 3:2
    glutInit(&argc, argv);
    glutCreateWindow("Investigación");
    glutDisplayFunc(dibuja);
    glutReshapeFunc(ajusta);
    glutKeyboardFunc(teclado);
    glutTimerFunc(1, anima, 0);
    glutSpecialFunc(rotar);
    if (argc > 1)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            fprintf(stderr, "%s [particles] [flow] [speed%%]\n", argv[0]);
            exit(0);
        }
        sscanf(argv[1], "%d", &num_particles);
        if (argc > 2)
            sscanf(argv[2], "%f", &flow);
        if (argc > 3)
            sscanf(argv[3], "%f", &slow_down);
    }
    particles = (PSparticle *)malloc(sizeof(PSparticle) * num_particles);
    glutVisibilityFunc(visible);
    glutMainLoop(); // bucle de eventos de GLUT
    return 0;
}
