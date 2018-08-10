/*
 *  androidgl.h
 *  tuxracer
 *
 */


#include "androidglu.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	IPHONE_QUADS = 0x10000,
	IPHONE_POLYGON
};

#define GL_QUADS IPHONE_QUADS
#define GL_POLYGON IPHONE_POLYGON
//FIXME : je suis pas sur du define ci-dessous
#define GL_QUAD_STRIP GL_TRIANGLE_STRIP
#define GL_CLAMP GL_CLAMP_TO_EDGE


void qglBegin(GLenum mode);void glDrawBuffer(GLenum mode);
void qglEnd(void);
void qglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void qglColor4fv(GLfloat *v);
void qglTexCoord2f(GLfloat s, GLfloat t);
void qglTexCoord2fv(GLfloat *v);
void qglTexGenfv( GLenum coord, GLenum pname, const GLfloat * param );
void qglNormal3f(GLfloat x, GLfloat y, GLfloat z);
void qglNormal3fv(GLfloat *v);
void qglVertex3f(GLfloat x, GLfloat y, GLfloat z);
void qglVertex3fv(GLfloat *v);
void qglBindTexture(GLenum target, GLuint texture);
void qglTexGeni(GLenum coord , GLenum pname , GLint param );

void qglEnable(GLenum target);
void qglDisable(GLenum target);
void qglCallList(GLuint list);

void qglVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void qglColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void qglTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void qglNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);

void qglDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void qglEnableClientState (GLenum array);
void qglDisableClientState (GLenum array);
void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void glMultMatrixd(const double * m);
void rotate2D(float angle, float pointOfRotationX, float pointOfRotationY);


enum {
    GL_S,
    GL_T
};

enum {
    GL_OBJECT_PLANE,
    GL_OBJECT_LINEAR,
    GL_SPHERE_MAP
};

enum {
    GL_TEXTURE_GEN_MODE
};

enum {
    GL_TEXTURE_GEN_S=0xdeadbeef,
    GL_TEXTURE_GEN_T=0xdeadbee2
};

#define glBegin qglBegin
#define glEnd qglEnd
#define glColor4f qglColor4f
#define glColor4fv qglColor4fv
#define glTexCoord2f qglTexCoord2f
#define glTexCoord2fv qglTexCoord2fv
#define glVertex3f qglVertex3f
#define glVertex3fv qglVertex3fv
#define glNormal3f qglNormal3f
#define glNormal3fv qglNormal3fv

#define glCallList qglCallList
#define glTexGenfv qglTexGenfv
#define glEnable qglEnable
#define glDisable qglDisable

#define glTexGeni qglTexGeni
#define glVertexPointer qglVertexPointer
#define glTexCoordPointer qglTexCoordPointer
#define glColorPointer qglColorPointer

#define glDrawElements qglDrawElements
#define glEnableClientState qglEnableClientState
#define glDisableClientState qglDisableClientState
#define glNormalPointer qglNormalPointer

#define glVertex2f(x, y)	qglVertex3f(x, y, 0.0)
#define glTexCoord2d(x, y)	glTexCoord2f(x, y)

static inline void glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
    glColor4f(r, g, b, 1.0);
}
	
static inline void glColor4dv(const double *v)
{
	glColor4f((float)v[0], (float)v[1], (float)v[2], (float)v[3]);
}

static inline void glColor3dv(const double *v)	
{
	glColor3f((float)v[0], (float)v[1], (float)v[2]);
}

static inline void glVertex2dv(const double *v)	
{
	glVertex2f((float)v[0], (float)v[1]);
}

static inline void glTexCoord2dv(const double *v)	
{
    GLfloat coord[2] = { (float)v[0], (float)v[1] };
	glTexCoord2fv(coord);
}

//FIXME : Not sure
static inline void glFogi( GLenum pname, GLint param ) 
{
	glFogf(pname,(GLfloat) param);
}
	
static inline void glRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
	glRectf(x1, y1, x2, y2);
}

#ifdef __cplusplus
}
#endif

