/***************************************************************************/
/*                                                                         */
/*  cidobjs.c                                                              */
/*                                                                         */
/*    CID objects manager (body).                                          */
/*                                                                         */
/*  Copyright 1996-2001 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include <ft2build.h>
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include "cidgload.h"
#include "cidload.h"
#include FT_INTERNAL_POSTSCRIPT_NAMES_H
#include FT_INTERNAL_POSTSCRIPT_AUX_H
#include FT_INTERNAL_POSTSCRIPT_HINTS_H

#include "ciderrs.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_cidobjs


  /*************************************************************************/
  /*                                                                       */
  /*                            SLOT  FUNCTIONS                            */
  /*                                                                       */
  /*************************************************************************/

  FT_LOCAL_DEF void
  CID_GlyphSlot_Done( CID_GlyphSlot  slot )
  {
    slot->root.internal->glyph_hints = 0;
  }


  FT_LOCAL_DEF FT_Error
  CID_GlyphSlot_Init( CID_GlyphSlot   slot )
  {
    CID_Face             face;
    PSHinter_Interface*  pshinter;


    face     = (CID_Face)slot->root.face;
    pshinter = (PSHinter_Interface *)face->pshinter;


    if ( pshinter )
    {
      FT_Module  module;


      module = FT_Get_Module( slot->root.face->driver->root.library,
                              "pshinter" );
      if ( module )
      {
        T1_Hints_Funcs  funcs;


        funcs = pshinter->get_t1_funcs( module );
        slot->root.internal->glyph_hints = (void*)funcs;
      }
    }

    return 0;
  }


  /*************************************************************************/
  /*                                                                       */
  /*                           SIZE  FUNCTIONS                             */
  /*                                                                       */
  /*************************************************************************/


  static PSH_Globals_Funcs
  CID_Size_Get_Globals_Funcs( CID_Size  size )
  {
    CID_Face             face     = (CID_Face)size->root.face;
    PSHinter_Interface*  pshinter = (PSHinter_Interface *)face->pshinter;
    FT_Module            module;


    module = FT_Get_Module( size->root.face->driver->root.library,
                            "pshinter" );
    return ( module && pshinter && pshinter->get_globals_funcs )
           ? pshinter->get_globals_funcs( module )
           : 0;
  }


  FT_LOCAL_DEF void
  CID_Size_Done( CID_Size  size )
  {
    if ( size->root.internal )
    {
      PSH_Globals_Funcs  funcs;


      funcs = CID_Size_Get_Globals_Funcs( size );
      if ( funcs )
        funcs->destroy( (PSH_Globals)size->root.internal );

      size->root.internal = 0;
    }
  }


  FT_LOCAL_DEF FT_Error
  CID_Size_Init( CID_Size  size )
  {
    FT_Error           error = 0;
    PSH_Globals_Funcs  funcs = CID_Size_Get_Globals_Funcs( size );


    if ( funcs )
    {
      PSH_Globals    globals;
      CID_Face       face = (CID_Face)size->root.face;
      CID_FontDict*  dict = face->cid.font_dicts + face->root.face_index;
      T1_Private*    priv = &dict->private_dict;


      error = funcs->create( size->root.face->memory, priv, &globals );
      if ( !error )
        size->root.internal = (FT_Size_Internal)(void*)globals;
    }

    return error;
  }


  FT_LOCAL_DEF FT_Error
  CID_Size_Reset( CID_Size  size )
  {
    PSH_Globals_Funcs  funcs = CID_Size_Get_Globals_Funcs( size );
    FT_Error           error = 0;


    if ( funcs )
      error = funcs->set_scale( (PSH_Globals)size->root.internal,
                                 size->root.metrics.x_scale,
                                 size->root.metrics.y_scale,
                                 0, 0 );
    return error;
  }




  /*************************************************************************/
  /*                                                                       */
  /*                           FACE  FUNCTIONS                             */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    CID_Face_Done                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalizes a given face object.                                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A pointer to the face object to destroy.                   */
  /*                                                                       */
  FT_LOCAL_DEF void
  CID_Face_Done( CID_Face  face )
  {
    FT_Memory  memory;


    if ( face )
    {
      CID_Info*     cid  = &face->cid;
      T1_FontInfo*  info = &cid->font_info;


      memory = face->root.memory;

      /* release subrs */
      if ( face->subrs )
      {
        FT_Int      n;
        

        for ( n = 0; n < cid->num_dicts; n++ )
        {
          CID_Subrs*  subr = face->subrs + n;
          

          if ( subr->code )
          {
            FREE( subr->code[0] );
            FREE( subr->code );
          }
        }

        FREE( face->subrs );
      }

      /* release FontInfo strings */
      FREE( info->version );
      FREE( info->notice );
      FREE( info->full_name );
      FREE( info->family_name );
      FREE( info->weight );

      /* release font dictionaries */
      FREE( cid->font_dicts );
      cid->num_dicts = 0;

      /* release other strings */
      FREE( cid->cid_font_name );
      FREE( cid->registry );
      FREE( cid->ordering );

      face->root.family_name = 0;
      face->root.style_name  = 0;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    CID_Face_Init                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given CID face object.                               */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream     :: The source font stream.                              */
  /*                                                                       */
  /*    face_index :: The index of the font face in the resource.          */
  /*                                                                       */
  /*    num_params :: Number of additional generic parameters.  Ignored.   */
  /*                                                                       */
  /*    params     :: Additional generic parameters.  Ignored.             */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face       :: The newly built face object.                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF FT_Error
  CID_Face_Init( FT_Stream      stream,
                 CID_Face       face,
                 FT_Int         face_index,
                 FT_Int         num_params,
                 FT_Parameter*  params )
  {
    FT_Error             error;
    PSNames_Interface*   psnames;
    PSAux_Interface*     psaux;
    PSHinter_Interface*  pshinter;

    FT_UNUSED( num_params );
    FT_UNUSED( params );
    FT_UNUSED( face_index );
    FT_UNUSED( stream );


    face->root.num_faces = 1;

    psnames = (PSNames_Interface*)face->psnames;
    if ( !psnames )
    {
      psnames = (PSNames_Interface*)FT_Get_Module_Interface(
                  FT_FACE_LIBRARY( face ), "psnames" );

      face->psnames = psnames;
    }

    psaux = (PSAux_Interface*)face->psaux;
    if ( !psaux )
    {
      psaux = (PSAux_Interface*)FT_Get_Module_Interface(
                FT_FACE_LIBRARY( face ), "psaux" );

      face->psaux = psaux;
    }

    pshinter = (PSHinter_Interface*)face->pshinter;
    if ( !pshinter )
    {
      pshinter = (PSHinter_Interface*)FT_Get_Module_Interface(
                   FT_FACE_LIBRARY( face ), "pshinter" );

      face->pshinter = pshinter;
    }

    /* open the tokenizer; this will also check the font format */
    if ( FILE_Seek( 0 ) )
      goto Exit;

    error = CID_Open_Face( face );
    if ( error )
      goto Exit;

    /* if we just wanted to check the format, leave successfully now */
    if ( face_index < 0 )
      goto Exit;

    /* check the face index */
    if ( face_index != 0 )
    {
      FT_ERROR(( "CID_Face_Init: invalid face index\n" ));
      error = CID_Err_Invalid_Argument;
      goto Exit;
    }

    /* Now, load the font program into the face object */
    {
      /* Init the face object fields */
      /* Now set up root face fields */
      {
        FT_Face  root = (FT_Face)&face->root;


        root->num_glyphs   = face->cid.cid_count;
        root->num_charmaps = 0;

        root->face_index = face_index;
        root->face_flags = FT_FACE_FLAG_SCALABLE;

        root->face_flags |= FT_FACE_FLAG_HORIZONTAL;

        if ( face->cid.font_info.is_fixed_pitch )
          root->face_flags |= FT_FACE_FLAG_FIXED_WIDTH;

        /* XXX: TODO: add kerning with .afm support */

        /* get style name -- be careful, some broken fonts only */
        /* have a /FontName dictionary entry!                   */
        root->family_name = face->cid.font_info.family_name;
        if ( root->family_name )
        {
          char*  full   = face->cid.font_info.full_name;
          char*  family = root->family_name;

          while ( *family && *full == *family )
          {
            family++;
            full++;
          }

          root->style_name = ( *full == ' ' ) ? full + 1
                                              : (char *)"Regular";
        }
        else
        {
          /* do we have a `/FontName'? */
          if ( face->cid.cid_font_name )
          {
            root->family_name = face->cid.cid_font_name;
            root->style_name  = (char *)"Regular";
          }
        }

        /* no embedded bitmap support */
        root->num_fixed_sizes = 0;
        root->available_sizes = 0;

        root->bbox.xMin =  face->cid.font_bbox.xMin >> 16;
        root->bbox.yMin =  face->cid.font_bbox.yMin >> 16;
        root->bbox.xMax = (face->cid.font_bbox.xMax + 0xFFFFU) >> 16;
        root->bbox.yMax = (face->cid.font_bbox.yMax + 0xFFFFU) >> 16;

        if ( !root->units_per_EM )
          root->units_per_EM  = 1000;

        root->ascender  = (FT_Short)( root->bbox.yMax );
        root->descender = (FT_Short)( root->bbox.yMin );
        root->height    = (FT_Short)(
          ( ( root->ascender + root->descender ) * 12 ) / 10 );

        root->underline_position  = face->cid.font_info.underline_position;
        root->underline_thickness = face->cid.font_info.underline_thickness;

        root->internal->max_points   = 0;
        root->internal->max_contours = 0;
      }
    }


  Exit:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    CID_Driver_Init                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given CID driver object.                             */
  /*                                                                       */
  /* <Input>                                                               */
  /*    driver :: A handle to the target driver object.                    */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF FT_Error
  CID_Driver_Init( CID_Driver  driver )
  {
    FT_UNUSED( driver );

    return CID_Err_Ok;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    CID_Driver_Done                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalizes a given CID driver.                                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    driver :: A handle to the target CID driver.                       */
  /*                                                                       */
  FT_LOCAL_DEF void
  CID_Driver_Done( CID_Driver  driver )
  {
    FT_UNUSED( driver );
  }


/* END */
