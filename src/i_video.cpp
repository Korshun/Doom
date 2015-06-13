// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL library
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>

#include <SDL/SDL.h>

#define FRAMEWIDTH 320
#define FRAMEHEIGHT 240

#include "glew.h"
#include "GL/gl.h"

#include "m_swap.h"
#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

static SDL_Surface *screen;
static SDL_Color palette[256];
static SDL_Color *framergb;
static GLuint frametexture = 0;
static GLuint uniformTexture = -1;

dboolean		grabMouse;// Fake mouse handling.

// Returns the smallest power-of-two number that is bigger than the given one.
int NextPowerOf2(int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}


void I_UpdateNoBlit (void) {}

void I_StartFrame (void) {}

//
//  Translates the key 
//

int xlatekey(SDL_keysym *key)
{

    int rc;

    switch(key->sym)
    {
      case SDLK_LEFT:	rc = KEY_LEFTARROW;	break;
      case SDLK_RIGHT:	rc = KEY_RIGHTARROW;	break;
      case SDLK_DOWN:	rc = KEY_DOWNARROW;	break;
      case SDLK_UP:	rc = KEY_UPARROW;	break;
      case SDLK_ESCAPE:	rc = KEY_ESCAPE;	break;
      case SDLK_RETURN:	rc = KEY_ENTER;		break;
      case SDLK_TAB:	rc = KEY_TAB;		break;
      case SDLK_F1:	rc = KEY_F1;		break;
      case SDLK_F2:	rc = KEY_F2;		break;
      case SDLK_F3:	rc = KEY_F3;		break;
      case SDLK_F4:	rc = KEY_F4;		break;
      case SDLK_F5:	rc = KEY_F5;		break;
      case SDLK_F6:	rc = KEY_F6;		break;
      case SDLK_F7:	rc = KEY_F7;		break;
      case SDLK_F8:	rc = KEY_F8;		break;
      case SDLK_F9:	rc = KEY_F9;		break;
      case SDLK_F10:	rc = KEY_F10;		break;
      case SDLK_F11:	rc = KEY_F11;		break;
      case SDLK_F12:	rc = KEY_F12;		break;
	
      case SDLK_BACKSPACE:
      case SDLK_DELETE:	rc = KEY_BACKSPACE;	break;

      case SDLK_PAUSE:	rc = KEY_PAUSE;		break;

      case SDLK_EQUALS:	rc = KEY_EQUALS;	break;

      case SDLK_KP_MINUS:
      case SDLK_MINUS:	rc = KEY_MINUS;		break;

      case SDLK_LSHIFT:
      case SDLK_RSHIFT:
	rc = KEY_RSHIFT;
	break;
	
      case SDLK_LCTRL:
      case SDLK_RCTRL:
	rc = KEY_RCTRL;
	break;
	
      case SDLK_LALT:
      case SDLK_LMETA:
      case SDLK_RALT:
      case SDLK_RMETA:
	rc = KEY_RALT;
	break;
	
      default:
        rc = key->sym;
	break;
    }

    return rc;

}

void I_ShutdownGraphics(void)
{
  SDL_Quit();
}



/* This processes SDL events */
void I_GetEvent(SDL_Event *Event)
{
    Uint8 buttonstate;
    event_t event;

    switch (Event->type)
    {
      case SDL_KEYDOWN:
	event.type = ev_keydown;
	event.data1 = xlatekey(&Event->key.keysym);
	D_PostEvent(&event);
        break;

      case SDL_KEYUP:
	event.type = ev_keyup;
	event.data1 = xlatekey(&Event->key.keysym);
	D_PostEvent(&event);
	break;

      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
	buttonstate = SDL_GetMouseState(NULL, NULL);
	event.type = ev_mouse;
	event.data1 = 0
	    | (buttonstate & SDL_BUTTON(1) ? 1 : 0)
	    | (buttonstate & SDL_BUTTON(2) ? 2 : 0)
	    | (buttonstate & SDL_BUTTON(3) ? 4 : 0);
	event.data2 = event.data3 = 0;
	D_PostEvent(&event);
	break;

#if (SDL_MAJOR_VERSION >= 0) && (SDL_MINOR_VERSION >= 9)
      case SDL_MOUSEMOTION:
	/* Ignore mouse warp events */
	if ((Event->motion.x != screen->w/2)||(Event->motion.y != screen->h/2))
	{
	    /* Warp the mouse back to the center */
	    if (grabMouse) {
		SDL_WarpMouse(screen->w/2, screen->h/2);
	    }
	    event.type = ev_mouse;
	    event.data1 = 0
	        | (Event->motion.state & SDL_BUTTON(1) ? 1 : 0)
	        | (Event->motion.state & SDL_BUTTON(2) ? 2 : 0)
	        | (Event->motion.state & SDL_BUTTON(3) ? 4 : 0);
	    event.data2 = Event->motion.xrel << 2;
	    event.data3 = -Event->motion.yrel << 2;
	    D_PostEvent(&event);
	}
	break;
#endif

      case SDL_QUIT:
	I_Quit();
    }

}

//
// I_StartTic
//
void I_StartTic (void)
{
    SDL_Event Event;

    while ( SDL_PollEvent(&Event) )
	I_GetEvent(&Event);
}


//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
	static int	lasttic;
	int		tics;
	int		i;
	int x, y;

	float u, v;

	// draws little dots on the bottom of the screen
	if (devparm)
	{
		i = I_GetTime();
		tics = i - lasttic;
		lasttic = i;
		if (tics > 20) tics = 20;

		for (i=0 ; i<tics*2 ; i+=2)
		    screens[0][ (FRAMEHEIGHT-1)*FRAMEWIDTH + i] = 0xff;
		for ( ; i<20*2 ; i+=2)
		    screens[0][ (FRAMEHEIGHT-1)*FRAMEWIDTH + i] = 0x0;
	}

	// Convert palletized framebuffer to RGB.
	for (i = 0; i < FRAMEWIDTH * FRAMEHEIGHT; i++)
	{
		framergb[i] = palette[screens[0][i]];
		//framergb[i].r = screens[0][i];
		//framergb[i].g = screens[0][i];
		//framergb[i].b = screens[0][i];
	}

	// Upload framebuffer to texture.
	glBindTexture(GL_TEXTURE_2D, frametexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FRAMEWIDTH, FRAMEHEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, framergb);
	//if (uniformTexture != -1)
		glUniform1i(uniformTexture, 0);

	// Draw it.
	// NPOT textures are slower.
	u = FRAMEWIDTH  / (float)NextPowerOf2(FRAMEWIDTH);
	v = 200 / (float)NextPowerOf2(FRAMEHEIGHT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0);
	glTexCoord2f(u,   0.0); glVertex2f(1.0, 0.0);
	glTexCoord2f(u,   v);   glVertex2f(1.0, 1.0);
	glTexCoord2f(0.0, v);   glVertex2f(0.0, 1.0);
	glEnd();

	SDL_GL_SwapBuffers();
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], FRAMEWIDTH*FRAMEHEIGHT);
}


//
// I_SetPalette
//
void I_SetPalette (byte* newpal)
{
	int i;
    for (i = 0; i < 256; ++i)
    {
    	palette[i].r = gammatable[usegamma][*newpal++];
    	palette[i].g = gammatable[usegamma][*newpal++];
    	palette[i].b = gammatable[usegamma][*newpal++];
    	palette[i].unused = 0;
    }
}

const char *vertexshader =
"#version 110\n"
"void main()\n"
"{\n"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
" 	gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"}\n";


const char *fragmentshader =
"\n"
"uniform sampler2D tex;"
"void main()\n"
"{\n"
#if 0
"  vec4 sum = vec4(0);\n"
"  vec2 texcoord = vec2(gl_TexCoord[0]);\n"
"  int j;\n"
"  int i;\n"
"  \n"
"  for( i= -4 ;i < 4; i++)\n"
"  {\n"
"       for (j = -3; j < 3; j++)\n"
"       {\n"
"           sum += texture2D(tex, texcoord + vec2(j, i)*0.004) * 0.25;\n"
"       }\n"
"  }\n"
"      if (texture2D(tex, texcoord).r < 0.3)\n"
"   {\n"
"      gl_FragColor = sum*sum*0.012 + texture2D(tex, texcoord);\n"
"   }\n"
"   else\n"
"   {\n"
"       if (texture2D(tex, texcoord).r < 0.5)\n"
"       {\n"
"           gl_FragColor = sum*sum*0.009 + texture2D(tex, texcoord);\n"
"       }\n"
"       else\n"
"       {\n"
"           gl_FragColor = sum*sum*0.0075 + texture2D(tex, texcoord);\n"
"       }\n"
"   }\n"
#else
	"gl_FragColor = texture2D(tex, gl_TexCoord[0].st);\n"
#endif
"}\n";

static void I_ShaderError(GLuint shader)
{
	char log[1024];
	int len;
	glGetInfoLogARB(shader, 1024, &len, log);
	fprintf(stderr, "Shader error:\n%s\n", log);
}

static void I_LoadShader(void)
{
	GLuint vertex, fragment;
	GLuint program;

	vertex   = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSourceARB(vertex,   1, &vertexshader, NULL);
	glShaderSourceARB(fragment, 1, &fragmentshader,  NULL);

	glCompileShader(vertex);
	glCompileShader(fragment);

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	glLinkProgram(program);
	glUseProgram(program);

	I_ShaderError(vertex);
	I_ShaderError(fragment);
	I_ShaderError(program);

	uniformTexture = glGetUniformLocation(program, "tex");
}

void I_InitGraphics(void)
{
	void *whatever;
	int texwidth, texheight;
    static dboolean firsttime = true;
    if (!firsttime)
    	return;
    firsttime = false;

    // check if the user wants to grab the mouse (quite unnice)
    grabMouse = !!M_CheckParm("-grabmouse");

    screen = SDL_SetVideoMode(1000, 750, 24, SDL_OPENGL);
    if (screen == NULL)
        I_Error("Could not set video mode: %s", SDL_GetError());

    SDL_ShowCursor(0);
    SDL_WM_SetCaption("BLooM DooM!", NULL);

    // Create framebuffer and its rgb copy.
    screen   = malloc(FRAMEWIDTH * FRAMEHEIGHT);
    framergb = malloc(FRAMEWIDTH * FRAMEHEIGHT * sizeof(SDL_Color));

  	// Set up opengl.
    glewInit();
    if (GLEW_VERSION_2_0)
    //if (false)
    {
    	printf("I_InitGraphics: Loading posteffect shader.\n");
    	I_LoadShader();
    }
    else
    	printf("I_InitGraphics: OpenGL version < 2.0, no shaders.\n");

    glOrtho(0, 1, 1, 0, -1.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Create opengl texture for frame.
    glGenTextures(1, &frametexture);
    glBindTexture(GL_TEXTURE_2D, frametexture);

    texwidth  = NextPowerOf2(FRAMEWIDTH);
    texheight = NextPowerOf2(FRAMEHEIGHT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
    		texwidth, texheight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    // Set texture filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

