#include "tuxracer.h"
#include <assert.h>
#include <stdbool.h>

#undef glBegin
#undef glEnd
#undef glColor4f
#undef glColor4fv
#undef glTexCoord2f
#undef glTexCoord2fv
#undef glVertex3f
#undef glVertex3fv
#undef glNormal3f
#undef glNormal3fv
#undef glCallList

#undef glBindTexture
#undef glTexGeni

#undef glEnable
#undef glDisable

#undef glVertexPointer
#undef glTexCoordPointer
#undef glColorPointer
#undef glNormalPointer

#undef glDrawElements
#undef glEnableClientState
#undef glDisableClientState

//#define DEBUG

#ifdef TR_DEBUG_MODE
# define TGLLog printf
# define TGLErr printf
#else
# define TGLLog if(0) printf
# define TGLErr if(0) printf
#endif


/*
 * Quake3 -- iPhone Port
 *
 * Seth Kingsley, January 2008.
 */

#define MAX_ARRAY_SIZE		1024


#define UNIMPL() TGLErr("ERR: %s is unimplemented\n", __func__ );

static GLenum _GLimp_beginmode;
static float _GLimp_texcoords[MAX_ARRAY_SIZE][2];
static float _GLimp_vertexes[MAX_ARRAY_SIZE][3];
static float _GLimp_colors[MAX_ARRAY_SIZE][4];
static float _GLimp_normal[MAX_ARRAY_SIZE][3];
static GLuint _GLimp_numInputVerts, _GLimp_numOutputVerts;
static bool _GLimp_texcoordbuffer;
static bool _GLimp_colorbuffer;
static bool _GLimp_normalbuffer;

static bool qglTexGenTObjectPlane = false;
static GLfloat qglTexGenTObjectPlane_param[4];
static bool qglTexGenSObjectPlane = false;
static GLfloat qglTexGenSObjectPlane_param[4];


unsigned int QGLBeginStarted = 0;

#define QGL_CHECK_GL_ERRORS

#ifdef QGL_CHECK_GL_ERRORS
void
QGLErrorBreak(void)
{
}

void
QGLCheckError(const char *message)
{
    GLenum error;
    static unsigned int errorCount = 0;

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
        if (errorCount == 100)
            printf("100 GL errors printed ... disabling further error reporting.\n");
        else if (errorCount < 100)
		{
            if (errorCount == 0)
                fprintf(stdout, "BREAK ON QGLErrorBreak to stop at the GL errors\n");
            fprintf(stdout, "OpenGL Error(%s): 0x%04x\n", message, (int)error);
            QGLErrorBreak();
        }
        ++errorCount;
    }
}

#endif // QGL_CHECK_GL_ERRORS


void qglTexGenfv( GLenum coord, GLenum pname, const GLfloat * param )
{
    assert(pname == GL_OBJECT_PLANE);

    if(coord == GL_S)
    {
        qglTexGenSObjectPlane = true;
        memcpy(qglTexGenSObjectPlane_param, param, sizeof(GLfloat) * 4);
    }
    else if(coord == GL_T)
    {
        qglTexGenTObjectPlane = true;
        memcpy(qglTexGenTObjectPlane_param, param, sizeof(GLfloat) * 4);
    }
    else
        assert(0);
}

void qglTexGeni(GLenum coord , GLenum pname , GLint param )
{
    assert(coord == GL_S || coord == GL_T); // We only support that one
    assert(GL_TEXTURE_GEN_MODE == pname); // We only support that one

    if(param == GL_OBJECT_LINEAR)
    {

    }
    else if(param == GL_SPHERE_MAP)
    {
        if(coord == GL_S)
            qglTexGenSObjectPlane = false;
        if(coord == GL_T)
            qglTexGenTObjectPlane = false;
    }
    else
        assert(0); // Not supported
}

static bool qglEnableTextureGenS = false;
static bool qglEnableTextureGenT = false;

void qglEnable(GLenum target)
{
    if(target == GL_TEXTURE_GEN_S)
    {
        qglEnableTextureGenS = true;
        return;
    }
    if(target == GL_TEXTURE_GEN_T)
    {
        qglEnableTextureGenT = true;
        return;
    }
    glEnable(target);
}

void qglDisable(GLenum target)
{
    if(target == GL_TEXTURE_GEN_S)
    {
        qglEnableTextureGenS = false;
        return;
    }
    if(target == GL_TEXTURE_GEN_T)
    {
        qglEnableTextureGenT = false;
        return;
    }

    glDisable(target);
}

void
qglBegin(GLenum mode)
{
	assert(!QGLBeginStarted);
	QGLBeginStarted = true;
	_GLimp_beginmode = mode;
	_GLimp_numInputVerts = _GLimp_numOutputVerts = 0;
	_GLimp_texcoordbuffer = false;
	_GLimp_colorbuffer = false;
	_GLimp_normalbuffer = false;
}

static bool qglEnableClientStateTextureCoordArray = false;
static bool qglEnableClientStateColorArray = false;
static bool qglEnableClientStateVertexArray = false;
static bool qglEnableClientStateNormalArray = false;

void qglEnableClientState (GLenum array)
{
    if(array == GL_TEXTURE_COORD_ARRAY)
        qglEnableClientStateTextureCoordArray = true;
    else if(array == GL_COLOR_ARRAY)
        qglEnableClientStateTextureCoordArray = true;
    else if(array == GL_VERTEX_ARRAY)
        qglEnableClientStateVertexArray = true;
    else if(array == GL_NORMAL_ARRAY)
        qglEnableClientStateNormalArray = true;
    glEnableClientState(array);
}

void qglDisableClientState (GLenum array)
{
    if(array == GL_TEXTURE_COORD_ARRAY)
        qglEnableClientStateTextureCoordArray = false;
    else if(array == GL_COLOR_ARRAY)
        qglEnableClientStateTextureCoordArray = false;
    else if(array == GL_VERTEX_ARRAY)
        qglEnableClientStateVertexArray = false;
    else if(array == GL_NORMAL_ARRAY)
        qglEnableClientStateNormalArray = false;
    glDisableClientState(array);

}

static void restoreClientState ()
{
    if(qglEnableClientStateTextureCoordArray)
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    else
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if(qglEnableClientStateColorArray)
        glEnableClientState(GL_COLOR_ARRAY);
    else
        glDisableClientState(GL_COLOR_ARRAY);
    if(qglEnableClientStateVertexArray)
        glEnableClientState(GL_VERTEX_ARRAY);
    else
        glDisableClientState(GL_VERTEX_ARRAY);
    if(qglEnableClientStateNormalArray)
        glEnableClientState(GL_NORMAL_ARRAY);
    else
        glDisableClientState(GL_NORMAL_ARRAY);
}

void
qglDrawBuffer(GLenum mode)
{
	if (mode != GL_BACK)
		UNIMPL();
}

GLint qglDrawElements_currentPos = 0;

GLint qglDrawElements_currentWindowSize = 0;

static int has_NormalPointer = 0;
static struct { 
    GLenum type; GLsizei stride; const GLvoid *pointer;
}
savedNormalPointer;

void qglNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
    qglDrawElements_currentPos = 0;
    has_NormalPointer = 1;
    savedNormalPointer.type = type;
    savedNormalPointer.stride = stride;
    savedNormalPointer.pointer = pointer;

    glNormalPointer(type, stride, pointer);
}

static int has_VertexPointer = 0;
static struct { 
    GLint size; GLenum type; GLsizei stride; const GLvoid *pointer;
}
savedVertexPointer;

void qglVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    qglDrawElements_currentPos = 0;

    has_VertexPointer = 1;
    savedVertexPointer.size = size;
    savedVertexPointer.type = type;
    savedVertexPointer.stride = stride;
    savedVertexPointer.pointer = pointer;

    glVertexPointer(size, type, stride, pointer);
}

static int has_TexCoordPointer = 0;
static struct { 
    GLint size; GLenum type; GLsizei stride; const GLvoid *pointer;
}
savedTexCoordPointer;

void qglTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    qglDrawElements_currentPos = 0;

    has_TexCoordPointer = 1;
    savedTexCoordPointer.size = size;
    savedTexCoordPointer.type = type;
    savedTexCoordPointer.stride = stride;
    savedTexCoordPointer.pointer = pointer;

    glTexCoordPointer(size, type, stride, pointer);
}

static int has_ColorPointer = 0;
static struct { 
    GLint size; GLenum type; GLsizei stride; const GLvoid *pointer;
} savedColorPointer;

void qglColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    qglDrawElements_currentPos = 0;

    has_ColorPointer = 1;
    savedColorPointer.size = size;
    savedColorPointer.type = type;
    savedColorPointer.stride = stride;
    savedColorPointer.pointer = pointer;

    glColorPointer(size, type, stride, pointer);
}

void qglDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
    static GLushort * ushortPtr = NULL;

    const long maxBufferSize = USHRT_MAX;

    if(type == GL_UNSIGNED_INT) {
        if(!ushortPtr)
            ushortPtr = (GLushort*)malloc(sizeof(GLushort) * maxBufferSize );
        assert(ushortPtr);
        unsigned int i;


        int newWindowSize = qglDrawElements_currentWindowSize;
        GLuint min = UINT_MAX, max = 0;
        GLint newPos =     qglDrawElements_currentPos;
        for(i = 0; i<count;i++) {
            GLuint val = ((GLuint*)indices)[i];
            if(val < min) {
                min = val;
            }
            if(val > max) {
                max = val;
            }
        }
        int idealValue = (int)max - (int)min + 1;

        const int minBufferSize = 0;// USHRT_MAX/10;
        if(idealValue < minBufferSize)
        {
            // Load enough data
            idealValue = minBufferSize;
        }

        if(idealValue > qglDrawElements_currentWindowSize) {
            TGLLog("DEBUG: Min and Max are %d-%d, which is way to too big for windowSize:%d, should be %d. Adjusting.\n", min, max, qglDrawElements_currentWindowSize, idealValue );
            if(idealValue <= maxBufferSize) {
                newWindowSize = idealValue;
            }
            else
            {
                  TGLErr("WARNING: %d is too big for %d\n", idealValue, maxBufferSize);
            }
        } else if(idealValue <= minBufferSize)
        {
            // Degrow
            newWindowSize = minBufferSize;
        }

//            assert((int)max - (int)min <= USHRT_MAX);
        if( min < qglDrawElements_currentPos || max >= qglDrawElements_currentPos + newWindowSize )
        {
            newPos = min;
        }

        for(i = 0; i<count;i++) {
            GLuint val = ((GLuint*)indices)[i];
//            assert((int)val - (int)currentPos <= USHRT_MAX);
//            assert((int)val - (int)currentPos >= 0);
            int newVal = (int)val - (int)newPos;
            if(newVal >= newWindowSize) {
                TGLErr("WARNING: %d is too big for %d (of %d where windowSize:%d)\n", val, newPos - val - newWindowSize, newWindowSize);
                ushortPtr[i] = 0;
            } else if (newVal < 0) {
                TGLErr("WARNING: %d is too small for %d (of %d where windowSize:%d)\n", val, newPos - val, newWindowSize);
                ushortPtr[i] = 0;
            } else
                ushortPtr[i] = (GLushort)newVal;
        }

        if(qglDrawElements_currentPos != newPos || qglDrawElements_currentWindowSize != newWindowSize) {
            qglDrawElements_currentPos = newPos;
            qglDrawElements_currentWindowSize = newWindowSize;
            TGLLog("DEBUG: Reloading from %d\n", newPos);
            if(has_NormalPointer) {
                TGLLog("DEBUG: Reloading normal %d\n", savedNormalPointer.stride);
                glNormalPointer(savedNormalPointer.type, savedNormalPointer.stride, (GLubyte*)savedNormalPointer.pointer + qglDrawElements_currentPos * savedNormalPointer.stride);
                assert(savedNormalPointer.stride);
            }
            if(has_ColorPointer) {
                TGLLog("DEBUG: Reloading color %d\n", savedColorPointer.stride);
                glColorPointer(savedColorPointer.size, savedColorPointer.type, savedColorPointer.stride, (GLubyte*)savedColorPointer.pointer + qglDrawElements_currentPos * savedColorPointer.stride);
                assert(savedColorPointer.stride);
            }

            if(has_TexCoordPointer) {
                TGLLog("DEBUG: Reloading texture %d\n", savedTexCoordPointer.stride);
                glTexCoordPointer(savedTexCoordPointer.size, savedTexCoordPointer.type, savedTexCoordPointer.stride, (GLubyte*)savedTexCoordPointer.pointer + qglDrawElements_currentPos * savedTexCoordPointer.stride);
                assert(savedTexCoordPointer.stride);
            }
            if(has_VertexPointer) {
                TGLLog("DEBUG: Reloading vertex %d\n", savedVertexPointer.stride);
                glVertexPointer(savedVertexPointer.size, savedVertexPointer.type, savedVertexPointer.stride, (GLubyte*)savedVertexPointer.pointer + qglDrawElements_currentPos * savedVertexPointer.stride);
                assert(savedVertexPointer.stride);
            }
        }

        indices = ushortPtr;
        type = GL_UNSIGNED_SHORT;
    }

    if(qglEnableTextureGenS && qglEnableTextureGenT)
    {
        if(qglTexGenSObjectPlane && qglTexGenTObjectPlane)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glMatrixMode(GL_TEXTURE);
            GLfloat m[4][4] = {
                { qglTexGenSObjectPlane_param[0], qglTexGenTObjectPlane_param[0], 0, 0 },
                { qglTexGenSObjectPlane_param[1], qglTexGenTObjectPlane_param[1], 0, 0 },
                { qglTexGenSObjectPlane_param[2], qglTexGenTObjectPlane_param[2], 1, 0 },
                { qglTexGenSObjectPlane_param[3], qglTexGenTObjectPlane_param[3], 0, 1 } //note 1's form diagonal of identity matrix
            };
            glLoadMatrixf((GLfloat *)m);
            glMatrixMode(GL_MODELVIEW);
        }
        else {
        }

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(savedVertexPointer.size, savedVertexPointer.type, savedVertexPointer.stride, (GLubyte*)savedVertexPointer.pointer + qglDrawElements_currentPos * savedVertexPointer.stride);
        glDrawElements(mode, count, type, indices);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
    }
    else
        glDrawElements(mode, count, type, indices);
}

static void restoreArrayState ()
{
    if(has_VertexPointer)
        glVertexPointer(savedVertexPointer.size, savedVertexPointer.type, savedVertexPointer.stride, (GLubyte*)savedVertexPointer.pointer + savedVertexPointer.stride * qglDrawElements_currentPos);
    if(has_ColorPointer)
        glColorPointer(savedColorPointer.size, savedColorPointer.type, savedColorPointer.stride, (GLubyte*)savedColorPointer.pointer + savedColorPointer.stride * qglDrawElements_currentPos);
    if(has_TexCoordPointer)
        glTexCoordPointer(savedTexCoordPointer.size, savedTexCoordPointer.type, savedTexCoordPointer.stride, (GLubyte*)savedTexCoordPointer.pointer + savedTexCoordPointer.stride * qglDrawElements_currentPos);
    if(has_NormalPointer)
        glNormalPointer(savedNormalPointer.type, savedNormalPointer.stride, (GLubyte*)savedNormalPointer.pointer + savedNormalPointer.stride * qglDrawElements_currentPos);
}

void
qglEnd(void)
{
	GLenum mode;

	assert(QGLBeginStarted);
	QGLBeginStarted = false;

    if(qglEnableTextureGenT && qglEnableTextureGenS && qglTexGenSObjectPlane && qglTexGenTObjectPlane)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glMatrixMode(GL_TEXTURE);
        GLfloat m[4][4] = {
            { qglTexGenSObjectPlane_param[0], qglTexGenTObjectPlane_param[0], 0, 0 },
            { qglTexGenSObjectPlane_param[1], qglTexGenTObjectPlane_param[1], 0, 0 },
            { qglTexGenSObjectPlane_param[2], qglTexGenTObjectPlane_param[2], 1, 0 },
            { qglTexGenSObjectPlane_param[3], qglTexGenTObjectPlane_param[3], 0, 1 } //note 1's form diagonal of identity matrix
        };
        glLoadMatrixf((GLfloat *)m);
        glMatrixMode(GL_MODELVIEW);
		glTexCoordPointer(3, GL_FLOAT, sizeof(_GLimp_vertexes[0]), _GLimp_vertexes);
    } else if (_GLimp_texcoordbuffer)
	{
		glTexCoordPointer(2, GL_FLOAT, sizeof(_GLimp_texcoords[0]), _GLimp_texcoords);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
    else
    {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

	if (_GLimp_colorbuffer)
	{
		glColorPointer(4, GL_FLOAT, sizeof(_GLimp_colors[0]), _GLimp_colors);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	else
		glDisableClientState(GL_COLOR_ARRAY);

	if (_GLimp_normalbuffer)
	{
		glNormalPointer(GL_FLOAT, sizeof(_GLimp_colors[0]), _GLimp_colors);
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	else
		glDisableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(_GLimp_vertexes[0]), _GLimp_vertexes);
	glEnableClientState(GL_VERTEX_ARRAY);

	if (_GLimp_beginmode == GL_QUADS)
		mode = GL_TRIANGLES;
	else if (_GLimp_beginmode == GL_POLYGON)
		assert(0);
	else
		mode = _GLimp_beginmode;

	glDrawArrays(mode, 0, _GLimp_numOutputVerts);

    if(qglEnableTextureGenT && qglEnableTextureGenS)
    {
        if(qglTexGenSObjectPlane && qglTexGenTObjectPlane)
        {
            // Revert that back
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
        }
    }

    restoreClientState();
    restoreArrayState();
}

void
qglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	GLfloat v[4] = {r, g, b, a};

	qglColor4fv(v);
}

void
qglColor4fv(GLfloat *v)
{
	if (QGLBeginStarted)
	{
		assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE);
		bcopy(v, _GLimp_colors[_GLimp_numOutputVerts], sizeof(_GLimp_colors[0]));
		_GLimp_colorbuffer = true;
	}
	else
	{
#ifdef QGL_CHECK_GL_ERRORS
		QGLCheckError("glColor4fv");
#endif // QGL_CHECK_GL_ERRORS
		glColor4f(v[0], v[1], v[2], v[3]);
#ifdef QGL_CHECK_GL_ERRORS
		QGLCheckError("glColor4fv");
#endif // QGL_CHECK_GL_ERRORS
	}
}

void
qglTexCoord2f(GLfloat s, GLfloat t)
{
	GLfloat v[2] = {s, t};

	qglTexCoord2fv(v);
}

void
qglTexCoord2fv(GLfloat *v)
{
	assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE);
	bcopy(v, _GLimp_texcoords[_GLimp_numOutputVerts], sizeof(_GLimp_texcoords[0]));
	_GLimp_texcoordbuffer = true;
}

void qglNormal3fv(GLfloat * v)
{
	if (!QGLBeginStarted)
	{
        glNormal3f(v[0],v[1],v[2]);
    }
    else
    {
        assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE);
        bcopy(v, _GLimp_normal[_GLimp_numOutputVerts], sizeof(_GLimp_normal[0]));
        _GLimp_normalbuffer = true;
    }
}

void qglNormal3f(GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat v[3] = {x, y, z};

	qglNormal3fv(v);
}

void
qglVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat v[3] = {x, y, z};

	qglVertex3fv(v);
}

void
qglVertex3fv(GLfloat *v)
{
	assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE);
	bcopy(v, _GLimp_vertexes[_GLimp_numOutputVerts++], sizeof(_GLimp_vertexes[0]));
	++_GLimp_numInputVerts;

	if (_GLimp_beginmode == GL_QUADS && _GLimp_numInputVerts % 4 == 0)
	{
		assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE - 2);
		bcopy(_GLimp_vertexes[_GLimp_numOutputVerts - 4],
				_GLimp_vertexes[_GLimp_numOutputVerts],
				sizeof(_GLimp_vertexes[0]));
		bcopy(_GLimp_texcoords[_GLimp_numOutputVerts - 4],
				_GLimp_texcoords[_GLimp_numOutputVerts],
				sizeof(_GLimp_texcoords[0]));
		bcopy(_GLimp_vertexes[_GLimp_numOutputVerts - 2],
				_GLimp_vertexes[_GLimp_numOutputVerts + 1],
				sizeof(_GLimp_vertexes[0]));
		bcopy(_GLimp_texcoords[_GLimp_numOutputVerts - 2],
				_GLimp_texcoords[_GLimp_numOutputVerts + 1],
				sizeof(_GLimp_texcoords[0]));
		_GLimp_numOutputVerts+= 2;
	}
	else if (_GLimp_beginmode == GL_POLYGON)
		assert(0);
}

void
qglCallList(GLuint list)
{
	UNIMPL();
}


void qglBindTexture(GLenum target, GLuint texture) 
{
    glBindTexture(target, texture);
}


void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) 
{
	qglBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	qglEnd();
}

void glMultMatrixd(const double * m) {
	GLfloat matrix[16];
	int i;
	for( i = 0; i < 16; i++ )
	{
        matrix[i] = (float)m[i];
	}
	glMultMatrixf( matrix );
}

void rotate2D(float angle, float pointOfRotationX, float pointOfRotationY)
{
	// Note that the steps here are in the reverse order of that mentioned above!!
	// That's because openGL transformations post-multiply the matrices.
	glTranslatef(pointOfRotationX, pointOfRotationY, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-pointOfRotationX, -pointOfRotationY, 0.0f);
}
