/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    CHUI
    sdl_dc_dev@yahoo.es

    based on SDL_dcvideo.h by

    BERO
    bero@geocities.co.jp
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_dcvideo.h,v 1.1 2002/10/05 16:50:56 slouken Exp $";
#endif

#ifndef _SDL_gp32video_h
#define _SDL_gp32video_h

#include "SDL_mouse.h"
#include "SDL_sysvideo.h"
#include "SDL_mutex.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this


/* Private display data */

struct SDL_PrivateVideoData {
    int w, h;
    void *buffer;
};

#endif /* _SDL_dcvideo_h */
