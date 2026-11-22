////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Config.hpp>

#include <glad/gl.h>

namespace sf::priv
{
[[nodiscard]] GladGLContext& getGlDispatch();
}

#define SFML_GL_DISPATCH                 (::sf::priv::getGlDispatch())
#define glActiveTexture                  SFML_GL_DISPATCH.ActiveTexture
#define glAttachShader                   SFML_GL_DISPATCH.AttachShader
#define glBeginQuery                     SFML_GL_DISPATCH.BeginQuery
#define glBeginQueryEXT                  SFML_GL_DISPATCH.BeginQueryEXT
#define glBeginTransformFeedback         SFML_GL_DISPATCH.BeginTransformFeedback
#define glBindAttribLocation             SFML_GL_DISPATCH.BindAttribLocation
#define glBindBuffer                     SFML_GL_DISPATCH.BindBuffer
#define glBindBufferBase                 SFML_GL_DISPATCH.BindBufferBase
#define glBindFramebuffer                SFML_GL_DISPATCH.BindFramebuffer
#define glBindRenderbuffer               SFML_GL_DISPATCH.BindRenderbuffer
#define glBindTexture                    SFML_GL_DISPATCH.BindTexture
#define glBindVertexArray                SFML_GL_DISPATCH.BindVertexArray
#define glBlendEquation                  SFML_GL_DISPATCH.BlendEquation
#define glBlendEquationSeparate          SFML_GL_DISPATCH.BlendEquationSeparate
#define glBlendFunc                      SFML_GL_DISPATCH.BlendFunc
#define glBlendFuncSeparate              SFML_GL_DISPATCH.BlendFuncSeparate
#define glBlitFramebuffer                SFML_GL_DISPATCH.BlitFramebuffer
#define glBufferData                     SFML_GL_DISPATCH.BufferData
#define glBufferSubData                  SFML_GL_DISPATCH.BufferSubData
#define glCheckFramebufferStatus         SFML_GL_DISPATCH.CheckFramebufferStatus
#define glClear                          SFML_GL_DISPATCH.Clear
#define glClearColor                     SFML_GL_DISPATCH.ClearColor
#define glClearStencil                   SFML_GL_DISPATCH.ClearStencil
#define glClientActiveTexture            SFML_GL_DISPATCH.ClientActiveTexture
#define glClientWaitSync                 SFML_GL_DISPATCH.ClientWaitSync
#define glColorMask                      SFML_GL_DISPATCH.ColorMask
#define glColorPointer                   SFML_GL_DISPATCH.ColorPointer
#define glCompileShader                  SFML_GL_DISPATCH.CompileShader
#define glCopyBufferSubData              SFML_GL_DISPATCH.CopyBufferSubData
#define glCopyTexSubImage2D              SFML_GL_DISPATCH.CopyTexSubImage2D
#define glCreateProgram                  SFML_GL_DISPATCH.CreateProgram
#define glCreateShader                   SFML_GL_DISPATCH.CreateShader
#define glDeleteBuffers                  SFML_GL_DISPATCH.DeleteBuffers
#define glDeleteFramebuffers             SFML_GL_DISPATCH.DeleteFramebuffers
#define glDeleteProgram                  SFML_GL_DISPATCH.DeleteProgram
#define glDeleteQueries                  SFML_GL_DISPATCH.DeleteQueries
#define glDeleteQueriesEXT               SFML_GL_DISPATCH.DeleteQueriesEXT
#define glDeleteRenderbuffers            SFML_GL_DISPATCH.DeleteRenderbuffers
#define glDeleteShader                   SFML_GL_DISPATCH.DeleteShader
#define glDeleteSync                     SFML_GL_DISPATCH.DeleteSync
#define glDeleteTextures                 SFML_GL_DISPATCH.DeleteTextures
#define glDeleteVertexArrays             SFML_GL_DISPATCH.DeleteVertexArrays
#define glDisable                        SFML_GL_DISPATCH.Disable
#define glDisableClientState             SFML_GL_DISPATCH.DisableClientState
#define glDisableVertexAttribArray       SFML_GL_DISPATCH.DisableVertexAttribArray
#define glDrawArrays                     SFML_GL_DISPATCH.DrawArrays
#define glDrawArraysInstanced            SFML_GL_DISPATCH.DrawArraysInstanced
#define glEnable                         SFML_GL_DISPATCH.Enable
#define glEnableClientState              SFML_GL_DISPATCH.EnableClientState
#define glEnableVertexAttribArray        SFML_GL_DISPATCH.EnableVertexAttribArray
#define glEndQuery                       SFML_GL_DISPATCH.EndQuery
#define glEndQueryEXT                    SFML_GL_DISPATCH.EndQueryEXT
#define glEndTransformFeedback           SFML_GL_DISPATCH.EndTransformFeedback
#define glFenceSync                      SFML_GL_DISPATCH.FenceSync
#define glFinish                         SFML_GL_DISPATCH.Finish
#define glFlush                          SFML_GL_DISPATCH.Flush
#define glFramebufferRenderbuffer        SFML_GL_DISPATCH.FramebufferRenderbuffer
#define glFramebufferTexture2D           SFML_GL_DISPATCH.FramebufferTexture2D
#define glGenBuffers                     SFML_GL_DISPATCH.GenBuffers
#define glGenFramebuffers                SFML_GL_DISPATCH.GenFramebuffers
#define glGenQueries                     SFML_GL_DISPATCH.GenQueries
#define glGenQueriesEXT                  SFML_GL_DISPATCH.GenQueriesEXT
#define glGenRenderbuffers               SFML_GL_DISPATCH.GenRenderbuffers
#define glGenTextures                    SFML_GL_DISPATCH.GenTextures
#define glGenVertexArrays                SFML_GL_DISPATCH.GenVertexArrays
#define glGenerateMipmap                 SFML_GL_DISPATCH.GenerateMipmap
#define glGetActiveUniform               SFML_GL_DISPATCH.GetActiveUniform
#define glGetBooleanv                    SFML_GL_DISPATCH.GetBooleanv
#define glGetError                       SFML_GL_DISPATCH.GetError
#define glGetFloatv                      SFML_GL_DISPATCH.GetFloatv
#define glGetIntegeri_v                  SFML_GL_DISPATCH.GetIntegeri_v
#define glGetIntegerv                    SFML_GL_DISPATCH.GetIntegerv
#define glGetProgramInfoLog              SFML_GL_DISPATCH.GetProgramInfoLog
#define glGetProgramiv                   SFML_GL_DISPATCH.GetProgramiv
#define glGetQueryObjectui64v            SFML_GL_DISPATCH.GetQueryObjectui64v
#define glGetQueryObjectui64vEXT         SFML_GL_DISPATCH.GetQueryObjectui64vEXT
#define glGetQueryObjectuiv              SFML_GL_DISPATCH.GetQueryObjectuiv
#define glGetQueryObjectuivEXT           SFML_GL_DISPATCH.GetQueryObjectuivEXT
#define glGetQueryiv                     SFML_GL_DISPATCH.GetQueryiv
#define glGetQueryivEXT                  SFML_GL_DISPATCH.GetQueryivEXT
#define glGetShaderInfoLog               SFML_GL_DISPATCH.GetShaderInfoLog
#define glGetShaderiv                    SFML_GL_DISPATCH.GetShaderiv
#define glGetString                      SFML_GL_DISPATCH.GetString
#define glGetTexImage                    SFML_GL_DISPATCH.GetTexImage
#define glGetUniformLocation             SFML_GL_DISPATCH.GetUniformLocation
#define glIsEnabled                      SFML_GL_DISPATCH.IsEnabled
#define glIsProgram                      SFML_GL_DISPATCH.IsProgram
#define glIsTexture                      SFML_GL_DISPATCH.IsTexture
#define glLinkProgram                    SFML_GL_DISPATCH.LinkProgram
#define glLoadIdentity                   SFML_GL_DISPATCH.LoadIdentity
#define glLoadMatrixf                    SFML_GL_DISPATCH.LoadMatrixf
#define glMapBuffer                      SFML_GL_DISPATCH.MapBuffer
#define glMapBufferRange                 SFML_GL_DISPATCH.MapBufferRange
#define glMatrixMode                     SFML_GL_DISPATCH.MatrixMode
#define glPixelStorei                    SFML_GL_DISPATCH.PixelStorei
#define glPopAttrib                      SFML_GL_DISPATCH.PopAttrib
#define glPopClientAttrib                SFML_GL_DISPATCH.PopClientAttrib
#define glPopMatrix                      SFML_GL_DISPATCH.PopMatrix
#define glPushAttrib                     SFML_GL_DISPATCH.PushAttrib
#define glPushClientAttrib               SFML_GL_DISPATCH.PushClientAttrib
#define glPushMatrix                     SFML_GL_DISPATCH.PushMatrix
#define glReadPixels                     SFML_GL_DISPATCH.ReadPixels
#define glRenderbufferStorage            SFML_GL_DISPATCH.RenderbufferStorage
#define glRenderbufferStorageMultisample SFML_GL_DISPATCH.RenderbufferStorageMultisample
#define glScissor                        SFML_GL_DISPATCH.Scissor
#define glShaderSource                   SFML_GL_DISPATCH.ShaderSource
#define glStencilFunc                    SFML_GL_DISPATCH.StencilFunc
#define glStencilFuncSeparate            SFML_GL_DISPATCH.StencilFuncSeparate
#define glStencilMaskSeparate            SFML_GL_DISPATCH.StencilMaskSeparate
#define glStencilOp                      SFML_GL_DISPATCH.StencilOp
#define glStencilOpSeparate              SFML_GL_DISPATCH.StencilOpSeparate
#define glTexCoordPointer                SFML_GL_DISPATCH.TexCoordPointer
#define glTexImage2D                     SFML_GL_DISPATCH.TexImage2D
#define glTexParameteri                  SFML_GL_DISPATCH.TexParameteri
#define glTexSubImage2D                  SFML_GL_DISPATCH.TexSubImage2D
#define glTransformFeedbackVaryings      SFML_GL_DISPATCH.TransformFeedbackVaryings
#define glUniform1fv                     SFML_GL_DISPATCH.Uniform1fv
#define glUniform1i                      SFML_GL_DISPATCH.Uniform1i
#define glUniform1iv                     SFML_GL_DISPATCH.Uniform1iv
#define glUniform2fv                     SFML_GL_DISPATCH.Uniform2fv
#define glUniform2iv                     SFML_GL_DISPATCH.Uniform2iv
#define glUniform3fv                     SFML_GL_DISPATCH.Uniform3fv
#define glUniform3iv                     SFML_GL_DISPATCH.Uniform3iv
#define glUniform4f                      SFML_GL_DISPATCH.Uniform4f
#define glUniform4fv                     SFML_GL_DISPATCH.Uniform4fv
#define glUniform4iv                     SFML_GL_DISPATCH.Uniform4iv
#define glUniformMatrix3fv               SFML_GL_DISPATCH.UniformMatrix3fv
#define glUniformMatrix4fv               SFML_GL_DISPATCH.UniformMatrix4fv
#define glUnmapBuffer                    SFML_GL_DISPATCH.UnmapBuffer
#define glUseProgram                     SFML_GL_DISPATCH.UseProgram
#define glVertexAttribDivisor            SFML_GL_DISPATCH.VertexAttribDivisor
#define glVertexAttribDivisorARB         SFML_GL_DISPATCH.VertexAttribDivisorARB
#define glVertexAttribPointer            SFML_GL_DISPATCH.VertexAttribPointer
#define glVertexPointer                  SFML_GL_DISPATCH.VertexPointer
#define glViewport                       SFML_GL_DISPATCH.Viewport

#define SF_GLAD_GL_ARB_copy_buffer             (SFML_GL_DISPATCH.ARB_copy_buffer)
#define SF_GLAD_GL_ARB_framebuffer_object      (SFML_GL_DISPATCH.ARB_framebuffer_object)
#define SF_GLAD_GL_ARB_framebuffer_sRGB        (SFML_GL_DISPATCH.ARB_framebuffer_sRGB)
#define SF_GLAD_GL_ARB_geometry_shader4        (SFML_GL_DISPATCH.ARB_geometry_shader4)
#define SF_GLAD_GL_ES_VERSION_2_0              (SFML_GL_DISPATCH.ES_VERSION_2_0)
#define SF_GLAD_GL_ES_VERSION_3_0              (SFML_GL_DISPATCH.ES_VERSION_3_0)
#define SF_GLAD_GL_ES_VERSION_3_2              (SFML_GL_DISPATCH.ES_VERSION_3_2)
#define SF_GLAD_GL_EXT_framebuffer_blit        (SFML_GL_DISPATCH.EXT_framebuffer_blit)
#define SF_GLAD_GL_EXT_framebuffer_multisample (SFML_GL_DISPATCH.EXT_framebuffer_multisample)
#define SF_GLAD_GL_EXT_framebuffer_object      (SFML_GL_DISPATCH.EXT_framebuffer_object)
#define SF_GLAD_GL_EXT_framebuffer_sRGB        (SFML_GL_DISPATCH.EXT_framebuffer_sRGB)
#define SF_GLAD_GL_EXT_geometry_shader         (SFML_GL_DISPATCH.EXT_geometry_shader)
#define SF_GLAD_GL_EXT_packed_depth_stencil    (SFML_GL_DISPATCH.EXT_packed_depth_stencil)
#define SF_GLAD_GL_EXT_sRGB                    (SFML_GL_DISPATCH.EXT_sRGB)
#define SF_GLAD_GL_EXT_texture_sRGB            (SFML_GL_DISPATCH.EXT_texture_sRGB)
#define SF_GLAD_GL_OES_packed_depth_stencil    (SFML_GL_DISPATCH.OES_packed_depth_stencil)
#define SF_GLAD_GL_OES_texture_npot            (SFML_GL_DISPATCH.OES_texture_npot)
#define SF_GLAD_GL_VERSION_1_0                 (SFML_GL_DISPATCH.VERSION_1_0)
#define SF_GLAD_GL_VERSION_1_1                 (SFML_GL_DISPATCH.VERSION_1_1)
#define SF_GLAD_GL_VERSION_1_2                 (SFML_GL_DISPATCH.VERSION_1_2)
#define SF_GLAD_GL_VERSION_1_3                 (SFML_GL_DISPATCH.VERSION_1_3)
#define SF_GLAD_GL_VERSION_1_4                 (SFML_GL_DISPATCH.VERSION_1_4)
#define SF_GLAD_GL_VERSION_1_5                 (SFML_GL_DISPATCH.VERSION_1_5)
#define SF_GLAD_GL_VERSION_2_0                 (SFML_GL_DISPATCH.VERSION_2_0)
#define SF_GLAD_GL_VERSION_2_1                 (SFML_GL_DISPATCH.VERSION_2_1)
#define SF_GLAD_GL_VERSION_3_0                 (SFML_GL_DISPATCH.VERSION_3_0)
#define SF_GLAD_GL_VERSION_3_1                 (SFML_GL_DISPATCH.VERSION_3_1)
#define SF_GLAD_GL_VERSION_3_2                 (SFML_GL_DISPATCH.VERSION_3_2)
#define SF_GLAD_GL_VERSION_3_3                 (SFML_GL_DISPATCH.VERSION_3_3)
#define SF_GLAD_GL_VERSION_4_0                 (SFML_GL_DISPATCH.VERSION_4_0)
#define SF_GLAD_GL_VERSION_4_1                 (SFML_GL_DISPATCH.VERSION_4_1)
#define SF_GLAD_GL_VERSION_4_2                 (SFML_GL_DISPATCH.VERSION_4_2)
#define SF_GLAD_GL_VERSION_4_3                 (SFML_GL_DISPATCH.VERSION_4_3)
#define SF_GLAD_GL_VERSION_4_4                 (SFML_GL_DISPATCH.VERSION_4_4)
#define SF_GLAD_GL_VERSION_4_5                 (SFML_GL_DISPATCH.VERSION_4_5)
#define SF_GLAD_GL_VERSION_4_6                 (SFML_GL_DISPATCH.VERSION_4_6)

#ifdef SFML_OPENGL_ES

// OpenGL ES 3.0 is the graphics backend baseline.
#define GLEXT_multitexture    SF_GLAD_GL_ES_VERSION_2_0
#define GLEXT_glActiveTexture glActiveTexture
#define GLEXT_GL_TEXTURE0     GL_TEXTURE0

#define GLEXT_multitexture_dependencies SF_GLAD_GL_ES_VERSION_2_0, glActiveTexture

#define GLEXT_texture_edge_clamp true
#define GLEXT_GL_CLAMP           GL_CLAMP_TO_EDGE
#define GLEXT_GL_CLAMP_TO_EDGE   GL_CLAMP_TO_EDGE

#define GLEXT_vertex_buffer_object SF_GLAD_GL_ES_VERSION_2_0
#define GLEXT_glBindBuffer         glBindBuffer
#define GLEXT_glBufferData         glBufferData
#define GLEXT_glBufferSubData      glBufferSubData
#define GLEXT_glDeleteBuffers      glDeleteBuffers
#define GLEXT_glGenBuffers         glGenBuffers
#define GLEXT_GL_ARRAY_BUFFER      GL_ARRAY_BUFFER
#define GLEXT_GL_DYNAMIC_DRAW      GL_DYNAMIC_DRAW
#define GLEXT_GL_STATIC_DRAW       GL_STATIC_DRAW
#define GLEXT_GL_STREAM_DRAW       GL_STREAM_DRAW

#define GLEXT_vertex_buffer_object_dependencies \
    SF_GLAD_GL_ES_VERSION_2_0, glBindBuffer, glBufferData, glBufferSubData, glDeleteBuffers, glGenBuffers

#define GLEXT_blend_subtract           SF_GLAD_GL_ES_VERSION_2_0
#define GLEXT_glBlendEquation          glBlendEquation
#define GLEXT_GL_FUNC_ADD              GL_FUNC_ADD
#define GLEXT_GL_FUNC_SUBTRACT         GL_FUNC_SUBTRACT
#define GLEXT_GL_FUNC_REVERSE_SUBTRACT GL_FUNC_REVERSE_SUBTRACT

#define GLEXT_blend_subtract_dependencies SF_GLAD_GL_ES_VERSION_2_0, glBlendEquation

#define GLEXT_blend_func_separate SF_GLAD_GL_ES_VERSION_2_0
#define GLEXT_glBlendFuncSeparate glBlendFuncSeparate

#define GLEXT_blend_func_separate_dependencies SF_GLAD_GL_ES_VERSION_2_0, glBlendFuncSeparate

#define GLEXT_blend_equation_separate SF_GLAD_GL_ES_VERSION_2_0
#define GLEXT_glBlendEquationSeparate glBlendEquationSeparate

#define GLEXT_blend_equation_separate_dependencies SF_GLAD_GL_ES_VERSION_2_0, glBlendEquationSeparate

#define GLEXT_texture_non_power_of_two (SF_GLAD_GL_ES_VERSION_3_0 || SF_GLAD_GL_OES_texture_npot)

#define GLEXT_framebuffer_object               SF_GLAD_GL_ES_VERSION_2_0
#define GLEXT_glBindRenderbuffer               glBindRenderbuffer
#define GLEXT_glDeleteRenderbuffers            glDeleteRenderbuffers
#define GLEXT_glGenRenderbuffers               glGenRenderbuffers
#define GLEXT_glRenderbufferStorage            glRenderbufferStorage
#define GLEXT_glBindFramebuffer                glBindFramebuffer
#define GLEXT_glDeleteFramebuffers             glDeleteFramebuffers
#define GLEXT_glGenFramebuffers                glGenFramebuffers
#define GLEXT_glCheckFramebufferStatus         glCheckFramebufferStatus
#define GLEXT_glFramebufferTexture2D           glFramebufferTexture2D
#define GLEXT_glFramebufferRenderbuffer        glFramebufferRenderbuffer
#define GLEXT_glGenerateMipmap                 glGenerateMipmap
#define GLEXT_GL_FRAMEBUFFER                   GL_FRAMEBUFFER
#define GLEXT_GL_RENDERBUFFER                  GL_RENDERBUFFER
#define GLEXT_GL_DEPTH_COMPONENT               GL_DEPTH_COMPONENT16
#define GLEXT_GL_STENCIL_INDEX8                GL_STENCIL_INDEX8
#define GLEXT_GL_COLOR_ATTACHMENT0             GL_COLOR_ATTACHMENT0
#define GLEXT_GL_DEPTH_ATTACHMENT              GL_DEPTH_ATTACHMENT
#define GLEXT_GL_FRAMEBUFFER_COMPLETE          GL_FRAMEBUFFER_COMPLETE
#define GLEXT_GL_FRAMEBUFFER_BINDING           GL_FRAMEBUFFER_BINDING
#define GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION GL_INVALID_FRAMEBUFFER_OPERATION
#define GLEXT_GL_STENCIL_ATTACHMENT            GL_STENCIL_ATTACHMENT

#define GLEXT_framebuffer_object_dependencies                                                                         \
    SF_GLAD_GL_ES_VERSION_2_0, glBindRenderbuffer, glDeleteRenderbuffers, glGenRenderbuffers, glRenderbufferStorage,  \
        glBindFramebuffer, glDeleteFramebuffers, glGenFramebuffers, glCheckFramebufferStatus, glFramebufferTexture2D, \
        glFramebufferRenderbuffer, glGenerateMipmap

#define GLEXT_packed_depth_stencil (SF_GLAD_GL_ES_VERSION_3_0 || SF_GLAD_GL_OES_packed_depth_stencil)
#define GLEXT_GL_DEPTH24_STENCIL8  GL_DEPTH24_STENCIL8

#define GLEXT_framebuffer_blit            SF_GLAD_GL_ES_VERSION_3_0
#define GLEXT_glBlitFramebuffer           glBlitFramebuffer
#define GLEXT_GL_READ_FRAMEBUFFER         GL_READ_FRAMEBUFFER
#define GLEXT_GL_DRAW_FRAMEBUFFER         GL_DRAW_FRAMEBUFFER
#define GLEXT_GL_DRAW_FRAMEBUFFER_BINDING GL_DRAW_FRAMEBUFFER_BINDING
#define GLEXT_GL_READ_FRAMEBUFFER_BINDING GL_READ_FRAMEBUFFER_BINDING

#define GLEXT_framebuffer_multisample          SF_GLAD_GL_ES_VERSION_3_0
#define GLEXT_glRenderbufferStorageMultisample glRenderbufferStorageMultisample
#define GLEXT_GL_MAX_SAMPLES                   GL_MAX_SAMPLES

#define GLEXT_framebuffer_multisample_blit SF_GLAD_GL_ES_VERSION_3_0

#define GLEXT_copy_buffer          SF_GLAD_GL_ES_VERSION_3_0
#define GLEXT_GL_COPY_READ_BUFFER  GL_COPY_READ_BUFFER
#define GLEXT_GL_COPY_WRITE_BUFFER GL_COPY_WRITE_BUFFER
#define GLEXT_glCopyBufferSubData  glCopyBufferSubData

#define GLEXT_texture_sRGB    (SF_GLAD_GL_ES_VERSION_3_0 || SF_GLAD_GL_EXT_sRGB)
#define GLEXT_GL_SRGB8_ALPHA8 (SF_GLAD_GL_ES_VERSION_3_0 ? GL_SRGB8_ALPHA8 : GL_SRGB_ALPHA_EXT)
#define GLEXT_GL_SRGB_FORMAT  (SF_GLAD_GL_ES_VERSION_3_0 ? GL_RGBA : GL_SRGB_ALPHA_EXT)

#define GLEXT_framebuffer_sRGB false

#define GLEXT_blend_minmax SF_GLAD_GL_ES_VERSION_3_0
#define GLEXT_GL_MIN       GL_MIN
#define GLEXT_GL_MAX       GL_MAX

#define GLEXT_blend_minmax_dependencies SF_GLAD_GL_ES_VERSION_3_0, glBlendEquation

#define GLEXT_blend_equation (GLEXT_blend_minmax || GLEXT_blend_subtract)

#else

// OpenGL 2.0 is SFML's required desktop baseline.
#define GLEXT_GL_DEPTH_COMPONENT    GL_DEPTH_COMPONENT
#define GLEXT_glClientActiveTexture glClientActiveTexture
#define GLEXT_GL_CLAMP              GL_CLAMP

// The following extensions are listed chronologically
// Extension macro first, followed by tokens then
// functions according to the corresponding specification

// The following extensions are optional.

// Core in the required OpenGL 2.0 baseline
#define GLEXT_texture_edge_clamp    true
#define GLEXT_GL_CLAMP_TO_EDGE      GL_CLAMP_TO_EDGE

// Core in the required OpenGL 2.0 baseline
#define GLEXT_blend_minmax          SF_GLAD_GL_VERSION_2_0
#define GLEXT_glBlendEquation       glBlendEquation
#define GLEXT_GL_FUNC_ADD           GL_FUNC_ADD
#define GLEXT_GL_MIN                GL_MIN
#define GLEXT_GL_MAX                GL_MAX

#define GLEXT_blend_minmax_dependencies SF_GLAD_GL_VERSION_2_0, glBlendEquation

// Core since 1.2 - EXT_blend_subtract
#define GLEXT_blend_subtract            SF_GLAD_GL_VERSION_2_0
#define GLEXT_GL_FUNC_SUBTRACT          GL_FUNC_SUBTRACT
#define GLEXT_GL_FUNC_REVERSE_SUBTRACT  GL_FUNC_REVERSE_SUBTRACT

#define GLEXT_blend_equation  SF_GLAD_GL_VERSION_2_0

// Core since 1.3 - ARB_multitexture
#define GLEXT_multitexture    SF_GLAD_GL_VERSION_2_0
#define GLEXT_glActiveTexture glActiveTexture
#define GLEXT_GL_TEXTURE0     GL_TEXTURE0

#define GLEXT_multitexture_dependencies SF_GLAD_GL_VERSION_2_0, glActiveTexture

// Core since 1.4 - EXT_blend_func_separate
#define GLEXT_blend_func_separate       SF_GLAD_GL_VERSION_2_0
#define GLEXT_glBlendFuncSeparate       glBlendFuncSeparate

#define GLEXT_blend_func_separate_dependencies SF_GLAD_GL_VERSION_2_0, glBlendFuncSeparate

// Core since 1.5 - ARB_vertex_buffer_object
#define GLEXT_vertex_buffer_object             SF_GLAD_GL_VERSION_2_0
#define GLEXT_GL_ARRAY_BUFFER                  GL_ARRAY_BUFFER
#define GLEXT_GL_DYNAMIC_DRAW                  GL_DYNAMIC_DRAW
#define GLEXT_GL_READ_ONLY                     GL_READ_ONLY
#define GLEXT_GL_STATIC_DRAW                   GL_STATIC_DRAW
#define GLEXT_GL_STREAM_DRAW                   GL_STREAM_DRAW
#define GLEXT_GL_WRITE_ONLY                    GL_WRITE_ONLY
#define GLEXT_glBindBuffer                     glBindBuffer
#define GLEXT_glBufferData                     glBufferData
#define GLEXT_glBufferSubData                  glBufferSubData
#define GLEXT_glDeleteBuffers                  glDeleteBuffers
#define GLEXT_glGenBuffers                     glGenBuffers
#define GLEXT_glMapBuffer                      glMapBuffer
#define GLEXT_glUnmapBuffer                    glUnmapBuffer

#define GLEXT_vertex_buffer_object_dependencies                                                                      \
    SF_GLAD_GL_VERSION_2_0, glBindBuffer, glBufferData, glBufferSubData, glDeleteBuffers, glGenBuffers, glMapBuffer, \
        glUnmapBuffer

#define GLEXT_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS

// Core since 2.0 - ARB_texture_non_power_of_two
#define GLEXT_texture_non_power_of_two            SF_GLAD_GL_VERSION_2_0

// Core since 2.0 - EXT_blend_equation_separate
#define GLEXT_blend_equation_separate             SF_GLAD_GL_VERSION_2_0
#define GLEXT_glBlendEquationSeparate             glBlendEquationSeparate

#define GLEXT_blend_equation_separate_dependencies SF_GLAD_GL_VERSION_2_0, glBlendEquationSeparate

// Core since 2.1 - EXT_texture_sRGB
#define GLEXT_texture_sRGB                         (SF_GLAD_GL_VERSION_2_1 || SF_GLAD_GL_EXT_texture_sRGB)
#define GLEXT_GL_SRGB_FORMAT                       GL_RGBA
#define GLEXT_GL_SRGB8_ALPHA8                      GL_SRGB8_ALPHA8

// Core since 3.0 - ARB_framebuffer_sRGB
#define GLEXT_framebuffer_sRGB \
    (SF_GLAD_GL_VERSION_3_0 || SF_GLAD_GL_ARB_framebuffer_sRGB || SF_GLAD_GL_EXT_framebuffer_sRGB)

// Core since 3.0 - EXT_framebuffer_object
#define GLEXT_framebuffer_object \
    (SF_GLAD_GL_VERSION_3_0 || SF_GLAD_GL_ARB_framebuffer_object || SF_GLAD_GL_EXT_framebuffer_object)
#define GLEXT_glBindRenderbuffer               glBindRenderbuffer
#define GLEXT_glDeleteRenderbuffers            glDeleteRenderbuffers
#define GLEXT_glGenRenderbuffers               glGenRenderbuffers
#define GLEXT_glRenderbufferStorage            glRenderbufferStorage
#define GLEXT_glBindFramebuffer                glBindFramebuffer
#define GLEXT_glDeleteFramebuffers             glDeleteFramebuffers
#define GLEXT_glGenFramebuffers                glGenFramebuffers
#define GLEXT_glCheckFramebufferStatus         glCheckFramebufferStatus
#define GLEXT_glFramebufferTexture2D           glFramebufferTexture2D
#define GLEXT_glFramebufferRenderbuffer        glFramebufferRenderbuffer
#define GLEXT_glGenerateMipmap                 glGenerateMipmap
#define GLEXT_GL_FRAMEBUFFER                   GL_FRAMEBUFFER
#define GLEXT_GL_RENDERBUFFER                  GL_RENDERBUFFER
#define GLEXT_GL_STENCIL_INDEX8                GL_STENCIL_INDEX8
#define GLEXT_GL_COLOR_ATTACHMENT0             GL_COLOR_ATTACHMENT0
#define GLEXT_GL_DEPTH_ATTACHMENT              GL_DEPTH_ATTACHMENT
#define GLEXT_GL_FRAMEBUFFER_COMPLETE          GL_FRAMEBUFFER_COMPLETE
#define GLEXT_GL_FRAMEBUFFER_BINDING           GL_FRAMEBUFFER_BINDING
#define GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION GL_INVALID_FRAMEBUFFER_OPERATION
#define GLEXT_GL_STENCIL_ATTACHMENT            GL_STENCIL_ATTACHMENT

#define GLEXT_framebuffer_object_dependencies                                                                         \
    GLEXT_framebuffer_object, glBindRenderbuffer, glDeleteRenderbuffers, glGenRenderbuffers, glRenderbufferStorage,   \
        glBindFramebuffer, glDeleteFramebuffers, glGenFramebuffers, glCheckFramebufferStatus, glFramebufferTexture2D, \
        glFramebufferRenderbuffer, glGenerateMipmap

// Core since 3.0 - EXT_packed_depth_stencil
#define GLEXT_packed_depth_stencil \
    (SF_GLAD_GL_VERSION_3_0 || SF_GLAD_GL_ARB_framebuffer_object || SF_GLAD_GL_EXT_packed_depth_stencil)
#define GLEXT_GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8

// Core since 3.0 - EXT_framebuffer_blit
#define GLEXT_framebuffer_blit \
    (SF_GLAD_GL_VERSION_3_0 || SF_GLAD_GL_ARB_framebuffer_object || SF_GLAD_GL_EXT_framebuffer_blit)
#define GLEXT_glBlitFramebuffer           glBlitFramebuffer
#define GLEXT_GL_READ_FRAMEBUFFER         GL_READ_FRAMEBUFFER
#define GLEXT_GL_DRAW_FRAMEBUFFER         GL_DRAW_FRAMEBUFFER
#define GLEXT_GL_DRAW_FRAMEBUFFER_BINDING GL_DRAW_FRAMEBUFFER_BINDING
#define GLEXT_GL_READ_FRAMEBUFFER_BINDING GL_READ_FRAMEBUFFER_BINDING

#define GLEXT_framebuffer_blit_dependencies GLEXT_framebuffer_blit, glBlitFramebuffer

// Core since 3.0 - EXT_framebuffer_multisample
#define GLEXT_framebuffer_multisample \
    (SF_GLAD_GL_VERSION_3_0 || SF_GLAD_GL_ARB_framebuffer_object || SF_GLAD_GL_EXT_framebuffer_multisample)
#define GLEXT_glRenderbufferStorageMultisample glRenderbufferStorageMultisample
#define GLEXT_GL_MAX_SAMPLES                   GL_MAX_SAMPLES

#define GLEXT_framebuffer_multisample_dependencies GLEXT_framebuffer_multisample, glRenderbufferStorageMultisample

#define GLEXT_framebuffer_multisample_blit (GLEXT_framebuffer_multisample && GLEXT_framebuffer_blit)

// Core since 3.1 - ARB_copy_buffer
#define GLEXT_copy_buffer                  (SF_GLAD_GL_VERSION_3_1 || SF_GLAD_GL_ARB_copy_buffer)
#define GLEXT_GL_COPY_READ_BUFFER          GL_COPY_READ_BUFFER
#define GLEXT_GL_COPY_WRITE_BUFFER         GL_COPY_WRITE_BUFFER
#define GLEXT_glCopyBufferSubData          glCopyBufferSubData

#define GLEXT_copy_buffer_dependencies GLEXT_copy_buffer, glCopyBufferSubData

// Core since 3.2 - ARB_geometry_shader4
#define GLEXT_geometry_shader4         SF_GLAD_GL_ARB_geometry_shader4
#define GLEXT_GL_GEOMETRY_SHADER       GL_GEOMETRY_SHADER_ARB

#endif

// OpenGL Versions
#define GLEXT_GL_VERSION_1_0 SF_GLAD_GL_VERSION_1_0
#define GLEXT_GL_VERSION_1_1 SF_GLAD_GL_VERSION_1_1
#define GLEXT_GL_VERSION_1_2 SF_GLAD_GL_VERSION_1_2
#define GLEXT_GL_VERSION_1_3 SF_GLAD_GL_VERSION_1_3
#define GLEXT_GL_VERSION_1_4 SF_GLAD_GL_VERSION_1_4
#define GLEXT_GL_VERSION_1_5 SF_GLAD_GL_VERSION_1_5
#define GLEXT_GL_VERSION_2_0 SF_GLAD_GL_VERSION_2_0
#define GLEXT_GL_VERSION_2_1 SF_GLAD_GL_VERSION_2_1
#define GLEXT_GL_VERSION_3_0 SF_GLAD_GL_VERSION_3_0
#define GLEXT_GL_VERSION_3_1 SF_GLAD_GL_VERSION_3_1
#define GLEXT_GL_VERSION_3_2 SF_GLAD_GL_VERSION_3_2
#define GLEXT_GL_VERSION_3_3 SF_GLAD_GL_VERSION_3_3
#define GLEXT_GL_VERSION_4_0 SF_GLAD_GL_VERSION_4_0
#define GLEXT_GL_VERSION_4_1 SF_GLAD_GL_VERSION_4_1
#define GLEXT_GL_VERSION_4_2 SF_GLAD_GL_VERSION_4_2
#define GLEXT_GL_VERSION_4_3 SF_GLAD_GL_VERSION_4_3
#define GLEXT_GL_VERSION_4_4 SF_GLAD_GL_VERSION_4_4
#define GLEXT_GL_VERSION_4_5 SF_GLAD_GL_VERSION_4_5
#define GLEXT_GL_VERSION_4_6 SF_GLAD_GL_VERSION_4_6

namespace sf::priv
{

////////////////////////////////////////////////////////////
/// \brief Make sure that extensions are initialized
///
////////////////////////////////////////////////////////////
void ensureExtensionsInit();

} // namespace sf::priv
