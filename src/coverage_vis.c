/*
 *****************************************************
 *
 *  SaVi by Lloyd Wood (lloydwood@users.sourceforge.net),
 *          Patrick Worfolk (worfolk@alum.mit.edu) and
 *          Robert Thurman.
 *
 *  Copyright (c) 1997 by The Geometry Center.
 *  Also Copyright (c) 2017 by Lloyd Wood.
 *
 *  This file is part of SaVi.  SaVi is free software;
 *  you can redistribute it and/or modify it only under
 *  the terms given in the file COPYRIGHT which you should
 *  have received along with this file.  SaVi may be
 *  obtained from:
 *  http://savi.sourceforge.net/
 *  http://www.geom.uiuc.edu/locate/SaVi
 *
 *****************************************************
 *
 * coverage_vis.c
 *
 * Coverage visualization routines.
 *
 * $Id: coverage_vis.c,v 1.138 2017/06/28 11:09:50 lloydwood Exp $
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _TK
#include <tk.h>
#endif

#ifndef NO_ZLIB
#include <zlib.h>
#endif /* NO_ZLIB */

#include "constants.h"
#include "fisheye.h"
#include "globals.h"
#include "gv_file.h"
#include "gv_utils.h"
#include "orbit_utils.h"
#include "savi.h"
#include "stats_utils.h"

#include "coverage_vis.h"

extern Tcl_Interp *interp;	/* Interpreter for this application. */

extern int tcl_script(char[]);

extern int no_access_flag;
extern int map_flag;

extern unsigned int geomview_flag;
extern unsigned int earth_on_flag;
extern unsigned int earth_geom_exists;
extern unsigned int use_fancy_earth;
extern unsigned int geomview_dynamic_coverage;

static int current_proj = -1; /* whatever the default, we start by drawing */

static int coverage_bitmap_drawn;

static char imagename[] = "im_coverage";

static const char format1[] =
  "set coverage \"Estimated map coverage: %.1f%%\"";
static const char format2[] =
 "set mean_coverage \"Averaged map coverage: %.1f%%\"";

static void write_image_as_ppm(FILE *f);

#ifndef NO_ZLIB
/* used only in this file */
static int gzip_compress2(Bytef *dest, uLongf *destLen,
                        const Bytef *source, uLong sourceLen,
                        int level);
static void write_image_to_stream_compressed();

static void * image_compress_scratch = NULL;
#endif /* NO_ZLIB */

static void set_coverage(int projection, grid *g);

static int reset_map_foreground(const unsigned char * map_pointer,
				const unsigned int map_size);
static int new_foreground(int projection, grid * g, unsigned int force);
static void draw_sin_map_edges(grid * g);
static Tk_PhotoImageBlock *get_image(void);
static int overlay_bitmap(unsigned int h, unsigned int w,
			  unsigned char *b, const char *filename);
static void latlon_to_grid_index(int grid_index[2], LatLon * pl, int projection,
				 grid * g);

static Tk_PhotoImageBlock *image = NULL;
static unsigned char *foreground = NULL;

#define PIXELSIZE 3		/* 3 for RGB */

typedef struct RGB {
    unsigned char red, green, blue;
} RGB;

/* Colors for coverage intensity (in RGB values)  */
/* We allow up to twenty values, for 0 to 19. *3, gives us 60. */
static unsigned char colors[60];
static unsigned char noaccess_colors[60];

/* Color for earth outline and ground track - see enum in coverage_vis.h */
static unsigned char outline_colors[] = { 255, 255, 255,  /* background */
					  0, 0, 0,	 /* earth outline */
					  0, 255, 0,    /* projected ground track */
					  255, 0, 255, /* selected ground track */
					  0, 255, 0,     /* satellite position */
            0, 255, 0,     /* rest of cross */
					  255, 0, 255,   /* selected satellite */
            255, 0, 255,   /* rest of cross */
            128, 128, 128,   /* ground location */
            128, 128, 128    /* rest of cross */
};

static unsigned char * COLDIV = &colors[0];
static unsigned char * COLDEC = &noaccess_colors[0];

static unsigned char * cyl_foreground;
static unsigned char * sin_foreground;
static unsigned char * sin_90_foreground;
static unsigned char * unp_foreground;
static unsigned char * unp_mask_foreground;
static unsigned char * sph_foreground;
static unsigned char * sph_90_foreground;

static int dynamic_errors_reported = FALSE;

/*
 * capable_of_compression - were we compiled with zlib available?
 */
int
capable_of_compression(void)
{
#ifndef NO_ZLIB
  return TRUE;
#endif
  return FALSE;
}

/*
 * color_hex - turn combined RRGGBB byte into three RGB bytes.
 */
static void
color_hex(int combined, RGB * color_set) {
  unsigned int rrggbb = (unsigned int) combined;

  color_set->blue = rrggbb & 0x0000FF;
  rrggbb = rrggbb >> 8;
  color_set->green = rrggbb & 0x0000FF;
  rrggbb = rrggbb >> 8;
  color_set->red = rrggbb & 0x0000FF;
}

/*
 * coverage_color_copy - slave color arrays to Tcl values
 */
void
coverage_color_copy(void)
{
  color_hex(DIV0, (RGB *) &COLDIV[0]);
  color_hex(DIV1, (RGB *) &COLDIV[3]);
  color_hex(DIV2, (RGB *) &COLDIV[6]);
  color_hex(DIV3, (RGB *) &COLDIV[9]);
  color_hex(DIV4, (RGB *) &COLDIV[12]);
  color_hex(DIV5, (RGB *) &COLDIV[15]);
  color_hex(DIV6, (RGB *) &COLDIV[18]);
  color_hex(DIV7, (RGB *) &COLDIV[21]);
  color_hex(DIV8, (RGB *) &COLDIV[24]);
  color_hex(DIV9, (RGB *) &COLDIV[27]);
  color_hex(DIV10, (RGB *) &COLDIV[30]);
  color_hex(DIV11, (RGB *) &COLDIV[33]);
  color_hex(DIV12, (RGB *) &COLDIV[36]);
  color_hex(DIV13, (RGB *) &COLDIV[39]);
  color_hex(DIV14, (RGB *) &COLDIV[42]);
  color_hex(DIV15, (RGB *) &COLDIV[45]);
  color_hex(DIV16, (RGB *) &COLDIV[48]);
  color_hex(DIV17, (RGB *) &COLDIV[51]);
  color_hex(DIV18, (RGB *) &COLDIV[54]);
  color_hex(DIV19, (RGB *) &COLDIV[57]);

  color_hex(DEC0, (RGB *) &COLDEC[0]);
  color_hex(DEC1, (RGB *) &COLDEC[3]);
  color_hex(DEC2, (RGB *) &COLDEC[6]);
  color_hex(DEC3, (RGB *) &COLDEC[9]);
  color_hex(DEC4, (RGB *) &COLDEC[12]);
  color_hex(DEC5, (RGB *) &COLDEC[15]);
  color_hex(DEC6, (RGB *) &COLDEC[18]);
  color_hex(DEC7, (RGB *) &COLDEC[21]);
  color_hex(DEC8, (RGB *) &COLDEC[24]);
  color_hex(DEC9, (RGB *) &COLDEC[27]);
  color_hex(DEC10, (RGB *) &COLDEC[30]);
  color_hex(DEC11, (RGB *) &COLDEC[33]);
  color_hex(DEC12, (RGB *) &COLDEC[36]);
  color_hex(DEC13, (RGB *) &COLDEC[39]);
  color_hex(DEC14, (RGB *) &COLDEC[42]);
  color_hex(DEC15, (RGB *) &COLDEC[45]);
  color_hex(DEC16, (RGB *) &COLDEC[48]);
  color_hex(DEC17, (RGB *) &COLDEC[51]);
  color_hex(DEC18, (RGB *) &COLDEC[54]);
  color_hex(DEC19, (RGB *) &COLDEC[57]);


}

/*
 * coverage_custom_height_valid - allow a custom map size, or not.
 */
int
coverage_custom_height_valid(int map_view_height)
{
  int valid;

  if (!map_view_height) return FALSE;

  valid = TRUE;

  if (map_view_height < IMAGE_MIN_HEIGHT) {
    fprintf(stderr, "\nSaVi: map height must be a minimum of %i pixels.",
	    IMAGE_MIN_HEIGHT);
    valid = FALSE;
  }
  if (map_view_height > IMAGE_MAX_HEIGHT) {
    fprintf(stderr, "\nSaVi: map height has a maximum limit of %i pixels.",
	  IMAGE_MAX_HEIGHT);
    valid = FALSE;
  }

  if (!valid) {
    error("using default map size instead.");
  }

  return(valid);
}

/*
 * image_init - Set up the Tk image structure based on the coverage grid.
 */
void
image_init(grid * g)
{
  unsigned int height, width, image_size, made;

  height = g->height;
  width = g->width;
  image_size = height * width;

  if (image_size <= 0) {
     error("image size must be greater than zero.");
     return;
  }

  image = (Tk_PhotoImageBlock *) malloc(sizeof(Tk_PhotoImageBlock));
  image->pixelPtr = (unsigned char *) malloc(image_size * PIXELSIZE *
					     sizeof(unsigned char *));
  image->width = width;
  image->height = height;
  image->pitch = width * PIXELSIZE;
  image->pixelSize = PIXELSIZE;
  image->offset[0] = 0;
  image->offset[1] = 1;
  image->offset[2] = 2;

  if (!foreground) {
    foreground = (unsigned char *) malloc(sizeof(char) * image_size);

    cyl_foreground = (unsigned char *) malloc(sizeof(char) * image_size);
    sin_foreground = (unsigned char *) malloc(sizeof(char) * image_size);
    sin_90_foreground = (unsigned char *) malloc(sizeof(char) * image_size);
    unp_foreground = (unsigned char *) malloc(sizeof(char) * image_size);
    unp_mask_foreground = (unsigned char *) malloc(sizeof(char) * image_size);
    sph_foreground = (unsigned char *) malloc(sizeof(char) * image_size);
    sph_90_foreground = (unsigned char *) malloc(sizeof(char) * image_size);

    if (width == IMAGE_LARGE_WIDTH) {
      made = overlay_bitmap(height, width, cyl_foreground, CYLINDRICAL_LARGE_BITMAP_NAME);
      if (!made) {
	free(cyl_foreground);
	cyl_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sin_foreground, SINUSOIDAL_LARGE_BITMAP_NAME);
      if (!made) {
	free(sin_foreground);
	sin_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sin_90_foreground, SINUSOIDAL_90_LARGE_BITMAP_NAME);
      if (!made) {
	free(sin_90_foreground);
	sin_90_foreground = NULL;
      }
      made = overlay_bitmap(height, width, unp_foreground, UNPROJECTED_LARGE_BITMAP_NAME);
      if (!made) {
	free(unp_foreground);
	unp_foreground = NULL;
      }
      made = overlay_bitmap(height, width, unp_mask_foreground, UNPROJECTED_MASK_LARGE_BITMAP_NAME);
      if (!made) {
	free(unp_mask_foreground);
	unp_mask_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sph_foreground, SPHERICAL_LARGE_BITMAP_NAME);
      if (!made) {
	free(sph_foreground);
	sph_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sph_90_foreground, SPHERICAL_90_LARGE_BITMAP_NAME);
      if (!made) {
        free(sph_90_foreground);
        sph_90_foreground = NULL;
      }

    } else {
      made = overlay_bitmap(height, width, cyl_foreground, CYLINDRICAL_BITMAP_NAME);
      if (!made) {
	free(cyl_foreground);
	cyl_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sin_foreground, SINUSOIDAL_BITMAP_NAME);
      if (!made) {
	free(sin_foreground);
	sin_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sin_90_foreground, SINUSOIDAL_90_BITMAP_NAME);
      if (!made) {
	free(sin_90_foreground);
	sin_90_foreground = NULL;
      }
      made = overlay_bitmap(height, width, unp_foreground, UNPROJECTED_BITMAP_NAME);
      if (!made) {
	free(unp_foreground);
	unp_foreground = NULL;
      }
      made = overlay_bitmap(height, width, unp_mask_foreground, UNPROJECTED_MASK_BITMAP_NAME);
      if (!made) {
	free(unp_mask_foreground);
	unp_mask_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sph_foreground, SPHERICAL_BITMAP_NAME);
      if (!made) {
	free(sph_foreground);
	sph_foreground = NULL;
      }
      made = overlay_bitmap(height, width, sph_90_foreground, SPHERICAL_90_BITMAP_NAME);
      if (!made) {
	free(sph_90_foreground);
	sph_90_foreground = NULL;
      }
    }

    draw_sin_map_edges(g);
  }
}

static void
write_image_as_ppm(FILE *f)
{
  unsigned int w, h, length, elements;
  unsigned char *p;

  /* this was grid g->width, but for temporary convenience... */
  w = Image_Width;
  h = Image_Height;

  /*
   * Write header file describing grid
   */
  fprintf(f,"P6\n%i %i\n255\n", w, h);

  p = image->pixelPtr;
  length = w * h * PIXELSIZE;
  elements = fwrite(p, sizeof(unsigned char), length, f);
  if (debug) {
     fprintf(stderr, "Took coverage image of %i bytes and wrote %i bytes to disk\n",
	     length, elements);
  }
}

void
write_image_to_file(char * name)
{
  FILE *f;

  if (NULL == (f = fopen(name, "w"))) {
    fprintf(stderr,"\nSaVi: unable to save coverage map to %s", name);
    return;
  }

  write_image_as_ppm(f);

  fclose(f);
}

static void
write_image_to_stream(void)
{
  unsigned int elements;
   unsigned int w = Image_Width;
   unsigned int h = Image_Height;

   unsigned char * p = image->pixelPtr;
   unsigned int length = w * h * PIXELSIZE;

   gv_start();
   gv_send("(read image {\n");
   gv_send("  define coverage\n");
   fprintf(gv_out, "  width %i\n", w);
   fprintf(gv_out, "  height %i\n", h);
   fprintf(gv_out,"data RGB raw %i {\n", length);
   elements = fwrite(p, sizeof(unsigned char), length, gv_out);
   if (debug) {
     fprintf(stderr, "Took coverage image of %i bytes and sent %i bytes to stream.\n", length, elements);
   }
   gv_send("\n} } )\n");
   gv_end();
}

#ifndef NO_ZLIB
/*
 * zlib's compress2 doesn't write the gzip header bytes.
 * This wrapper gets them added by signalling MAX_WBITS+16
 * Taken from Claus-Justus Heine's gv_compress2 code at end of
 * geomview/src/lib/shade/image.c
 *
 * function requires zlib */
static int gzip_compress2(Bytef *dest, uLongf *destLen,
                        const Bytef *source, uLong sourceLen,
                        int level)
{
    z_stream stream;
    int err;


    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;


    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;


    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;


    err = deflateInit2(&stream, level, Z_DEFLATED, MAX_WBITS+16, MAX_MEM_LEVEL,
                       Z_DEFAULT_STRATEGY);
    if (err != Z_OK) return err;


    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;


    err = deflateEnd(&stream);
    return err;
}

/* function requires zlib */
/* needs Geomview >= 1.9.0.
 * Geomview 1.9.0-.4 spawns gzip process to decompress image when we stream gzip.
 * 1.9.5 and later does this internally, but also supports more efficient raw.zlib..
 * so we default to sending 1.9.5 and later straight compress2, without munging
 * through gzip_compress2() to add gzip headers.
 */
static void
write_image_to_stream_compressed(void)
{
   int code;
   unsigned int w = Image_Width;
   unsigned int h = Image_Height;

   unsigned char * p = image->pixelPtr;
   unsigned int length = w * h * PIXELSIZE;
   unsigned int complength = (length * 102)/100 + 512;
   uLongf complength_f = complength;

   if (!image_compress_scratch) {
     image_compress_scratch = malloc(complength);
   }

   gv_start();
   gv_send("(read image {\n");
   gv_send("  define coverage\n");
   fprintf(gv_out, "  width %i\n", w);
   fprintf(gv_out, "  height %i\n", h);

   if (geomview_compress2_textures) {
     code = compress2(image_compress_scratch, &complength_f, p, length, 6);
     complength = complength_f;
     if (code != Z_OK) {
       fprintf(stderr, "Problem compress2'ing coverage image of %i bytes to %i bytes for stream.\n",
               length, complength);
     } else {
       if (debug) {
         fprintf(stderr, "Compress2'ed coverage image of %i bytes to %i bytes for stream.\n",
                length, complength);
       }
     }
     fprintf(gv_out,"data RGB raw.zlib %i {\n", complength);
     fwrite(image_compress_scratch, sizeof(unsigned char), complength, gv_out);
   } else {
     code = gzip_compress2(image_compress_scratch, &complength_f, p, length, 6);
     complength = complength_f;
     if (code != Z_OK) {
       fprintf(stderr, "Problem gv_compress2'ing coverage image of %i bytes to %i bytes for stream.\n",
	             length, complength);
     } else {
       if (debug) {
         fprintf(stderr, "gv_compress2'ed coverage image of %i bytes to %i bytes for stream.\n",
                 length, complength);
       }
     }
     fprintf(gv_out,"data RGB raw.gz %i {\n", complength);
     fwrite(image_compress_scratch, sizeof(unsigned char), complength, gv_out);
   }

   gv_send("\n} } )\n");
   gv_end();
}
#endif /* NO_ZLIB */

static void
coverage_output_image(void)
{
  if (geomview_stream_textures) {
    if (geomview_compressed_images) {
#ifndef NO_ZLIB
      write_image_to_stream_compressed();
#else
      error("geomview_compressed_images was set wrongly. Not sending image.");
#endif
    } else {
      write_image_to_stream();
    }
  }
}

int
coverage_dynamic_initialised(void)
{

  if (!geomview_stream_textures) {
    error("Geomview does not support streaming images.");
    return(FALSE);
  }

  if (!dynamic_errors_reported) {
#ifndef NO_ZLIB
    /* We can support compressed images. User may have set
     * geomview_compressed_images to FALSE with -uncompressed flag.
     */
    if(!geomview_compressed_images){
      error("texturemaps will be uncompressed.");
    }
#else
    /* We can't support compressed images. */
    geomview_compressed_images = FALSE;
    error("texturemaps will be uncompressed. Compile SaVi with zlib for compressed.");
#endif

    if (Image_Height < IMAGE_LARGE_HEIGHT) {
      error("dynamic texture mapping benefits from large coverage map (-large-map).");
    }
    dynamic_errors_reported = TRUE;
  }

  return(TRUE);
}

void
coverage_dynamic_cleanup(void)
{
  if (geomview_dynamic_coverage) {
    if (geomview_stream_textures) {
#ifndef NO_ZLIB
      free(image_compress_scratch);
#endif
    }
  }
}

void
reset_foreground(int projection, grid * g)
{
  if (foreground)
    new_foreground(projection, g, TRUE);
}

/*
 * get_image - Return pointer to the image structure.
 */
static Tk_PhotoImageBlock *
get_image(void)
{
  if (!image) {
    error("get_image: image not yet initialized.");
  }

  return image;
}

/*
 * update_display - Update the Tk coverage image, given a completed coverage grid.
 */
void
update_display(int projection, grid * g, int coverage_flag, int tracks_flag,
	       int project_tracks_flag)
{
  int map_repeat = 1;

  Tk_PhotoHandle handle;

  /*
   * Re-write the entire image, unless we are just displaying ground
   * tracks, and there's not a new projection, in which case we can
   * simply write the foreground (which includes ground tracks).
   */
  int fgOnly = !new_foreground(projection, g, FALSE) && !coverage_flag &&
	       !tracks_flag && !project_tracks_flag;
  if (fisheye_viewpoint_flag) {
    fisheye_to_foreground(projection,g);
  }
  grid_and_foreground_to_image(projection, g, fgOnly);

  handle = Tk_FindPhoto(interp, imagename);
  if (handle) {
    /* double width of image to tile bitmaps horizontally */
    if (coverage_bitmap_drawn) {
      map_repeat = 2;
    } else {
      /* only show the one blank map */
      Tk_PhotoBlank(handle);
    }
#if ( (TK_MAJOR_VERSION >= 8) && (TK_MINOR_VERSION >= 5) )
    /* changed for version 8.5 alpha/beta, November 2007 */
    Tk_PhotoPutBlock(interp, handle, get_image(), 0, 0, image->width*map_repeat,
		     image->height, TK_PHOTO_COMPOSITE_OVERLAY);
#elif ( (TK_MAJOR_VERSION >= 8) && (TK_MINOR_VERSION >= 4) )
   /* changed for version 8.4, September 2002. */
    Tk_PhotoPutBlock(handle, get_image(), 0, 0, image->width*map_repeat,
		     image->height, TK_PHOTO_COMPOSITE_OVERLAY);
#else
    Tk_PhotoPutBlock(handle, get_image(), 0, 0, image->width*map_repeat,
		     image->height);
#endif
  } else {
    error("coverage_display: bad image name.");
  }

  /* display coverage percentages */
  set_coverage(projection, g);
}

/*
 * fill interval-decay with high value meaning many intervals since coverage
 */
void
clear_interval(grid * g)
{
  fill_interval(g, NUM_COLORS);
}


/*
 * set_coverage - Display total percent coverage.
 */
static void
set_coverage(int projection, grid * g)
{
  sprintf(cmd, format1, total_coverage(projection, TRUE, g));
  tcl_script(cmd);
  sprintf(cmd, format2, mean_coverage(projection, g));
  tcl_script(cmd);
}

/*
 * overlay_bitmap - Takes a bitmap and lies it on top of our image array.
 * h,w = height, width of the image array.
 */
static int
overlay_bitmap(unsigned int h, unsigned int w, unsigned char *b,
	       const char filename[])
{
  FILE *f;
  int pbmtype, length;
  unsigned int image_size, width, height, i, j, c, mask;

  if (NULL == (f = fopen(filename, "r"))) {
    fprintf(stderr, "\nSaVi: couldn't read in %s", filename);
    return FALSE;
  }

  forward_over_comments(f);

  fscanf(f, "P%d", &pbmtype);
  forward_over_comments(f);

  fscanf(f, "%d", &length);
  width = length;
  forward_over_comments(f);

  fscanf(f, "%d", &length);
  height = length;

  /* skip one char */
  c = getc(f);

  if ((pbmtype == 4) && (width == w) && (height == h)) {
    image_size = h*w;
    for (i = 0; i < image_size; i += 8) {
      c = getc(f);
      mask = 0x80;
      for (j = 0; j < 8; j++) {
	if (c & mask) {
	  *(b++) = 1;
	} else {
	  *(b++) = 0;
	}
	mask = mask / 2;
      }
    }
  } else {
    fprintf(stderr, "\nSaVi: %s has unexpected pbm header format: P%d %d %d",
	    filename, pbmtype, width, height);
    fprintf(stderr, "\n      was expecting the pbm header format: P4 %d %d\n",
	    w, h);
    fclose(f);
    return FALSE;
  }

  fclose(f);
  return TRUE;
}

static void
draw_sin_map_edges(grid * g)
{
  SphericalCoordinates point;
  int left[2], right[2];
  unsigned int h = g->height;
  unsigned int w = g->width;
  unsigned int i;

  if (!sin_foreground) return;

  point.r = 1;
  point.theta = 0;
  point.phi = 0;

  for (i = 0; i < h; i++) {
    intensity_edges(left, right, &point, SINUSOIDAL, g);
    sin_foreground[(w * i + left[0])] = 1;
    sin_foreground[(w * i + right[0])] = 1;
    point.phi += PI / (h - 1);	/* Increment phi by one pixel */
  }
}

/*
 * reset_map_foreground - Copies fresh Earth outline to foreground.
 */
static int
reset_map_foreground(const unsigned char * map_pointer, const unsigned int map_size) {

  /*
   * Only copy map over if map foreground was created by loading into memory.
   */
  if (map_pointer) {
    memcpy(foreground, map_pointer, map_size);
    return TRUE;
  } else {
    /* to erase previously plotted satellites */
    memset(foreground, 0, map_size);
    return FALSE;
  }
}

/*
 * new_foreground - Initialize foreground intensity image, including earth outline.
 * Returns TRUE if foreground must be re-drawn, FALSE otherwise.
 */
static int
new_foreground(int projection, grid * g, unsigned int force)
{
  unsigned int image_size;

  if (!force && (projection == current_proj))
    return (FALSE);

  if (current_proj != projection) {
    current_proj = projection;
    clear_intensity(g);
    clear_interval(g);
  }

  image_size = g->height * g->width;

  switch (projection) {
  case UNPROJECTED:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE;
    coverage_bitmap_drawn = reset_map_foreground(unp_foreground, image_size);
    break;
  case UNPROJECTED_MASK:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE;
    coverage_bitmap_drawn = reset_map_foreground(unp_mask_foreground, image_size);
    break;
  case SINUSOIDAL:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE;
    coverage_bitmap_drawn = reset_map_foreground(sin_foreground, image_size);
    break;
  case SINUSOIDAL_90:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE_90W;
    coverage_bitmap_drawn = reset_map_foreground(sin_90_foreground, image_size);
    break;
  case SPHERICAL:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE;
    coverage_bitmap_drawn = reset_map_foreground(sph_foreground, image_size);
    break;
  case SPHERICAL_90:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE_90W;
    coverage_bitmap_drawn = reset_map_foreground(sph_90_foreground, image_size);
    break;
  case CYLINDRICAL:
  default:
    Longitude_Center_Line = LONGITUDE_CENTER_LINE;
    coverage_bitmap_drawn = reset_map_foreground(cyl_foreground, image_size);
  }

  return (TRUE);
}

/*
 * Copy foreground (land masses and ground tracks, if any) only onto image.
 * The routine re-draws an image pixel only for corresponding foreground array
 * elements which are non-zero.
 */
void
grid_and_foreground_to_image(int projection, grid * g, int fgOnly)
{
  int drawWidth, middleWidth, projection_supported, skip_map;
  unsigned int w, h, i, j, offset, ci;
  unsigned int data_value, map_threshold, send_coverage;
  unsigned int sinu_surround = FALSE;
  unsigned int sph_surround = FALSE;
  unsigned char *c;
  unsigned char *f = foreground;
  unsigned char fg, cross_fg, fill0, fill1, fill2;
  unsigned char *p = image->pixelPtr;
  unsigned char *q, *r;
  float fw, fw2, lat, width;

  /*
   * Do we need to draw a blank surround outside
   * the map?
   */
  if (!fgOnly) {
    if ((projection == SINUSOIDAL) || (projection == SINUSOIDAL_90)) {
      sinu_surround = TRUE;
    }
    if ((projection == SPHERICAL) || (projection == SPHERICAL_90)) {
      sph_surround = TRUE;
    }
  }

  projection_supported = (projection == CYLINDRICAL) ||
    (((projection == UNPROJECTED)||(projection == UNPROJECTED_MASK)) && geomview_stream_textures);

  send_coverage = geomview_flag && geomview_dynamic_texture_flag &&
                  texture_flag && projection_supported &&
                  earth_on_flag && earth_geom_exists;

   /*
    * If threshold is 0, we don't draw white background (0), but we
    * do draw map outline (1) above that. If 1, we will not draw
    * map outline. For 0/1, could have used || instead of MAX,
    * but MAX is clearer in intent.
    */

  skip_map = !map_flag || (projection == UNPROJECTED_MASK);
  
  map_threshold = MAX((send_coverage && !geomview_texture_with_map), skip_map);

  /* set to match Earth outline */
  fill0 = outline_colors[3];
  fill1 = outline_colors[4];
  fill2 = outline_colors[5];

  h = g->height;
  w = g->width;
  drawWidth = w;
  middleWidth = drawWidth/2;
  fw = (float) w;
  fw2 = fw * fw;

  for (i = 0; i < h; i++) {
    lat = 90.0 - 180.0 * i / h;
    if (sinu_surround) {
      width = (fw * cos(DEG_TO_RAD * lat));
      drawWidth = (int) (0.5 + (fw - width) / 2);
    }

    if (sph_surround) {
      width = fw/4 - sqrt(fw2/16-fw2*lat*lat/16/90/90);
      drawWidth = (int) width;
      middleWidth = w/2 - drawWidth;
    }

    offset = i * w;
    q = p;

    for (j = 0; j < w; j++, p += PIXELSIZE) {
      fg = *(f++);

      if (sinu_surround || sph_surround) {
	q = p; /* updating for cross wrap-around */
	if ((j <= drawWidth) || j >= (w - drawWidth)) {
	  p[0] = fill0;
	  p[1] = fill1;
	  p[2] = fill2;
	  continue;
	}
	if (sph_surround && (j > middleWidth) && (j < w - middleWidth)) {
	  p[0] = fill0;
	  p[1] = fill1;
	  p[2] = fill2;
	  continue;
	}
      }

      if (fg > map_threshold) {
	/* skipping Earth outline if map is to be sent */
	c = outline_colors + PIXELSIZE * fg;

	p[0] = c[0];
	p[1] = c[1];
	p[2] = c[2];

        /*
	 * Indicate sub-satellite point by drawing four arms of cross.
	 */
	if ((fg == GROUND_TRACKS_CI) || (fg == SPECIAL_GROUND_TRACKS_CI) || (fg == GROUND_LOCATION_CI)) {
	  cross_fg = fg + 1;

	  if (j > 0) {
	    /* draw left arm of cross */
	    *(f-2) = cross_fg; /* set retroactively for future updates. */
	    r = p - PIXELSIZE;
	    r[0] = c[0];
	    r[1] = c[1];
	    r[2] = c[2];
	  } else {
	    /* left arm wrap around - set pixel at j=w */
	    /* not well-matched to sinusoidal */
	    *(f+w-2) = cross_fg; /* ready to draw at end of this line */
	  }
	  if (i > 0) {
	    /* draw top arm of cross */
	    *(f-w-1) = cross_fg; /* set retroactively for future updates. */
	    r = p - w * PIXELSIZE;
	    r[0] = c[0];
	    r[1] = c[1];
	    r[2] = c[2];
	  }
	  if (j < w-1) {
	    /* ready to draw right arm of cross, next j iteration */
	    *f = cross_fg;
	  } else {
	    /* right arm wrap around - set pixel at j=0 i same value */
	    /* not well-matched to sinusoidal */
	    *(f-j) = cross_fg;
	    q[0] = c[0];
	    q[1] = c[1];
	    q[2] = c[2];
	  }
	  if (i < h-1) {
	    /* ready to draw bottom of cross, next i iteration/row pass */
	    *(f-1 + w) = cross_fg;
	  }
	}

	continue;
      }

      if (!fgOnly) {
	data_value = g->data[offset + j];

	if (projection == UNPROJECTED_MASK) {
	  ci = MIN(data_value + 1, NUM_COLORS);
	  if (*(f-1) == 0) {
              c = noaccess_colors + PIXELSIZE * ci;
	      if (!no_access_flag && (ci == 1)) {
	          c = outline_colors;
	      }

          } else {
            c = colors + PIXELSIZE * ci;
          }
	} else {
	  if (data_value || !(no_access_flag || sun_flag)) {
	    ci = MIN(data_value, NUM_COLORS);
	    c = colors + PIXELSIZE * ci;
	  } else {
	    ci = MIN(g->noaccess[offset + j], NUM_COLORS);
	    c = noaccess_colors + PIXELSIZE * ci;
	  }
	}

	p[0] = c[0];
	p[1] = c[1];
	p[2] = c[2];
      }
    }
  }

  if (send_coverage) {
    coverage_output_image();
  }

  if (projection == UNPROJECTED_MASK) {
    if (map_flag) {
      f = unp_foreground;
    } else {
      f = foreground;
      return; /* do not draw continents mask */
    }
  } else if (!map_flag) {
    /* map outline is not needed. */
    return;
  } else {
    f = foreground;
  }
  
  p = image->pixelPtr;

  /* map outline is 1 - cache colors for that from array */
  c = outline_colors + PIXELSIZE;
  fill0 = c[0];
  fill1 = c[1];
  fill2 = c[2];

  drawWidth = h*w;
  for (i = 0; i < drawWidth; i++, p+=PIXELSIZE) {
    fg = *(f++);
    if (fg == 1) {
      p[0] = fill0;
      p[1] = fill1;
      p[2] = fill2;
    }
  }
}

/*
 * tracks_to_foreground - place dot on the foreground map which corresponds
 * to the subsatellite point (nadir).  Do this for every satellite
 * in the list.
 */
void
tracks_to_foreground(const Satellite_list SL, int projection, grid * g,
		     unsigned int color_index, const CentralBody * pcb)
{
  unsigned int color;
  Satellite_list sl = SL;

  while (sl) {
    if (sl->s->tag) {
      /* inelegant choice of special color */
      color = SPECIAL_GROUND_TRACKS_CI;
    } else {
      color = color_index;
    }

    if (sl->s->can_display_satellite) {
      track_to_foreground(sl->s, projection, g, color, pcb);
    }

    sl = sl->next;
  }
}

/*
 * track_to_foreground - routine to increment the foreground map at the point
 * underneath the satellite.  For a single satellite.
 *
 * grid_index = color index to insert into the foreground array
 */
void
track_to_foreground(Satellite s, int projection, grid * g,
		    unsigned int color_index, const CentralBody * pcb)
{
  LatLon l;

  spherical_to_lat_lon(&l, &(s->x_S), s->t, pcb);
  latlon_to_foreground(&l, projection, g, color_index);
}

void
latlon_to_foreground(LatLon *pl, int projection, grid * g, unsigned int color_index)
{
  int coords[2];

  latlon_to_grid_index(coords, pl, projection, g);
  foreground[g->width * coords[1] + coords[0]] = color_index;
}

void
fisheye_to_foreground(int projection, grid * g)
{
  LatLon l;

  fisheye_location(&l);
  latlon_to_foreground(&l, projection, g, GROUND_LOCATION_CI);
}

/*
 * Given lat/lon coordinates of point on central body, return corresponding
 * index into grid.
 *        grid_index[0]=column index, grid_index[1]=row index.
 */
static void
latlon_to_grid_index(int grid_index[2], LatLon * pl, int projection, grid * g)
{
  double proj[2];
  LatLon l;

  /*
   * The vertical center-line of the grid corresponds to longitude =
   * Longitude_Center_Line, whereas both 2-d projections place longitude = 0
   * along this line.  So shift the input longitude as required.
   */
  l.lat = pl->lat;
  l.lon = pl->lon - Longitude_Center_Line;

  if (l.lon < -180.0) {
    l.lon += 360.0;
  } else if (l.lon > 180.0) {
    l.lon -= 360.0;
  }

  switch (projection) {
  case UNPROJECTED:
  case UNPROJECTED_MASK:
    project_latlon_unprojected(proj, &l);
    break;
  case SINUSOIDAL:
  case SINUSOIDAL_90:
    project_latlon_sinusoidal(proj, &l);
    break;
  case SPHERICAL:
  case SPHERICAL_90:
    project_latlon_spherical(proj, &l);
    break;
  case CYLINDRICAL:
  default:
    project_latlon_cylindrical(proj, &l);
  }

  /*
   * It's assumed that proj[0] ranges from -180 to 180, and
   *                   proj[1] ranges from -90 to 90,  no matter what.
   */
  grid_index[0] = ((int) (0.5 + (proj[0] + 180.0) * (g->width) / 360.0));
  grid_index[1] = ((int) (0.5 + proj[1] * (g->height - 1) / 180.0));
}
