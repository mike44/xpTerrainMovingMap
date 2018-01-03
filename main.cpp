//#if LIN
// in order to get function prototypes from glext.h, define GL_GLEXT_PROTOTYPES before including glext.h
#define GL_GLEXT_PROTOTYPES
//#endif
#if IBM
#define GLEW_STATIC
#include <GL/glew.h>
//#include <GL/glut.h>
#endif

#include "XPLMDefs.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMScenery.h"
#include <stdio.h>
#include <string.h>
#if APL
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include <GL/glInfo.h>



GLuint fbo, color_tex, depth_rbo;

XPLMDataRef panel_total_pnl_l, panel_total_pnl_b;
XPLMDataRef local_x, local_y, local_z;
XPLMDataRef psi, theta, phi;

double dWeltLat, dWeltLon, dWeltAlt;
XPLMProbeRef		TerrainProbe=NULL;
////// Terrain Data
float terrainMap[256][256][3];
float rgbColor[3];

int GaugesCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
// Saved state
GLint current_program, bound_fbo, viewport[4];
GLboolean color_mask[4];
GLclampf clear_color[4], fog_color[4];
GLint fog_mode;
GLfloat fog_start, fog_end;

// We want fog and depth testing/writing
XPLMSetGraphicsState(1, 0, 0, 0, 0, 1, 1);

// Save some state
glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &bound_fbo);
glGetIntegerv(GL_VIEWPORT, viewport);
glGetBooleanv(GL_COLOR_WRITEMASK, color_mask);
glGetFloatv(GL_COLOR_CLEAR_VALUE, clear_color);
glGetFloatv(GL_FOG_COLOR, fog_color);
glGetIntegerv(GL_FOG_MODE, &fog_mode);
glGetFloatv(GL_FOG_START, &fog_start);
glGetFloatv(GL_FOG_END, &fog_end);

// Set up our framebuffer as well as some state
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
glClearColor(0.4, 0.5, 0.7, 1);
//glFogi(GL_FOG_MODE, GL_LINEAR);
//glFogf(GL_FOG_START, 1);
//glFogf(GL_FOG_END, 5);
//glFogfv(GL_FOG_COLOR, (GLfloat[]){0, 0, 0, 1});
glUseProgram(0);
glViewport(0, 0, 256, 256);

// Clear the framebuffer and set up camera
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
//glFrustum(-0.7, 0.7, -0.7, 0.7, 1, 128);
glFrustum(-0.7, 0.7, -0.7, 0.7, 1, 5000);
glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();

glRotatef(XPLMGetDataf(psi), 0, 1, 0);
glRotatef(XPLMGetDataf(theta), -1, 0, 0);
glRotatef(XPLMGetDataf(phi), 0, 0, 1);
glTranslatef(-XPLMGetDatad(local_x), -XPLMGetDatad(local_y), -XPLMGetDatad(local_z));

float offx = XPLMGetDataf(panel_total_pnl_l);
float offy = XPLMGetDataf(panel_total_pnl_b);

float local_x_val = XPLMGetDatad(local_x);
float local_y_val = XPLMGetDatad(local_y);
float local_z_val = XPLMGetDatad(local_z);
float scale = 1000; // zoomlevel?
float sample_y = 1000000 + local_y_val; //terrain height?
int iProbeCount = 0;

for (int x=0; x<128; ++x) {
    float sample_x = scale * (x - 64) + local_x_val;
    for (int z=0; z<128; ++z) {
        float sample_z = scale * (z - 64) + local_z_val;
        XPLMProbeInfo_t info;
        info.structSize = sizeof info;
        switch (XPLMProbeTerrainXYZ(TerrainProbe, sample_x, sample_y, sample_z, &info)) {
            case xplm_ProbeHitTerrain:
                terrainMap[x][z][0] = info.locationX;
                terrainMap[x][z][1] = info.locationY;
                terrainMap[x][z][2] = info.locationZ;

            			if( info.is_wet ){
            				rgbColor[0] = 0.5f;
            				rgbColor[1] = 0.6f;
            				rgbColor[2] = 0.9f;

            			}else if( info.locationY <= 0.f ){
            				rgbColor[0] = 0.839f;
            				rgbColor[1] = 0.788f;
            				rgbColor[2] = 0.333f;

            			}else if( info.locationY <= 300.f ){
            				rgbColor[0] = 0.663f;
            				rgbColor[1] = 0.701f;
            				rgbColor[2] = 0.572f;

            			}else if( info.locationY <= 600.f ){
            				rgbColor[0] = 0.520f;
            				rgbColor[1] = 0.64f;
            				rgbColor[2] = 0.471f;

            			}else if( info.locationY <= 1000.f ){
            				rgbColor[0] = 0.454f;
            				rgbColor[1] = 0.494f;
            				rgbColor[2] = 0.411f;

            			}else if( info.locationY <= 1500.f ){
            				rgbColor[0] = 0.419f;
            				rgbColor[1] = 0.443f;
            				rgbColor[2] = 0.392f;

            			}else if( info.locationY <= 2000.f ){
            				rgbColor[0] = 0.549f;
            				rgbColor[1] = 0.529f;
            				rgbColor[2] = 0.4f;

            			}else if( info.locationY <= 2800.f ){
            				rgbColor[0] = 0.721f;
            				rgbColor[1] = 0.658f;
            				rgbColor[2] = 0.529f;

            			}else if( info.locationY <= 3300.f ){
            				rgbColor[0] = 0.768f;
            				rgbColor[1] = 0.721f;
            				rgbColor[2] = 0.627f;

            			}else if( info.locationY <= 4000.f ){
            				rgbColor[0] = 0.823f;
            				rgbColor[1] = 0.784f;
            				rgbColor[2] = 0.705f;

            			}else if( info.locationY <= 4500.f ){
            				rgbColor[0] = 0.878f;
            				rgbColor[1] = 0.850f;
            				rgbColor[2] = 0.8f;

            			}else if( info.locationY >= 5200.f ){
            				rgbColor[0] = 0.925f;
            				rgbColor[1] = 0.909f;
            				rgbColor[2] = 0.882f;

            			}else { //bugs?
            				rgbColor[0] = 0.925f;
            				rgbColor[1] = 0.909f;
            				rgbColor[2] = 0.882f;

            			}

				glBegin(GL_TRIANGLE_STRIP);
						// draw vertex 0
				        glColor3f(rgbColor[0], rgbColor[1], rgbColor[2]);
						glVertex3f(terrainMap[x][z][0], terrainMap[x][z][1], terrainMap[x][z][2]);

						// draw vertex 1
					    glColor3f(rgbColor[0], rgbColor[1], rgbColor[2]);
						glVertex3f(terrainMap[x+1][z][0], terrainMap[x+1][z][1], terrainMap[x+1][z][2]);

						// draw vertex 2
					    glColor3f(rgbColor[0], rgbColor[1], rgbColor[2]);
						glVertex3f(terrainMap[x][z+1][0], terrainMap[x][z+1][1], terrainMap[x][z+1][2]);

						// draw vertex 3
					    glColor3f(rgbColor[0], rgbColor[1], rgbColor[2]);
						glVertex3f(terrainMap[x+1][z+1][0], terrainMap[x+1][z+1][1], terrainMap[x+1][z+1][2]);

					glEnd();

                break;
        }
    }
    iProbeCount++;
}


// Pop the camera; leave in modelview
glPopMatrix();
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);

// Restore state
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, bound_fbo);
glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
glColorMask(color_mask[0], color_mask[1], color_mask[2], color_mask[3]);
glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
glFogi(GL_FOG_MODE, fog_mode);
glFogf(GL_FOG_START, fog_start);
glFogf(GL_FOG_END, fog_end);
glFogfv(GL_FOG_COLOR, fog_color);
glUseProgram(current_program);

// Finally, draw our texture on the panel

XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0); // 1 texture and alpha blending
XPLMBindTexture2d(color_tex, 0);
glColor3f(1, 1, 1);
glBegin(GL_QUADS);
glTexCoord2f(0, 0);
glVertex2f(offx + 1059, offy + 749);
glTexCoord2f(0, 1);
glVertex2f(offx + 1059, offy + 877);
glTexCoord2f(1, 1);
glVertex2f(offx + 1187, offy + 877);
glTexCoord2f(1, 0);
glVertex2f(offx + 1187, offy + 749);
glEnd();

return 1;
}

PLUGIN_API int XPluginStart(char *outName, char *outSignature, char
*outDescription)
{
strcpy(outName, "X-Hacking");
strcpy(outSignature, "thanks.Jonathan.Marginal");
strcpy(outDescription, "Gross hacks of sgier.com");

GLenum err = glewInit();
if (GLEW_OK != err)
{
  /* Problem: glewInit failed, something is seriously wrong. */
  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
}

panel_total_pnl_l = XPLMFindDataRef("sim/graphics/view/panel_total_pnl_l");
panel_total_pnl_b = XPLMFindDataRef("sim/graphics/view/panel_total_pnl_b");
local_x = XPLMFindDataRef("sim/flightmodel/position/local_x");
local_y = XPLMFindDataRef("sim/flightmodel/position/local_y");
local_z = XPLMFindDataRef("sim/flightmodel/position/local_z");
psi = XPLMFindDataRef("sim/flightmodel/position/psi");
theta = XPLMFindDataRef("sim/flightmodel/position/theta");
phi = XPLMFindDataRef("sim/flightmodel/position/phi");

XPLMRegisterDrawCallback(GaugesCallback, xplm_Phase_Gauges, 0, 0);
TerrainProbe = XPLMCreateProbe(xplm_ProbeY);

return 1;
}

PLUGIN_API void XPluginStop(void)
{
XPLMUnregisterDrawCallback(GaugesCallback, xplm_Phase_Gauges, 0, 0);
}

PLUGIN_API int XPluginEnable(void)
{
// Capture state
GLint renderbuffer_binding, framebuffer_binding;
glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &renderbuffer_binding);
glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &framebuffer_binding);

// Set up texture
int texID;
XPLMGenerateTextureNumbers(&texID, 1);
color_tex = texID; // Type punning is immoral, unlike breaking X-Plane
XPLMBindTexture2d(color_tex, 0);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA,
GL_UNSIGNED_BYTE, 0);

// Set up renderbuffer
glGenRenderbuffersEXT(1, &depth_rbo);
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rbo);
glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, 256, 256);

// Set up framebuffer
glGenFramebuffersEXT(1, &fbo);
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
GL_TEXTURE_2D, color_tex, 0);
glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
GL_RENDERBUFFER_EXT, depth_rbo);

// Restore state
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer_binding);
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_binding);

return 1;
}

PLUGIN_API void XPluginDisable(void)
{
// Delete our stuff
glDeleteFramebuffersEXT(1, &fbo);
glDeleteTextures(1, &color_tex);
glDeleteRenderbuffersEXT(1, &depth_rbo);
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage,
void *inRefcon)
{
}
