/*
    Copyright 2005-2010 Intel Corporation.  All Rights Reserved.

    The source code contained or described herein and all documents related
    to the source code ("Material") are owned by Intel Corporation or its
    suppliers or licensors.  Title to the Material remains with Intel
    Corporation or its suppliers and licensors.  The Material is protected
    by worldwide copyright laws and treaty provisions.  No part of the
    Material may be used, copied, reproduced, modified, published, uploaded,
    posted, transmitted, distributed, or disclosed in any way without
    Intel's prior express written permission.

    No license under any patent, copyright, trade secret or other
    intellectual property right is granted to or conferred upon you by
    disclosure or delivery of the Materials, either expressly, by
    implication, inducement, estoppel or otherwise.  Any license under such
    intellectual property rights must be express and approved by Intel in
    writing.
*/

/*
    The original source for this example is
    Copyright (c) 1994-2008 John E. Stone
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#include "machine.h"
#include "types.h"
#include "macros.h"
#include "vector.h"
#include "tgafile.h"
#include "trace.h"
#include "light.h"
#include "shade.h"
#include "camera.h"
#include "util.h"
#include "intersect.h"
#include "global.h"
#include "ui.h"
#include "video.h"

// shared but read-only so could be private too
static thr_parms *all_parms;
static scenedef scene;
static int startx;
static int stopx;
static int starty;
static int stopy;
static flt jitterscale;
static int totaly;


/* buffer size parameters for the global buffer used in find_hotspots */
#define mem_array_i_max 120    /* row size */
#define mem_array_j_max 120    /* column size */

unsigned int mem_array [mem_array_i_max * mem_array_j_max];
unsigned int fill_value = 4;

void initialize_2D_buffer (unsigned int mem_array [], unsigned int *fill_value)
{
	// First (slower) method of filling array
	// Array is NOT filled in consecutive memory address order
	/***********************************
	for (int i = 0; i < mem_array_i_max; i++)
	{
		// Try to defeat hardware prefetching by varying the stride
		int j(0), iteration_count(0);
		do {
			mem_array [j*mem_array_i_max+i] = *fill_value + 2;
			// Code to give the array accesses a non-uniform stride to defeat hardware prefetch
			if ((iteration_count % 3) == 0) j=j+3;
			else j=iteration_count;
			iteration_count++;
		} while (j < mem_array_j_max);
	}
	***********************************/


	// Faster method of filling array
	// The for loops are interchanged
	// Array IS filled in consecutive memory address order
	for (int j = 0; j < mem_array_j_max; j++)
	{
		for (int i = 0; i < mem_array_i_max; i++)
		{
			mem_array [j*mem_array_i_max+i] = *fill_value + 2;
		}
	}
}


static color_t render_one_pixel (int x, int y, unsigned int *local_mbox, unsigned int &serial,
                                 int startx, int stopx, int starty, int stopy)
{
    /* private vars moved inside loop */
    ray primary, sample;
    color col, avcol;
    int R,G,B;
    intersectstruct local_intersections;
    int alias;
    /* end private */

    primary=camray(&scene, x, y);
    primary.intstruct = &local_intersections;
    primary.flags = RT_RAY_REGULAR;

    serial++;
    primary.serial = serial;
    primary.mbox = local_mbox;
    primary.maxdist = FHUGE;
    primary.scene = &scene;
    col=trace(&primary);
    serial = primary.serial;

    // Memory operation to demonstrate cache analysis
    // Demonstrate loop interchange optimization in initialize_2D_buffer
    initialize_2D_buffer(mem_array, &fill_value);

    /* perform antialiasing if enabled.. */
    if (scene.antialiasing > 0) {
        for (alias=0; alias < scene.antialiasing; alias++) {

            serial++; /* increment serial number */
            sample=primary;  /* copy the regular primary ray to start with */
            sample.serial = serial;

            {
                sample.d.x+=((std::rand() % 100) - 50) / jitterscale;
                sample.d.y+=((std::rand() % 100) - 50) / jitterscale;
                sample.d.z+=((std::rand() % 100) - 50) / jitterscale;
            }

            avcol=trace(&sample);

            serial = sample.serial; /* update our overall serial # */

            col.r += avcol.r;
            col.g += avcol.g;
            col.b += avcol.b;
        }

        col.r /= (scene.antialiasing + 1.0);
        col.g /= (scene.antialiasing + 1.0);
        col.b /= (scene.antialiasing + 1.0);
    }

    /* Handle overexposure and underexposure here... */
    R=(int) (col.r*255);
    if (R > 255) R = 255;
    else if (R < 0) R = 0;

    G=(int) (col.g*255);
    if (G > 255) G = 255;
    else if (G < 0) G = 0;

    B=(int) (col.b*255);
    if (B > 255) B = 255;
    else if (B < 0) B = 0;

    return video->get_color(R, G, B);

}

static void draw_trace (void)
{
    unsigned int serial = 1;
	unsigned int mboxsize = sizeof(unsigned int)*(max_objectid() + 20);
    unsigned int * local_mbox = (unsigned int *) alloca(mboxsize);
	memset(local_mbox,0,mboxsize);

    for (int y = starty; y < stopy; y++) { {
        drawing_area drawing(startx, totaly-y, stopx-startx, 1);
        for (int x = startx; x < stopx; x++) {
            color_t c = render_one_pixel (x, y, local_mbox, serial, startx, stopx, starty, stopy);
            drawing.put_pixel(c);
        } }
        if(!video->next_frame()) return;
     }
}

void * thread_trace(thr_parms * parms)
{
    // shared but read-only so could be private too
    all_parms = parms;
    scene = parms->scene;
    startx = parms->startx;
    stopx = parms->stopx;
    starty = parms->starty;
    stopy = parms->stopy;
    jitterscale = 40.0*(scene.hres + scene.vres);
    totaly = parms->scene.vres-1;

    draw_trace ();

    return(NULL);
}
