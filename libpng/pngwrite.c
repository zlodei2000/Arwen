
/* pngwrite.c - general routines to write a PNG file
 *
 * libpng 1.1.0e - February 19, 2000
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
 * Copyright (c) 1996, 1997 Andreas Dilger
 * Copyright (c) 1998, 1999, 2000 Glenn Randers-Pehrson
 */

/* get internal access to png.h */
#define PNG_INTERNAL
#include "png.h"

/* Writes all the PNG information.  This is the suggested way to use the
 * library.  If you have a new chunk to add, make a function to write it,
 * and put it in the correct location here.  If you want the chunk written
 * after the image data, put it in png_write_end().  I strongly encourage
 * you to supply a PNG_INFO_ flag, and check info_ptr->valid before writing
 * the chunk, as that will keep the code from breaking if you want to just
 * write a plain PNG file.  If you have long comments, I suggest writing
 * them in png_write_end(), and compressing them.
 */
void
png_write_info_before_PLTE(png_structp png_ptr, png_infop info_ptr)
{
   png_debug(1, "in png_write_info_before_PLTE\n");
   if (!(png_ptr->mode & PNG_WROTE_INFO_BEFORE_PLTE))
   {
   png_write_sig(png_ptr); /* write PNG signature */
   /* write IHDR information. */
   png_write_IHDR(png_ptr, info_ptr->width, info_ptr->height,
      info_ptr->bit_depth, info_ptr->color_type, info_ptr->compression_type,
      info_ptr->filter_type,
#if defined(PNG_WRITE_INTERLACING_SUPPORTED)
      info_ptr->interlace_type);
#else
      0);
#endif
   /* the rest of these check to see if the valid field has the appropriate
      flag set, and if it does, writes the chunk. */
#if defined(PNG_WRITE_gAMA_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_gAMA)
   {
#  ifdef PNG_FLOATING_POINT_SUPPORTED
      png_write_gAMA(png_ptr, info_ptr->gamma.float_gamma);
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
      png_write_gAMA_fixed(png_ptr, info_ptr->gamma.int_gamma);
#  endif
#endif
   }
#endif
#if defined(PNG_WRITE_sRGB_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sRGB)
      png_write_sRGB(png_ptr, (int)info_ptr->srgb.intent);
#endif
#if defined(PNG_WRITE_iCCP_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_iCCP)
      png_write_iCCP(png_ptr, info_ptr->iccp.name, PNG_TEXT_COMPRESSION_NONE,
                     info_ptr->iccp.profile, (int)info_ptr->iccp.proflen);
#endif
#if defined(PNG_WRITE_sBIT_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sBIT)
      png_write_sBIT(png_ptr, &(info_ptr->sig_bit), info_ptr->color_type);
#endif
#if defined(PNG_WRITE_cHRM_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_cHRM)
   {
#ifdef PNG_FLOATING_POINT_SUPPORTED
      png_write_cHRM(png_ptr,
         info_ptr->chrm.x_white, info_ptr->chrm.y_white,
         info_ptr->chrm.x_red, info_ptr->chrm.y_red,
         info_ptr->chrm.x_green, info_ptr->chrm.y_green,
         info_ptr->chrm.x_blue, info_ptr->chrm.y_blue);
#else
#  ifdef PNG_FIXED_POINT_SUPPORTED
      png_write_cHRM_fixed(png_ptr,
         info_ptr->chrm.int_x_white, info_ptr->chrm.int_y_white,
         info_ptr->chrm.int_x_red, info_ptr->chrm.int_y_red,
         info_ptr->chrm.int_x_green, info_ptr->chrm.int_y_green,
         info_ptr->chrm.int_x_blue, info_ptr->chrm.int_y_blue);
#  endif
#endif
   }
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   if (info_ptr->unknown_chunks_num)
   {
       png_unknown_chunk *up;

       png_debug(5, "writing extra chunks\n");

       for (up = info_ptr->unknown_chunks;
            up < info_ptr->unknown_chunks + info_ptr->unknown_chunks_num;
            up++)
       {
         int keep=png_handle_as_unknown(png_ptr, up->name);
         if (keep != HANDLE_CHUNK_NEVER &&
            up->location && (!(up->location & PNG_HAVE_PLTE)) &&
            ((up->name[3] & 0x20) || keep == HANDLE_CHUNK_ALWAYS ||
            (png_ptr->flags & PNG_FLAG_KEEP_UNSAFE_CHUNKS)))
         {
            png_write_chunk(png_ptr, png_get_uint_32(up->name), up->data,
               up->size);
         }
       }
   }
#endif
      png_ptr->mode |= PNG_WROTE_INFO_BEFORE_PLTE;
   }
}

void
png_write_info(png_structp png_ptr, png_infop info_ptr)
{
#if defined(PNG_WRITE_TEXT_SUPPORTED) || defined(PNG_WRITE_sPLT_SUPPORTED)
   int i;
#endif

   png_debug(1, "in png_write_info\n");

   png_write_info_before_PLTE(png_ptr, info_ptr);

   if (info_ptr->valid & PNG_INFO_PLTE)
      png_write_PLTE(png_ptr, info_ptr->palette.colors,
         (png_uint_32)info_ptr->palette.size);
   else if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      png_error(png_ptr, "Valid palette required for paletted images\n");

#if defined(PNG_WRITE_tRNS_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_tRNS)
      {
#if defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
         /* invert the alpha channel (in tRNS) */
         if ((png_ptr->transformations & PNG_INVERT_ALPHA) &&
            info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
         {
            int j;
            for (j=0; j<(int)info_ptr->num_trans; j++)
               info_ptr->trans[j] = (png_byte)(255 - info_ptr->trans[j]);
         }
#endif
      png_write_tRNS(png_ptr, info_ptr->trans, &(info_ptr->trans_values),
         info_ptr->num_trans, info_ptr->color_type);
      }
#endif
#if defined(PNG_WRITE_bKGD_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_bKGD)
      png_write_bKGD(png_ptr, &(info_ptr->background), info_ptr->color_type);
#endif
#if defined(PNG_WRITE_hIST_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_hIST)
      png_write_hIST(png_ptr, info_ptr->hist.frequencies, info_ptr->palette.size);
#endif
#if defined(PNG_WRITE_oFFs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_oFFs)
      png_write_oFFs(png_ptr, info_ptr->offset.x, info_ptr->offset.y,
         info_ptr->offset.unit_type);
#endif
#if defined(PNG_WRITE_pCAL_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pCAL)
      png_write_pCAL(png_ptr, info_ptr->pcal.purpose, info_ptr->pcal.X0,
         info_ptr->pcal.X1, info_ptr->pcal.type, info_ptr->pcal.nparams,
         info_ptr->pcal.units, info_ptr->pcal.params);
#endif
#if defined(PNG_WRITE_sCAL_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sCAL)
#ifdef PNG_FLOATING_POINT_SUPPORTED
      png_write_sCAL(png_ptr, (int)info_ptr->scal.unit,
          info_ptr->scal.pixel_width, info_ptr->scal.pixel_height);
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
      png_write_sCAL_s(png_ptr, (int)info_ptr->scal.unit,
          info_ptr->scal.s_width, info_ptr->scal.s_height);
#endif
#endif
#endif
#if defined(PNG_WRITE_pHYs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pHYs)
      png_write_pHYs(png_ptr, info_ptr->phys.x_pixels_per_unit,
         info_ptr->phys.y_pixels_per_unit, info_ptr->phys.unit_type);
#endif
#if defined(PNG_WRITE_tIME_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_tIME)
   {
      png_write_tIME(png_ptr, &(info_ptr->mod_time));
      png_ptr->mode |= PNG_WROTE_tIME;
   }
#endif
#if defined(PNG_WRITE_sPLT_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sPLT)
     for (i = 0; i < (int)info_ptr->splt.palettes_num; i++)
       png_write_sPLT(png_ptr, info_ptr->splt.palettes + i);
#endif
#if defined(PNG_WRITE_TEXT_SUPPORTED)
   /* Check to see if we need to write text chunks */
   for (i = 0; i < info_ptr->annotations.num_text; i++)
   {
      png_debug2(2, "Writing header text chunk %d, type %d\n", i,
         info_ptr->annotations.text[i].compression);
      /* an internationalized chunk? */
      if (info_ptr->annotations.text[i].compression > 0)
      {
#if defined(PNG_WRITE_iTXt_SUPPORTED)
          /* write international chunk */
          png_write_iTXt(png_ptr,
                         info_ptr->annotations.text[i].compression,
                         info_ptr->annotations.text[i].key,
                         info_ptr->annotations.text[i].lang,
                         info_ptr->annotations.text[i].lang_key,
                         info_ptr->annotations.text[i].text);
#else
          png_warning(png_ptr, "Unable to write international text\n");
#endif
          /* Mark this chunk as written */
          info_ptr->annotations.text[i].compression = PNG_TEXT_COMPRESSION_NONE_WR;
      }
      /* If we want a compressed text chunk */
      else if (info_ptr->annotations.text[i].compression == PNG_TEXT_COMPRESSION_zTXt)
      {
#if defined(PNG_WRITE_zTXt_SUPPORTED)
         /* write compressed chunk */
         png_write_zTXt(png_ptr, info_ptr->annotations.text[i].key,
            info_ptr->annotations.text[i].text, 0,
            info_ptr->annotations.text[i].compression);
#else
         png_warning(png_ptr, "Unable to write compressed text\n");
#endif
         /* Mark this chunk as written */
         info_ptr->annotations.text[i].compression = PNG_TEXT_COMPRESSION_zTXt_WR;
      }
      else if (info_ptr->annotations.text[i].compression == PNG_TEXT_COMPRESSION_NONE)
      {
#if defined(PNG_WRITE_tEXt_SUPPORTED)
         /* write uncompressed chunk */
         png_write_tEXt(png_ptr, info_ptr->annotations.text[i].key,
                         info_ptr->annotations.text[i].text,
                         0);
#else
         png_warning(png_ptr, "Unable to write uncompressed text\n");
#endif
         /* Mark this chunk as written */
         info_ptr->annotations.text[i].compression = PNG_TEXT_COMPRESSION_NONE_WR;
      }
   }
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   if (info_ptr->unknown_chunks_num)
   {
       png_unknown_chunk *up;

       png_debug(5, "writing extra chunks\n");

       for (up = info_ptr->unknown_chunks;
            up < info_ptr->unknown_chunks + info_ptr->unknown_chunks_num;
            up++)
       {
         int keep=png_handle_as_unknown(png_ptr, up->name);
         if (keep != HANDLE_CHUNK_NEVER &&
            up->location && (up->location & PNG_HAVE_PLTE) &&
            !(up->location & PNG_HAVE_IDAT) &&
            ((up->name[3] & 0x20) || keep == HANDLE_CHUNK_ALWAYS ||
            (png_ptr->flags & PNG_FLAG_KEEP_UNSAFE_CHUNKS)))
         {
            png_write_chunk(png_ptr, png_get_uint_32(up->name), up->data,
               up->size);
         }
       }
   }
#endif
}

/* Writes the end of the PNG file.  If you don't want to write comments or
 * time information, you can pass NULL for info.  If you already wrote these
 * in png_write_info(), do not write them again here.  If you have long
 * comments, I suggest writing them here, and compressing them.
 */
void
png_write_end(png_structp png_ptr, png_infop info_ptr)
{
   png_debug(1, "in png_write_end\n");
   if (!(png_ptr->mode & PNG_HAVE_IDAT))
      png_error(png_ptr, "No IDATs written into file");

   /* see if user wants us to write information chunks */
   if (info_ptr != NULL)
   {
#if defined(PNG_WRITE_TEXT_SUPPORTED)
      int i; /* local index variable */
#endif
#if defined(PNG_WRITE_tIME_SUPPORTED)
      /* check to see if user has supplied a time chunk */
      if ((info_ptr->valid & PNG_INFO_tIME) &&
         !(png_ptr->mode & PNG_WROTE_tIME))
         png_write_tIME(png_ptr, &(info_ptr->mod_time));
#endif
#if defined(PNG_WRITE_TEXT_SUPPORTED)
      /* loop through comment chunks */
      for (i = 0; i < info_ptr->annotations.num_text; i++)
      {
         png_debug2(2, "Writing trailer text chunk %d, type %d\n", i,
            info_ptr->annotations.text[i].compression);
         /* an internationalized chunk? */
         if (info_ptr->annotations.text[i].compression > 0)
         {
#if defined(PNG_WRITE_iTXt_SUPPORTED)
             /* write international chunk */
             png_write_iTXt(png_ptr,
                         info_ptr->annotations.text[i].compression,
                         info_ptr->annotations.text[i].key,
                         info_ptr->annotations.text[i].lang,
                         info_ptr->annotations.text[i].lang_key,
                         info_ptr->annotations.text[i].text);
#else
             png_warning(png_ptr, "Unable to write international text\n");
#endif
             /* Mark this chunk as written */
             info_ptr->annotations.text[i].compression = PNG_TEXT_COMPRESSION_NONE_WR;
         }
         else if (info_ptr->annotations.text[i].compression >= PNG_TEXT_COMPRESSION_zTXt)
         {
#if defined(PNG_WRITE_zTXt_SUPPORTED)
            /* write compressed chunk */
            png_write_zTXt(png_ptr, info_ptr->annotations.text[i].key,
               info_ptr->annotations.text[i].text, 0,
               info_ptr->annotations.text[i].compression);
#else
            png_warning(png_ptr, "Unable to write compressed text\n");
#endif
            /* Mark this chunk as written */
            info_ptr->annotations.text[i].compression = PNG_TEXT_COMPRESSION_zTXt_WR;
         }
         else if (info_ptr->annotations.text[i].compression == PNG_TEXT_COMPRESSION_NONE)
         {
#if defined(PNG_WRITE_tEXt_SUPPORTED)
            /* write uncompressed chunk */
            png_write_tEXt(png_ptr, info_ptr->annotations.text[i].key,
               info_ptr->annotations.text[i].text, 0);
#else
            png_warning(png_ptr, "Unable to write uncompressed text\n");
#endif

            /* Mark this chunk as written */
            info_ptr->annotations.text[i].compression = PNG_TEXT_COMPRESSION_NONE_WR;
         }
      }
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   if (info_ptr->unknown_chunks_num)
   {
       png_unknown_chunk *up;

       png_debug(5, "writing extra chunks\n");

       for (up = info_ptr->unknown_chunks;
            up < info_ptr->unknown_chunks + info_ptr->unknown_chunks_num;
            up++)
       {
         int keep=png_handle_as_unknown(png_ptr, up->name);
         if (keep != HANDLE_CHUNK_NEVER &&
            up->location && (up->location & PNG_AFTER_IDAT) &&
            ((up->name[3] & 0x20) || keep == HANDLE_CHUNK_ALWAYS ||
            (png_ptr->flags & PNG_FLAG_KEEP_UNSAFE_CHUNKS)))
         {
            png_write_chunk(png_ptr, png_get_uint_32(up->name), up->data,
               up->size);
         }
       }
   }
#endif
   }

   png_ptr->mode |= PNG_AFTER_IDAT;

   /* write end of PNG file */
   png_write_IEND(png_ptr);
}

#if defined(PNG_WRITE_tIME_SUPPORTED)
void
png_convert_from_struct_tm(png_timep ptime, struct tm FAR * ttime)
{
   png_debug(1, "in png_convert_from_struct_tm\n");
   ptime->year = (png_uint_16)(1900 + ttime->tm_year);
   ptime->month = (png_byte)(ttime->tm_mon + 1);
   ptime->day = (png_byte)ttime->tm_mday;
   ptime->hour = (png_byte)ttime->tm_hour;
   ptime->minute = (png_byte)ttime->tm_min;
   ptime->second = (png_byte)ttime->tm_sec;
}

void
png_convert_from_time_t(png_timep ptime, time_t ttime)
{
   struct tm *tbuf;

   png_debug(1, "in png_convert_from_time_t\n");
   tbuf = gmtime(&ttime);
   png_convert_from_struct_tm(ptime, tbuf);
}
#endif

/* Initialize png_ptr structure, and allocate any memory needed */
png_structp
png_create_write_struct(png_const_charp user_png_ver, png_voidp error_ptr,
   png_error_ptr error_fn, png_error_ptr warn_fn)
{
#ifdef PNG_USER_MEM_SUPPORTED
   return (png_create_write_struct_2(user_png_ver, error_ptr, error_fn,
      warn_fn, NULL, NULL, NULL));
}

/* Alternate initialize png_ptr structure, and allocate any memory needed */
png_structp
png_create_write_struct_2(png_const_charp user_png_ver, png_voidp error_ptr,
   png_error_ptr error_fn, png_error_ptr warn_fn, png_voidp mem_ptr,
   png_malloc_ptr malloc_fn, png_free_ptr free_fn)
{
#endif /* PNG_USER_MEM_SUPPORTED */
   png_structp png_ptr;

   png_debug(1, "in png_create_write_struct\n");
#ifdef PNG_USER_MEM_SUPPORTED
   if ((png_ptr = (png_structp)png_create_struct_2(PNG_STRUCT_PNG,
      (png_malloc_ptr)malloc_fn)) == NULL)
#else
   if ((png_ptr = (png_structp)png_create_struct(PNG_STRUCT_PNG)) == NULL)
#endif /* PNG_USER_MEM_SUPPORTED */
   {
      return ((png_structp)NULL);
   }
#ifdef PNG_SETJMP_SUPPORTED
   if (png_setjmp(png_ptr))
   {
      png_free(png_ptr, png_ptr->zbuf);
      png_destroy_struct(png_ptr);
      return ((png_structp)NULL);
   }
#endif
#ifdef PNG_USER_MEM_SUPPORTED
   png_set_mem_fn(png_ptr, mem_ptr, malloc_fn, free_fn);
#endif /* PNG_USER_MEM_SUPPORTED */
   png_set_error_fn(png_ptr, error_ptr, error_fn, warn_fn);

   /* Libpng 0.90 and later are binary incompatible with libpng 0.89, so
    * we must recompile any applications that use any older library version.
    * For versions after libpng 1.0, we will be compatible, so we need
    * only check the first digit.
    */
   if (user_png_ver == NULL || user_png_ver[0] != png_libpng_ver[0] ||
       (png_libpng_ver[0] == '0' && user_png_ver[2] < '9'))
   {
      png_error(png_ptr,
         "Incompatible libpng version in application and library");
   }

   /* initialize zbuf - compression buffer */
   png_ptr->zbuf_size = PNG_ZBUF_SIZE;
   png_ptr->zbuf = (png_bytep)png_malloc(png_ptr,
      (png_uint_32)png_ptr->zbuf_size);

   png_set_write_fn(png_ptr, NULL, NULL, NULL);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
   png_set_filter_heuristics(png_ptr, PNG_FILTER_HEURISTIC_DEFAULT,
      1, NULL, NULL);
#endif

   return ((png_structp)png_ptr);
}

/* Initialize png_ptr structure, and allocate any memory needed */
void
png_write_init(png_structp png_ptr)
{
#ifdef PNG_SETJMP_SUPPORTED
   png_jmpbuf tmp_jmp; /* to save current jump buffer */
#endif

   png_debug(1, "in png_write_init\n");
#ifdef PNG_SETJMP_SUPPORTED
   /* save jump buffer and error functions */
   png_memcpy(&tmp_jmp, &(png_ptr->png_jmpbuf_struct.env), sizeof (png_jmpbuf));
#endif

   /* reset all variables to 0 */
   png_memset(png_ptr, 0, sizeof (png_struct));

#ifdef PNG_SETJMP_SUPPORTED
   /* restore jump buffer */
   png_memcpy(&(png_ptr->png_jmpbuf_struct.env), &tmp_jmp, sizeof (png_jmpbuf));
#endif

   /* initialize zbuf - compression buffer */
   png_ptr->zbuf_size = PNG_ZBUF_SIZE;
   png_ptr->zbuf = (png_bytep)png_malloc(png_ptr,
      (png_uint_32)png_ptr->zbuf_size);
   png_set_write_fn(png_ptr, NULL, NULL, NULL);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
   png_set_filter_heuristics(png_ptr, PNG_FILTER_HEURISTIC_DEFAULT,
      1, NULL, NULL);
#endif
}

/* Write a few rows of image data.  If the image is interlaced,
 * either you will have to write the 7 sub images, or, if you
 * have called png_set_interlace_handling(), you will have to
 * "write" the image seven times.
 */
void
png_write_rows(png_structp png_ptr, png_bytepp row,
   png_uint_32 num_rows)
{
   png_uint_32 i; /* row counter */
   png_bytepp rp; /* row pointer */

   png_debug(1, "in png_write_rows\n");
   /* loop through the rows */
   for (i = 0, rp = row; i < num_rows; i++, rp++)
   {
      png_write_row(png_ptr, *rp);
   }
}

/* Write the image.  You only need to call this function once, even
 * if you are writing an interlaced image.
 */
void
png_write_image(png_structp png_ptr, png_bytepp image)
{
   png_uint_32 i; /* row index */
   int pass, num_pass; /* pass variables */
   png_bytepp rp; /* points to current row */

   png_debug(1, "in png_write_image\n");
#if defined(PNG_WRITE_INTERLACING_SUPPORTED)
   /* intialize interlace handling.  If image is not interlaced,
      this will set pass to 1 */
   num_pass = png_set_interlace_handling(png_ptr);
#else
   num_pass = 1;
#endif
   /* loop through passes */
   for (pass = 0; pass < num_pass; pass++)
   {
      /* loop through image */
      for (i = 0, rp = image; i < png_ptr->height; i++, rp++)
      {
         png_write_row(png_ptr, *rp);
      }
   }
}

/* called by user to write a row of image data */
void
png_write_row(png_structp png_ptr, png_bytep row)
{
   png_debug2(1, "in png_write_row (row %ld, pass %d)\n",
      png_ptr->row_number, png_ptr->pass);
   /* initialize transformations and other stuff if first time */
   if (png_ptr->row_number == 0 && png_ptr->pass == 0)
   {
   /* check for transforms that have been set but were defined out */
#if !defined(PNG_WRITE_INVERT_SUPPORTED) && defined(PNG_READ_INVERT_SUPPORTED)
   if (png_ptr->transformations & PNG_INVERT_MONO)
      png_warning(png_ptr, "PNG_WRITE_INVERT_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_FILLER_SUPPORTED) && defined(PNG_READ_FILLER_SUPPORTED)
   if (png_ptr->transformations & PNG_FILLER)
      png_warning(png_ptr, "PNG_WRITE_FILLER_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_PACKSWAP_SUPPORTED) && defined(PNG_READ_PACKSWAP_SUPPORTED)
   if (png_ptr->transformations & PNG_PACKSWAP)
      png_warning(png_ptr, "PNG_WRITE_PACKSWAP_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_PACK_SUPPORTED) && defined(PNG_READ_PACK_SUPPORTED)
   if (png_ptr->transformations & PNG_PACK)
      png_warning(png_ptr, "PNG_WRITE_PACK_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_SHIFT_SUPPORTED) && defined(PNG_READ_SHIFT_SUPPORTED)
   if (png_ptr->transformations & PNG_SHIFT)
      png_warning(png_ptr, "PNG_WRITE_SHIFT_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_BGR_SUPPORTED) && defined(PNG_READ_BGR_SUPPORTED)
   if (png_ptr->transformations & PNG_BGR)
      png_warning(png_ptr, "PNG_WRITE_BGR_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_SWAP_SUPPORTED) && defined(PNG_READ_SWAP_SUPPORTED)
   if (png_ptr->transformations & PNG_SWAP_BYTES)
      png_warning(png_ptr, "PNG_WRITE_SWAP_SUPPORTED is not defined.");
#endif

      png_write_start_row(png_ptr);
   }

#if defined(PNG_WRITE_INTERLACING_SUPPORTED)
   /* if interlaced and not interested in row, return */
   if (png_ptr->interlaced && (png_ptr->transformations & PNG_INTERLACE))
   {
      switch (png_ptr->pass)
      {
         case 0:
            if (png_ptr->row_number & 0x07)
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
         case 1:
            if ((png_ptr->row_number & 0x07) || png_ptr->width < 5)
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
         case 2:
            if ((png_ptr->row_number & 0x07) != 4)
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
         case 3:
            if ((png_ptr->row_number & 0x03) || png_ptr->width < 3)
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
         case 4:
            if ((png_ptr->row_number & 0x03) != 2)
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
         case 5:
            if ((png_ptr->row_number & 0x01) || png_ptr->width < 2)
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
         case 6:
            if (!(png_ptr->row_number & 0x01))
            {
               png_write_finish_row(png_ptr);
               return;
            }
            break;
      }
   }
#endif

   /* set up row info for transformations */
   png_ptr->row_info.color_type = png_ptr->color_type;
   png_ptr->row_info.width = png_ptr->usr_width;
   png_ptr->row_info.channels = png_ptr->usr_channels;
   png_ptr->row_info.bit_depth = png_ptr->usr_bit_depth;
   png_ptr->row_info.pixel_depth = (png_byte)(png_ptr->row_info.bit_depth *
      png_ptr->row_info.channels);

   png_ptr->row_info.rowbytes = ((png_ptr->row_info.width *
      (png_uint_32)png_ptr->row_info.pixel_depth + 7) >> 3);

   png_debug1(3, "row_info->color_type = %d\n", png_ptr->row_info.color_type);
   png_debug1(3, "row_info->width = %d\n", png_ptr->row_info.width);
   png_debug1(3, "row_info->channels = %d\n", png_ptr->row_info.channels);
   png_debug1(3, "row_info->bit_depth = %d\n", png_ptr->row_info.bit_depth);
   png_debug1(3, "row_info->pixel_depth = %d\n", png_ptr->row_info.pixel_depth);
   png_debug1(3, "row_info->rowbytes = %d\n", png_ptr->row_info.rowbytes);

   /* Copy user's row into buffer, leaving room for filter byte. */
   png_memcpy_check(png_ptr, png_ptr->row_buf + 1, row,
      png_ptr->row_info.rowbytes);

#if defined(PNG_WRITE_INTERLACING_SUPPORTED)
   /* handle interlacing */
   if (png_ptr->interlaced && png_ptr->pass < 6 &&
      (png_ptr->transformations & PNG_INTERLACE))
   {
      png_do_write_interlace(&(png_ptr->row_info),
         png_ptr->row_buf + 1, png_ptr->pass);
      /* this should always get caught above, but still ... */
      if (!(png_ptr->row_info.width))
      {
         png_write_finish_row(png_ptr);
         return;
      }
   }
#endif

   /* handle other transformations */
   if (png_ptr->transformations)
      png_do_write_transformations(png_ptr);

   /* Find a filter if necessary, filter the row and write it out. */
   png_write_find_filter(png_ptr, &(png_ptr->row_info));

   if (png_ptr->write_row_fn != NULL)
      (*(png_ptr->write_row_fn))(png_ptr, png_ptr->row_number, png_ptr->pass);
}

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
/* Set the automatic flush interval or 0 to turn flushing off */
void
png_set_flush(png_structp png_ptr, int nrows)
{
   png_debug(1, "in png_set_flush\n");
   png_ptr->flush_dist = (nrows < 0 ? 0 : nrows);
}

/* flush the current output buffers now */
void
png_write_flush(png_structp png_ptr)
{
   int wrote_IDAT;

   png_debug(1, "in png_write_flush\n");
   /* We have already written out all of the data */
   if (png_ptr->row_number >= png_ptr->num_rows)
     return;

   do
   {
      int ret;

      /* compress the data */
      ret = deflate(&png_ptr->zstream, Z_SYNC_FLUSH);
      wrote_IDAT = 0;

      /* check for compression errors */
      if (ret != Z_OK)
      {
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }

      if (!(png_ptr->zstream.avail_out))
      {
         /* write the IDAT and reset the zlib output buffer */
         png_write_IDAT(png_ptr, png_ptr->zbuf,
                        png_ptr->zbuf_size);
         png_ptr->zstream.next_out = png_ptr->zbuf;
         png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
         wrote_IDAT = 1;
      }
   } while(wrote_IDAT == 1);

   /* If there is any data left to be output, write it into a new IDAT */
   if (png_ptr->zbuf_size != png_ptr->zstream.avail_out)
   {
      /* write the IDAT and reset the zlib output buffer */
      png_write_IDAT(png_ptr, png_ptr->zbuf,
                     png_ptr->zbuf_size - png_ptr->zstream.avail_out);
      png_ptr->zstream.next_out = png_ptr->zbuf;
      png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
   }
   png_ptr->flush_rows = 0;
   png_flush(png_ptr);
}
#endif /* PNG_WRITE_FLUSH_SUPPORTED */

/* free all memory used by the write */
void
png_destroy_write_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr)
{
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
#ifdef PNG_USER_MEM_SUPPORTED
   png_free_ptr free_fn = NULL;
#endif

   png_debug(1, "in png_destroy_write_struct\n");
   if (png_ptr_ptr != NULL)
   {
      png_ptr = *png_ptr_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
      free_fn = png_ptr->free_fn;
#endif
   }

   if (info_ptr_ptr != NULL)
      info_ptr = *info_ptr_ptr;

   if (info_ptr != NULL)
   {
      png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);

      if (png_ptr->num_chunk_list)
      {
         png_free(png_ptr, png_ptr->chunk_list);
         png_ptr->num_chunk_list=0;
      }

#ifdef PNG_USER_MEM_SUPPORTED
      png_destroy_struct_2((png_voidp)info_ptr, free_fn);
#else
      png_destroy_struct((png_voidp)info_ptr);
#endif
      *info_ptr_ptr = (png_infop)NULL;
   }

   if (png_ptr != NULL)
   {
      png_write_destroy(png_ptr);
#ifdef PNG_USER_MEM_SUPPORTED
      png_destroy_struct_2((png_voidp)png_ptr, free_fn);
#else
      png_destroy_struct((png_voidp)png_ptr);
#endif
      *png_ptr_ptr = (png_structp)NULL;
   }
}


/* Free any memory used in png_ptr struct (old method) */
void
png_write_destroy(png_structp png_ptr)
{
#ifdef PNG_SETJMP_SUPPORTED
   png_jmpbuf tmp_jmp; /* save jump buffer */
#endif
   png_error_ptr error_fn;
   png_error_ptr warning_fn;
   png_voidp error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
   png_free_ptr free_fn;
#endif

   png_debug(1, "in png_write_destroy\n");
   /* free any memory zlib uses */
   deflateEnd(&png_ptr->zstream);

   /* free our memory.  png_free checks NULL for us. */
   png_free(png_ptr, png_ptr->zbuf);
   png_free(png_ptr, png_ptr->row_buf);
   png_free(png_ptr, png_ptr->prev_row);
   png_free(png_ptr, png_ptr->sub_row);
   png_free(png_ptr, png_ptr->up_row);
   png_free(png_ptr, png_ptr->avg_row);
   png_free(png_ptr, png_ptr->paeth_row);

#if defined(PNG_TIME_RFC1123_SUPPORTED)
   png_free(png_ptr, png_ptr->time_buffer);
#endif

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
   png_free(png_ptr, png_ptr->prev_filters);
   png_free(png_ptr, png_ptr->filter_weights);
   png_free(png_ptr, png_ptr->inv_filter_weights);
   png_free(png_ptr, png_ptr->filter_costs);
   png_free(png_ptr, png_ptr->inv_filter_costs);
#endif

#ifdef PNG_SETJMP_SUPPORTED
   /* reset structure */
   png_memcpy(&tmp_jmp, &(png_ptr->png_jmpbuf_struct.env), sizeof (png_jmpbuf));
#endif

   error_fn = png_ptr->error_fn;
   warning_fn = png_ptr->warning_fn;
   error_ptr = png_ptr->error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
   free_fn = png_ptr->free_fn;
#endif

   png_memset(png_ptr, 0, sizeof (png_struct));

   png_ptr->error_fn = error_fn;
   png_ptr->warning_fn = warning_fn;
   png_ptr->error_ptr = error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
   png_ptr->free_fn = free_fn;
#endif

#ifdef PNG_SETJMP_SUPPORTED
   png_memcpy(&(png_ptr->png_jmpbuf_struct.env), &tmp_jmp, sizeof (png_jmpbuf));
#endif
}

/* Allow the application to select one or more row filters to use. */
void
png_set_filter(png_structp png_ptr, int method, int filters)
{
   png_debug(1, "in png_set_filter\n");
   /* We allow 'method' only for future expansion of the base filter method. */
   if (method == PNG_FILTER_TYPE_BASE)
   {
      switch (filters & (PNG_ALL_FILTERS | 0x07))
      {
         case 5:
         case 6:
         case 7: png_warning(png_ptr, "Unknown row filter for method 0");
         case PNG_FILTER_VALUE_NONE:  png_ptr->do_filter=PNG_FILTER_NONE; break;
         case PNG_FILTER_VALUE_SUB:   png_ptr->do_filter=PNG_FILTER_SUB;  break;
         case PNG_FILTER_VALUE_UP:    png_ptr->do_filter=PNG_FILTER_UP;   break;
         case PNG_FILTER_VALUE_AVG:   png_ptr->do_filter=PNG_FILTER_AVG;  break;
         case PNG_FILTER_VALUE_PAETH: png_ptr->do_filter=PNG_FILTER_PAETH;break;
         default: png_ptr->do_filter = (png_byte)filters; break;
      }

      /* If we have allocated the row_buf, this means we have already started
       * with the image and we should have allocated all of the filter buffers
       * that have been selected.  If prev_row isn't already allocated, then
       * it is too late to start using the filters that need it, since we
       * will be missing the data in the previous row.  If an application
       * wants to start and stop using particular filters during compression,
       * it should start out with all of the filters, and then add and
       * remove them after the start of compression.
       */
      if (png_ptr->row_buf != NULL)
      {
         if ((png_ptr->do_filter & PNG_FILTER_SUB) && png_ptr->sub_row == NULL)
         {
            png_ptr->sub_row = (png_bytep)png_malloc(png_ptr,
              (png_ptr->rowbytes + 1));
            png_ptr->sub_row[0] = PNG_FILTER_VALUE_SUB;
         }

         if ((png_ptr->do_filter & PNG_FILTER_UP) && png_ptr->up_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Up filter after starting");
               png_ptr->do_filter &= ~PNG_FILTER_UP;
            }
            else
            {
               png_ptr->up_row = (png_bytep)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->up_row[0] = PNG_FILTER_VALUE_UP;
            }
         }

         if ((png_ptr->do_filter & PNG_FILTER_AVG) && png_ptr->avg_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Average filter after starting");
               png_ptr->do_filter &= ~PNG_FILTER_AVG;
            }
            else
            {
               png_ptr->avg_row = (png_bytep)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->avg_row[0] = PNG_FILTER_VALUE_AVG;
            }
         }

         if ((png_ptr->do_filter & PNG_FILTER_PAETH) &&
             png_ptr->paeth_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Paeth filter after starting");
               png_ptr->do_filter &= (png_byte)(~PNG_FILTER_PAETH);
            }
            else
            {
               png_ptr->paeth_row = (png_bytep)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->paeth_row[0] = PNG_FILTER_VALUE_PAETH;
            }
         }

         if (png_ptr->do_filter == PNG_NO_FILTERS)
            png_ptr->do_filter = PNG_FILTER_NONE;
      }
   }
   else
      png_error(png_ptr, "Unknown custom filter method");
}

/* This allows us to influence the way in which libpng chooses the "best"
 * filter for the current scanline.  While the "minimum-sum-of-absolute-
 * differences metric is relatively fast and effective, there is some
 * question as to whether it can be improved upon by trying to keep the
 * filtered data going to zlib more consistent, hopefully resulting in
 * better compression.
 */
#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)      /* GRR 970116 */
void
png_set_filter_heuristics(png_structp png_ptr, int heuristic_method,
   int num_weights, png_doublep filter_weights,
   png_doublep filter_costs)
{
   int i;

   png_debug(1, "in png_set_filter_heuristics\n");
   if (heuristic_method >= PNG_FILTER_HEURISTIC_LAST)
   {
      png_warning(png_ptr, "Unknown filter heuristic method");
      return;
   }

   if (heuristic_method == PNG_FILTER_HEURISTIC_DEFAULT)
   {
      heuristic_method = PNG_FILTER_HEURISTIC_UNWEIGHTED;
   }

   if (num_weights < 0 || filter_weights == NULL ||
      heuristic_method == PNG_FILTER_HEURISTIC_UNWEIGHTED)
   {
      num_weights = 0;
   }

   png_ptr->num_prev_filters = (png_byte)num_weights;
   png_ptr->heuristic_method = (png_byte)heuristic_method;

   if (num_weights > 0)
   {
      if (png_ptr->prev_filters == NULL)
      {
         png_ptr->prev_filters = (png_bytep)png_malloc(png_ptr,
            (png_uint_32)(sizeof(png_byte) * num_weights));

         /* To make sure that the weighting starts out fairly */
         for (i = 0; i < num_weights; i++)
         {
            png_ptr->prev_filters[i] = 255;
         }
      }

      if (png_ptr->filter_weights == NULL)
      {
         png_ptr->filter_weights = (png_uint_16p) png_malloc(png_ptr,
            (png_uint_32)(sizeof(png_uint_16) * num_weights));

         png_ptr->inv_filter_weights = (png_uint_16p) png_malloc(png_ptr,
            (png_uint_32)(sizeof(png_uint_16) * num_weights));

         for (i = 0; i < num_weights; i++)
         {
            png_ptr->inv_filter_weights[i] =
            png_ptr->filter_weights[i] = PNG_WEIGHT_FACTOR;
         }
      }

      for (i = 0; i < num_weights; i++)
      {
         if (filter_weights[i] < 0.0)
         {
            png_ptr->inv_filter_weights[i] =
            png_ptr->filter_weights[i] = PNG_WEIGHT_FACTOR;
         }
         else
         {
            png_ptr->inv_filter_weights[i] =
               (png_uint_16)((double)PNG_WEIGHT_FACTOR*filter_weights[i]+0.5);
            png_ptr->filter_weights[i] =
               (png_uint_16)((double)PNG_WEIGHT_FACTOR/filter_weights[i]+0.5);
         }
      }
   }

   /* If, in the future, there are other filter methods, this would
    * need to be based on png_ptr->filter.
    */
   if (png_ptr->filter_costs == NULL)
   {
      png_ptr->filter_costs = (png_uint_16p) png_malloc(png_ptr,
         (png_uint_32)(sizeof(png_uint_16) * PNG_FILTER_VALUE_LAST));

      png_ptr->inv_filter_costs = (png_uint_16p) png_malloc(png_ptr,
         (png_uint_32)(sizeof(png_uint_16) * PNG_FILTER_VALUE_LAST));

      for (i = 0; i < PNG_FILTER_VALUE_LAST; i++)
      {
         png_ptr->inv_filter_costs[i] =
         png_ptr->filter_costs[i] = PNG_COST_FACTOR;
      }
   }

   /* Here is where we set the relative costs of the different filters.  We
    * should take the desired compression level into account when setting
    * the costs, so that Paeth, for instance, has a high relative cost at low
    * compression levels, while it has a lower relative cost at higher
    * compression settings.  The filter types are in order of increasing
    * relative cost, so it would be possible to do this with an algorithm.
    */
   for (i = 0; i < PNG_FILTER_VALUE_LAST; i++)
   {
      if (filter_costs == NULL || filter_costs[i] < 0.0)
      {
         png_ptr->inv_filter_costs[i] =
         png_ptr->filter_costs[i] = PNG_COST_FACTOR;
      }
      else if (filter_costs[i] >= 1.0)
      {
         png_ptr->inv_filter_costs[i] =
            (png_uint_16)((double)PNG_COST_FACTOR / filter_costs[i] + 0.5);
         png_ptr->filter_costs[i] =
            (png_uint_16)((double)PNG_COST_FACTOR * filter_costs[i] + 0.5);
      }
   }
}
#endif /* PNG_WRITE_WEIGHTED_FILTER_SUPPORTED */

void
png_set_compression_level(png_structp png_ptr, int level)
{
   png_debug(1, "in png_set_compression_level\n");
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_LEVEL;
   png_ptr->zlib_level = level;
}

void
png_set_compression_mem_level(png_structp png_ptr, int mem_level)
{
   png_debug(1, "in png_set_compression_mem_level\n");
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL;
   png_ptr->zlib_mem_level = mem_level;
}

void
png_set_compression_strategy(png_structp png_ptr, int strategy)
{
   png_debug(1, "in png_set_compression_strategy\n");
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_STRATEGY;
   png_ptr->zlib_strategy = strategy;
}

void
png_set_compression_window_bits(png_structp png_ptr, int window_bits)
{
   if (window_bits > 15)
      png_warning(png_ptr, "Only compression windows <= 32k supported by PNG");
   else if (window_bits < 8)
      png_warning(png_ptr, "Only compression windows >= 256 supported by PNG");
#ifndef WBITS_8_OK
   /* avoid libpng bug with 256-byte windows */
   if (window_bits == 8)
     {
       png_warning(png_ptr, "Compression window is being reset to 512");
       window_bits=9;
     }
#endif
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS;
   png_ptr->zlib_window_bits = window_bits;
}

void
png_set_compression_method(png_structp png_ptr, int method)
{
   png_debug(1, "in png_set_compression_method\n");
   if (method != 8)
      png_warning(png_ptr, "Only compression method 8 is supported by PNG");
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_METHOD;
   png_ptr->zlib_method = method;
}

void
png_set_write_status_fn(png_structp png_ptr, png_write_status_ptr write_row_fn)
{
   png_ptr->write_row_fn = write_row_fn;
}

#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
void
png_set_write_user_transform_fn(png_structp png_ptr, png_user_transform_ptr
   write_user_transform_fn)
{
   png_debug(1, "in png_set_write_user_transform_fn\n");
   png_ptr->transformations |= PNG_USER_TRANSFORM;
   png_ptr->write_user_transform_fn = write_user_transform_fn;
}
#endif


#if defined(PNG_INFO_IMAGE_SUPPORTED)
void png_write_png(png_structp png_ptr, png_infop info_ptr,
                   int transforms,
                   voidp params)
{
   if(transforms == 0 || params == (voidp)NULL)
      /* quiet compiler warnings */ ;

#if defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
   /* invert the alpha channel from opacity to transparency */
   if (transforms & PNG_TRANSFORM_INVERT_ALPHA)
       png_set_invert_alpha(png_ptr);
#endif

   /* Write the file header information. */
   png_write_info(png_ptr, info_ptr);

   /* ------ these transformations don't touch the info structure ------- */

#if defined(PNG_WRITE_INVERT_SUPPORTED)
   /* invert monochrome pixels */
   if (transforms & PNG_TRANSFORM_INVERT_MONO)
       png_set_invert_mono(png_ptr);
#endif

#if defined(PNG_WRITE_SHIFT_SUPPORTED)
   /* Shift the pixels up to a legal bit depth and fill in
    * as appropriate to correctly scale the image.
    */
   if ((transforms & PNG_TRANSFORM_SHIFT) &&
       (info_ptr->valid & PNG_INFO_sBIT))
       png_set_shift(png_ptr, &info_ptr->sig_bit);
#endif

#if defined(PNG_WRITE_PACK_SUPPORTED)
   /* pack pixels into bytes */
   if (transforms & PNG_TRANSFORM_PACKING)
       png_set_packing(png_ptr);
#endif

#if defined(PNG_WRITE_SWAP_ALPHA_SUPPORTED)
   /* swap location of alpha bytes from ARGB to RGBA */
   if (transforms & PNG_TRANSFORM_SWAP_ALPHA)
       png_set_swap_alpha(png_ptr);
#endif

#if defined(PNG_WRITE_FILLER_SUPPORTED)
   /* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
    * RGB (4 channels -> 3 channels). The second parameter is not used.
    */
   if (transforms & PNG_TRANSFORM_STRIP_FILLER)
       png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);
#endif

#if defined(PNG_WRITE_BGR_SUPPORTED)
   /* flip BGR pixels to RGB */
   if (transforms & PNG_TRANSFORM_BGR)
       png_set_bgr(png_ptr);
#endif

#if defined(PNG_WRITE_SWAP_SUPPORTED)
   /* swap bytes of 16-bit files to most significant byte first */
   if (transforms & PNG_TRANSFORM_SWAP_ENDIAN)
       png_set_swap(png_ptr);
#endif

#if defined(PNG_WRITE_PACKSWAP_SUPPORTED)
   /* swap bits of 1, 2, 4 bit packed pixel formats */
   if (transforms & PNG_TRANSFORM_PACKSWAP)
       png_set_packswap(png_ptr);
#endif

   /* ----------------------- end of transformations ------------------- */

   /* write the bits */
   if (info_ptr->valid & PNG_INFO_IDAT)
       png_write_image(png_ptr, info_ptr->row_pointers);

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);
}
#endif
