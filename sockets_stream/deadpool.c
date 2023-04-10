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
#include <regex.h>

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

void parseString(char *text)
{
    char *pattern = ":(-?[0-9]+[.][0-9]+).*:([-]?[0-9]+[.][0-9]+).*:(-?[0-9]+[.][0-9]+)";

    //  {data:{"gyroX":0.005093295592814684,"gyroY":-0.0009986853692680597,"gyroZ":0.0009321063989773393}}
    // char *text = "{data:{\"gyroX\":-0.005093295592814684,\"gyroY\":-0.0009986853692680597,\"gyroZ\":-0.0009321063989773393}}";

    // Compile la expresión regular
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret)
    {
        printf("No se pudo compilar la expresión regular.\n");
        exit(1);
    }

    // Busque la primera coincidencia
    regmatch_t matches[4];
    ret = regexec(&regex, text, 4, matches, 0);
    if (ret == 0)
    {
        // Obtenga las coincidencias y guárdelas en variables independientes
        char n1[100];
        char n2[100];
        char n3[100];
        strncpy(n1, text + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        strncpy(n2, text + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
        strncpy(n3, text + matches[3].rm_so, matches[3].rm_eo - matches[3].rm_so);
        n1[matches[1].rm_eo - matches[1].rm_so] = '\0';
        n2[matches[2].rm_eo - matches[2].rm_so] = '\0';
        n3[matches[3].rm_eo - matches[3].rm_so] = '\0';

        // parseo de string a float
        gyroX = atof(n1);
        gyroY = atof(n2);
        gyroZ = atof(n3);

        // Imprima las coincidencias
        // printf("\ngyroX: %f\n", gyroX);
        // printf("gyroY: %f\n", gyroY);
        // printf("gyroZ: %f\n\n", gyroZ);
    }

    // Libere la memoria
    regfree(&regex);
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

    // Cuerpo
    glPushMatrix();
    glTranslatef(0.0, 4.7 - cambia_r(1.3), 0.0);

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
    float giro_x_brazo = cambia_r(100);
    float giro_z_brazo = cambia_r(15);
    float giro_x_codo = cambia_r(137);
    float giro_z_codo = cambia_r(20);
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
        glTranslatef(0.0, 5.85, 0.0);

        // Cabeza
        glPushMatrix();
        float tam_cab_ar = 1.2;
        float tam_cab_ab = tam_cab_ar * 0.9;
        float tam_cab_barba = 0.8;
        float dist_cab = 0.9;
        float dist_cab_ab = 0.5;
        glColor3fv(rojo);
        glPushMatrix();
        glScalef(1.0, 0.5, 1.0);
        glutSolidSphere(tam_cab_ar, 20, 20);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.0, -1, 0.0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), tam_cab_ab, tam_cab_ar, dist_cab, 20, 20);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.0, -dist_cab, 0.0);
        glScalef(1.0, 0.5, 1.0);
        glutSolidSphere(tam_cab_ab, 20, 20);
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
    float giro_z_pierna = cambia_r(10);
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
        glRotatef(-10 + giro_z_pierna, 0, 0, 1);
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
        glRotatef(10 - giro_z_pierna, 0, 0, 1);
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
    // leer el contenido de un archivo y guardarlo en un string
    FILE *fp;
    float xg,yg,zg;
    fp = fopen("data.json", "r");

    char texto[200];
    fscanf(fp, "%[^\n]+", texto);
    if (fp != NULL) {
        parseString(texto);
    }
    fclose(fp);

    // girax = 0, giray = 0;

    // float gyroX; 
    // float gyroY; // controlar el giro en y
    // float gyroZ; // controla el giro en x
    
    gyroY=-gyroY; // Arreglar posibles confusiones //! Acelerometro
    
    // Cambiar el rango desde -1->1 a -180->180
    giray = gyroX * 90; //! Acelerometro
    girax = gyroY * 90; //! Acelerometro

    
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
    default:
        break;
    }
    glutPostRedisplay();
}

// Funciones con Teclas Especiales
void rotar(int key, int x, int y)
{
    // switch (key)
    // {
    // case GLUT_KEY_LEFT: // rotacion en el eje Y
    //     giray -= 15;
    //     break;
    // case GLUT_KEY_RIGHT: // rotacion en el eje Y
    //     giray += 15;
    //     break;
    // case GLUT_KEY_UP: // rotacion en el eje X
    //     girax -= 15;
    //     break;
    // case GLUT_KEY_DOWN: // rotacion en el eje X
    //     girax += 15;
    //     break;
    // }
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
    glutMainLoop(); // bucle de eventos de GLUT
    return 0;
}
