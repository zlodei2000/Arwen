
/* pngrutil.c - utilities to read a PNG file
 *
 * libpng 1.1.0e - February 19, 2000
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
 * Copyright (c) 1996, 1997 Andreas Dilger
 * Copyright (c) 1998, 1999, 2000 Glenn Randers-Pehrson
 *
 * This file contains routines that are only called from within
 * libpng itself during the course of reading an image.
 */

#define PNG_INTERNAL
#include "png.h"

#ifndef PNG_READ_BIG_ENDIAN_SUPPORTED
/* Grab an unsigned 32-bit integer from a buffer in big-endian format. */
png_uint_32
png_get_uint_32(png_bytep buf)
{
   png_uint_32 i = ((png_uint_32)(*buf) << 24) +
      ((png_uint_32)(*(buf + 1)) << 16) +
      ((png_uint_32)(*(buf + 2)) << 8) +
      (png_uint_32)(*(buf + 3));

   return (i);
}

#if defined(PNG_READ_pCAL_SUPPORTED) || defined(PNG_READ_oFFs_SUPPORTED)
/* Grab a signed 32-bit integer from a buffer in big-endian format.  The
 * data is stored in the PNG file in two's complement format, and it is
 * assumed that the machine format for signed integers is the same. */
png_int_32
png_get_int_32(png_bytep buf)
{
   png_int_32 i = ((png_int_32)(*buf) << 24) +
      ((png_int_32)(*(buf + 1)) << 16) +
      ((png_int_32)(*(buf + 2)) << 8) +
      (png_int_32)(*(buf + 3));

   return (i);
}
#endif /* PNG_READ_pCAL_SUPPORTED */

/* Grab an unsigned 16-bit integer from a buffer in big-endian format. */
png_uint_16
png_get_uint_16(png_bytep buf)
{
   png_uint_16 i = (png_uint_16)(((png_uint_16)(*buf) << 8) +
      (png_uint_16)(*(buf + 1)));

   return (i);
}
#endif /* PNG_READ_BIG_ENDIAN_SUPPORTED */

/* Read data, and (optionally) run it through the CRC. */
void
png_crc_read(png_structp png_ptr, png_bytep buf, png_size_t length)
{
   png_read_data(png_ptr, buf, length);
   png_calculate_crc(png_ptr, buf, length);
}

/* Optionally skip data and then check the CRC.  Depending on whether we
   are reading a ancillary or critical chunk, and how the program has set
   things up, we may calculate the CRC on the data and print a message.
   Returns '1' if there was a CRC error, '0' otherwise. */
int
png_crc_finish(png_structp png_ptr, png_uint_32 skip)
{
   png_size_t i;
   png_size_t istop = png_ptr->zbuf_size;

   for (i = (png_size_t)skip; i > istop; i -= istop)
   {
      png_crc_read(png_ptr, png_ptr->zbuf, png_ptr->zbuf_size);
   }
   if (i)
   {
      png_crc_read(png_ptr, png_ptr->zbuf, i);
   }

   if (png_crc_error(png_ptr))
   {
      if (((png_ptr->chunk_name[0] & 0x20) &&                /* Ancillary */
           !(png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_NOWARN)) ||
          (!(png_ptr->chunk_name[0] & 0x20) &&             /* Critical  */
          (png_ptr->flags & PNG_FLAG_CRC_CRITICAL_USE)))
      {
         png_chunk_warning(png_ptr, "CRC error");
      }
      else
      {
         png_chunk_error(png_ptr, "CRC error");
      }
      return (1);
   }

   return (0);
}

/* Compare the CRC stored in the PNG file with that calculated by libpng from
   the data it has read thus far. */
int
png_crc_error(png_structp png_ptr)
{
   png_byte crc_bytes[4];
   png_uint_32 crc;
   int need_crc = 1;

   if (png_ptr->chunk_name[0] & 0x20)                     /* ancillary */
   {
      if ((png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_MASK) ==
          (PNG_FLAG_CRC_ANCILLARY_USE | PNG_FLAG_CRC_ANCILLARY_NOWARN))
         need_crc = 0;
   }
   else                                                    /* critical */
   {
      if (png_ptr->flags & PNG_FLAG_CRC_CRITICAL_IGNORE)
         need_crc = 0;
   }

   png_read_data(png_ptr, crc_bytes, 4);

   if (need_crc)
   {
      crc = png_get_uint_32(crc_bytes);
      return ((int)(crc != png_ptr->crc));
   }
   else
      return (0);
}

#if defined(PNG_READ_zTXt_SUPPORTED) || defined(PNG_READ_iTXt_SUPPORTED) || \
    defined(PNG_READ_iCCP_SUPPORTED) || defined(PNG_READ_sPLT_SUPPORTED)
/*
 * Decompress trailing data in a chunk.  The assumption is that chunkdata
 * points at an allocated area holding the contents of a chunk with a
 * trailing compressed part.  What we get back is an allocated area
 * holding the original prefix part and an uncompressed version of the
 * trailing part (the malloc area passed in is freed).
 */
png_charp png_decompress_chunk(png_structp png_ptr, int comp_type,
                              png_charp chunkdata, png_size_t chunklength,
                              png_size_t prefix_size)
{
   static char msg[] = "Error decoding compressed text";
   png_charp text = NULL;
   png_size_t text_size = (chunklength - prefix_size);

   if (comp_type == PNG_TEXT_COMPRESSION_zTXt)
   {
      png_ptr->zstream.next_in = (png_bytep)(chunkdata + prefix_size);
      png_ptr->zstream.avail_in = (uInt)(chunklength - prefix_size);
      png_ptr->zstream.next_out = png_ptr->zbuf;
      png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;

      text_size = 0;
      text = NULL;

      while (png_ptr->zstream.avail_in)
      {
         int ret;

         ret = inflate(&png_ptr->zstream, Z_PARTIAL_FLUSH);
         if (ret != Z_OK && ret != Z_STREAM_END)
         {
            if (png_ptr->zstream.msg != NULL)
               png_warning(png_ptr, png_ptr->zstream.msg);
            else
               png_warning(png_ptr, msg);
            inflateReset(&png_ptr->zstream);
            png_ptr->zstream.avail_in = 0;

            if (text ==  NULL)
            {
               text_size = prefix_size + sizeof(msg) + 1;
               text = (png_charp)png_malloc(png_ptr, text_size);
               png_memcpy(text, chunkdata, prefix_size);
            }

            text[text_size - 1] = 0x00;

            /* Copy what we can of the error message into the text chunk */
            text_size = (png_size_t)(chunklength - (text - chunkdata) - 1);
            text_size = sizeof(msg) > text_size ? text_size : sizeof(msg);
            png_memcpy(text + prefix_size, msg, text_size + 1);
            break;
         }
         if (!png_ptr->zstream.avail_out || ret == Z_STREAM_END)
         {
            if (text == NULL)
            {
               text_size = prefix_size +
                   png_ptr->zbuf_size - png_ptr->zstream.avail_out;
               text = (png_charp)png_malloc(png_ptr, text_size + 1);
               png_memcpy(text + prefix_size, png_ptr->zbuf,
                          text_size - prefix_size);
               png_memcpy(text, chunkdata, prefix_size);
               *(text + text_size) = 0x00;
            }
            else
            {
               png_charp tmp;

               tmp = text;
               text = (png_charp)png_malloc(png_ptr, (png_uint_32)(text_size +
                  png_ptr->zbuf_size - png_ptr->zstream.avail_out + 1));
               png_memcpy(text, tmp, text_size);
               png_free(png_ptr, tmp);
               png_memcpy(text + text_size, png_ptr->zbuf,
                  (png_ptr->zbuf_size - png_ptr->zstream.avail_out));
               text_size += png_ptr->zbuf_size - png_ptr->zstream.avail_out;
               *(text + text_size) = 0x00;
            }
            if (ret == Z_STREAM_END)
               break;
            else
            {
               png_ptr->zstream.next_out = png_ptr->zbuf;
               png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
            }
         }
      }

      inflateReset(&png_ptr->zstream);
      png_ptr->zstream.avail_in = 0;

      png_free(png_ptr, chunkdata);
      chunkdata = text;
   }
   else /* if (comp_type >= PNG_TEXT_COMPRESSION_LAST) */
   {
#if !defined(PNG_NO_STDIO)
      char umsg[50];

      sprintf(umsg, "Unknown compression type %d", comp_type);
      png_warning(png_ptr, umsg);
#else
      png_warning(png_ptr, "Unknown compression type");
#endif

      /* Copy what we can of the error message into the text chunk */
      text_size = (png_size_t)(chunklength - (text - chunkdata));
      text_size = sizeof(msg) > text_size ? text_size : sizeof(msg);
      png_memcpy(text, msg, text_size);
   }

   return chunkdata;
}
#endif

int
png_read_IHDR(png_structp png_ptr, png_IHDR_tp ihdrp, png_uint_32 length)
/* this sets state in png_ptr structure */
{
   png_byte buf[13];

   /* check the length */
   if (length != 13)
      png_error(png_ptr, "Invalid IHDR chunk");

   png_crc_read(png_ptr, buf, 13);
   png_crc_finish(png_ptr, 0);

   ihdrp->width = png_get_uint_32(buf);
   ihdrp->height = png_get_uint_32(buf + 4);
   ihdrp->bit_depth = buf[8];
   ihdrp->color_type = buf[9];
   ihdrp->compression_type = buf[10];
   ihdrp->filter_type = buf[11];
   ihdrp->interlace_type = buf[12];

   /* check for width and height valid values */
   if (ihdrp->width == 0 || ihdrp->width > PNG_MAX_UINT
                || ihdrp->height == 0 || ihdrp->height > PNG_MAX_UINT)
      png_error(png_ptr, "Invalid image size in IHDR");

   /* check other values */
   if (ihdrp->bit_depth != 1 && ihdrp->bit_depth != 2
        && ihdrp->bit_depth != 4 && ihdrp->bit_depth != 8
        && ihdrp->bit_depth != 16)
      png_error(png_ptr, "Invalid bit depth in IHDR");

   if (ihdrp->color_type == 1 ||
                ihdrp->color_type == 5 || ihdrp->color_type > 6)
      png_error(png_ptr, "Invalid color type in IHDR");

   if (((ihdrp->color_type==PNG_COLOR_TYPE_PALETTE) && ihdrp->bit_depth > 8) ||
       ((ihdrp->color_type == PNG_COLOR_TYPE_RGB ||
         ihdrp->color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
         ihdrp->color_type == PNG_COLOR_TYPE_RGB_ALPHA) && ihdrp->bit_depth<8))
      png_error(png_ptr, "Invalid color type/bit depth combination in IHDR");

   if (ihdrp->interlace_type >= PNG_INTERLACE_LAST)
      png_error(png_ptr, "Unknown interlace method in IHDR");

   if (ihdrp->compression_type != PNG_COMPRESSION_TYPE_BASE)
      png_error(png_ptr, "Unknown compression method in IHDR");

   if (ihdrp->filter_type != PNG_FILTER_TYPE_BASE)
      png_error(png_ptr, "Unknown filter method in IHDR");

   if (png_ptr->mode != PNG_BEFORE_IHDR)
      png_error(png_ptr, "Out of place IHDR");

   /* set internal variables */
   png_ptr->width = ihdrp->width;
   png_ptr->height = ihdrp->height;
   png_ptr->bit_depth = (png_byte)ihdrp->bit_depth;
   png_ptr->interlaced = (png_byte)ihdrp->interlace_type;
   png_ptr->color_type = (png_byte)ihdrp->color_type;

   /* find number of channels */
   switch (png_ptr->color_type)
   {
      case PNG_COLOR_TYPE_GRAY:
      case PNG_COLOR_TYPE_PALETTE:
         png_ptr->channels = 1;
         break;
      case PNG_COLOR_TYPE_RGB:
         png_ptr->channels = 3;
         break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
         png_ptr->channels = 2;
         break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
         png_ptr->channels = 4;
         break;
   }

   /* set up other useful info */
   png_ptr->pixel_depth = (png_byte)(png_ptr->bit_depth *
   png_ptr->channels);
   png_ptr->rowbytes = ((png_ptr->width *
      (png_uint_32)png_ptr->pixel_depth + 7) >> 3);
   png_debug1(3,"bit_depth = %d\n", png_ptr->bit_depth);
   png_debug1(3,"channels = %d\n", png_ptr->channels);
   png_debug1(3,"rowbytes = %d\n", png_ptr->rowbytes);

   /* just so it will have the same type as the other *_read_* functions */
   return(1);
}

/* read and check the IDHR chunk */
void
png_handle_IHDR(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_IHDR_t        ihdr;

   png_debug(1, "in png_handle_IHDR\n");

   png_read_IHDR(png_ptr, &ihdr, length);
   png_set_IHDR(png_ptr, info_ptr,
                ihdr.width, ihdr.height, ihdr.bit_depth, ihdr.color_type,
                ihdr.interlace_type, ihdr.compression_type, ihdr.filter_type);
}

int
png_read_PLTE(png_structp png_ptr, png_PLTE_tp pltep, png_uint_32 length)
/* this is dependent on the color_type in the png_ptr structure */
{
   int i;

   if (length % 3)
   {
      if (png_ptr->color_type != PNG_COLOR_TYPE_PALETTE)
      {
         png_warning(png_ptr, "Invalid palette chunk");
         png_crc_finish(png_ptr, length);
         return(0);
      }
      else
      {
         png_error(png_ptr, "Invalid palette chunk");
      }
   }

   pltep->size = (png_uint_16)(length / 3);
   pltep->colors = (png_colorp)png_zalloc(png_ptr, (uInt)pltep->size, sizeof (png_color));
   png_ptr->free_me |= PNG_FREE_PLTE;
   for (i = 0; i < pltep->size; i++)
   {
      png_byte buf[3];

      png_crc_read(png_ptr, buf, 3);
      /* don't depend upon png_color being any order */
      pltep->colors[i].red = buf[0];
      pltep->colors[i].green = buf[1];
      pltep->colors[i].blue = buf[2];
   }

   /* If we actually NEED the PLTE chunk (ie for a paletted image), we do
      whatever the normal CRC configuration tells us.  However, if we
      have an RGB image, the PLTE can be considered ancillary, so
      we will act as though it is. */
#if !defined(PNG_READ_OPT_PLTE_SUPPORTED)
   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
#endif
   {
      png_crc_finish(png_ptr, 0);
   }
#if !defined(PNG_READ_OPT_PLTE_SUPPORTED)
   else if (png_crc_error(png_ptr))  /* Only if we have a CRC error */
   {
      /* If we don't want to use the data from an ancillary chunk,
         we have two options: an error abort, or a warning and we
         ignore the data in this chunk (which should be OK, since
         it's considered ancillary for a RGB or RGBA image). */
      if (!(png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_USE))
      {
         if (png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_NOWARN)
         {
            png_chunk_error(png_ptr, "CRC error");
         }
         else
         {
            png_chunk_warning(png_ptr, "CRC error");
            png_ptr->free_me &= ~PNG_FREE_PLTE;
            png_zfree(png_ptr, palette);
            return(0);
         }
      }
      /* Otherwise, we (optionally) emit a warning and use the chunk. */
      else if (!(png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_NOWARN))
      {
         png_chunk_warning(png_ptr, "CRC error");
      }
   }
#endif
   png_ptr->palette = *pltep;

   return(1);
}

/* read and check the palette */
void
png_handle_PLTE(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_PLTE_t        plte;

   png_debug(1, "in png_handle_PLTE\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before PLTE");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid PLTE after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      png_error(png_ptr, "Duplicate PLTE chunk");

#if !defined(PNG_READ_OPT_PLTE_SUPPORTED)
   if (png_ptr->color_type != PNG_COLOR_TYPE_PALETTE)
   {
      png_crc_finish(png_ptr, length);
      return;
   }
#endif

   if (png_read_PLTE(png_ptr, &plte, length))
   {
       png_set_PLTE(png_ptr, info_ptr, plte.colors, plte.size);

#if defined (PNG_READ_tRNS_SUPPORTED)
       if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
       {
           if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_tRNS))
           {
               if (png_ptr->num_trans > png_ptr->palette.size)
               {
                   png_warning(png_ptr, "Truncating incorrect tRNS chunk length");
                   png_ptr->num_trans = png_ptr->palette.size;
               }
           }
       }
#endif
   }
}

void
png_handle_IEND(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_debug(1, "in png_handle_IEND\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR) || !(png_ptr->mode & PNG_HAVE_IDAT))
   {
      png_error(png_ptr, "No image in file");

      /* to quiet compiler warnings about unused info_ptr */
      if (info_ptr == NULL)
         return;
   }

   if (length != 0)
   {
      png_warning(png_ptr, "Incorrect IEND chunk length");
   }
   png_crc_finish(png_ptr, length);
}

#if defined(PNG_READ_gAMA_SUPPORTED)
int
png_read_gAMA(png_structp png_ptr, png_gAMA_tp gamma, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_fixed_point igamma;
#ifdef PNG_FLOATING_POINT_SUPPORTED
   float file_gamma;
#endif
   png_byte buf[4];

   if (length != 4)
   {
      png_warning(png_ptr, "Incorrect gAMA chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, 4);
   if (png_crc_finish(png_ptr, 0))
      return(0);

   igamma = (png_fixed_point)png_get_uint_32(buf);

#ifdef PNG_FLOATING_POINT_SUPPORTED
   file_gamma = (float)igamma / (float)100000.0;
#  ifdef PNG_READ_GAMMA_SUPPORTED
   png_ptr->file_gamma = file_gamma;
#  endif
   gamma->float_gamma = file_gamma;
#endif
#ifdef PNG_FIXED_POINT_SUPPORTED
   gamma->int_gamma = igamma;
#endif

   return(1);
}

void
png_handle_gAMA(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_gAMA_t        gamma;

   png_debug(1, "in png_handle_gAMA\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before gAMA");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid gAMA after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place gAMA chunk");

   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_gAMA)
#if defined(PNG_READ_sRGB_SUPPORTED)
      && !(info_ptr->valid & PNG_INFO_sRGB)
#endif
      )
   {
      png_warning(png_ptr, "Duplicate gAMA chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   /* check for zero gamma */
   if (!png_read_gAMA(png_ptr, &gamma, length) || gamma.int_gamma == 0)
      return;

#if defined(PNG_READ_sRGB_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sRGB)
      if(gamma.int_gamma < 45000L || gamma.int_gamma > 46000L)
      {
         png_warning(png_ptr,
           "Ignoring incorrect gAMA value when sRGB is also present");
#ifndef PNG_NO_CONSOLE_IO
         fprintf(stderr, "gamma = (%d/100000)\n", (int)gamma.int_gamma);
#endif
         return;
      }
#endif /* PNG_READ_sRGB_SUPPORTED */

#ifdef PNG_FLOATING_POINT_SUPPORTED
    png_set_gAMA(png_ptr, info_ptr, gamma.float_gamma);
#endif
#ifdef PNG_FIXED_POINT_SUPPORTED
    png_set_gAMA_fixed(png_ptr, info_ptr, gamma.int_gamma);
#endif
}
#endif

#if defined(PNG_READ_sBIT_SUPPORTED)
int
png_read_sBIT(png_structp png_ptr, png_sBIT_tp sbitp, png_uint_32 length)
/* this is dependent on the color_type in the png_ptr structure */
{
   png_size_t truelen;
   png_byte buf[4];

   png_debug(1, "in png_handle_sBIT\n");

   buf[0] = buf[1] = buf[2] = buf[3] = 0;

   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      truelen = 3;
   else
      truelen = (png_size_t)png_ptr->channels;

   if (length != truelen)
   {
      png_warning(png_ptr, "Incorrect sBIT chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, truelen);
   if (png_crc_finish(png_ptr, 0))
      return(0);

   if (png_ptr->color_type & PNG_COLOR_MASK_COLOR)
   {
      sbitp->red = buf[0];
      sbitp->green = buf[1];
      sbitp->blue = buf[2];
      sbitp->alpha = buf[3];
   }
   else
   {
      sbitp->gray = buf[0];
      sbitp->red = buf[0];
      sbitp->green = buf[0];
      sbitp->blue = buf[0];
      sbitp->alpha = buf[1];
   }

   return(1);
}

void
png_handle_sBIT(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sBIT");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sBIT after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
   {
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place sBIT chunk");
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sBIT))
   {
      png_warning(png_ptr, "Duplicate sBIT chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_sBIT(png_ptr, &(png_ptr->sig_bit), length))
       png_set_sBIT(png_ptr, info_ptr, &(png_ptr->sig_bit));
}
#endif

#if defined(PNG_READ_cHRM_SUPPORTED)
int
png_read_cHRM(png_structp png_ptr, png_cHRM_tp chrmp, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_byte buf[4];
   if (length != 32)
   {
      png_warning(png_ptr, "Incorrect cHRM chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_x_white = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_y_white = (png_fixed_point)png_get_uint_32(buf);

   if (chrmp->int_x_white > 80000L || chrmp->int_y_white > 80000L ||
      chrmp->int_x_white + chrmp->int_y_white > 100000L)
   {
      png_warning(png_ptr, "Invalid cHRM white point");
      png_crc_finish(png_ptr, 24);
      return(0);
   }

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_x_red = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_y_red = (png_fixed_point)png_get_uint_32(buf);

   if (chrmp->int_x_red > 80000L || chrmp->int_y_red > 80000L ||
      chrmp->int_x_red + chrmp->int_y_red > 100000L)
   {
      png_warning(png_ptr, "Invalid cHRM red point");
      png_crc_finish(png_ptr, 16);
      return(0);
   }

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_x_green = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_y_green = (png_fixed_point)png_get_uint_32(buf);

   if (chrmp->int_x_green > 80000L || chrmp->int_y_green > 80000L ||
      chrmp->int_x_green + chrmp->int_y_green > 100000L)
   {
      png_warning(png_ptr, "Invalid cHRM green point");
      png_crc_finish(png_ptr, 8);
      return(0);
   }

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_x_blue = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   chrmp->int_y_blue = (png_fixed_point)png_get_uint_32(buf);

   if (chrmp->int_x_blue > 80000L || chrmp->int_y_blue > 80000L ||
      chrmp->int_x_blue + chrmp->int_y_blue > 100000L)
   {
      png_warning(png_ptr, "Invalid cHRM blue point");
      png_crc_finish(png_ptr, 0);
      return(0);
   }
#ifdef PNG_FLOATING_POINT_SUPPORTED
   chrmp->x_white = (float)chrmp->int_x_white / (float)100000.0;
   chrmp->y_white = (float)chrmp->int_y_white / (float)100000.0;
   chrmp->x_red   = (float)chrmp->int_x_red   / (float)100000.0;
   chrmp->y_red   = (float)chrmp->int_y_red   / (float)100000.0;
   chrmp->x_green = (float)chrmp->int_x_green / (float)100000.0;
   chrmp->y_green = (float)chrmp->int_y_green / (float)100000.0;
   chrmp->x_blue  = (float)chrmp->int_x_blue  / (float)100000.0;
   chrmp->y_blue  = (float)chrmp->int_y_blue  / (float)100000.0;
#endif

   if (png_crc_finish(png_ptr, 0))
      return(0);

   return(1);
}

void
png_handle_cHRM(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_cHRM_t        chrm;

   png_debug(1, "in png_handle_cHRM\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before cHRM");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid cHRM after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Missing PLTE before cHRM");

   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_cHRM)
#if defined(PNG_READ_sRGB_SUPPORTED)
      && !(info_ptr->valid & PNG_INFO_sRGB)
#endif
      )
   {
      png_warning(png_ptr, "Duplicate cHRM chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (!png_read_cHRM(png_ptr, &chrm, length))
       return;

#if defined(PNG_READ_sRGB_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sRGB)
      {
      if (abs(chrm.int_x_white - 31270L) > 1000 ||
          abs(chrm.int_y_white - 32900L) > 1000 ||
          abs(  chrm.int_x_red - 64000L) > 1000 ||
          abs(  chrm.int_y_red - 33000L) > 1000 ||
          abs(chrm.int_x_green - 30000L) > 1000 ||
          abs(chrm.int_y_green - 60000L) > 1000 ||
          abs( chrm.int_x_blue - 15000L) > 1000 ||
          abs( chrm.int_y_blue -  6000L) > 1000)
         {

            png_warning(png_ptr,
              "Ignoring incorrect cHRM value when sRGB is also present");
#ifndef PNG_NO_CONSOLE_IO
#ifdef PNG_FLOATING_POINT_SUPPORTED
            fprintf(stderr,"wx=%f, wy=%f, rx=%f, ry=%f\n",
               chrm.x_white, chrm.y_white, chrm.x_red, chrm.y_red);
            fprintf(stderr,"gx=%f, gy=%f, bx=%f, by=%f\n",
               chrm.x_green, chrm.y_green, chrm.x_blue, chrm.y_blue);
#else
            fprintf(stderr,"wx=%ld, wy=%ld, rx=%ld, ry=%ld\n",
               chrm.int_x_white, chrm.int_y_white, chrm.int_x_red, chrm.int_y_red);
            fprintf(stderr,"gx=%ld, gy=%ld, bx=%ld, by=%ld\n",
               chrm.int_x_green, chrm.int_y_green, chrm.int_x_blue, chrm.int_y_blue);
#endif
#endif /* PNG_NO_CONSOLE_IO */
         }
         return;
      }
#endif /* PNG_READ_sRGB_SUPPORTED */

#ifdef PNG_FLOATING_POINT_SUPPORTED
   png_set_cHRM(png_ptr, info_ptr,
      chrm.x_white, chrm.y_white, chrm.x_red, chrm.y_red, chrm.x_green, chrm.y_green, chrm.x_blue, chrm.y_blue);
#endif
#ifdef PNG_FIXED_POINT_SUPPORTED
   png_set_cHRM_fixed(png_ptr, info_ptr,
      chrm.int_x_white, chrm.int_y_white, chrm.int_x_red, chrm.int_y_red, chrm.int_x_green,
      chrm.int_y_green, chrm.int_x_blue, chrm.int_y_blue);
#endif
}
#endif

#if defined(PNG_READ_sRGB_SUPPORTED)
int
png_read_sRGB(png_structp png_ptr, png_sRGB_tp srgbp, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_byte buf[1];

   if (length != 1)
   {
      png_warning(png_ptr, "Incorrect sRGB chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, 1);
   if (png_crc_finish(png_ptr, 0))
      return(0);

   srgbp->intent = buf[0];
   /* check for bad intent */
   if (srgbp->intent >= PNG_sRGB_INTENT_LAST)
   {
      png_warning(png_ptr, "Unknown sRGB intent");
      return(0);
   }

   return(1);
}

void
png_handle_sRGB(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_sRGB_t        srgb;

   png_debug(1, "in png_handle_sRGB\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sRGB");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sRGB after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place sRGB chunk");

   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sRGB))
   {
      png_warning(png_ptr, "Duplicate sRGB chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (!png_read_sRGB(png_ptr, &srgb, length))
       return;

#if defined(PNG_READ_gAMA_SUPPORTED) && defined(PNG_READ_GAMMA_SUPPORTED)
   if ((info_ptr->valid & PNG_INFO_gAMA))
   {
   int igamma;
#ifdef PNG_FIXED_POINT_SUPPORTED
      igamma=(int)info_ptr->gamma.int_gamma;
#else
#  ifdef PNG_FLOATING_POINT_SUPPORTED
      igamma=info_ptr->gamma.file_gamma * 100000.;
#  endif
#endif
      if(igamma < 45000L || igamma > 46000L)
      {
         png_warning(png_ptr,
           "Ignoring incorrect gAMA value when sRGB is also present");
#ifndef PNG_NO_CONSOLE_IO
#  ifdef PNG_FIXED_POINT_SUPPORTED
         fprintf(stderr,"incorrect gamma=(%d/100000)\n",(int)png_ptr->int_gamma);
#  else
#    ifdef PNG_FLOATING_POINT_SUPPORTED
         fprintf(stderr,"incorrect gamma=%f\n",png_ptr->gamma);
#    endif
#  endif
#endif
      }
   }
#endif /* PNG_READ_gAMA_SUPPORTED */

#ifdef PNG_READ_cHRM_SUPPORTED
   if (info_ptr->valid & PNG_INFO_cHRM)
      if (abs(info_ptr->chrm.int_x_white - 31270L) > 1000 ||
          abs(info_ptr->chrm.int_y_white - 32900L) > 1000 ||
          abs(  info_ptr->chrm.int_x_red - 64000L) > 1000 ||
          abs(  info_ptr->chrm.int_y_red - 33000L) > 1000 ||
          abs(info_ptr->chrm.int_x_green - 30000L) > 1000 ||
          abs(info_ptr->chrm.int_y_green - 60000L) > 1000 ||
          abs( info_ptr->chrm.int_x_blue - 15000L) > 1000 ||
          abs( info_ptr->chrm.int_y_blue -  6000L) > 1000)
         {
            png_warning(png_ptr,
              "Ignoring incorrect cHRM value when sRGB is also present");
         }
#endif /* PNG_READ_cHRM_SUPPORTED */

   png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb.intent);
}
#endif /* PNG_READ_sRGB_SUPPORTED */

#if defined(PNG_READ_iCCP_SUPPORTED)
int
png_read_iCCP(png_structp png_ptr, png_iCCP_tp iccpp, png_uint_32 length)
/* this does not properly handle chunks that are > 64K under DOS */
/* this has no state dependencies on the png_ptr structure */
{
   png_charp chunkdata;
   png_charp profile;
   png_uint_32 skip = 0;
   png_size_t slength, prefix_length;

#ifdef PNG_MAX_MALLOC_64K
   if (length > (png_uint_32)65535L)
   {
      png_warning(png_ptr, "iCCP chunk too large to fit in memory");
      skip = length - (png_uint_32)65535L;
      length = (png_uint_32)65535L;
   }
#endif

   chunkdata = (png_charp)png_malloc(png_ptr, length + 1);
   png_ptr->free_me |= PNG_FREE_ICCP;
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)chunkdata, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, chunkdata);
      return(0);
   }

   chunkdata[slength] = 0x00;

   for (profile = chunkdata; *profile; profile++)
      /* empty loop to find end of name */ ;
   ++profile;

   /* there should be at least one NUL (the compression type byte)
      following the separator, and we should be on it  */
   if (profile >= chunkdata + slength)
   {
      png_free(png_ptr, chunkdata);
      png_error(png_ptr, "malformed iCCP chunk");
   }

   /* compression should always be zero */
   iccpp->compression = *profile++;

   prefix_length = profile - chunkdata;
   iccpp->name = png_decompress_chunk(png_ptr, iccpp->compression, chunkdata,
                                    slength, prefix_length);
   iccpp->profile = iccpp->name + prefix_length;
   iccpp->proflen = png_strlen(iccpp->name + prefix_length);

   return(1);
}

void
png_handle_iCCP(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_iCCP_t        iccp;

   png_debug(1, "in png_handle_iCCP\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before iCCP");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid iCCP after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place iCCP chunk");
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_iCCP))
   {
      png_warning(png_ptr, "Duplicate iCCP chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_iCCP(png_ptr, &iccp, length))
       png_set_iCCP(png_ptr, info_ptr, iccp.name, iccp.compression,
                iccp.profile, iccp.proflen);
   png_free(png_ptr, iccp.name);
}
#endif /* PNG_READ_iCCP_SUPPORTED */

#if defined(PNG_READ_sPLT_SUPPORTED)
int
png_read_sPLT(png_structp png_ptr, png_sPLT_tp spltp, png_uint_32 length)
/* this does not properly handle chunks that are > 64K under DOS */
/* this has no state dependencies on the png_ptr structure */
{
   png_bytep chunkdata;
   png_bytep entry_start;
   int data_length, entry_size, i;
   png_uint_32 skip = 0;
   png_size_t slength;

#ifdef PNG_MAX_MALLOC_64K
   if (length > (png_uint_32)65535L)
   {
      png_warning(png_ptr, "sPLT chunk too large to fit in memory");
      skip = length - (png_uint_32)65535L;
      length = (png_uint_32)65535L;
   }
#endif

   chunkdata = (png_bytep)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, chunkdata, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, chunkdata);
      return(0);
   }

   chunkdata[slength] = 0x00;

   for (entry_start = chunkdata; *entry_start; entry_start++)
      /* empty loop to find end of name */ ;
   ++entry_start;

   /* a sample depth should follow the separator, and we should be on it  */
   if (entry_start > chunkdata + slength)
   {
      png_free(png_ptr, chunkdata);
      png_error(png_ptr, "malformed sPLT chunk");
   }

   spltp->depth = *entry_start++;
   entry_size = (spltp->depth == 8 ? 6 : 10);
   data_length = (slength - (entry_start - chunkdata));

   /* integrity-check the data length */
   if (data_length % entry_size)
   {
      png_free(png_ptr, chunkdata);
      png_error(png_ptr, "sPLT chunk has bad length");
   }

   spltp->nentries = data_length / entry_size;
   spltp->entries = (png_sPLT_entryp)png_malloc(
       png_ptr, spltp->nentries * sizeof(png_sPLT_entry));

   for (i = 0; i < spltp->nentries; i++)
   {
      png_sPLT_entryp pp = spltp->entries + i;

      if (spltp->depth == 8)
      {
          pp->red = *entry_start++;
          pp->green = *entry_start++;
          pp->blue = *entry_start++;
          pp->alpha = *entry_start++;
      }
      else
      {
          pp->red   = png_get_uint_16(entry_start); entry_start += 2;
          pp->green = png_get_uint_16(entry_start); entry_start += 2;
          pp->blue  = png_get_uint_16(entry_start); entry_start += 2;
          pp->alpha = png_get_uint_16(entry_start); entry_start += 2;
      }
      pp->frequency = png_get_uint_16(entry_start); entry_start += 2;
   }

   /* discard all chunk data except the name and stash that */
   spltp->name = (png_charp)chunkdata;

   return(1);
}

void
png_handle_sPLT(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_sPLT_t        splt;

   png_debug(1, "in png_handle_sPLT\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sPLT");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sPLT after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (!png_read_sPLT(png_ptr, &splt, length))
   {
       png_set_sPLT(png_ptr, info_ptr, &splt, 1);

       png_free(png_ptr, splt.name);
       png_free(png_ptr, splt.entries);
   }
}
#endif /* PNG_READ_sPLT_SUPPORTED */

#if defined(PNG_READ_tRNS_SUPPORTED)
int
png_read_tRNS(png_structp png_ptr, png_tRNS_tp trnsp, png_uint_32 length)
/* this is dependent on the color_type and palette.size in the
 * png_ptr structure */
{
   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
   {
      if (!(png_ptr->mode & PNG_HAVE_PLTE))
      {
         /* Should be an error, but we can cope with it */
         png_warning(png_ptr, "Missing PLTE before tRNS");
      }
      else if (length > png_ptr->palette.size)
      {
         png_warning(png_ptr, "Incorrect tRNS chunk length");
         png_crc_finish(png_ptr, length);
         return(0);
      }
      if (length == 0)
      {
         png_warning(png_ptr, "Zero length tRNS chunk");
         png_crc_finish(png_ptr, length);
         return(0);
      }

      png_ptr->trans = (png_bytep)png_malloc(png_ptr, length);
      png_ptr->free_me |= PNG_FREE_TRNS;
      png_crc_read(png_ptr, png_ptr->trans, (png_size_t)length);
      png_ptr->num_trans = (png_uint_16)length;
   }
   else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
   {
      png_byte buf[6];

      if (length != 6)
      {
         png_warning(png_ptr, "Incorrect tRNS chunk length");
         png_crc_finish(png_ptr, length);
         return(0);
      }

      png_crc_read(png_ptr, buf, (png_size_t)length);
      png_ptr->num_trans = 1;
      png_ptr->trans_values.red = png_get_uint_16(buf);
      png_ptr->trans_values.green = png_get_uint_16(buf + 2);
      png_ptr->trans_values.blue = png_get_uint_16(buf + 4);
   }
   else if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY)
   {
      png_byte buf[6];

      if (length != 2)
      {
         png_warning(png_ptr, "Incorrect tRNS chunk length");
         png_crc_finish(png_ptr, length);
         return(0);
      }

      png_crc_read(png_ptr, buf, 2);
      png_ptr->num_trans = 1;
      png_ptr->trans_values.gray = png_get_uint_16(buf);
   }
   else
   {
      png_warning(png_ptr, "tRNS chunk not allowed with alpha channel");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   if (png_crc_finish(png_ptr, 0))
      return(0);

   trnsp->trans = png_ptr->trans;
   trnsp->num_trans = png_ptr->num_trans;
   trnsp->trans_values = png_ptr->trans_values;
   return(1);
}

void
png_handle_tRNS(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_tRNS_t        trns;

   png_debug(1, "in png_handle_tRNS\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before tRNS");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid tRNS after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_tRNS))
   {
      png_warning(png_ptr, "Duplicate tRNS chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_tRNS(png_ptr, &trns, length))
       png_set_tRNS(png_ptr, info_ptr, png_ptr->trans, png_ptr->num_trans,
                    &(png_ptr->trans_values));
}
#endif

#if defined(PNG_READ_bKGD_SUPPORTED)
int
png_read_bKGD(png_structp png_ptr, png_bKGD_tp bkgdp, png_uint_32 length)
/* this depends on the color_type and palette.size fields in png_ptr */
{
   png_size_t truelen;
   png_byte buf[6];

   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      truelen = 1;
   else if (png_ptr->color_type & PNG_COLOR_MASK_COLOR)
      truelen = 6;
   else
      truelen = 2;

   if (length != truelen)
   {
      png_warning(png_ptr, "Incorrect bKGD chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, truelen);
   if (png_crc_finish(png_ptr, 0))
      return(0);

   /* We convert the index value into RGB components so that we can allow
    * arbitrary RGB values for background when we have transparency, and
    * so it is easy to determine the RGB values of the background color
    * from the info_ptr struct. */
   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
   {
      bkgdp->index = buf[0];
      if(png_ptr->palette.size)        /* XXX formerly info_ptr->palette.size */
      {
          if(buf[0] > png_ptr->palette.size)
          {
             png_warning(png_ptr, "Incorrect bKGD chunk index value");
             png_crc_finish(png_ptr, length);
             return(0);
          }
          bkgdp->red =   (png_uint_16)png_ptr->palette.colors[buf[0]].red;
          bkgdp->green = (png_uint_16)png_ptr->palette.colors[buf[0]].green;
          bkgdp->blue =  (png_uint_16)png_ptr->palette.colors[buf[0]].blue;
      }
   }
   else if (!(png_ptr->color_type & PNG_COLOR_MASK_COLOR)) /* GRAY */
   {
      bkgdp->red =
      bkgdp->green =
      bkgdp->blue =
      bkgdp->gray = png_get_uint_16(buf);
   }
   else
   {
      bkgdp->red = png_get_uint_16(buf);
      bkgdp->green = png_get_uint_16(buf + 2);
      bkgdp->blue = png_get_uint_16(buf + 4);
   }

   return(1);
}

void
png_handle_bKGD(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_bKGD_t        bkgd;

   png_debug(1, "in png_handle_bKGD\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before bKGD");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid bKGD after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE &&
            !(png_ptr->mode & PNG_HAVE_PLTE))
   {
      png_warning(png_ptr, "Missing PLTE before bKGD");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_bKGD))
   {
      png_warning(png_ptr, "Duplicate bKGD chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_bKGD(png_ptr, &bkgd, length))
   {
       png_ptr->background = bkgd;
       png_set_bKGD(png_ptr, info_ptr, &(png_ptr->background));
   }
}
#endif

#if defined(PNG_READ_hIST_SUPPORTED)
int
png_read_hIST(png_structp png_ptr, png_hIST_tp histp, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   int num, i;

   if (length != (png_uint_32)(2 * png_ptr->palette.size))
   {
      png_warning(png_ptr, "Incorrect hIST chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   num = (int)length / 2 ;
   png_ptr->hist.frequencies = (png_uint_16p)png_malloc(png_ptr,
      (png_uint_32)(num * sizeof (png_uint_16)));
   png_ptr->free_me |= PNG_FREE_HIST;
   for (i = 0; i < num; i++)
   {
      png_byte buf[2];

      png_crc_read(png_ptr, buf, 2);
      png_ptr->hist.frequencies[i] = png_get_uint_16(buf);
   }

   if (png_crc_finish(png_ptr, 0))
      return(0);

   *histp = png_ptr->hist;
   return(1);
}

void
png_handle_hIST(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_hIST_t hist;

   png_debug(1, "in png_handle_hIST\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before hIST");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid hIST after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (!(png_ptr->mode & PNG_HAVE_PLTE))
   {
      png_warning(png_ptr, "Missing PLTE before hIST");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_hIST))
   {
      png_warning(png_ptr, "Duplicate hIST chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_hIST(png_ptr, &hist, length))
       png_set_hIST(png_ptr, info_ptr, png_ptr->hist.frequencies);
}
#endif

#if defined(PNG_READ_pHYs_SUPPORTED)
int
png_read_pHYs(png_structp png_ptr, png_pHYs_tp physp, png_uint_32 length)
{
   png_byte buf[9];

   if (length != 9)
   {
      png_warning(png_ptr, "Incorrect pHYs chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, 9);
   if (png_crc_finish(png_ptr, 0))
      return(0);

   physp->x_pixels_per_unit = png_get_uint_32(buf);
   physp->y_pixels_per_unit = png_get_uint_32(buf + 4);
   physp->unit_type = buf[8];

   return(1);
}

void
png_handle_pHYs(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_pHYs_t        phys;

   png_debug(1, "in png_handle_pHYs\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before pHYS");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid pHYS after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_pHYs))
   {
      png_warning(png_ptr, "Duplicate pHYS chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_pHYs(png_ptr, &phys, length))
       png_set_pHYs(png_ptr, info_ptr,
                    phys.x_pixels_per_unit, phys.y_pixels_per_unit,
                    phys.unit_type);
}
#endif

#if defined(PNG_READ_oFFs_SUPPORTED)
void
png_handle_oFFs(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_byte buf[9];
   png_int_32 offset_x, offset_y;
   int unit_type;

   png_debug(1, "in png_handle_oFFs\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before oFFs");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid oFFs after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_oFFs))
   {
      png_warning(png_ptr, "Duplicate oFFs chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (length != 9)
   {
      png_warning(png_ptr, "Incorrect oFFs chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, 9);
   if (png_crc_finish(png_ptr, 0))
      return;

   offset_x = png_get_int_32(buf);
   offset_y = png_get_int_32(buf + 4);
   unit_type = buf[8];
   png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);
}
#endif

#if defined(PNG_READ_pCAL_SUPPORTED)
/* read the pCAL chunk (png-scivis-19970203) */
int
png_read_pCAL(png_structp png_ptr, png_pCAL_tp pcalp, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_charp buf, endptr;
   png_size_t slength;
   int i;

   png_debug1(2, "Allocating and reading pCAL chunk data (%d bytes)\n",
      length + 1);
   pcalp->purpose = (png_charp)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)pcalp->purpose, slength);

   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, pcalp->purpose);
      return(0);
   }

   pcalp->purpose[slength] = 0x00; /* null terminate the last string */

   png_debug(3, "Finding end of pCAL purpose string\n");
   for (buf = pcalp->purpose; *buf; buf++)
      /* empty loop */ ;

   endptr = pcalp->purpose + slength;

   /* We need to have at least 12 bytes after the purpose string
      in order to get the parameter information. */
   if (endptr <= buf + 12)
   {
      png_warning(png_ptr, "Invalid pCAL data");
      png_free(png_ptr, pcalp->purpose);
      return(0);
   }

   png_debug(3, "Reading pCAL X0, X1, type, nparams, and units\n");
   pcalp->X0 = png_get_int_32((png_bytep)buf+1);
   pcalp->X1 = png_get_int_32((png_bytep)buf+5);
   pcalp->type = buf[9];
   pcalp->nparams = buf[10];
   pcalp->units = buf + 11;

   png_debug(3, "Checking pCAL equation type and number of parameters\n");
   /* Check that we have the right number of parameters for known
      equation types. */
   if ((pcalp->type == PNG_EQUATION_LINEAR && pcalp->nparams != 2) ||
       (pcalp->type == PNG_EQUATION_BASE_E && pcalp->nparams != 3) ||
       (pcalp->type == PNG_EQUATION_ARBITRARY && pcalp->nparams != 3) ||
       (pcalp->type == PNG_EQUATION_HYPERBOLIC && pcalp->nparams != 4))
   {
      png_warning(png_ptr, "Invalid pCAL parameters for equation type");
      png_free(png_ptr, pcalp->purpose);
      return(0);
   }
   else if (pcalp->type >= PNG_EQUATION_LAST)
   {
      png_warning(png_ptr, "Unrecognized equation type for pCAL chunk");
   }

   for (buf = pcalp->units; *buf; buf++)
      /* Empty loop to move past the units string. */ ;

   png_debug(3, "Allocating pCAL parameters array\n");
   pcalp->params = (png_charpp)png_malloc(png_ptr, (png_uint_32)(pcalp->nparams
      *sizeof(png_charp))) ;

   /* Get pointers to the start of each parameter string. */
   for (i = 0; i < (int)pcalp->nparams; i++)
   {
      buf++; /* Skip the null string terminator from previous parameter. */

      png_debug1(3, "Reading pCAL parameter %d\n", i);
      for (pcalp->params[i] = buf; *buf != 0x00 && buf <= endptr; buf++)
         /* Empty loop to move past each parameter string */ ;

      /* Make sure we haven't run out of data yet */
      if (buf > endptr)
      {
         png_warning(png_ptr, "Invalid pCAL data");
         png_free(png_ptr, pcalp->purpose);
         png_free(png_ptr, pcalp->params);
         return(0);
      }
   }

   return(1);
}

void
png_handle_pCAL(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_pCAL_t pcal;

   png_debug(1, "in png_handle_pCAL\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before pCAL");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid pCAL after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_pCAL))
   {
      png_warning(png_ptr, "Duplicate pCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_pCAL(png_ptr, &pcal, length))
   {
       png_set_pCAL(png_ptr, info_ptr,
                    pcal.purpose, pcal.X0, pcal.X1,
                    pcal.type, pcal.nparams, pcal.units, pcal.params);

       png_free(png_ptr, pcal.purpose);
       png_free(png_ptr, pcal.params);
   }
}
#endif

#if defined(PNG_READ_sCAL_SUPPORTED)
int
png_read_sCAL(png_structp png_ptr, png_sCAL_tp scalp, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_charp buffer, ep;
#ifdef PNG_FLOATING_POINT_SUPPORTED
   png_charp vp;
#endif
   png_size_t slength;

   png_debug1(2, "Allocating and reading sCAL chunk data (%d bytes)\n",
      length + 1);
   buffer = (png_charp)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)buffer, slength);

   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, buffer);
      return(0);
   }

   scalp->unit = buffer[0];
   buffer[slength] = 0x00; /* null terminate the last string */

   ep = buffer + 1;        /* skip unit byte */

#ifdef PNG_FLOATING_POINT_SUPPORTED
   scalp->pixel_width = scalp->pixel_height = 0.0;
   scalp->pixel_width = strtod(ep, &vp);
   if (*vp)
       png_error(png_ptr, "malformed width string in sCAL chunk");
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   scalp->s_width = (png_charp)png_malloc(png_ptr, strlen(ep) + 1);
   png_memcpy(scalp->s_width, ep, (png_size_t)strlen(ep));
#endif
#endif

   for (ep = buffer; *ep; ep++)
      /* empty loop */ ;
   ep++;

#ifdef PNG_FLOATING_POINT_SUPPORTED
   scalp->pixel_height = strtod(ep, &vp);
   if (*vp)
       png_error(png_ptr, "malformed height string in sCAL chunk");
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   scalp->s_height = (png_charp)png_malloc(png_ptr, strlen(ep) + 1);
   png_memcpy(scalp->s_height, ep, (png_size_t)strlen(ep));
#endif
#endif

   if (buffer + slength < ep
#ifdef PNG_FLOATING_POINT_SUPPORTED
      || scalp->pixel_width <= 0. || scalp->pixel_height <= 0.
#endif
      )
   {
      png_warning(png_ptr, "Invalid sCAL data");
      png_free(png_ptr, buffer);
#if defined(PNG_FIXED_POINT_SUPPORTED) && !defined(PNG_FLOATING_POINT_SUPPORTED)
      png_free(png_ptr, scalp->s_width);
      png_free(png_ptr, scalp->s_height);
#endif
      return(0);
   }

   png_free(png_ptr, buffer);
   return(1);
}

/* read the sCAL chunk */
void
png_handle_sCAL(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_sCAL_t        scal;

   png_debug(1, "in png_handle_sCAL\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sCAL");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sCAL after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sCAL))
   {
      png_warning(png_ptr, "Duplicate sCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_sCAL(png_ptr, &scal, length))
   {
#ifdef PNG_FLOATING_POINT_SUPPORTED
       png_set_sCAL(png_ptr, info_ptr,
          scal.unit, scal.pixel_width, scal.pixel_height);
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
       png_set_sCAL_s(png_ptr, info_ptr,
          scal.unit, scal.s_width, scal.s_height);
#endif
#endif

#if defined(PNG_FIXED_POINT_SUPPORTED) && !defined(PNG_FLOATING_POINT_SUPPORTED)
       png_free(png_ptr, scal.s_width);
       png_free(png_ptr, scal.s_height);
#endif
   }
}
#endif

#if defined(PNG_READ_tIME_SUPPORTED)
int
png_read_tIME(png_structp png_ptr, png_tIME_tp timep, png_uint_32 length)
/* this has no state dependencies on the png_ptr structure */
{
   png_byte buf[7];

   if (png_ptr->mode & PNG_HAVE_IDAT)
      png_ptr->mode |= PNG_AFTER_IDAT;

   if (length != 7)
   {
      png_warning(png_ptr, "Incorrect tIME chunk length");
      png_crc_finish(png_ptr, length);
      return(0);
   }

   png_crc_read(png_ptr, buf, 7);
   if (png_crc_finish(png_ptr, 0))
      return(0);

   timep->second = buf[6];
   timep->minute = buf[5];
   timep->hour = buf[4];
   timep->day = buf[3];
   timep->month = buf[2];
   timep->year = png_get_uint_16(buf);

   return(1);
}

void
png_handle_tIME(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_tIME_t        time;

   png_debug(1, "in png_handle_tIME\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Out of place tIME chunk");
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_tIME))
   {
      png_warning(png_ptr, "Duplicate tIME chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_read_tIME(png_ptr, &time, length))
       png_set_tIME(png_ptr, info_ptr, &time);
}
#endif

#if defined(PNG_READ_tEXt_SUPPORTED)
int
png_read_tEXt(png_structp png_ptr, png_textp textp, png_uint_32 length)
/* this does not properly handle chunks that are > 64K under DOS */
/* this has no state dependencies on the png_ptr structure */
{
   png_charp key, text;
   png_uint_32 skip = 0;
   png_size_t slength;

#ifdef PNG_MAX_MALLOC_64K
   if (length > (png_uint_32)65535L)
   {
      png_warning(png_ptr, "tEXt chunk too large to fit in memory");
      skip = length - (png_uint_32)65535L;
      length = (png_uint_32)65535L;
   }
#endif

   key = (png_charp)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)key, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, key);
      return(0);
   }

   key[slength] = 0x00;

   for (text = key; *text; text++)
      /* empty loop to find end of key */ ;

   if (text != key + slength)
      text++;

   textp->compression = PNG_TEXT_COMPRESSION_NONE;
   textp->key = key;
   textp->lang = NULL;
   textp->lang_key = NULL;
   textp->text = text;
   textp->text_length = png_strlen(text);
   textp->itxt_length = 0;

   return(1);
}

void
png_handle_tEXt(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_text text;

   png_debug(1, "in png_handle_tEXt\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before tEXt");

   if (png_ptr->mode & PNG_HAVE_IDAT)
      png_ptr->mode |= PNG_AFTER_IDAT;

   if (png_read_tEXt(png_ptr, &text, length))
   {
       png_set_text(png_ptr, info_ptr, &text, 1);
       png_free(png_ptr, text.key);
   }
}
#endif

#if defined(PNG_READ_zTXt_SUPPORTED)
int
png_read_zTXt(png_structp png_ptr, png_textp textp, png_uint_32 length)
/* this does not properly handle chunks that are > 64K under DOS */
/* this has no state dependencies on the png_ptr structure */
{
   png_charp chunkdata, text;
   int comp_type = PNG_TEXT_COMPRESSION_NONE;
   png_size_t slength, prefix_len;

#ifdef PNG_MAX_MALLOC_64K
   /* We will no doubt have problems with chunks even half this size, but
      there is no hard and fast rule to tell us where to stop. */
   if (length > (png_uint_32)65535L)
   {
     png_warning(png_ptr,"zTXt chunk too large to fit in memory");
     png_crc_finish(png_ptr, length);
     return(0);
   }
#endif

   chunkdata = (png_charp)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)chunkdata, slength);
   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, chunkdata);
      return(0);
   }

   chunkdata[slength] = 0x00;

   for (text = chunkdata; *text; text++)
      /* empty loop */ ;

   /* zTXt must have some text after the chunkdataword */
   if (text == chunkdata + slength)
   {
      comp_type = PNG_TEXT_COMPRESSION_NONE;
      png_warning(png_ptr, "Zero length zTXt chunk");
   }
   else
   {
       comp_type = *(++text);
       text++;        /* skip the compression_method byte */
   }
   prefix_len = text - chunkdata;

   chunkdata = (png_charp)png_decompress_chunk(png_ptr, comp_type, chunkdata,
                        (png_size_t)length, prefix_len);

   textp->compression = comp_type;
   textp->lang = NULL;
   textp->key = chunkdata;
   textp->lang_key = NULL;
   textp->text = chunkdata + prefix_len;
   textp->text_length = png_strlen(text);
   textp->itxt_length = 0;

   return(1);
}

void
png_handle_zTXt(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_text text;;

   png_debug(1, "in png_handle_zTXt\n");
   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before zTXt");

   if (png_ptr->mode & PNG_HAVE_IDAT)
      png_ptr->mode |= PNG_AFTER_IDAT;

   if (png_read_zTXt(png_ptr, &text, length))
   {
       png_set_text(png_ptr, info_ptr, &text, 1);
       png_free(png_ptr, text.key);
   }
}
#endif

#if defined(PNG_READ_iTXt_SUPPORTED)
int
png_read_iTXt(png_structp png_ptr, png_textp textp, png_uint_32 length)
/* this does not correctly handle chunks that are > 64K under DOS */
/* this has no state dependencies on the png_ptr structure */
{
   png_charp chunkdata;
   png_charp key, lang, text, lang_key;
   int comp_flag = PNG_TEXT_COMPRESSION_NONE;
   int comp_type = 0;
   png_size_t slength, prefix_len;

#ifdef PNG_MAX_MALLOC_64K
   /* We will no doubt have problems with chunks even half this size, but
      there is no hard and fast rule to tell us where to stop. */
   if (length > (png_uint_32)65535L)
   {
     png_warning(png_ptr,"iTXt chunk too large to fit in memory");
     png_crc_finish(png_ptr, length);
     return;
   }
#endif

   chunkdata = (png_charp)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)chunkdata, slength);
   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, chunkdata);
      return(0);
   }

   chunkdata[slength] = 0x00;

   for (lang = chunkdata; *lang; lang++)
      /* empty loop */ ;
   lang++;        /* skip NUL separator */

   /* iTXt must have a language tag (possibly empty), two compression bytes,
      translated keyword (possibly empty), and possibly some text after the
      keyword */

   if (lang >= chunkdata + slength)
   {
      comp_flag = PNG_TEXT_COMPRESSION_NONE;
      png_warning(png_ptr, "Zero length iTXt chunk");
   }
   else
   {
       comp_flag = *lang++;
       comp_type = *lang++;
   }

   for (lang_key = lang; *lang_key; lang_key++)
      /* empty loop */ ;
   lang_key++;        /* skip NUL separator */

   for (text = lang_key; *text; text++)
      /* empty loop */ ;
   text++;        /* skip NUL separator */

   prefix_len = text - chunkdata;

   key=chunkdata;
   if (comp_flag)
       chunkdata = png_decompress_chunk(png_ptr, comp_type, chunkdata,
          (size_t)length, prefix_len);
   textp->compression = (int)comp_flag + 1;
   textp->lang_key = chunkdata+(lang_key-key);
   textp->lang = chunkdata+(lang-key);
   textp->key = chunkdata;
   textp->text = chunkdata + prefix_len;
   textp->text_length = 0;
   textp->itxt_length = png_strlen(textp->text);

   return(1);
}

void
png_handle_iTXt(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_text text;;

   png_debug(1, "in png_handle_iTXt\n");
   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before iTXt");

   if (png_ptr->mode & PNG_HAVE_IDAT)
      png_ptr->mode |= PNG_AFTER_IDAT;

   if (png_read_iTXt(png_ptr, &text, length))
   {
       png_set_text(png_ptr, info_ptr, &text, 1);
       png_free(png_ptr, text.key);
   }
}
#endif

/* This function is called when we haven't found a handler for a
   chunk.  If there isn't a problem with the chunk itself (ie bad
   chunk name, CRC, or a critical chunk), the chunk is silently ignored
   -- unless the PNG_FLAG_UNKNOWN_CHUNKS_SUPPORTED flag is on in which
   case it will be saved away to be written out later. */
void
png_handle_unknown(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_uint_32 skip = 0;

   png_debug(1, "in png_handle_unknown\n");

   if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      if (png_get_uint_32(png_ptr->chunk_name) != PNG_UINT_IDAT)
         png_ptr->mode |= PNG_AFTER_IDAT;
   }

   png_check_chunk_name(png_ptr, png_ptr->chunk_name);

   if (!(png_ptr->chunk_name[0] & 0x20))
   {
      if(png_handle_as_unknown(png_ptr, png_ptr->chunk_name) !=
           HANDLE_CHUNK_ALWAYS
#if defined(PNG_READ_USER_CHUNKS_SUPPORTED)
           && png_ptr->read_user_chunk_fn == (png_user_chunk_ptr)NULL
#endif
        )
          png_chunk_error(png_ptr, "unknown critical chunk");
   }

#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
   if (png_ptr->flags & PNG_FLAG_KEEP_UNKNOWN_CHUNKS)
   {
       png_unknown_chunk chunk;

#ifdef PNG_MAX_MALLOC_64K
       if (length > (png_uint_32)65535L)
       {
           png_warning(png_ptr, "unknown chunk too large to fit in memory");
           skip = length - (png_uint_32)65535L;
           length = (png_uint_32)65535L;
       }
#endif
       strcpy((png_charp)chunk.name, (png_charp)png_ptr->chunk_name);
       chunk.data = (png_bytep)png_malloc(png_ptr, length);
       png_crc_read(png_ptr, chunk.data, length);
       chunk.size = length;
#if defined(PNG_READ_USER_CHUNKS_SUPPORTED)
       if(png_ptr->read_user_chunk_fn != (png_user_chunk_ptr)NULL)
       {
          /* callback to user unknown chunk handler */
          if ((*(png_ptr->read_user_chunk_fn)) (png_ptr, &chunk) <= 0)
          {
             if (!(png_ptr->chunk_name[0] & 0x20))
                if(png_handle_as_unknown(png_ptr, png_ptr->chunk_name) !=
                     HANDLE_CHUNK_ALWAYS)
                   png_chunk_error(png_ptr, "unknown critical chunk");
             png_set_unknown_chunks(png_ptr, info_ptr, &chunk, 1);
          }
       }
       else
#endif
          png_set_unknown_chunks(png_ptr, info_ptr, &chunk, 1);
       png_free(png_ptr, chunk.data);
   }
   else
#endif
      skip = length;

   png_crc_finish(png_ptr, skip);

#if !defined(PNG_READ_USER_CHUNKS_SUPPORTED)
   if (info_ptr == NULL)
     /* quiet compiler warnings about unused info_ptr */ ;
#endif
}

/* This function is called to verify that a chunk name is valid.
   This function can't have the "critical chunk check" incorporated
   into it, since in the future we will need to be able to call user
   functions to handle unknown critical chunks after we check that
   the chunk name itself is valid. */

#define isnonalpha(c) ((c) < 41 || (c) > 122 || ((c) > 90 && (c) < 97))

void
png_check_chunk_name(png_structp png_ptr, png_bytep chunk_name)
{
   png_debug(1, "in png_check_chunk_name\n");
   if (isnonalpha(chunk_name[0]) || isnonalpha(chunk_name[1]) ||
       isnonalpha(chunk_name[2]) || isnonalpha(chunk_name[3]))
   {
      png_chunk_error(png_ptr, "invalid chunk type");
   }
}

/* Combines the row recently read in with the existing pixels in the
   row.  This routine takes care of alpha and transparency if requested.
   This routine also handles the two methods of progressive display
   of interlaced images, depending on the mask value.
   The mask value describes which pixels are to be combined with
   the row.  The pattern always repeats every 8 pixels, so just 8
   bits are needed.  A one indicates the pixel is to be combined,
   a zero indicates the pixel is to be skipped.  This is in addition
   to any alpha or transparency value associated with the pixel.  If
   you want all pixels to be combined, pass 0xff (255) in mask.  */
void
#ifdef PNG_HAVE_ASSEMBLER_COMBINE_ROW
png_combine_row_c
#else
png_combine_row
#endif /* PNG_HAVE_ASSEMBLER_COMBINE_ROW */
   (png_structp png_ptr, png_bytep row, int mask)
{
   png_debug(1,"in png_combine_row\n");
   if (mask == 0xff)
   {
      png_memcpy(row, png_ptr->row_buf + 1,
         (png_size_t)((png_ptr->width *
         png_ptr->row_info.pixel_depth + 7) >> 3));
   }
   else
   {
      switch (png_ptr->row_info.pixel_depth)
      {
         case 1:
         {
            png_bytep sp = png_ptr->row_buf + 1;
            png_bytep dp = row;
            int s_inc, s_start, s_end;
            int m = 0x80;
            int shift;
            png_uint_32 i;
            png_uint_32 row_width = png_ptr->width;

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
            if (png_ptr->transformations & PNG_PACKSWAP)
            {
                s_start = 0;
                s_end = 7;
                s_inc = 1;
            }
            else
#endif
            {
                s_start = 7;
                s_end = 0;
                s_inc = -1;
            }

            shift = s_start;

            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  int value;

                  value = (*sp >> shift) & 0x01;
                  *dp &= (png_byte)((0x7f7f >> (7 - shift)) & 0xff);
                  *dp |= (png_byte)(value << shift);
               }

               if (shift == s_end)
               {
                  shift = s_start;
                  sp++;
                  dp++;
               }
               else
                  shift += s_inc;

               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
         case 2:
         {
            png_bytep sp = png_ptr->row_buf + 1;
            png_bytep dp = row;
            int s_start, s_end, s_inc;
            int m = 0x80;
            int shift;
            png_uint_32 i;
            png_uint_32 row_width = png_ptr->width;
            int value;

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
            if (png_ptr->transformations & PNG_PACKSWAP)
            {
               s_start = 0;
               s_end = 6;
               s_inc = 2;
            }
            else
#endif
            {
               s_start = 6;
               s_end = 0;
               s_inc = -2;
            }

            shift = s_start;

            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  value = (*sp >> shift) & 0x03;
                  *dp &= (png_byte)((0x3f3f >> (6 - shift)) & 0xff);
                  *dp |= (png_byte)(value << shift);
               }

               if (shift == s_end)
               {
                  shift = s_start;
                  sp++;
                  dp++;
               }
               else
                  shift += s_inc;
               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
         case 4:
         {
            png_bytep sp = png_ptr->row_buf + 1;
            png_bytep dp = row;
            int s_start, s_end, s_inc;
            int m = 0x80;
            int shift;
            png_uint_32 i;
            png_uint_32 row_width = png_ptr->width;
            int value;

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
            if (png_ptr->transformations & PNG_PACKSWAP)
            {
               s_start = 0;
               s_end = 4;
               s_inc = 4;
            }
            else
#endif
            {
               s_start = 4;
               s_end = 0;
               s_inc = -4;
            }
            shift = s_start;

            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  value = (*sp >> shift) & 0xf;
                  *dp &= (png_byte)((0xf0f >> (4 - shift)) & 0xff);
                  *dp |= (png_byte)(value << shift);
               }

               if (shift == s_end)
               {
                  shift = s_start;
                  sp++;
                  dp++;
               }
               else
                  shift += s_inc;
               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
         default:
         {
            png_bytep sp = png_ptr->row_buf + 1;
            png_bytep dp = row;
            png_size_t pixel_bytes = (png_ptr->row_info.pixel_depth >> 3);
            png_uint_32 i;
            png_uint_32 row_width = png_ptr->width;
            png_byte m = 0x80;


            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  png_memcpy(dp, sp, pixel_bytes);
               }

               sp += pixel_bytes;
               dp += pixel_bytes;

               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
      }
   }
}

#if defined(PNG_READ_INTERLACING_SUPPORTED)
void
#ifdef PNG_HAVE_ASSEMBLER_READ_INTERLACE
png_do_read_interlace_c
#else
png_do_read_interlace
#endif /* PNG_HAVE_ASSEMBLER_READ_INTERLACE */
   (png_row_infop row_info, png_bytep row, int pass,
   png_uint_32 transformations)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   /* arrays to facilitate easy interlacing - use pass (0 - 6) as index */

   /* offset to next interlace block */
   const int png_pass_inc[7] = {8, 8, 4, 4, 2, 2, 1};
#endif

   png_debug(1,"in png_do_read_interlace\n");
   if (row != NULL && row_info != NULL)
   {
      png_uint_32 final_width;

      final_width = row_info->width * png_pass_inc[pass];

      switch (row_info->pixel_depth)
      {
         case 1:
         {
            png_bytep sp = row + (png_size_t)((row_info->width - 1) >> 3);
            png_bytep dp = row + (png_size_t)((final_width - 1) >> 3);
            int sshift, dshift;
            int s_start, s_end, s_inc;
            int jstop = png_pass_inc[pass];
            png_byte v;
            png_uint_32 i;
            int j;

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
            if (transformations & PNG_PACKSWAP)
            {
                sshift = (int)((row_info->width + 7) & 0x07);
                dshift = (int)((final_width + 7) & 0x07);
                s_start = 7;
                s_end = 0;
                s_inc = -1;
            }
            else
#endif
            {
                sshift = 7 - (int)((row_info->width + 7) & 0x07);
                dshift = 7 - (int)((final_width + 7) & 0x07);
                s_start = 0;
                s_end = 7;
                s_inc = 1;
            }

            for (i = 0; i < row_info->width; i++)
            {
               v = (png_byte)((*sp >> sshift) & 0x01);
               for (j = 0; j < jstop; j++)
               {
                  *dp &= (png_byte)((0x7f7f >> (7 - dshift)) & 0xff);
                  *dp |= (png_byte)(v << dshift);
                  if (dshift == s_end)
                  {
                     dshift = s_start;
                     dp--;
                  }
                  else
                     dshift += s_inc;
               }
               if (sshift == s_end)
               {
                  sshift = s_start;
                  sp--;
               }
               else
                  sshift += s_inc;
            }
            break;
         }
         case 2:
         {
            png_bytep sp = row + (png_uint_32)((row_info->width - 1) >> 2);
            png_bytep dp = row + (png_uint_32)((final_width - 1) >> 2);
            int sshift, dshift;
            int s_start, s_end, s_inc;
            int jstop = png_pass_inc[pass];
            png_uint_32 i;

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
            if (transformations & PNG_PACKSWAP)
            {
               sshift = (int)(((row_info->width + 3) & 0x03) << 1);
               dshift = (int)(((final_width + 3) & 0x03) << 1);
               s_start = 6;
               s_end = 0;
               s_inc = -2;
            }
            else
#endif
            {
               sshift = (int)((3 - ((row_info->width + 3) & 0x03)) << 1);
               dshift = (int)((3 - ((final_width + 3) & 0x03)) << 1);
               s_start = 0;
               s_end = 6;
               s_inc = 2;
            }

            for (i = 0; i < row_info->width; i++)
            {
               png_byte v;
               int j;

               v = (png_byte)((*sp >> sshift) & 0x03);
               for (j = 0; j < jstop; j++)
               {
                  *dp &= (png_byte)((0x3f3f >> (6 - dshift)) & 0xff);
                  *dp |= (png_byte)(v << dshift);
                  if (dshift == s_end)
                  {
                     dshift = s_start;
                     dp--;
                  }
                  else
                     dshift += s_inc;
               }
               if (sshift == s_end)
               {
                  sshift = s_start;
                  sp--;
               }
               else
                  sshift += s_inc;
            }
            break;
         }
         case 4:
         {
            png_bytep sp = row + (png_size_t)((row_info->width - 1) >> 1);
            png_bytep dp = row + (png_size_t)((final_width - 1) >> 1);
            int sshift, dshift;
            int s_start, s_end, s_inc;
            png_uint_32 i;
            int jstop = png_pass_inc[pass];

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
            if (transformations & PNG_PACKSWAP)
            {
               sshift = (int)(((row_info->width + 1) & 0x01) << 2);
               dshift = (int)(((final_width + 1) & 0x01) << 2);
               s_start = 4;
               s_end = 0;
               s_inc = -4;
            }
            else
#endif
            {
               sshift = (int)((1 - ((row_info->width + 1) & 0x01)) << 2);
               dshift = (int)((1 - ((final_width + 1) & 0x01)) << 2);
               s_start = 0;
               s_end = 4;
               s_inc = 4;
            }

            for (i = 0; i < row_info->width; i++)
            {
               png_byte v = (png_byte)((*sp >> sshift) & 0xf);
               int j;

               for (j = 0; j < jstop; j++)
               {
                  *dp &= (png_byte)((0xf0f >> (4 - dshift)) & 0xff);
                  *dp |= (png_byte)(v << dshift);
                  if (dshift == s_end)
                  {
                     dshift = s_start;
                     dp--;
                  }
                  else
                     dshift += s_inc;
               }
               if (sshift == s_end)
               {
                  sshift = s_start;
                  sp--;
               }
               else
                  sshift += s_inc;
            }
            break;
         }
         default:
         {
            png_size_t pixel_bytes = (row_info->pixel_depth >> 3);
            png_bytep sp = row + (png_size_t)(row_info->width - 1) * pixel_bytes;
            png_bytep dp = row + (png_size_t)(final_width - 1) * pixel_bytes;

            int jstop = png_pass_inc[pass];
            png_uint_32 i;

            for (i = 0; i < row_info->width; i++)
            {
               png_byte v[8];
               int j;

               png_memcpy(v, sp, pixel_bytes);
               for (j = 0; j < jstop; j++)
               {
                  png_memcpy(dp, v, pixel_bytes);
                  dp -= pixel_bytes;
               }
               sp -= pixel_bytes;
            }
            break;
         }
      }
      row_info->width = final_width;
      row_info->rowbytes = ((final_width *
         (png_uint_32)row_info->pixel_depth + 7) >> 3);
   }
}
#endif

void
#ifdef PNG_HAVE_ASSEMBLER_READ_FILTER_ROW
png_read_filter_row_c
#else
png_read_filter_row
#endif /* PNG_HAVE_ASSEMBLER_READ_FILTER_ROW */
   (png_structp png_ptr, png_row_infop row_info, png_bytep row,
   png_bytep prev_row, int filter)
{
   png_debug(1, "in png_read_filter_row\n");
   png_debug2(2,"row = %d, filter = %d\n", png_ptr->row_number, filter);
   switch (filter)
   {
      case PNG_FILTER_VALUE_NONE:
         break;
      case PNG_FILTER_VALUE_SUB:
      {
         png_uint_32 i;
         png_uint_32 istop = row_info->rowbytes;
         png_uint_32 bpp = (row_info->pixel_depth + 7) >> 3;
         png_bytep rp = row + bpp;
         png_bytep lp = row;

         for (i = bpp; i < istop; i++)
         {
            *rp = (png_byte)(((int)(*rp) + (int)(*lp++)) & 0xff);
            rp++;
         }
         break;
      }
      case PNG_FILTER_VALUE_UP:
      {
         png_uint_32 i;
         png_uint_32 istop = row_info->rowbytes;
         png_bytep rp = row;
         png_bytep pp = prev_row;

         for (i = 0; i < istop; i++)
         {
            *rp = (png_byte)(((int)(*rp) + (int)(*pp++)) & 0xff);
            rp++;
         }
         break;
      }
      case PNG_FILTER_VALUE_AVG:
      {
         png_uint_32 i;
         png_bytep rp = row;
         png_bytep pp = prev_row;
         png_bytep lp = row;
         png_uint_32 bpp = (row_info->pixel_depth + 7) >> 3;
         png_uint_32 istop = row_info->rowbytes - bpp;

         for (i = 0; i < bpp; i++)
         {
            *rp = (png_byte)(((int)(*rp) +
               ((int)(*pp++) / 2 )) & 0xff);
            rp++;
         }

         for (i = 0; i < istop; i++)
         {
            *rp = (png_byte)(((int)(*rp) +
               (int)(*pp++ + *lp++) / 2 ) & 0xff);
            rp++;
         }
         break;
      }
      case PNG_FILTER_VALUE_PAETH:
      {
         png_uint_32 i;
         png_bytep rp = row;
         png_bytep pp = prev_row;
         png_bytep lp = row;
         png_bytep cp = prev_row;
         png_uint_32 bpp = (row_info->pixel_depth + 7) >> 3;
         png_uint_32 istop=row_info->rowbytes - bpp;

         for (i = 0; i < bpp; i++)
         {
            *rp = (png_byte)(((int)(*rp) + (int)(*pp++)) & 0xff);
            rp++;
         }

         for (i = 0; i < istop; i++)   /* use leftover rp,pp */
         {
            int a, b, c, pa, pb, pc, p;

            a = *lp++;
            b = *pp++;
            c = *cp++;

            p = b - c;
            pc = a - c;

#ifdef PNG_USE_ABS
            pa = abs(p);
            pb = abs(pc);
            pc = abs(p + pc);
#else
            pa = p < 0 ? -p : p;
            pb = pc < 0 ? -pc : pc;
            pc = (p + pc) < 0 ? -(p + pc) : p + pc;
#endif

            /*
               if (pa <= pb && pa <= pc)
                  p = a;
               else if (pb <= pc)
                  p = b;
               else
                  p = c;
             */

            p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

            *rp = (png_byte)(((int)(*rp) + p) & 0xff);
            rp++;
         }
         break;
      }
      default:
         png_warning(png_ptr, "Ignoring bad adaptive filter type");
         *row=0;
         break;
   }
}

void
png_read_finish_row(png_structp png_ptr)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   /* arrays to facilitate easy interlacing - use pass (0 - 6) as index */

   /* start of interlace block */
   const int png_pass_start[7] = {0, 4, 0, 2, 0, 1, 0};

   /* offset to next interlace block */
   const int png_pass_inc[7] = {8, 8, 4, 4, 2, 2, 1};

   /* start of interlace block in the y direction */
   const int png_pass_ystart[7] = {0, 0, 4, 0, 2, 0, 1};

   /* offset to next interlace block in the y direction */
   const int png_pass_yinc[7] = {8, 8, 8, 4, 4, 2, 2};
#endif

   png_debug(1, "in png_read_finish_row\n");
   png_ptr->row_number++;
   if (png_ptr->row_number < png_ptr->num_rows)
      return;

   if (png_ptr->interlaced)
   {
      png_ptr->row_number = 0;
      png_memset_check(png_ptr, png_ptr->prev_row, 0, png_ptr->rowbytes + 1);
      do
      {
         png_ptr->pass++;
         if (png_ptr->pass >= 7)
            break;
         png_ptr->iwidth = (png_ptr->width +
            png_pass_inc[png_ptr->pass] - 1 -
            png_pass_start[png_ptr->pass]) /
            png_pass_inc[png_ptr->pass];
            png_ptr->irowbytes = ((png_ptr->iwidth *
               (png_uint_32)png_ptr->pixel_depth + 7) >> 3) +1;

         if (!(png_ptr->transformations & PNG_INTERLACE))
         {
            png_ptr->num_rows = (png_ptr->height +
               png_pass_yinc[png_ptr->pass] - 1 -
               png_pass_ystart[png_ptr->pass]) /
               png_pass_yinc[png_ptr->pass];
            if (!(png_ptr->num_rows))
               continue;
         }
         else  /* if (png_ptr->transformations & PNG_INTERLACE) */
            break;
      } while (png_ptr->iwidth == 0);

      if (png_ptr->pass < 7)
         return;
   }

   if (!(png_ptr->flags & PNG_FLAG_ZLIB_FINISHED))
   {
      char extra;
      int ret;

      png_ptr->zstream.next_out = (Byte *)&extra;
      png_ptr->zstream.avail_out = (uInt)1;
      for(;;)
      {
         if (!(png_ptr->zstream.avail_in))
         {
            while (!png_ptr->idat_size)
            {
               png_byte chunk_length[4];

               png_crc_finish(png_ptr, 0);

               png_read_data(png_ptr, chunk_length, 4);
               png_ptr->idat_size = png_get_uint_32(chunk_length);

               png_reset_crc(png_ptr);
               png_crc_read(png_ptr, png_ptr->chunk_name, 4);
               if (png_get_uint_32(png_ptr->chunk_name) != PNG_UINT_IDAT)
                  png_error(png_ptr, "Not enough image data");

            }
            png_ptr->zstream.avail_in = (uInt)png_ptr->zbuf_size;
            png_ptr->zstream.next_in = png_ptr->zbuf;
            if (png_ptr->zbuf_size > png_ptr->idat_size)
               png_ptr->zstream.avail_in = (uInt)png_ptr->idat_size;
            png_crc_read(png_ptr, png_ptr->zbuf, png_ptr->zstream.avail_in);
            png_ptr->idat_size -= png_ptr->zstream.avail_in;
         }
         ret = inflate(&png_ptr->zstream, Z_PARTIAL_FLUSH);
         if (ret == Z_STREAM_END)
         {
            if (!(png_ptr->zstream.avail_out) || png_ptr->zstream.avail_in ||
               png_ptr->idat_size)
               png_error(png_ptr, "Extra compressed data");
            png_ptr->mode |= PNG_AFTER_IDAT;
            png_ptr->flags |= PNG_FLAG_ZLIB_FINISHED;
            break;
         }
         if (ret != Z_OK)
            png_error(png_ptr, png_ptr->zstream.msg ? png_ptr->zstream.msg :
                      "Decompression Error");

         if (!(png_ptr->zstream.avail_out))
            png_error(png_ptr, "Extra compressed data");

      }
      png_ptr->zstream.avail_out = 0;
   }

   if (png_ptr->idat_size || png_ptr->zstream.avail_in)
      png_error(png_ptr, "Extra compression data");

   inflateReset(&png_ptr->zstream);

   png_ptr->mode |= PNG_AFTER_IDAT;
}

void
png_read_start_row(png_structp png_ptr)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   /* arrays to facilitate easy interlacing - use pass (0 - 6) as index */

   /* start of interlace block */
   const int png_pass_start[7] = {0, 4, 0, 2, 0, 1, 0};

   /* offset to next interlace block */
   const int png_pass_inc[7] = {8, 8, 4, 4, 2, 2, 1};

   /* start of interlace block in the y direction */
   const int png_pass_ystart[7] = {0, 0, 4, 0, 2, 0, 1};

   /* offset to next interlace block in the y direction */
   const int png_pass_yinc[7] = {8, 8, 8, 4, 4, 2, 2};
#endif

   int max_pixel_depth;
   png_uint_32 row_bytes;

   png_debug(1, "in png_read_start_row\n");
   png_ptr->zstream.avail_in = 0;
   png_init_read_transformations(png_ptr);
   if (png_ptr->interlaced)
   {
      if (!(png_ptr->transformations & PNG_INTERLACE))
         png_ptr->num_rows = (png_ptr->height + png_pass_yinc[0] - 1 -
            png_pass_ystart[0]) / png_pass_yinc[0];
      else
         png_ptr->num_rows = png_ptr->height;

      png_ptr->iwidth = (png_ptr->width +
         png_pass_inc[png_ptr->pass] - 1 -
         png_pass_start[png_ptr->pass]) /
         png_pass_inc[png_ptr->pass];

         row_bytes = ((png_ptr->iwidth *
            (png_uint_32)png_ptr->pixel_depth + 7) >> 3) +1;
         png_ptr->irowbytes = (png_size_t)row_bytes;
         if((png_uint_32)png_ptr->irowbytes != row_bytes)
            png_error(png_ptr, "Rowbytes overflow in png_read_start_row");
   }
   else
   {
      png_ptr->num_rows = png_ptr->height;
      png_ptr->iwidth = png_ptr->width;
      png_ptr->irowbytes = png_ptr->rowbytes + 1;
   }
   max_pixel_depth = png_ptr->pixel_depth;

#if defined(PNG_READ_PACK_SUPPORTED)
   if ((png_ptr->transformations & PNG_PACK) && png_ptr->bit_depth < 8)
      max_pixel_depth = 8;
#endif

#if defined(PNG_READ_EXPAND_SUPPORTED)
   if (png_ptr->transformations & PNG_EXPAND)
   {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      {
         if (png_ptr->num_trans)
            max_pixel_depth = 32;
         else
            max_pixel_depth = 24;
      }
      else if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY)
      {
         if (max_pixel_depth < 8)
            max_pixel_depth = 8;
         if (png_ptr->num_trans)
            max_pixel_depth *= 2;
      }
      else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
      {
         if (png_ptr->num_trans)
         {
            max_pixel_depth *= 4;
            max_pixel_depth /= 3;
         }
      }
   }
#endif

#if defined(PNG_READ_FILLER_SUPPORTED)
   if (png_ptr->transformations & (PNG_FILLER))
   {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
         max_pixel_depth = 32;
      else if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY)
      {
         if (max_pixel_depth <= 8)
            max_pixel_depth = 16;
         else
            max_pixel_depth = 32;
      }
      else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
      {
         if (max_pixel_depth <= 32)
            max_pixel_depth = 32;
         else
            max_pixel_depth = 64;
      }
   }
#endif

#if defined(PNG_READ_GRAY_TO_RGB_SUPPORTED)
   if (png_ptr->transformations & PNG_GRAY_TO_RGB)
   {
      if (
#if defined(PNG_READ_EXPAND_SUPPORTED)
        (png_ptr->num_trans && (png_ptr->transformations & PNG_EXPAND)) ||
#endif
#if defined(PNG_READ_FILLER_SUPPORTED)
        (png_ptr->transformations & (PNG_FILLER)) ||
#endif
        png_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      {
         if (max_pixel_depth <= 16)
            max_pixel_depth = 32;
         else
            max_pixel_depth = 64;
      }
      else
      {
         if (max_pixel_depth <= 8)
           {
             if (png_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
               max_pixel_depth = 32;
             else
               max_pixel_depth = 24;
           }
         else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            max_pixel_depth = 64;
         else
            max_pixel_depth = 48;
      }
   }
#endif

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
   if(png_ptr->transformations & PNG_USER_TRANSFORM)
     {
       int user_pixel_depth=png_ptr->user_transform_depth*
         png_ptr->user_transform_channels;
       if(user_pixel_depth > max_pixel_depth)
         max_pixel_depth=user_pixel_depth;
     }
#endif

   /* align the width on the next larger 8 pixels.  Mainly used
      for interlacing */
   row_bytes = ((png_ptr->width + 7) & ~((png_uint_32)7));
   /* calculate the maximum bytes needed, adding a byte and a pixel
      for safety's sake */
   row_bytes = ((row_bytes * (png_uint_32)max_pixel_depth + 7) >> 3) +
      1 + ((max_pixel_depth + 7) >> 3);
#ifdef PNG_MAX_MALLOC_64K
   if (row_bytes > (png_uint_32)65536L)
      png_error(png_ptr, "This image requires a row greater than 64KB");
#endif
   png_ptr->row_buf = (png_bytep)png_malloc(png_ptr, row_bytes);

#ifdef PNG_MAX_MALLOC_64K
   if ((png_uint_32)png_ptr->rowbytes + 1 > (png_uint_32)65536L)
      png_error(png_ptr, "This image requires a row greater than 64KB");
#endif
   png_ptr->prev_row = (png_bytep)png_malloc(png_ptr, (png_uint_32)(
      png_ptr->rowbytes + 1));

   png_memset_check(png_ptr, png_ptr->prev_row, 0, png_ptr->rowbytes + 1);

   png_debug1(3, "width = %d,\n", png_ptr->width);
   png_debug1(3, "height = %d,\n", png_ptr->height);
   png_debug1(3, "iwidth = %d,\n", png_ptr->iwidth);
   png_debug1(3, "num_rows = %d\n", png_ptr->num_rows);
   png_debug1(3, "rowbytes = %d,\n", png_ptr->rowbytes);
   png_debug1(3, "irowbytes = %d,\n", png_ptr->irowbytes);

   png_ptr->flags |= PNG_FLAG_ROW_INIT;
}
