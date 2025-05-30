
/* pngtest.c - a simple test program to test libpng
 *
 * libpng 1.1.0e - February 19, 2000
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
 * Copyright (c) 1996, 1997 Andreas Dilger
 * Copyright (c) 1998, 1999, 2000 Glenn Randers-Pehrson
 *
 * This program reads in a PNG image, writes it out again, and then
 * compares the two files.  If the files are identical, this shows that
 * the basic chunk handling, filtering, and (de)compression code is working
 * properly.  It does not currently test all of the transforms, although
 * it probably should.
 *
 * The program will report "FAIL" in certain legitimate cases:
 * 1) when the compression level or filter selection method is changed.
 * 2) when the maximum IDAT size (PNG_ZBUF_SIZE in pngconf.h) is not 8192.
 * 3) unknown ancillary chunks exist in the input file.
 * 4) others not listed here...
 * In these cases, it is best to check with another tool such as "pngcheck"
 * to see what the differences between the two files are.
 *
 * If a filename is given on the command-line, then this file is used
 * for the input, rather than the default "pngtest.png".  This allows
 * testing a wide variety of files easily.  You can also test a number
 * of files at once by typing "pngtest -m file1.png file2.png ..."
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Makes pngtest verbose so we can find problems (needs to be before png.h) */
#ifndef PNG_DEBUG
#define PNG_DEBUG 0
#endif

/* Turn on CPU timing
#define PNGTEST_TIMING
*/

#ifdef PNG_NO_FLOATING_POINT_SUPPORTED
#undef PNGTEST_TIMING
#endif

#ifdef PNGTEST_TIMING
static float t_start, t_stop, t_decode, t_encode, t_misc;
#include <time.h>
#endif

#include "png.h"

#ifdef PNGTEST_TIMING
static float t_start, t_stop, t_decode, t_encode, t_misc;
#if !defined(PNG_tIME_SUPPORTED)
#include <time.h>
#endif
#endif

#if defined(PNG_TIME_RFC1123_SUPPORTED)
static int tIME_chunk_present=0;
static char tIME_string[30] = "no tIME chunk present in file";
#endif

static int verbose = 0;

int test_one_file PNGARG((PNG_CONST char *inname, PNG_CONST char *outname));

#ifdef __TURBOC__
#include <mem.h>
#endif

/* defined so I can write to a file on gui/windowing platforms */
/*  #define STDERR stderr  */
#define STDERR stdout   /* for DOS */

/* example of using row callbacks to make a simple progress meter */
static int status_pass=1;
static int status_dots_requested=0;
static int status_dots=1;

void
read_row_callback(png_structp png_ptr, png_uint_32 row_number, int pass)
{
    if(png_ptr == NULL || row_number > PNG_MAX_UINT) return;
    if(status_pass != pass)
    {
       fprintf(stdout,"\n Pass %d: ",pass);
       status_pass = pass;
       status_dots = 31;
    }
    status_dots--;
    if(status_dots == 0)
    {
       fprintf(stdout, "\n         ");
       status_dots=30;
    }
    fprintf(stdout, "r");
}

void
write_row_callback(png_structp png_ptr, png_uint_32 row_number, int pass)
{
    if(png_ptr == NULL || row_number > PNG_MAX_UINT || pass > 7) return;
    fprintf(stdout, "w");
}


#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
/* Example of using user transform callback (we don't transform anything,
   but merely examine the row filters.  We set this to 256 rather than
   5 in case illegal filter values are present.) */
static png_uint_32 filters_used[256];
void
count_filters(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
    if(png_ptr != NULL && row_info != NULL)
      ++filters_used[*(data-1)];
}
#endif

#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
/* example of using user transform callback (we don't transform anything,
   but merely count the zero samples) */

static png_uint_32 zero_samples;

void
count_zero_samples(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
   png_bytep dp = data;
   if(png_ptr == NULL)return;

   /* contents of row_info:
    *  png_uint_32 width      width of row
    *  png_uint_32 rowbytes   number of bytes in row
    *  png_byte color_type    color type of pixels
    *  png_byte bit_depth     bit depth of samples
    *  png_byte channels      number of channels (1-4)
    *  png_byte pixel_depth   bits per pixel (depth*channels)
    */


    /* counts the number of zero samples (or zero pixels if color_type is 3 */

    if(row_info->color_type == 0 || row_info->color_type == 3)
    {
       int pos=0;
       png_uint_32 n, nstop;
       for (n=0, nstop=row_info->width; n<nstop; n++)
       {
          if(row_info->bit_depth == 1)
          {
             if(((*dp << pos++ ) & 0x80) == 0) zero_samples++;
             if(pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if(row_info->bit_depth == 2)
          {
             if(((*dp << (pos+=2)) & 0xc0) == 0) zero_samples++;
             if(pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if(row_info->bit_depth == 4)
          {
             if(((*dp << (pos+=4)) & 0xf0) == 0) zero_samples++;
             if(pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if(row_info->bit_depth == 8)
             if(*dp++ == 0) zero_samples++;
          if(row_info->bit_depth == 16)
          {
             if((*dp | *(dp+1)) == 0) zero_samples++;
             dp+=2;
          }
       }
    }
    else /* other color types */
    {
       png_uint_32 n, nstop;
       int channel;
       int color_channels = row_info->channels;
       if(row_info->color_type > 3)color_channels--;

       for (n=0, nstop=row_info->width; n<nstop; n++)
       {
          for (channel = 0; channel < color_channels; channel++)
          {
             if(row_info->bit_depth == 8)
                if(*dp++ == 0) zero_samples++;
             if(row_info->bit_depth == 16)
             {
                if((*dp | *(dp+1)) == 0) zero_samples++;
                dp+=2;
             }
          }
          if(row_info->color_type > 3)
          {
             dp++;
             if(row_info->bit_depth == 16)dp++;
          }
       }
    }
}
#endif /* PNG_WRITE_USER_TRANSFORM_SUPPORTED */

static int wrote_question = 0;

#if defined(PNG_NO_STDIO)
/* START of code to validate stdio-free compilation */
/* These copies of the default read/write functions come from pngrio.c and */
/* pngwio.c.  They allow "don't include stdio" testing of the library. */
/* This is the function that does the actual reading of data.  If you are
   not reading from a standard C stream, you should create a replacement
   read_data function and use it at run time with png_set_read_fn(), rather
   than changing the library. */
#ifndef USE_FAR_KEYWORD
static void
png_default_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_size_t check;

   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   check = (png_size_t)fread(data, (png_size_t)1, length,
      (FILE *)png_ptr->io_ptr);

   if (check != length)
   {
      png_error(png_ptr, "Read Error");
   }
}
#else
/* this is the model-independent version. Since the standard I/O library
   can't handle far buffers in the medium and small models, we have to copy
   the data.
*/

#define NEAR_BUF_SIZE 1024
#define MIN(a,b) (a <= b ? a : b)

static void
png_default_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   int check;
   png_byte *n_data;
   FILE *io_ptr;

   /* Check if data really is near. If so, use usual code. */
   n_data = (png_byte *)CVT_PTR_NOCHECK(data);
   io_ptr = (FILE *)CVT_PTR(png_ptr->io_ptr);
   if ((png_bytep)n_data == data)
   {
      check = fread(n_data, 1, length, io_ptr);
   }
   else
   {
      png_byte buf[NEAR_BUF_SIZE];
      png_size_t read, remaining, err;
      check = 0;
      remaining = length;
      do
      {
         read = MIN(NEAR_BUF_SIZE, remaining);
         err = fread(buf, (png_size_t)1, read, io_ptr);
         png_memcpy(data, buf, read); /* copy far buffer to near buffer */
         if(err != read)
            break;
         else
            check += err;
         data += read;
         remaining -= read;
      }
      while (remaining != 0);
   }
   if (check != length)
   {
      png_error(png_ptr, "read Error");
   }
}
#endif /* USE_FAR_KEYWORD */

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
static void
png_default_flush(png_structp png_ptr)
{
   FILE *io_ptr;
   io_ptr = (FILE *)CVT_PTR((png_ptr->io_ptr));
   if (io_ptr != NULL)
      fflush(io_ptr);
}
#endif

/* This is the function that does the actual writing of data.  If you are
   not writing to a standard C stream, you should create a replacement
   write_data function and use it at run time with png_set_write_fn(), rather
   than changing the library. */
#ifndef USE_FAR_KEYWORD
static void
png_default_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_uint_32 check;

   check = fwrite(data, 1, length, (FILE *)(png_ptr->io_ptr));
   if (check != length)
   {
      png_error(png_ptr, "Write Error");
   }
}
#else
/* this is the model-independent version. Since the standard I/O library
   can't handle far buffers in the medium and small models, we have to copy
   the data.
*/

#define NEAR_BUF_SIZE 1024
#define MIN(a,b) (a <= b ? a : b)

static void
png_default_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_uint_32 check;
   png_byte *near_data;  /* Needs to be "png_byte *" instead of "png_bytep" */
   FILE *io_ptr;

   /* Check if data really is near. If so, use usual code. */
   near_data = (png_byte *)CVT_PTR_NOCHECK(data);
   io_ptr = (FILE *)CVT_PTR(png_ptr->io_ptr);
   if ((png_bytep)near_data == data)
   {
      check = fwrite(near_data, 1, length, io_ptr);
   }
   else
   {
      png_byte buf[NEAR_BUF_SIZE];
      png_size_t written, remaining, err;
      check = 0;
      remaining = length;
      do
      {
         written = MIN(NEAR_BUF_SIZE, remaining);
         png_memcpy(buf, data, written); /* copy far buffer to near buffer */
         err = fwrite(buf, 1, written, io_ptr);
         if (err != written)
            break;
         else
            check += err;
         data += written;
         remaining -= written;
      }
      while (remaining != 0);
   }
   if (check != length)
   {
      png_error(png_ptr, "Write Error");
   }
}

#endif /* USE_FAR_KEYWORD */

/* This function is called when there is a warning, but the library thinks
 * it can continue anyway.  Replacement functions don't have to do anything
 * here if you don't want to.  In the default configuration, png_ptr is
 * not used, but it is passed in case it may be useful.
 */
static void
png_default_warning(png_structp png_ptr, png_const_charp message)
{
   PNG_CONST char *name = "UNKNOWN (ERROR!)";
   if (png_ptr != NULL && png_ptr->error_ptr != NULL)
      name = png_ptr->error_ptr;
   fprintf(STDERR, "%s: libpng warning: %s\n", name, message);
}

/* This is the default error handling function.  Note that replacements for
 * this function MUST NOT RETURN, or the program will likely crash.  This
 * function is used by default, or if the program supplies NULL for the
 * error function pointer in png_set_error_fn().
 */
static void
png_default_error(png_structp png_ptr, png_const_charp message)
{
   png_default_warning(png_ptr, message);
   /* We can return because png_error calls the default handler, which is
    * actually OK in this case. */
}
#endif /* PNG_NO_STDIO */
/* END of code to validate stdio-free compilation */

/* START of code to validate memory allocation and deallocation */
#ifdef PNG_USER_MEM_SUPPORTED

/* Allocate memory.  For reasonable files, size should never exceed
   64K.  However, zlib may allocate more then 64K if you don't tell
   it not to.  See zconf.h and png.h for more information.  zlib does
   need to allocate exactly 64K, so whatever you call here must
   have the ability to do that.

   This piece of code can be compiled to validate max 64K allocations
   by setting MAXSEG_64K in zlib zconf.h *or* PNG_MAX_MALLOC_64K. */
typedef struct memory_information
{
   png_uint_32                    size;
   png_voidp                 pointer;
   struct memory_information FAR *next;
} memory_information;
typedef memory_information FAR *memory_infop;

static memory_infop pinformation = NULL;
static int current_allocation = 0;
static int maximum_allocation = 0;

extern PNG_EXPORT(png_voidp,png_debug_malloc) PNGARG((png_structp png_ptr,
   png_uint_32 size));
extern PNG_EXPORT(void,png_debug_free) PNGARG((png_structp png_ptr,
   png_voidp ptr));

png_voidp
png_debug_malloc(png_structp png_ptr, png_uint_32 size)
{

   /* png_malloc has already tested for NULL; png_create_struct calls
      png_debug_malloc directly, with png_ptr == NULL which is OK */

   if (size == 0)
      return (png_voidp)(NULL);

   /* This calls the library allocator twice, once to get the requested
      buffer and once to get a new free list entry. */
   {
      memory_infop pinfo = png_malloc_default(png_ptr, sizeof *pinfo);
      pinfo->size = size;
      current_allocation += size;
      if (current_allocation > maximum_allocation)
         maximum_allocation = current_allocation;
      pinfo->pointer = png_malloc_default(png_ptr, size);
      pinfo->next = pinformation;
      pinformation = pinfo;
      /* Make sure the caller isn't assuming zeroed memory. */
      png_memset(pinfo->pointer, 0xdd, pinfo->size);
#if PNG_DEBUG
      if(verbose)
         printf("png_malloc %d bytes at %x\n",size,pinfo->pointer);
#endif
      assert(pinfo->size != 12345);
      return (png_voidp)(pinfo->pointer);
   }
}

/* Free a pointer.  It is removed from the list at the same time. */
void
png_debug_free(png_structp png_ptr, png_voidp ptr)
{
   if (png_ptr == NULL)
      fprintf(STDERR, "NULL pointer to png_debug_free.\n");
   if (ptr == 0)
   {
#if 0 /* This happens all the time. */
      fprintf(STDERR, "WARNING: freeing NULL pointer\n");
#endif
      return;
   }

   /* Unlink the element from the list. */
   {
      memory_infop FAR *ppinfo = &pinformation;
      for (;;)
      {
         memory_infop pinfo = *ppinfo;
         if (pinfo->pointer == ptr)
         {
            *ppinfo = pinfo->next;
            current_allocation -= pinfo->size;
            if (current_allocation < 0)
               fprintf(STDERR, "Duplicate free of memory\n");
            /* We must free the list element too, but first kill
               the memory that is to be freed. */
            memset(ptr, 0x55, pinfo->size);
            png_free_default(png_ptr, pinfo);
            break;
         }
         if (pinfo->next == NULL)
         {
            fprintf(STDERR, "Pointer %x not found\n", ptr);
            break;
         }
         ppinfo = &pinfo->next;
      }
   }

   /* Finally free the data. */
#if PNG_DEBUG
   if(verbose)
      printf("Freeing %x\n",ptr);
#endif
   png_free_default(png_ptr, ptr);
}
#endif /* PNG_USER_MEM_SUPPORTED */
/* END of code to test memory allocation/deallocation */

/* Test one file */
int
test_one_file(PNG_CONST char *inname, PNG_CONST char *outname)
{
   static FILE *fpin, *fpout;  /* "static" prevents setjmp corruption */
   png_structp read_ptr, write_ptr;
   png_infop read_info_ptr, write_info_ptr, end_info_ptr, write_end_info_ptr;
   png_bytep row_buf;
   png_uint_32 y;
   png_uint_32 width, height;
   int num_pass, pass;
   int bit_depth, color_type;
#ifdef PNG_SETJMP_SUPPORTED
#ifdef USE_FAR_KEYWORD
   jmp_buf jmp_env;
#endif
#endif

   char inbuf[256], outbuf[256];

   row_buf = (png_bytep)NULL;

   if ((fpin = fopen(inname, "rb")) == NULL)
   {
      fprintf(STDERR, "Could not find input file %s\n", inname);
      return (1);
   }

   if ((fpout = fopen(outname, "wb")) == NULL)
   {
      fprintf(STDERR, "Could not open output file %s\n", outname);
      fclose(fpin);
      return (1);
   }

   png_debug(0, "Allocating read and write structures\n");
#ifdef PNG_USER_MEM_SUPPORTED
   read_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL, (png_voidp)NULL,
      (png_malloc_ptr)png_debug_malloc, (png_free_ptr)png_debug_free);
#else
   read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);
#endif
#if defined(PNG_NO_STDIO)
   png_set_error_fn(read_ptr, (png_voidp)inname, png_default_error,
       png_default_warning);
#endif
#ifdef PNG_USER_MEM_SUPPORTED
   write_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL, (png_voidp)NULL,
      (png_malloc_ptr)png_debug_malloc, (png_free_ptr)png_debug_free);
#else
   write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);
#endif
#if defined(PNG_NO_STDIO)
   png_set_error_fn(write_ptr, (png_voidp)inname, png_default_error,
       png_default_warning);
#endif
   png_debug(0, "Allocating read_info, write_info and end_info structures\n");
   read_info_ptr = png_create_info_struct(read_ptr);
   write_info_ptr = png_create_info_struct(write_ptr);
   end_info_ptr = png_create_info_struct(read_ptr);
   write_end_info_ptr = png_create_info_struct(write_ptr);
#ifdef PNG_USER_MEM_SUPPORTED
#endif

#ifdef PNG_SETJMP_SUPPORTED
   png_debug(0, "Setting jmp_env for read struct\n");
#ifdef USE_FAR_KEYWORD
   if (setjmp(jmp_env))
#else
   if (setjmp(png_jmp_env(read_ptr)))
#endif
   {
      fprintf(STDERR, "%s -> %s: libpng read error\n", inname, outname);
      png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);
      png_destroy_info_struct(write_ptr, &write_end_info_ptr);
      png_destroy_write_struct(&write_ptr, &write_info_ptr);
      fclose(fpin);
      fclose(fpout);
      return (1);
   }
#ifdef USE_FAR_KEYWORD
   png_memcpy(png_jmp_env(read_ptr),jmp_env,sizeof(jmp_buf));
#endif

   png_debug(0, "Setting jmp_env for write struct\n");
#ifdef USE_FAR_KEYWORD
   if (setjmp(jmp_env))
#else
   if (setjmp(png_jmp_env(write_ptr)))
#endif
   {
      fprintf(STDERR, "%s -> %s: libpng write error\n", inname, outname);
      png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);
      png_destroy_info_struct(write_ptr, &write_end_info_ptr);
      png_destroy_write_struct(&write_ptr, &write_info_ptr);
      fclose(fpin);
      fclose(fpout);
      return (1);
   }
#ifdef USE_FAR_KEYWORD
   png_memcpy(png_jmp_env(write_ptr),jmp_env,sizeof(jmp_buf));
#endif
#endif

   png_debug(0, "Initializing input and output streams\n");
#if !defined(PNG_NO_STDIO)
   png_init_io(read_ptr, fpin);
   png_init_io(write_ptr, fpout);
#else
   png_set_read_fn(read_ptr, (png_voidp)fpin, png_default_read_data);
   png_set_write_fn(write_ptr, (png_voidp)fpout,  png_default_write_data,
#if defined(PNG_WRITE_FLUSH_SUPPORTED)
      png_default_flush);
#else
      NULL);
#endif
#endif
   if(status_dots_requested == 1)
   {
      png_set_write_status_fn(write_ptr, write_row_callback);
      png_set_read_status_fn(read_ptr, read_row_callback);
   }
   else
   {
      png_set_write_status_fn(write_ptr, NULL);
      png_set_read_status_fn(read_ptr, NULL);
   }

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
   {
     int i;
     for(i=0; i<256; i++)
        filters_used[i]=0;
     png_set_read_user_transform_fn(read_ptr, count_filters);
   }
#endif
#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
   zero_samples=0;
   png_set_write_user_transform_fn(write_ptr, count_zero_samples);
#endif

#define HANDLE_CHUNK_IF_SAFE      2
#define HANDLE_CHUNK_ALWAYS       3
#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
   png_set_keep_unknown_chunks(read_ptr, HANDLE_CHUNK_ALWAYS, NULL, 0);
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   png_set_keep_unknown_chunks(write_ptr, HANDLE_CHUNK_IF_SAFE, NULL, 0);
#endif

   png_debug(0, "Reading info struct\n");
   png_read_info(read_ptr, read_info_ptr);

   png_debug(0, "Transferring info struct\n");
   {
      int interlace_type, compression_type, filter_type;

      if (png_get_IHDR(read_ptr, read_info_ptr, &width, &height, &bit_depth,
          &color_type, &interlace_type, &compression_type, &filter_type))
      {
         png_set_IHDR(write_ptr, write_info_ptr, width, height, bit_depth,
#if defined(PNG_WRITE_INTERLACING_SUPPORTED)
            color_type, interlace_type, compression_type, filter_type);
#else
            color_type, PNG_INTERLACE_NONE, compression_type, filter_type);
#endif
      }
   }
#if defined(PNG_cHRM_SUPPORTED)
   {
      png_fixed_point white_x, white_y, red_x, red_y, green_x, green_y, blue_x,
         blue_y;

      if (png_get_cHRM_fixed(read_ptr, read_info_ptr, &white_x, &white_y, &red_x,
         &red_y, &green_x, &green_y, &blue_x, &blue_y))
      {
         png_set_cHRM_fixed(write_ptr, write_info_ptr, white_x, white_y, red_x,
            red_y, green_x, green_y, blue_x, blue_y);
      }
   }
#endif
#if defined(PNG_gAMA_SUPPORTED)
   {
      png_fixed_point gamma;

      if (png_get_gAMA_fixed(read_ptr, read_info_ptr, &gamma))
      {
         png_set_gAMA_fixed(write_ptr, write_info_ptr, gamma);
      }
   }
#endif
#if defined(PNG_iCCP_SUPPORTED)
   {
      png_charp name;
      png_charp profile;
      png_uint_32 proflen;
      int compression_type;

      if (png_get_iCCP(read_ptr, read_info_ptr, &name, &compression_type,
                      &profile, &proflen))
      {
         png_set_iCCP(write_ptr, write_info_ptr, name, compression_type,
                      profile, proflen);
      }
   }
#endif
#if defined(PNG_sRGB_SUPPORTED)
   {
      int intent;

      if (png_get_sRGB(read_ptr, read_info_ptr, &intent))
      {
         png_set_sRGB(write_ptr, write_info_ptr, intent);
      }
   }
#endif
   {
      png_colorp palette;
      int num_palette;

      if (png_get_PLTE(read_ptr, read_info_ptr, &palette, &num_palette))
      {
         png_set_PLTE(write_ptr, write_info_ptr, palette, num_palette);
      }
   }
#if defined(PNG_bKGD_SUPPORTED)
   {
      png_color_16p background;

      if (png_get_bKGD(read_ptr, read_info_ptr, &background))
      {
         png_set_bKGD(write_ptr, write_info_ptr, background);
      }
   }
#endif
#if defined(PNG_hIST_SUPPORTED)
   {
      png_uint_16p hist;

      if (png_get_hIST(read_ptr, read_info_ptr, &hist))
      {
         png_set_hIST(write_ptr, write_info_ptr, hist);
      }
   }
#endif
#if defined(PNG_oFFs_SUPPORTED)
   {
      png_int_32 offset_x, offset_y;
      int unit_type;

      if (png_get_oFFs(read_ptr, read_info_ptr,&offset_x,&offset_y,&unit_type))
      {
         png_set_oFFs(write_ptr, write_info_ptr, offset_x, offset_y, unit_type);
      }
   }
#endif
#if defined(PNG_pCAL_SUPPORTED)
   {
      png_charp purpose, units;
      png_charpp params;
      png_int_32 X0, X1;
      int type, nparams;

      if (png_get_pCAL(read_ptr, read_info_ptr, &purpose, &X0, &X1, &type,
         &nparams, &units, &params))
      {
         png_set_pCAL(write_ptr, write_info_ptr, purpose, X0, X1, type,
            nparams, units, params);
      }
   }
#endif
#if defined(PNG_pHYs_SUPPORTED)
   {
      png_uint_32 res_x, res_y;
      int unit_type;

      if (png_get_pHYs(read_ptr, read_info_ptr, &res_x, &res_y, &unit_type))
      {
         png_set_pHYs(write_ptr, write_info_ptr, res_x, res_y, unit_type);
      }
   }
#endif
#if defined(PNG_sBIT_SUPPORTED)
   {
      png_color_8p sig_bit;

      if (png_get_sBIT(read_ptr, read_info_ptr, &sig_bit))
      {
         png_set_sBIT(write_ptr, write_info_ptr, sig_bit);
      }
   }
#endif
#if defined(PNG_sCAL_SUPPORTED)
#ifdef PNG_FLOATING_POINT_SUPPORTED
   {
      int unit;
      double width, height;

      if (png_get_sCAL(read_ptr, read_info_ptr, &unit, &width, &height))
      {
         png_set_sCAL(write_ptr, write_info_ptr, unit, width, height);
      }
   }
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   {
      int unit;
      png_charp width, height;

      if (png_get_sCAL_s(read_ptr, read_info_ptr, &unit, &width, &height))
      {
         png_set_sCAL_s(write_ptr, write_info_ptr, unit, width, height);
      }
   }
#endif
#endif
#endif
#if defined(PNG_TEXT_SUPPORTED)
   {
      png_textp text_ptr;
      int num_text;

      if (png_get_text(read_ptr, read_info_ptr, &text_ptr, &num_text) > 0)
      {
         png_debug1(0, "Handling %d iTXt/tEXt/zTXt chunks\n", num_text);
         png_set_text(write_ptr, write_info_ptr, text_ptr, num_text);
      }
   }
#endif
#if defined(PNG_tIME_SUPPORTED)
   {
      png_timep mod_time;

      if (png_get_tIME(read_ptr, read_info_ptr, &mod_time))
      {
         png_set_tIME(write_ptr, write_info_ptr, mod_time);
#if defined(PNG_TIME_RFC1123_SUPPORTED)
         /* we have to use png_strcpy instead of "=" because the string
            pointed to by png_convert_to_rfc1123() gets free'ed before
            we use it */
         png_strcpy(tIME_string,png_convert_to_rfc1123(read_ptr, mod_time));
         tIME_chunk_present++;
#endif /* PNG_TIME_RFC1123_SUPPORTED */
      }
   }
#endif
#if defined(PNG_tRNS_SUPPORTED)
   {
      png_bytep trans;
      int num_trans;
      png_color_16p trans_values;

      if (png_get_tRNS(read_ptr, read_info_ptr, &trans, &num_trans,
         &trans_values))
      {
         png_set_tRNS(write_ptr, write_info_ptr, trans, num_trans,
            trans_values);
      }
   }
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   {
      png_unknown_chunkp unknowns;
      int num_unknowns = (int)png_get_unknown_chunks(read_ptr, read_info_ptr,
         &unknowns);
      if (num_unknowns)
      {
         png_size_t i;
         png_set_unknown_chunks(write_ptr, write_info_ptr, unknowns,
           num_unknowns);
         /* copy the locations from the read_info_ptr.  The automatically
            generated locations in write_info_ptr are wrong because we
            haven't written anything yet */
         for (i = 0; i < (png_size_t)num_unknowns; i++)
           write_info_ptr->unknown_chunks[i].location = unknowns[i].location;
      }
   }
#endif

   png_debug(0, "\nWriting info struct\n");

/* If we wanted, we could write info in two steps:
   png_write_info_before_PLTE(write_ptr, write_info_ptr);
 */
   png_write_info(write_ptr, write_info_ptr);

   png_debug(0, "\nAllocating row buffer \n");
   row_buf = (png_bytep)png_malloc(read_ptr,
      png_get_rowbytes(read_ptr, read_info_ptr));
   if (row_buf == NULL)
   {
      fprintf(STDERR, "No memory to allocate row buffer\n");
      png_destroy_read_struct(&read_ptr, &read_info_ptr, (png_infopp)NULL);
      png_destroy_info_struct(write_ptr, &write_end_info_ptr);
      png_destroy_write_struct(&write_ptr, &write_info_ptr);
      fclose(fpin);
      fclose(fpout);
      return (1);
   }
   png_debug(0, "Writing row data\n");

#if defined(PNG_READ_INTERLACING_SUPPORTED) || \
  defined(PNG_WRITE_INTERLACING_SUPPORTED)
   num_pass = png_set_interlace_handling(read_ptr);
   png_set_interlace_handling(write_ptr);
#else
   num_pass=1;
#endif

#ifdef PNGTEST_TIMING
   t_stop = (float)clock();
   t_misc += (t_stop - t_start);
   t_start = t_stop;
#endif
   for (pass = 0; pass < num_pass; pass++)
   {
      png_debug1(0, "Writing row data for pass %d\n",pass);
      for (y = 0; y < height; y++)
      {
         png_read_rows(read_ptr, (png_bytepp)&row_buf, (png_bytepp)NULL, 1);
#ifdef PNGTEST_TIMING
         t_stop = (float)clock();
         t_decode += (t_stop - t_start);
         t_start = t_stop;
#endif
         png_write_rows(write_ptr, (png_bytepp)&row_buf, 1);
#ifdef PNGTEST_TIMING
         t_stop = (float)clock();
         t_encode += (t_stop - t_start);
         t_start = t_stop;
#endif
      }
   }

#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
   png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   png_free_data(write_ptr, write_info_ptr, PNG_FREE_UNKN, -1);
#endif

   png_debug(0, "Reading and writing end_info data\n");

   png_read_end(read_ptr, end_info_ptr);
#if defined(PNG_TEXT_SUPPORTED)
   {
      png_textp text_ptr;
      int num_text;

      if (png_get_text(read_ptr, end_info_ptr, &text_ptr, &num_text) > 0)
      {
         png_debug1(0, "Handling %d iTXt/tEXt/zTXt chunks\n", num_text);
         png_set_text(write_ptr, write_end_info_ptr, text_ptr, num_text);
      }
   }
#endif
#if defined(PNG_tIME_SUPPORTED)
   {
      png_timep mod_time;

      if (png_get_tIME(read_ptr, end_info_ptr, &mod_time))
      {
         png_set_tIME(write_ptr, write_end_info_ptr, mod_time);
#if defined(PNG_TIME_RFC1123_SUPPORTED)
         /* we have to use png_strcpy instead of "=" because the string
            pointed to by png_convert_to_rfc1123() gets free'ed before
            we use it */
         png_strcpy(tIME_string,png_convert_to_rfc1123(read_ptr, mod_time));
         tIME_chunk_present++;
#endif /* PNG_TIME_RFC1123_SUPPORTED */
      }
   }
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   {
      png_unknown_chunkp unknowns;
      int num_unknowns;
      num_unknowns = (int)png_get_unknown_chunks(read_ptr, end_info_ptr,
         &unknowns);
      if (num_unknowns)
      {
         png_size_t i;
         png_set_unknown_chunks(write_ptr, write_end_info_ptr, unknowns,
           num_unknowns);
         /* copy the locations from the read_info_ptr.  The automatically
            generated locations in write_end_info_ptr are wrong because we
            haven't written the end_info yet */
         for (i = 0; i < (png_size_t)num_unknowns; i++)
           write_end_info_ptr->unknown_chunks[i].location =
              unknowns[i].location;
      }
   }
#endif
   png_write_end(write_ptr, write_end_info_ptr);

#ifdef PNG_EASY_ACCESS_SUPPORTED
   if(verbose)
   {
      png_uint_32 iwidth, iheight;
      iwidth = png_get_image_width(write_ptr, write_info_ptr);
      iheight = png_get_image_height(write_ptr, write_info_ptr);
      fprintf(STDERR, "Image width = %lu, height = %lu\n",
         iwidth, iheight);
   }
#endif

   png_debug(0, "Destroying data structs\n");
   png_free(read_ptr, row_buf);
   png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);
   png_destroy_info_struct(write_ptr, &write_end_info_ptr);
   png_destroy_write_struct(&write_ptr, &write_info_ptr);

   fclose(fpin);
   fclose(fpout);

   png_debug(0, "Opening files for comparison\n");
   if ((fpin = fopen(inname, "rb")) == NULL)
   {
      fprintf(STDERR, "Could not find file %s\n", inname);
      return (1);
   }

   if ((fpout = fopen(outname, "rb")) == NULL)
   {
      fprintf(STDERR, "Could not find file %s\n", outname);
      fclose(fpin);
      return (1);
   }

   for(;;)
   {
      png_size_t num_in, num_out;

      num_in = fread(inbuf, 1, 1, fpin);
      num_out = fread(outbuf, 1, 1, fpout);

      if (num_in != num_out)
      {
         fprintf(STDERR, "\nFiles %s and %s are of a different size\n",
                 inname, outname);
         if(wrote_question == 0)
         {
            fprintf(STDERR,
         "   Was %s written with the same maximum IDAT chunk size (%d bytes),",
              inname,PNG_ZBUF_SIZE);
            fprintf(STDERR,
              "\n   filtering heuristic (libpng default), compression");
            fprintf(STDERR,
              " level (zlib default),\n   and zlib version (%s)?\n\n",
              ZLIB_VERSION);
            wrote_question=1;
         }
         fclose(fpin);
         fclose(fpout);
         return (0);
      }

      if (!num_in)
         break;

      if (png_memcmp(inbuf, outbuf, num_in))
      {
         fprintf(STDERR, "\nFiles %s and %s are different\n", inname, outname);
         if(wrote_question == 0)
         {
            fprintf(STDERR,
         "   Was %s written with the same maximum IDAT chunk size (%d bytes),",
                 inname,PNG_ZBUF_SIZE);
            fprintf(STDERR,
              "\n   filtering heuristic (libpng default), compression");
            fprintf(STDERR,
              " level (zlib default),\n   and zlib version (%s)?\n\n",
              ZLIB_VERSION);
            wrote_question=1;
         }
         fclose(fpin);
         fclose(fpout);
         return (0);
      }
   }

   fclose(fpin);
   fclose(fpout);

   return (0);
}

/* input and output filenames */
#ifdef RISCOS
static PNG_CONST char *inname = "pngtest/png";
static PNG_CONST char *outname = "pngout/png";
#else
static PNG_CONST char *inname = "pngtest.png";
static PNG_CONST char *outname = "pngout.png";
#endif

int
main(int argc, char *argv[])
{
   int multiple = 0;
   int ierror = 0;

   fprintf(STDERR, "Testing libpng version %s\n", PNG_LIBPNG_VER_STRING);
   fprintf(STDERR, "   with zlib   version %s\n", ZLIB_VERSION);
   fprintf(STDERR,"%s",png_get_copyright(NULL));
   /* Show the version of libpng used in building the library */
   fprintf(STDERR," library:%s",png_get_header_version(NULL));
   /* Show the version of libpng used in building the application */
   fprintf(STDERR," pngtest:%s",PNG_HEADER_VERSION_STRING);

   /* Do some consistency checking on the memory allocation settings, I'm
      not sure this matters, but it is nice to know, the first of these
      tests should be impossible because of the way the macros are set
      in pngconf.h */
#if defined(MAXSEG_64K) && !defined(PNG_MAX_MALLOC_64K)
      fprintf(STDERR, " NOTE: Zlib compiled for max 64k, libpng not\n");
#endif
   /* I think the following can happen. */
#if !defined(MAXSEG_64K) && defined(PNG_MAX_MALLOC_64K)
      fprintf(STDERR, " NOTE: libpng compiled for max 64k, zlib not\n");
#endif

   if (strcmp(png_libpng_ver, PNG_LIBPNG_VER_STRING))
   {
      fprintf(STDERR,
         "Warning: versions are different between png.h and png.c\n");
      fprintf(STDERR, "  png.h version: %s\n", PNG_LIBPNG_VER_STRING);
      fprintf(STDERR, "  png.c version: %s\n\n", png_libpng_ver);
      ++ierror;
   }

   if (argc > 1)
   {
      if (strcmp(argv[1], "-m") == 0)
      {
         multiple = 1;
         status_dots_requested = 0;
      }
      else if (strcmp(argv[1], "-mv") == 0 ||
               strcmp(argv[1], "-vm") == 0 )
      {
         multiple = 1;
         verbose = 1;
         status_dots_requested = 1;
      }
      else if (strcmp(argv[1], "-v") == 0)
      {
         verbose = 1;
         status_dots_requested = 1;
         inname = argv[2];
      }
      else
      {
         inname = argv[1];
         status_dots_requested = 0;
      }
   }

   if (!multiple && argc == 3+verbose)
     outname = argv[2+verbose];

   if ((!multiple && argc > 3+verbose) || (multiple && argc < 2))
   {
     fprintf(STDERR,
       "usage: %s [infile.png] [outfile.png]\n\t%s -m {infile.png}\n",
        argv[0], argv[0]);
     fprintf(STDERR,
       "  reads/writes one PNG file (without -m) or multiple files (-m)\n");
     fprintf(STDERR,
       "  with -m %s is used as a temporary file\n", outname);
     exit(1);
   }

   if (multiple)
   {
      int i;
#ifdef PNG_USER_MEM_SUPPORTED
      int allocation_now = current_allocation;
#endif
      for (i=2; i<argc; ++i)
      {
         int k, kerror;
         fprintf(STDERR, "Testing %s:",argv[i]);
         kerror = test_one_file(argv[i], outname);
         if (kerror == 0)
         {
#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
            fprintf(STDERR, "\n PASS (%lu zero samples)\n",zero_samples);
#else
            fprintf(STDERR, " PASS\n");
#endif
#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
            for (k=0; k<256; k++)
               if(filters_used[k])
                  fprintf(STDERR, " Filter %d was used %lu times\n",
                     k,filters_used[k]);
#endif
#if defined(PNG_TIME_RFC1123_SUPPORTED)
         if(tIME_chunk_present != 0)
            fprintf(STDERR, " tIME = %s\n",tIME_string);
         tIME_chunk_present = 0;
#endif /* PNG_TIME_RFC1123_SUPPORTED */
         }
         else
         {
            fprintf(STDERR, " FAIL\n");
            ierror += kerror;
         }
#ifdef PNG_USER_MEM_SUPPORTED
         if (allocation_now != current_allocation)
            fprintf(STDERR, "MEMORY ERROR: %d bytes lost\n",
               current_allocation-allocation_now);
         if (current_allocation != 0)
         {
            memory_infop pinfo = pinformation;

            fprintf(STDERR, "MEMORY ERROR: %d bytes still allocated\n",
               current_allocation);
            while (pinfo != NULL)
            {
               fprintf(STDERR, " %d bytes at %x\n", pinfo->size, pinfo->pointer);
               pinfo = pinfo->next;
            }
         }
#endif
      }
#ifdef PNG_USER_MEM_SUPPORTED
         fprintf(STDERR, " Current memory allocation: %d bytes\n",
            current_allocation);
         fprintf(STDERR, " Maximum memory allocation: %d bytes\n",
            maximum_allocation);
#endif
   }
   else
   {
      int i;
      for (i=0; i<3; ++i)
      {
         int kerror;
#ifdef PNG_USER_MEM_SUPPORTED
         int allocation_now = current_allocation;
#endif
         if (i == 1) status_dots_requested = 1;
         else if(verbose == 0)status_dots_requested = 0;
         if (i == 0 || verbose == 1 || ierror != 0)
            fprintf(STDERR, "Testing %s:",inname);
         kerror = test_one_file(inname, outname);
         if(kerror == 0)
         {
            if(verbose == 1 || i == 2)
            {
                int k;
#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
                fprintf(STDERR, "\n PASS (%lu zero samples)\n",zero_samples);
#else
                fprintf(STDERR, " PASS\n");
#endif
#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
                for (k=0; k<256; k++)
                   if(filters_used[k])
                      fprintf(STDERR, " Filter %d was used %lu times\n",
                         k,filters_used[k]);
#endif
#if defined(PNG_TIME_RFC1123_SUPPORTED)
             if(tIME_chunk_present != 0)
                fprintf(STDERR, " tIME = %s\n",tIME_string);
#endif /* PNG_TIME_RFC1123_SUPPORTED */
            }
         }
         else
         {
            if(verbose == 0 && i != 2)
               fprintf(STDERR, "Testing %s:",inname);
            fprintf(STDERR, " FAIL\n");
            ierror += kerror;
         }
#ifdef PNG_USER_MEM_SUPPORTED
         if (allocation_now != current_allocation)
             fprintf(STDERR, "MEMORY ERROR: %d bytes lost\n",
               current_allocation-allocation_now);
         if (current_allocation != 0)
         {
             memory_infop pinfo = pinformation;

             fprintf(STDERR, "MEMORY ERROR: %d bytes still allocated\n",
                current_allocation);
             while (pinfo != NULL)
             {
                fprintf(STDERR," %d bytes at %x\n",
                   pinfo->size, pinfo->pointer);
                pinfo = pinfo->next;
             }
          }
#endif
       }
#ifdef PNG_USER_MEM_SUPPORTED
       fprintf(STDERR, " Current memory allocation: %d bytes\n",
          current_allocation);
       fprintf(STDERR, " Maximum memory allocation: %d bytes\n",
          maximum_allocation);
#endif
   }

#ifdef PNGTEST_TIMING
   t_stop = (float)clock();
   t_misc += (t_stop - t_start);
   t_start = t_stop;
   fprintf(STDERR," CPU time used = %.3f seconds",
      (t_misc+t_decode+t_encode)/(float)CLOCKS_PER_SEC);
   fprintf(STDERR," (decoding %.3f,\n",
      t_decode/(float)CLOCKS_PER_SEC);
   fprintf(STDERR,"        encoding %.3f ,",
      t_encode/(float)CLOCKS_PER_SEC);
   fprintf(STDERR," other %.3f seconds)\n\n",
      t_misc/(float)CLOCKS_PER_SEC);
#endif

   if (ierror == 0)
      fprintf(STDERR, "libpng passes test\n");
   else
      fprintf(STDERR, "libpng FAILS test\n");
   return (int)(ierror != 0);
}

/* Generate a compiler error if there is an old png.h in the search path. */
void
png_check_pngtest_version
   (version_1_1_0e png_h_is_not_version_1_1_0e)
{
   if(png_h_is_not_version_1_1_0e == NULL) return;
}
