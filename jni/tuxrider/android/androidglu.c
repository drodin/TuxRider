//
//  androidglu.c
//  tuxracer
//
//  Created by emmanuel de roux on 23/10/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#include "tuxracer.h"

static GLboolean legalFormat(GLenum format);
static GLboolean legalType(GLenum);
static GLboolean isLegalFormatForPackedPixelType(GLenum format, GLenum type);
static GLint image_size(GLint width, GLint height, GLenum format, GLenum type);
static GLint is_index(GLenum format);
static GLint elements_per_group(GLenum format, GLenum type);
static void empty_image(const PixelStorageModes *psm,
						GLint width, GLint height, GLenum format,
						GLenum type, GLboolean index_format,
						const GLushort *oldimage, void *userdata);
static void scale_internal(GLint components, GLint widthin, GLint heightin,
						   const GLushort *datain,
						   GLint widthout, GLint heightout,
						   GLushort *dataout);
static void fill_image(const PixelStorageModes *psm,
					   GLint width, GLint height, GLenum format,
					   GLenum type, GLboolean index_format,
					   const void *userdata, GLushort *newimage);
static void retrieveStoreModes(PixelStorageModes *psm);

static GLboolean isTypePackedPixel(GLenum type);
static GLfloat bytes_per_element(GLenum type);
static void halveImage(GLint components, GLuint width, GLuint height,
					   const GLushort *datain, GLushort *dataout);
					   
/* packedpixel type scale routines */
static void extract332(int,const void *, GLfloat []);
static void shove332(const GLfloat [],int ,void *);
static void extract233rev(int,const void *, GLfloat []);
static void shove233rev(const GLfloat [],int ,void *);
static void extract565(int,const void *, GLfloat []);
static void shove565(const GLfloat [],int ,void *);
static void extract565rev(int,const void *, GLfloat []);
static void shove565rev(const GLfloat [],int ,void *);
static void extract4444(int,const void *, GLfloat []);
static void shove4444(const GLfloat [],int ,void *);
static void extract4444rev(int,const void *, GLfloat []);
static void shove4444rev(const GLfloat [],int ,void *);
static void extract5551(int,const void *, GLfloat []);
static void shove5551(const GLfloat [],int ,void *);
static void extract1555rev(int,const void *, GLfloat []);
static void shove1555rev(const GLfloat [],int ,void *);
static void extract8888(int,const void *, GLfloat []);
static void shove8888(const GLfloat [],int ,void *);
static void extract8888rev(int,const void *, GLfloat []);
static void shove8888rev(const GLfloat [],int ,void *);
static void extract1010102(int,const void *, GLfloat []);
static void shove1010102(const GLfloat [],int ,void *);
static void extract2101010rev(int,const void *, GLfloat []);
static void shove2101010rev(const GLfloat [],int ,void *);
int GLAPIENTRY gluScaleImage(GLenum format, GLsizei widthin, GLsizei heightin,
							 GLenum typein, const void *datain,
							 GLsizei widthout, GLsizei heightout, GLenum typeout,
							 void *dataout)
{
    int components;
    GLushort *beforeImage;
    GLushort *afterImage;
    PixelStorageModes psm;
	
    if (widthin == 0 || heightin == 0 || widthout == 0 || heightout == 0) {
		return 0;
    }
    if (widthin < 0 || heightin < 0 || widthout < 0 || heightout < 0) {
		return GLU_INVALID_VALUE;
    }
    if (!legalFormat(format) || !legalType(typein) || !legalType(typeout)) {
		return GLU_INVALID_ENUM;
    }
    if (!isLegalFormatForPackedPixelType(format, typein)) {
		return GLU_INVALID_OPERATION;
    }
    if (!isLegalFormatForPackedPixelType(format, typeout)) {
		return GLU_INVALID_OPERATION;
    }
    beforeImage =
	malloc(image_size(widthin, heightin, format, GL_UNSIGNED_SHORT));
    afterImage =
	malloc(image_size(widthout, heightout, format, GL_UNSIGNED_SHORT));
    if (beforeImage == NULL || afterImage == NULL) {
		return GLU_OUT_OF_MEMORY;
    }
	
    retrieveStoreModes(&psm);
    fill_image(&psm,widthin, heightin, format, typein, is_index(format),
			   datain, beforeImage);
    components = elements_per_group(format, 0);
    scale_internal(components, widthin, heightin, beforeImage,
				   widthout, heightout, afterImage);
    empty_image(&psm,widthout, heightout, format, typeout,
				is_index(format), afterImage, dataout);
    free((GLbyte *) beforeImage);
    free((GLbyte *) afterImage);
	
    return 0;
}

static GLboolean legalFormat(GLenum format)
{
    switch(format) {
		case GL_COLOR_INDEX:
		case GL_STENCIL_INDEX:
		case GL_DEPTH_COMPONENT:
		case GL_RED:
		case GL_GREEN:
		case GL_BLUE:
		case GL_ALPHA:
		case GL_RGB:
		case GL_RGBA:
		case GL_LUMINANCE:
		case GL_LUMINANCE_ALPHA:
		case GL_BGR:
			return GL_TRUE;
		default:
			return GL_FALSE;
    }
}

static GLboolean legalType(GLenum type)
{
    switch(type) {
		case GL_BITMAP:
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:  
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			return GL_TRUE;
		default:
			return GL_FALSE;
    }
}

/* Determines if the packed pixel type is compatible with the format */
static GLboolean isLegalFormatForPackedPixelType(GLenum format, GLenum type)
{
	/* if not a packed pixel type then return true */
	if (!isTypePackedPixel(type)) {
		return GL_TRUE;
	}
	
	/* 3_3_2/2_3_3_REV & 5_6_5/5_6_5_REV are only compatible with RGB */
	if ((type == GL_UNSIGNED_BYTE_3_3_2 || type == GL_UNSIGNED_BYTE_2_3_3_REV||
		 type == GL_UNSIGNED_SHORT_5_6_5|| type == GL_UNSIGNED_SHORT_5_6_5_REV)
		&& format != GL_RGB)
		return GL_FALSE;
	
	/* 4_4_4_4 & 5_5_5_1/1_5_5_5_REV & 8_8_8_8/8_8_8_8_REV &
	 * 10_10_10_2/2_10_10_10_REV are only compatible with RGBA, BGRA & ABGR_EXT.
	 */
	if ((type == GL_UNSIGNED_SHORT_4_4_4_4 ||
		 type == GL_UNSIGNED_SHORT_5_5_5_1 ||
		 type == GL_UNSIGNED_SHORT_1_5_5_5_REV ||
		 type == GL_UNSIGNED_INT_8_8_8_8 ||
		 type == GL_UNSIGNED_INT_8_8_8_8_REV ||
		 type == GL_UNSIGNED_INT_10_10_10_2 ||
		 type == GL_UNSIGNED_INT_2_10_10_10_REV) &&
		(format != GL_RGBA)) {
		return GL_FALSE;
	}
	
	return GL_TRUE;
} /* isLegalFormatForPackedPixelType() */

static GLint image_size(GLint width, GLint height, GLenum format, GLenum type)
{
    int bytes_per_row;
    int components;
	
	assert(width > 0);
	assert(height > 0);
    components = elements_per_group(format,type);
    if (type == GL_BITMAP) {
		bytes_per_row = (width + 7) / 8;
    } else {
		bytes_per_row = bytes_per_element(type) * width;
    }
    return bytes_per_row * height * components;
}

static void retrieveStoreModes(PixelStorageModes *psm)
{
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &psm->unpack_alignment);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &psm->unpack_row_length);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &psm->unpack_skip_rows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &psm->unpack_skip_pixels);
    glGetIntegerv(GL_UNPACK_LSB_FIRST, &psm->unpack_lsb_first);
    glGetIntegerv(GL_UNPACK_SWAP_BYTES, &psm->unpack_swap_bytes);
	
    glGetIntegerv(GL_PACK_ALIGNMENT, &psm->pack_alignment);
    glGetIntegerv(GL_PACK_ROW_LENGTH, &psm->pack_row_length);
    glGetIntegerv(GL_PACK_SKIP_ROWS, &psm->pack_skip_rows);
    glGetIntegerv(GL_PACK_SKIP_PIXELS, &psm->pack_skip_pixels);
    glGetIntegerv(GL_PACK_LSB_FIRST, &psm->pack_lsb_first);
    glGetIntegerv(GL_PACK_SWAP_BYTES, &psm->pack_swap_bytes);
}

/*
 ** Extract array from user's data applying all pixel store modes.
 ** The internal format used is an array of unsigned shorts.
 */
static void fill_image(const PixelStorageModes *psm,
					   GLint width, GLint height, GLenum format,
					   GLenum type, GLboolean index_format,
					   const void *userdata, GLushort *newimage)
{
    GLint components;
    GLint element_size;
    GLint rowsize;
    GLint padding;
    GLint groups_per_line;
    GLint group_size;
    GLint elements_per_line;
    const GLubyte *start;
    const GLubyte *iter;
    GLushort *iter2;
    GLint i, j, k;
    GLint myswap_bytes;
	
    myswap_bytes = psm->unpack_swap_bytes;
    components = elements_per_group(format,type);
    if (psm->unpack_row_length > 0) {
		groups_per_line = psm->unpack_row_length;
    } else {
		groups_per_line = width;
    }
	
    /* All formats except GL_BITMAP fall out trivially */
    if (type == GL_BITMAP) {
		GLint bit_offset;
		GLint current_bit;
		
		rowsize = (groups_per_line * components + 7) / 8;
		padding = (rowsize % psm->unpack_alignment);
		if (padding) {
			rowsize += psm->unpack_alignment - padding;
		}
		start = (const GLubyte *) userdata + psm->unpack_skip_rows * rowsize +
		(psm->unpack_skip_pixels * components / 8);
		elements_per_line = width * components;
		iter2 = newimage;
		for (i = 0; i < height; i++) {
			iter = start;
			bit_offset = (psm->unpack_skip_pixels * components) % 8;
			for (j = 0; j < elements_per_line; j++) {
				/* Retrieve bit */
				if (psm->unpack_lsb_first) {
					current_bit = iter[0] & (1 << bit_offset);
				} else {
					current_bit = iter[0] & (1 << (7 - bit_offset));
				}
				if (current_bit) {
					if (index_format) {
						*iter2 = 1;
					} else {
						*iter2 = 65535;
					}
				} else {
					*iter2 = 0;
				}
				bit_offset++;
				if (bit_offset == 8) {
					bit_offset = 0;
					iter++;
				}
				iter2++;
			}
			start += rowsize;
		}
    } else {
		element_size = bytes_per_element(type);
		group_size = element_size * components;
		if (element_size == 1) myswap_bytes = 0;
		
		rowsize = groups_per_line * group_size;
		padding = (rowsize % psm->unpack_alignment);
		if (padding) {
			rowsize += psm->unpack_alignment - padding;
		}
		start = (const GLubyte *) userdata + psm->unpack_skip_rows * rowsize +
		psm->unpack_skip_pixels * group_size;
		elements_per_line = width * components;
		
		iter2 = newimage;
		for (i = 0; i < height; i++) {
			iter = start;
			for (j = 0; j < elements_per_line; j++) {
				Type_Widget widget;
				float extractComponents[4];
				
				switch(type) {
					case GL_UNSIGNED_BYTE_3_3_2:
						extract332(0,iter,extractComponents);
						for (k = 0; k < 3; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_BYTE_2_3_3_REV:
						extract233rev(0,iter,extractComponents);
						for (k = 0; k < 3; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_BYTE:
						if (index_format) {
							*iter2++ = *iter;
						} else {
							*iter2++ = (*iter) * 257;
						}
						break;
					case GL_BYTE:
						if (index_format) {
							*iter2++ = *((const GLbyte *) iter);
						} else {
							/* rough approx */
							*iter2++ = (*((const GLbyte *) iter)) * 516;
						}
						break;
					case GL_UNSIGNED_SHORT_5_6_5: 		        
						extract565(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 3; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_SHORT_5_6_5_REV:		        
						extract565rev(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 3; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_SHORT_4_4_4_4:	        
						extract4444(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_SHORT_5_5_5_1:	        
						extract5551(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_SHORT_1_5_5_5_REV:
						extract1555rev(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_SHORT:
					case GL_SHORT:
						if (myswap_bytes) {
							widget.ub[0] = iter[1];
							widget.ub[1] = iter[0];
						} else {
							widget.ub[0] = iter[0];
							widget.ub[1] = iter[1];
						}
						if (type == GL_SHORT) {
							if (index_format) {
								*iter2++ = widget.s[0];
							} else {
								/* rough approx */
								*iter2++ = widget.s[0]*2;
							}
						} else {
							*iter2++ = widget.us[0];
						}
						break;
					case GL_UNSIGNED_INT_8_8_8_8:         
						extract8888(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_INT_8_8_8_8_REV:	        
						extract8888rev(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_INT_10_10_10_2:	        
						extract1010102(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_UNSIGNED_INT_2_10_10_10_REV:
						extract2101010rev(myswap_bytes,iter,extractComponents);
						for (k = 0; k < 4; k++) {
							*iter2++ = (GLushort)(extractComponents[k]*65535);
						}
						break;
					case GL_INT:
					case GL_UNSIGNED_INT:
					case GL_FLOAT:
						if (myswap_bytes) {
							widget.ub[0] = iter[3];
							widget.ub[1] = iter[2];
							widget.ub[2] = iter[1];
							widget.ub[3] = iter[0];
						} else {
							widget.ub[0] = iter[0];
							widget.ub[1] = iter[1];
							widget.ub[2] = iter[2];
							widget.ub[3] = iter[3];
						}
						if (type == GL_FLOAT) {
							if (index_format) {
								*iter2++ = widget.f;
							} else {
								*iter2++ = 65535 * widget.f;
							}
						} else if (type == GL_UNSIGNED_INT) {
							if (index_format) {
								*iter2++ = widget.ui;
							} else {
								*iter2++ = widget.ui >> 16;
							}
						} else {
							if (index_format) {
								*iter2++ = widget.i;
							} else {
								*iter2++ = widget.i >> 15;
							}
						}
						break;
				}
				iter += element_size;
			} /* for j */
			start += rowsize;
#if 1
			/* want 'iter' pointing at start, not within, row for assertion
			 * purposes
			 */
			iter= start;        
#endif
		} /* for i */
		
		/* iterators should be one byte past end */
		if (!isTypePackedPixel(type)) {
			assert(iter2 == &newimage[width*height*components]);
		}
		else {
			assert(iter2 == &newimage[width*height*
									  elements_per_group(format,0)]);
		}
		assert( iter == &((const GLubyte *)userdata)[rowsize*height +
													 psm->unpack_skip_rows * rowsize +
													 psm->unpack_skip_pixels * group_size] );
		
    } /* else */
} /* fill_image() */

static GLint is_index(GLenum format)
{
    return format == GL_COLOR_INDEX || format == GL_STENCIL_INDEX;
}

static GLint elements_per_group(GLenum format, GLenum type)
{
    /*
     * Return the number of elements per group of a specified format
     */
	
    /* If the type is packedpixels then answer is 1 (ignore format) */
    if (type == GL_UNSIGNED_BYTE_3_3_2 ||
		type == GL_UNSIGNED_BYTE_2_3_3_REV ||
		type == GL_UNSIGNED_SHORT_5_6_5 ||
		type == GL_UNSIGNED_SHORT_5_6_5_REV ||
		type == GL_UNSIGNED_SHORT_4_4_4_4 ||
		type == GL_UNSIGNED_SHORT_5_5_5_1  ||
		type == GL_UNSIGNED_SHORT_1_5_5_5_REV  ||
		type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV ||
		type == GL_UNSIGNED_INT_10_10_10_2 ||
		type == GL_UNSIGNED_INT_2_10_10_10_REV) {
		return 1;
    }
	
    /* Types are not packed pixels, so get elements per group */
    switch(format) {
		case GL_RGB:
		case GL_BGR:
			return 3;
		case GL_LUMINANCE_ALPHA:
			return 2;
		case GL_RGBA:
			return 4;
		default:
			return 1;
    }
}

static void scale_internal(GLint components, GLint widthin, GLint heightin,
						   const GLushort *datain,
						   GLint widthout, GLint heightout,
						   GLushort *dataout)
{
    float x, lowx, highx, convx, halfconvx;
    float y, lowy, highy, convy, halfconvy;
    float xpercent,ypercent;
    float percent;
    /* Max components in a format is 4, so... */
    float totals[4];
    float area;
    int i,j,k,yint,xint,xindex,yindex;
    int temp;
	
    if (widthin == widthout*2 && heightin == heightout*2) {
		halveImage(components, widthin, heightin, datain, dataout);
		return;
    }
    convy = (float) heightin/heightout;
    convx = (float) widthin/widthout;
    halfconvx = convx/2;
    halfconvy = convy/2;
    for (i = 0; i < heightout; i++) {
		y = convy * (i+0.5);
		if (heightin > heightout) {
			highy = y + halfconvy;
			lowy = y - halfconvy;
		} else {
			highy = y + 0.5;
			lowy = y - 0.5;
		}
		for (j = 0; j < widthout; j++) {
			x = convx * (j+0.5);
			if (widthin > widthout) {
				highx = x + halfconvx;
				lowx = x - halfconvx;
			} else {
				highx = x + 0.5;
				lowx = x - 0.5;
			}
			
			/*
			 ** Ok, now apply box filter to box that goes from (lowx, lowy)
			 ** to (highx, highy) on input data into this pixel on output
			 ** data.
			 */
			totals[0] = totals[1] = totals[2] = totals[3] = 0.0;
			area = 0.0;
			
			y = lowy;
			yint = floor(y);
			while (y < highy) {
				yindex = (yint + heightin) % heightin;
				if (highy < yint+1) {
					ypercent = highy - y;
				} else {
					ypercent = yint+1 - y;
				}
				
				x = lowx;
				xint = floor(x);
				
				while (x < highx) {
					xindex = (xint + widthin) % widthin;
					if (highx < xint+1) {
						xpercent = highx - x;
					} else {
						xpercent = xint+1 - x;
					}
					
					percent = xpercent * ypercent;
					area += percent;
					temp = (xindex + (yindex * widthin)) * components;
					for (k = 0; k < components; k++) {
						totals[k] += datain[temp + k] * percent;
					}
					
					xint++;
					x = xint;
				}
				yint++;
				y = yint;
			}
			
			temp = (j + (i * widthout)) * components;
			for (k = 0; k < components; k++) {
				/* totals[] should be rounded in the case of enlarging an RGB
				 * ramp when the type is 332 or 4444
				 */
				dataout[temp + k] = (totals[k]+0.5)/area;
			}
		}
    }
}

/*
 ** Insert array into user's data applying all pixel store modes.
 ** The internal format is an array of unsigned shorts.
 ** empty_image() because it is the opposite of fill_image().
 */
static void empty_image(const PixelStorageModes *psm,
						GLint width, GLint height, GLenum format,
						GLenum type, GLboolean index_format,
						const GLushort *oldimage, void *userdata)
{
    GLint components;
    GLint element_size;
    GLint rowsize;
    GLint padding;
    GLint groups_per_line;
    GLint group_size;
    GLint elements_per_line;
    GLubyte *start;
    GLubyte *iter;
    const GLushort *iter2;
    GLint i, j, k;
    GLint myswap_bytes;
	
    myswap_bytes = psm->pack_swap_bytes;
    components = elements_per_group(format,type);
    if (psm->pack_row_length > 0) {
		groups_per_line = psm->pack_row_length;
    } else {
		groups_per_line = width;
    }
	
    /* All formats except GL_BITMAP fall out trivially */
    if (type == GL_BITMAP) {
		GLint bit_offset;
		GLint current_bit;
		
		rowsize = (groups_per_line * components + 7) / 8;
		padding = (rowsize % psm->pack_alignment);
		if (padding) {
			rowsize += psm->pack_alignment - padding;
		}
		start = (GLubyte *) userdata + psm->pack_skip_rows * rowsize +
		(psm->pack_skip_pixels * components / 8);
		elements_per_line = width * components;
		iter2 = oldimage;
		for (i = 0; i < height; i++) {
			iter = start;
			bit_offset = (psm->pack_skip_pixels * components) % 8;
			for (j = 0; j < elements_per_line; j++) {
				if (index_format) {
					current_bit = iter2[0] & 1;
				} else {
					if (iter2[0] > 32767) {
						current_bit = 1;
					} else {
						current_bit = 0;
					}
				}
				
				if (current_bit) {
					if (psm->pack_lsb_first) {
						*iter |= (1 << bit_offset);
					} else {
						*iter |= (1 << (7 - bit_offset));
					}
				} else {
					if (psm->pack_lsb_first) {
						*iter &= ~(1 << bit_offset);
					} else {
						*iter &= ~(1 << (7 - bit_offset));
					}
				}
				
				bit_offset++;
				if (bit_offset == 8) {
					bit_offset = 0;
					iter++;
				}
				iter2++;
			}
			start += rowsize;
		}
    } else {
		float shoveComponents[4];
		
		element_size = bytes_per_element(type);
		group_size = element_size * components;
		if (element_size == 1) myswap_bytes = 0;
		
		rowsize = groups_per_line * group_size;
		padding = (rowsize % psm->pack_alignment);
		if (padding) {
			rowsize += psm->pack_alignment - padding;
		}
		start = (GLubyte *) userdata + psm->pack_skip_rows * rowsize +
		psm->pack_skip_pixels * group_size;
		elements_per_line = width * components;
		
		iter2 = oldimage;
		for (i = 0; i < height; i++) {
			iter = start;
			for (j = 0; j < elements_per_line; j++) {
				Type_Widget widget;
				
				switch(type) {
					case GL_UNSIGNED_BYTE_3_3_2:
						for (k = 0; k < 3; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove332(shoveComponents,0,(void *)iter);
						break;
					case GL_UNSIGNED_BYTE_2_3_3_REV:
						for (k = 0; k < 3; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove233rev(shoveComponents,0,(void *)iter);
						break;
					case GL_UNSIGNED_BYTE:
						if (index_format) {
							*iter = *iter2++;
						} else {
							*iter = *iter2++ >> 8;
						}
						break;
					case GL_BYTE:
						if (index_format) {
							*((GLbyte *) iter) = *iter2++;
						} else {
							*((GLbyte *) iter) = *iter2++ >> 9;
						}
						break;
					case GL_UNSIGNED_SHORT_5_6_5:         
						for (k = 0; k < 3; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove565(shoveComponents,0,(void *)&widget.us[0]);
						if (myswap_bytes) {
							iter[0] = widget.ub[1];
							iter[1] = widget.ub[0];
						}
						else {
							*(GLushort *)iter = widget.us[0];
						}
						break;
					case GL_UNSIGNED_SHORT_5_6_5_REV:	        
						for (k = 0; k < 3; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove565rev(shoveComponents,0,(void *)&widget.us[0]);
						if (myswap_bytes) {
							iter[0] = widget.ub[1];
							iter[1] = widget.ub[0];
						}
						else {
							*(GLushort *)iter = widget.us[0];
						}
						break;
					case GL_UNSIGNED_SHORT_4_4_4_4:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove4444(shoveComponents,0,(void *)&widget.us[0]);
						if (myswap_bytes) {
							iter[0] = widget.ub[1];
							iter[1] = widget.ub[0];
						} else {
							*(GLushort *)iter = widget.us[0];
						}
						break;
					case GL_UNSIGNED_SHORT_5_5_5_1:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove5551(shoveComponents,0,(void *)&widget.us[0]);
						if (myswap_bytes) {
							iter[0] = widget.ub[1];
							iter[1] = widget.ub[0];
						} else {
							*(GLushort *)iter = widget.us[0];
						}
						break;
					case GL_UNSIGNED_SHORT_1_5_5_5_REV:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove1555rev(shoveComponents,0,(void *)&widget.us[0]);
						if (myswap_bytes) {
							iter[0] = widget.ub[1];
							iter[1] = widget.ub[0];
						} else {
							*(GLushort *)iter = widget.us[0];
						}
						break;
					case GL_UNSIGNED_SHORT:
					case GL_SHORT:
						if (type == GL_SHORT) {
							if (index_format) {
								widget.s[0] = *iter2++;
							} else {
								widget.s[0] = *iter2++ >> 1;
							}
						} else {
							widget.us[0] = *iter2++;
						}
						if (myswap_bytes) {
							iter[0] = widget.ub[1];
							iter[1] = widget.ub[0];
						} else {
							iter[0] = widget.ub[0];
							iter[1] = widget.ub[1];
						}
						break;
					case GL_UNSIGNED_INT_8_8_8_8:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove8888(shoveComponents,0,(void *)&widget.ui);
						if (myswap_bytes) {
							iter[3] = widget.ub[0];
							iter[2] = widget.ub[1];
							iter[1] = widget.ub[2];
							iter[0] = widget.ub[3];
						} else {
							*(GLuint *)iter= widget.ui;
						}
						
						break;
					case GL_UNSIGNED_INT_8_8_8_8_REV:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove8888rev(shoveComponents,0,(void *)&widget.ui);
						if (myswap_bytes) {
							iter[3] = widget.ub[0];
							iter[2] = widget.ub[1];
							iter[1] = widget.ub[2];
							iter[0] = widget.ub[3];
						} else {
							*(GLuint *)iter= widget.ui;
						}
						break;
					case GL_UNSIGNED_INT_10_10_10_2:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove1010102(shoveComponents,0,(void *)&widget.ui);
						if (myswap_bytes) {
							iter[3] = widget.ub[0];
							iter[2] = widget.ub[1];
							iter[1] = widget.ub[2];
							iter[0] = widget.ub[3];
						} else {
							*(GLuint *)iter= widget.ui;
						}
						break;
					case GL_UNSIGNED_INT_2_10_10_10_REV:
						for (k = 0; k < 4; k++) {
							shoveComponents[k]= *iter2++ / 65535.0;
						}
						shove2101010rev(shoveComponents,0,(void *)&widget.ui);
						if (myswap_bytes) {
							iter[3] = widget.ub[0];
							iter[2] = widget.ub[1];
							iter[1] = widget.ub[2];
							iter[0] = widget.ub[3];
						} else {
							*(GLuint *)iter= widget.ui;
						}
						break;
					case GL_INT:
					case GL_UNSIGNED_INT:
					case GL_FLOAT:
						if (type == GL_FLOAT) {
							if (index_format) {
								widget.f = *iter2++;
							} else {
								widget.f = *iter2++ / (float) 65535.0;
							}
						} else if (type == GL_UNSIGNED_INT) {
							if (index_format) {
								widget.ui = *iter2++;
							} else {
								widget.ui = (unsigned int) *iter2++ * 65537;
							}
						} else {
							if (index_format) {
								widget.i = *iter2++;
							} else {
								widget.i = ((unsigned int) *iter2++ * 65537)/2;
							}
						}
						if (myswap_bytes) {
							iter[3] = widget.ub[0];
							iter[2] = widget.ub[1];
							iter[1] = widget.ub[2];
							iter[0] = widget.ub[3];
						} else {
							iter[0] = widget.ub[0];
							iter[1] = widget.ub[1];
							iter[2] = widget.ub[2];
							iter[3] = widget.ub[3];
						}
						break;
				}
				iter += element_size;
			} /* for j */
			start += rowsize;
#if 1
			/* want 'iter' pointing at start, not within, row for assertion
			 * purposes
			 */
			iter= start;        
#endif
		} /* for i */
		
		/* iterators should be one byte past end */
		if (!isTypePackedPixel(type)) {
			assert(iter2 == &oldimage[width*height*components]);
		}
		else {
			assert(iter2 == &oldimage[width*height*
									  elements_per_group(format,0)]);
		}
		assert( iter == &((GLubyte *)userdata)[rowsize*height +
											   psm->pack_skip_rows * rowsize +
											   psm->pack_skip_pixels * group_size] );
		
    } /* else */
} /* empty_image() */

static GLboolean isTypePackedPixel(GLenum type)
{
	assert(legalType(type));
	
	if (type == GL_UNSIGNED_BYTE_3_3_2 ||
		type == GL_UNSIGNED_BYTE_2_3_3_REV ||
		type == GL_UNSIGNED_SHORT_5_6_5 ||
		type == GL_UNSIGNED_SHORT_5_6_5_REV ||
		type == GL_UNSIGNED_SHORT_4_4_4_4 ||
		type == GL_UNSIGNED_SHORT_5_5_5_1 ||
		type == GL_UNSIGNED_SHORT_1_5_5_5_REV ||
		type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV ||
		type == GL_UNSIGNED_INT_10_10_10_2 ||
		type == GL_UNSIGNED_INT_2_10_10_10_REV) {
		return 1;
	}
	else return 0;
}

static GLfloat bytes_per_element(GLenum type)
{
    /*
     * Return the number of bytes per element, based on the element type
     */
    switch(type) {
		case GL_BITMAP:
			return 1.0 / 8.0;
		case GL_UNSIGNED_SHORT:
			return(sizeof(GLushort));
		case GL_SHORT:
			return(sizeof(GLshort));
		case GL_UNSIGNED_BYTE:
			return(sizeof(GLubyte));
		case GL_BYTE:
			return(sizeof(GLbyte));
		case GL_INT:
			return(sizeof(GLint));
		case GL_UNSIGNED_INT:
			return(sizeof(GLuint));
		case GL_FLOAT:
			return(sizeof(GLfloat));
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:  
			return(sizeof(GLubyte));
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			return(sizeof(GLushort));
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			return(sizeof(GLuint));
		default:
			return 4;
    }
}

static void halveImage(GLint components, GLuint width, GLuint height,
					   const GLushort *datain, GLushort *dataout)
{
    int i, j, k;
    int newwidth, newheight;
    int delta;
    GLushort *s;
    const GLushort *t;
	
    newwidth = width / 2;
    newheight = height / 2;
    delta = width * components;
    s = dataout;
    t = datain;
	
    /* Piece o' cake! */
    for (i = 0; i < newheight; i++) {
		for (j = 0; j < newwidth; j++) {
			for (k = 0; k < components; k++) {
				s[0] = (t[0] + t[components] + t[delta] +
						t[delta+components] + 2) / 4;
				s++; t++;
			}
			t += components;
		}
		t += delta;
    }
}


/*--------------------------------------------------------------------------
 * Decimation of packed pixel types
 *--------------------------------------------------------------------------
 */
static void extract332(int isSwap,
					   const void *packedPixel, GLfloat extractComponents[])
{
	GLubyte ubyte= *(const GLubyte *)packedPixel;
	
	isSwap= isSwap;		/* turn off warnings */
	
	/* 11100000 == 0xe0 */
	/* 00011100 == 0x1c */
	/* 00000011 == 0x03 */
	
	extractComponents[0]=   (float)((ubyte & 0xe0)  >> 5) / 7.0;
	extractComponents[1]=   (float)((ubyte & 0x1c)  >> 2) / 7.0; /* 7 = 2^3-1 */
	extractComponents[2]=   (float)((ubyte & 0x03)      ) / 3.0; /* 3 = 2^2-1 */
} /* extract332() */

static void shove332(const GLfloat shoveComponents[],
					 int index, void *packedPixel)      
{
	/* 11100000 == 0xe0 */
	/* 00011100 == 0x1c */
	/* 00000011 == 0x03 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLubyte *)packedPixel)[index]  =
	((GLubyte)((shoveComponents[0] * 7)+0.5)  << 5) & 0xe0;
	((GLubyte *)packedPixel)[index] |=
	((GLubyte)((shoveComponents[1] * 7)+0.5)  << 2) & 0x1c;
	((GLubyte *)packedPixel)[index]  |=
	((GLubyte)((shoveComponents[2] * 3)+0.5)	   ) & 0x03;
} /* shove332() */

static void extract233rev(int isSwap,
						  const void *packedPixel, GLfloat extractComponents[])
{
	GLubyte ubyte= *(const GLubyte *)packedPixel;
	
	isSwap= isSwap;		/* turn off warnings */
	
	/* 0000,0111 == 0x07 */
	/* 0011,1000 == 0x38 */
	/* 1100,0000 == 0xC0 */
	
	extractComponents[0]= (float)((ubyte & 0x07)     ) / 7.0;
	extractComponents[1]= (float)((ubyte & 0x38) >> 3) / 7.0;
	extractComponents[2]= (float)((ubyte & 0xC0) >> 6) / 3.0;
} /* extract233rev() */

static void shove233rev(const GLfloat shoveComponents[],
						int index, void *packedPixel)   
{
	/* 0000,0111 == 0x07 */
	/* 0011,1000 == 0x38 */
	/* 1100,0000 == 0xC0 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLubyte *)packedPixel)[index] =
	((GLubyte)((shoveComponents[0] * 7.0)+0.5)     ) & 0x07;
	((GLubyte *)packedPixel)[index]|=
	((GLubyte)((shoveComponents[1] * 7.0)+0.5) << 3) & 0x38;
	((GLubyte *)packedPixel)[index]|=
	((GLubyte)((shoveComponents[2] * 3.0)+0.5) << 6) & 0xC0;
} /* shove233rev() */

static void extract565(int isSwap,
					   const void *packedPixel, GLfloat extractComponents[])
{
	GLushort ushort= *(const GLushort *)packedPixel;
	
	if (isSwap) {
		ushort= __GLU_SWAP_2_BYTES(packedPixel);
	}
	else {
		ushort= *(const GLushort *)packedPixel;
	}
	
	/* 11111000,00000000 == 0xf800 */
	/* 00000111,11100000 == 0x07e0 */
	/* 00000000,00011111 == 0x001f */
	
	extractComponents[0]=(float)((ushort & 0xf800) >> 11) / 31.0;/* 31 = 2^5-1*/
	extractComponents[1]=(float)((ushort & 0x07e0) >>  5) / 63.0;/* 63 = 2^6-1*/
	extractComponents[2]=(float)((ushort & 0x001f)      ) / 31.0;
} /* extract565() */

static void shove565(const GLfloat shoveComponents[],
					 int index,void *packedPixel)
{
	/* 11111000,00000000 == 0xf800 */
	/* 00000111,11100000 == 0x07e0 */
	/* 00000000,00011111 == 0x001f */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLushort *)packedPixel)[index] =
	((GLushort)((shoveComponents[0] * 31)+0.5) << 11) & 0xf800;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[1] * 63)+0.5) <<  5) & 0x07e0;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[2] * 31)+0.5)      ) & 0x001f;
} /* shove565() */

static void extract565rev(int isSwap,
						  const void *packedPixel, GLfloat extractComponents[])
{
	GLushort ushort= *(const GLushort *)packedPixel;
	
	if (isSwap) {
		ushort= __GLU_SWAP_2_BYTES(packedPixel);
	}
	else {
		ushort= *(const GLushort *)packedPixel;
	}
	
	/* 00000000,00011111 == 0x001f */
	/* 00000111,11100000 == 0x07e0 */
	/* 11111000,00000000 == 0xf800 */
	
	extractComponents[0]= (float)((ushort & 0x001F)	) / 31.0;
	extractComponents[1]= (float)((ushort & 0x07E0) >>  5) / 63.0;
	extractComponents[2]= (float)((ushort & 0xF800) >> 11) / 31.0;
} /* extract565rev() */

static void shove565rev(const GLfloat shoveComponents[],
						int index,void *packedPixel)
{
	/* 00000000,00011111 == 0x001f */
	/* 00000111,11100000 == 0x07e0 */
	/* 11111000,00000000 == 0xf800 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLushort *)packedPixel)[index] =
	((GLushort)((shoveComponents[0] * 31.0)+0.5)      ) & 0x001F;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[1] * 63.0)+0.5) <<  5) & 0x07E0;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[2] * 31.0)+0.5) << 11) & 0xF800;
} /* shove565rev() */

static void extract4444(int isSwap,const void *packedPixel,
						GLfloat extractComponents[])
{
	GLushort ushort;
	
	if (isSwap) {
		ushort= __GLU_SWAP_2_BYTES(packedPixel);
	}
	else {
		ushort= *(const GLushort *)packedPixel;
	}
	
	/* 11110000,00000000 == 0xf000 */
	/* 00001111,00000000 == 0x0f00 */
	/* 00000000,11110000 == 0x00f0 */
	/* 00000000,00001111 == 0x000f */
	
	extractComponents[0]= (float)((ushort & 0xf000) >> 12) / 15.0;/* 15=2^4-1 */
	extractComponents[1]= (float)((ushort & 0x0f00) >>  8) / 15.0;
	extractComponents[2]= (float)((ushort & 0x00f0) >>  4) / 15.0;
	extractComponents[3]= (float)((ushort & 0x000f)	) / 15.0;
} /* extract4444() */

static void shove4444(const GLfloat shoveComponents[],
					  int index,void *packedPixel)
{
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLushort *)packedPixel)[index] =
	((GLushort)((shoveComponents[0] * 15)+0.5) << 12) & 0xf000;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[1] * 15)+0.5) <<  8) & 0x0f00;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[2] * 15)+0.5) <<  4) & 0x00f0;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[3] * 15)+0.5)      ) & 0x000f;
} /* shove4444() */

static void extract4444rev(int isSwap,const void *packedPixel,
						   GLfloat extractComponents[])
{
	GLushort ushort;
	
	if (isSwap) {
		ushort= __GLU_SWAP_2_BYTES(packedPixel);
	}
	else {
		ushort= *(const GLushort *)packedPixel;
	}
	
	/* 00000000,00001111 == 0x000f */
	/* 00000000,11110000 == 0x00f0 */
	/* 00001111,00000000 == 0x0f00 */
	/* 11110000,00000000 == 0xf000 */
	
	/* 15 = 2^4-1 */
	extractComponents[0]= (float)((ushort & 0x000F)	) / 15.0;
	extractComponents[1]= (float)((ushort & 0x00F0) >>  4) / 15.0;
	extractComponents[2]= (float)((ushort & 0x0F00) >>  8) / 15.0;
	extractComponents[3]= (float)((ushort & 0xF000) >> 12) / 15.0;
} /* extract4444rev() */

static void shove4444rev(const GLfloat shoveComponents[],
						 int index,void *packedPixel)
{
	/* 00000000,00001111 == 0x000f */
	/* 00000000,11110000 == 0x00f0 */
	/* 00001111,00000000 == 0x0f00 */
	/* 11110000,00000000 == 0xf000 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLushort *)packedPixel)[index] =
	((GLushort)((shoveComponents[0] * 15)+0.5)      ) & 0x000F;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[1] * 15)+0.5) <<  4) & 0x00F0;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[2] * 15)+0.5) <<  8) & 0x0F00;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[3] * 15)+0.5) << 12) & 0xF000;
} /* shove4444rev() */

static void extract5551(int isSwap,const void *packedPixel,
						GLfloat extractComponents[])
{
	GLushort ushort;
	
	if (isSwap) {
		ushort= __GLU_SWAP_2_BYTES(packedPixel);
	}
	else {
		ushort= *(const GLushort *)packedPixel;
	}
	
	/* 11111000,00000000 == 0xf800 */
	/* 00000111,11000000 == 0x07c0 */
	/* 00000000,00111110 == 0x003e */
	/* 00000000,00000001 == 0x0001 */
	
	extractComponents[0]=(float)((ushort & 0xf800) >> 11) / 31.0;/* 31 = 2^5-1*/
	extractComponents[1]=(float)((ushort & 0x07c0) >>  6) / 31.0;
	extractComponents[2]=(float)((ushort & 0x003e) >>  1) / 31.0;
	extractComponents[3]=(float)((ushort & 0x0001)      );
} /* extract5551() */

static void shove5551(const GLfloat shoveComponents[],
					  int index,void *packedPixel)
{
	/* 11111000,00000000 == 0xf800 */
	/* 00000111,11000000 == 0x07c0 */
	/* 00000000,00111110 == 0x003e */
	/* 00000000,00000001 == 0x0001 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLushort *)packedPixel)[index]  =
	((GLushort)((shoveComponents[0] * 31)+0.5) << 11) & 0xf800;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[1] * 31)+0.5) <<  6) & 0x07c0;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[2] * 31)+0.5) <<  1) & 0x003e;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[3])+0.5)	     ) & 0x0001;
} /* shove5551() */

static void extract1555rev(int isSwap,const void *packedPixel,
						   GLfloat extractComponents[])
{
	GLushort ushort;
	
	if (isSwap) {
		ushort= __GLU_SWAP_2_BYTES(packedPixel);
	}
	else {
		ushort= *(const GLushort *)packedPixel;
	}
	
	/* 00000000,00011111 == 0x001F */
	/* 00000011,11100000 == 0x03E0 */
	/* 01111100,00000000 == 0x7C00 */
	/* 10000000,00000000 == 0x8000 */
	
	/* 31 = 2^5-1 */
	extractComponents[0]= (float)((ushort & 0x001F)	) / 31.0;
	extractComponents[1]= (float)((ushort & 0x03E0) >>  5) / 31.0;
	extractComponents[2]= (float)((ushort & 0x7C00) >> 10) / 31.0;
	extractComponents[3]= (float)((ushort & 0x8000) >> 15);
} /* extract1555rev() */

static void shove1555rev(const GLfloat shoveComponents[],
						 int index,void *packedPixel)
{
	/* 00000000,00011111 == 0x001F */
	/* 00000011,11100000 == 0x03E0 */
	/* 01111100,00000000 == 0x7C00 */
	/* 10000000,00000000 == 0x8000 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLushort *)packedPixel)[index] =
	((GLushort)((shoveComponents[0] * 31)+0.5)      ) & 0x001F;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[1] * 31)+0.5) <<  5) & 0x03E0;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[2] * 31)+0.5) << 10) & 0x7C00;
	((GLushort *)packedPixel)[index]|=
	((GLushort)((shoveComponents[3])+0.5)	<< 15) & 0x8000;
} /* shove1555rev() */

static void extract8888(int isSwap,
						const void *packedPixel, GLfloat extractComponents[])
{
	GLuint uint;
	
	if (isSwap) {
		uint= __GLU_SWAP_4_BYTES(packedPixel);
	}
	else {
		uint= *(const GLuint *)packedPixel;
	}
	
	/* 11111111,00000000,00000000,00000000 == 0xff000000 */
	/* 00000000,11111111,00000000,00000000 == 0x00ff0000 */
	/* 00000000,00000000,11111111,00000000 == 0x0000ff00 */
	/* 00000000,00000000,00000000,11111111 == 0x000000ff */
	
	/* 255 = 2^8-1 */
	extractComponents[0]= (float)((uint & 0xff000000) >> 24) / 255.0;
	extractComponents[1]= (float)((uint & 0x00ff0000) >> 16) / 255.0;
	extractComponents[2]= (float)((uint & 0x0000ff00) >>  8) / 255.0;
	extractComponents[3]= (float)((uint & 0x000000ff)	  ) / 255.0;
} /* extract8888() */

static void shove8888(const GLfloat shoveComponents[],
					  int index,void *packedPixel)
{
	/* 11111111,00000000,00000000,00000000 == 0xff000000 */
	/* 00000000,11111111,00000000,00000000 == 0x00ff0000 */
	/* 00000000,00000000,11111111,00000000 == 0x0000ff00 */
	/* 00000000,00000000,00000000,11111111 == 0x000000ff */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLuint *)packedPixel)[index] =
	((GLuint)((shoveComponents[0] * 255)+0.5) << 24) & 0xff000000;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[1] * 255)+0.5) << 16) & 0x00ff0000;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[2] * 255)+0.5) <<  8) & 0x0000ff00;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[3] * 255)+0.5)	    ) & 0x000000ff;
} /* shove8888() */

static void extract8888rev(int isSwap,
						   const void *packedPixel,GLfloat extractComponents[])
{
	GLuint uint;
	
	if (isSwap) {
		uint= __GLU_SWAP_4_BYTES(packedPixel);
	}
	else {
		uint= *(const GLuint *)packedPixel;
	}
	
	/* 00000000,00000000,00000000,11111111 == 0x000000ff */
	/* 00000000,00000000,11111111,00000000 == 0x0000ff00 */
	/* 00000000,11111111,00000000,00000000 == 0x00ff0000 */
	/* 11111111,00000000,00000000,00000000 == 0xff000000 */
	
	/* 255 = 2^8-1 */
	extractComponents[0]= (float)((uint & 0x000000FF)	  ) / 255.0;
	extractComponents[1]= (float)((uint & 0x0000FF00) >>  8) / 255.0;
	extractComponents[2]= (float)((uint & 0x00FF0000) >> 16) / 255.0;
	extractComponents[3]= (float)((uint & 0xFF000000) >> 24) / 255.0;
} /* extract8888rev() */

static void shove8888rev(const GLfloat shoveComponents[],
						 int index,void *packedPixel)
{
	/* 00000000,00000000,00000000,11111111 == 0x000000ff */
	/* 00000000,00000000,11111111,00000000 == 0x0000ff00 */
	/* 00000000,11111111,00000000,00000000 == 0x00ff0000 */
	/* 11111111,00000000,00000000,00000000 == 0xff000000 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLuint *)packedPixel)[index] =
	((GLuint)((shoveComponents[0] * 255)+0.5)	    ) & 0x000000FF;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[1] * 255)+0.5) <<  8) & 0x0000FF00;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[2] * 255)+0.5) << 16) & 0x00FF0000;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[3] * 255)+0.5) << 24) & 0xFF000000;
} /* shove8888rev() */

static void extract1010102(int isSwap,
						   const void *packedPixel,GLfloat extractComponents[])
{
	GLuint uint;
	
	if (isSwap) {
		uint= __GLU_SWAP_4_BYTES(packedPixel);
	}
	else {
		uint= *(const GLuint *)packedPixel;
	}
	
	/* 11111111,11000000,00000000,00000000 == 0xffc00000 */
	/* 00000000,00111111,11110000,00000000 == 0x003ff000 */
	/* 00000000,00000000,00001111,11111100 == 0x00000ffc */
	/* 00000000,00000000,00000000,00000011 == 0x00000003 */
	
	/* 1023 = 2^10-1 */
	extractComponents[0]= (float)((uint & 0xffc00000) >> 22) / 1023.0;
	extractComponents[1]= (float)((uint & 0x003ff000) >> 12) / 1023.0;
	extractComponents[2]= (float)((uint & 0x00000ffc) >>  2) / 1023.0;
	extractComponents[3]= (float)((uint & 0x00000003)	  ) / 3.0;
} /* extract1010102() */

static void shove1010102(const GLfloat shoveComponents[],
						 int index,void *packedPixel)
{
	/* 11111111,11000000,00000000,00000000 == 0xffc00000 */
	/* 00000000,00111111,11110000,00000000 == 0x003ff000 */
	/* 00000000,00000000,00001111,11111100 == 0x00000ffc */
	/* 00000000,00000000,00000000,00000011 == 0x00000003 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLuint *)packedPixel)[index] =
	((GLuint)((shoveComponents[0] * 1023)+0.5) << 22) & 0xffc00000;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[1] * 1023)+0.5) << 12) & 0x003ff000;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[2] * 1023)+0.5) <<  2) & 0x00000ffc;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[3] * 3)+0.5)	     ) & 0x00000003;
} /* shove1010102() */

static void extract2101010rev(int isSwap,
							  const void *packedPixel,
							  GLfloat extractComponents[])
{
	GLuint uint;
	
	if (isSwap) {
		uint= __GLU_SWAP_4_BYTES(packedPixel);
	}
	else {
		uint= *(const GLuint *)packedPixel;
	}
	
	/* 00000000,00000000,00000011,11111111 == 0x000003FF */
	/* 00000000,00001111,11111100,00000000 == 0x000FFC00 */
	/* 00111111,11110000,00000000,00000000 == 0x3FF00000 */
	/* 11000000,00000000,00000000,00000000 == 0xC0000000 */
	
	/* 1023 = 2^10-1 */
	extractComponents[0]= (float)((uint & 0x000003FF)	  ) / 1023.0;
	extractComponents[1]= (float)((uint & 0x000FFC00) >> 10) / 1023.0;
	extractComponents[2]= (float)((uint & 0x3FF00000) >> 20) / 1023.0;
	extractComponents[3]= (float)((uint & 0xC0000000) >> 30) / 3.0;
	/* 3 = 2^2-1 */
} /* extract2101010rev() */

static void shove2101010rev(const GLfloat shoveComponents[],
							int index,void *packedPixel)
{
	/* 00000000,00000000,00000011,11111111 == 0x000003FF */
	/* 00000000,00001111,11111100,00000000 == 0x000FFC00 */
	/* 00111111,11110000,00000000,00000000 == 0x3FF00000 */
	/* 11000000,00000000,00000000,00000000 == 0xC0000000 */
	
	assert(0.0 <= shoveComponents[0] && shoveComponents[0] <= 1.0);
	assert(0.0 <= shoveComponents[1] && shoveComponents[1] <= 1.0);
	assert(0.0 <= shoveComponents[2] && shoveComponents[2] <= 1.0);
	assert(0.0 <= shoveComponents[3] && shoveComponents[3] <= 1.0);
	
	/* due to limited precision, need to round before shoving */
	((GLuint *)packedPixel)[index] =
	((GLuint)((shoveComponents[0] * 1023)+0.5)      ) & 0x000003FF;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[1] * 1023)+0.5) << 10) & 0x000FFC00;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[2] * 1023)+0.5) << 20) & 0x3FF00000;
	((GLuint *)packedPixel)[index]|=
	((GLuint)((shoveComponents[3] * 3)+0.5)	<< 30) & 0xC0000000;
} /* shove2101010rev() */

