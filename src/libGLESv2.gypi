# Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables':
    {
        # These file lists are shared with the GN build.
        'libangle_common_sources':
        [
            'common/Float16ToFloat32.cpp',
            'common/IterableBitSet.h',
            'common/MemoryBuffer.cpp',
            'common/MemoryBuffer.h',
            'common/Optional.h',
            'common/angleutils.cpp',
            'common/angleutils.h',
            'common/debug.cpp',
            'common/debug.h',
            'common/mathutil.cpp',
            'common/mathutil.h',
            'common/matrix_utils.h',
            'common/platform.h',
            'common/string_utils.cpp',
            'common/string_utils.h',
            'common/tls.cpp',
            'common/tls.h',
            'common/utilities.cpp',
            'common/utilities.h',
            'common/version.h',
        ],
        'libangle_includes':
        [
            '../include/angle_gl.h',
            '../include/export.h',
            '../include/EGL/egl.h',
            '../include/EGL/eglext.h',
            '../include/EGL/eglplatform.h',
            '../include/GLES2/gl2.h',
            '../include/GLES2/gl2ext.h',
            '../include/GLES2/gl2platform.h',
            '../include/GLES3/gl3.h',
            '../include/GLES3/gl3ext.h',
            '../include/GLES3/gl3platform.h',
            '../include/GLSLANG/ShaderLang.h',
            '../include/GLSLANG/ShaderVars.h',
            '../include/KHR/khrplatform.h',
            '../include/platform/Platform.h',
        ],
        'libangle_sources':
        [
            'common/event_tracer.cpp',
            'common/event_tracer.h',
            'libANGLE/AttributeMap.cpp',
            'libANGLE/AttributeMap.h',
            'libANGLE/BinaryStream.h',
            'libANGLE/Buffer.cpp',
            'libANGLE/Buffer.h',
            'libANGLE/Caps.cpp',
            'libANGLE/Caps.h',
            'libANGLE/Compiler.cpp',
            'libANGLE/Compiler.h',
            'libANGLE/Config.cpp',
            'libANGLE/Config.h',
            'libANGLE/Constants.h',
            'libANGLE/Context.cpp',
            'libANGLE/Context.h',
            'libANGLE/Data.cpp',
            'libANGLE/Data.h',
            'libANGLE/Device.cpp',
            'libANGLE/Device.h',
            'libANGLE/Display.cpp',
            'libANGLE/Display.h',
            'libANGLE/Error.cpp',
            'libANGLE/Error.h',
            'libANGLE/Error.inl',
            'libANGLE/Fence.cpp',
            'libANGLE/Fence.h',
            'libANGLE/Framebuffer.cpp',
            'libANGLE/Framebuffer.h',
            'libANGLE/FramebufferAttachment.cpp',
            'libANGLE/FramebufferAttachment.h',
            'libANGLE/HandleAllocator.cpp',
            'libANGLE/HandleAllocator.h',
            'libANGLE/ImageIndex.h',
            'libANGLE/ImageIndex.cpp',
            'libANGLE/IndexRangeCache.cpp',
            'libANGLE/IndexRangeCache.h',
            'libANGLE/Platform.cpp',
            'libANGLE/Program.cpp',
            'libANGLE/Program.h',
            'libANGLE/Query.cpp',
            'libANGLE/Query.h',
            'libANGLE/RefCountObject.cpp',
            'libANGLE/RefCountObject.h',
            'libANGLE/Renderbuffer.cpp',
            'libANGLE/Renderbuffer.h',
            'libANGLE/ResourceManager.cpp',
            'libANGLE/ResourceManager.h',
            'libANGLE/Sampler.cpp',
            'libANGLE/Sampler.h',
            'libANGLE/Shader.cpp',
            'libANGLE/Shader.h',
            'libANGLE/State.cpp',
            'libANGLE/State.h',
            'libANGLE/Surface.cpp',
            'libANGLE/Surface.h',
            'libANGLE/Texture.cpp',
            'libANGLE/Texture.h',
            'libANGLE/TransformFeedback.cpp',
            'libANGLE/TransformFeedback.h',
            'libANGLE/Uniform.cpp',
            'libANGLE/Uniform.h',
            'libANGLE/Version.h',
            'libANGLE/Version.inl',
            'libANGLE/VertexArray.cpp',
            'libANGLE/VertexArray.h',
            'libANGLE/VertexAttribute.cpp',
            'libANGLE/VertexAttribute.h',
            'libANGLE/VertexAttribute.inl',
            'libANGLE/angletypes.cpp',
            'libANGLE/angletypes.h',
            'libANGLE/angletypes.inl',
            'libANGLE/features.h',
            'libANGLE/formatutils.cpp',
            'libANGLE/formatutils.h',
            'libANGLE/histogram_macros.h',
            'libANGLE/queryconversions.cpp',
            'libANGLE/queryconversions.h',
            'libANGLE/renderer/BufferImpl.h',
            'libANGLE/renderer/CompilerImpl.h',
            'libANGLE/renderer/DeviceImpl.cpp',
            'libANGLE/renderer/DeviceImpl.h',
            'libANGLE/renderer/DisplayImpl.cpp',
            'libANGLE/renderer/DisplayImpl.h',
            'libANGLE/renderer/FenceNVImpl.h',
            'libANGLE/renderer/FenceSyncImpl.h',
            'libANGLE/renderer/FramebufferImpl.h',
            'libANGLE/renderer/ImplFactory.h',
            'libANGLE/renderer/ProgramImpl.cpp',
            'libANGLE/renderer/ProgramImpl.h',
            'libANGLE/renderer/QueryImpl.h',
            'libANGLE/renderer/RenderbufferImpl.h',
            'libANGLE/renderer/Renderer.cpp',
            'libANGLE/renderer/Renderer.h',
            'libANGLE/renderer/ShaderImpl.h',
            'libANGLE/renderer/SurfaceImpl.cpp',
            'libANGLE/renderer/SurfaceImpl.h',
            'libANGLE/renderer/TextureImpl.h',
            'libANGLE/renderer/TransformFeedbackImpl.h',
            'libANGLE/renderer/VertexArrayImpl.h',
            'libANGLE/validationEGL.cpp',
            'libANGLE/validationEGL.h',
            'libANGLE/validationES.cpp',
            'libANGLE/validationES.h',
            'libANGLE/validationES2.cpp',
            'libANGLE/validationES2.h',
            'libANGLE/validationES3.cpp',
            'libANGLE/validationES3.h',
            'third_party/murmurhash/MurmurHash3.cpp',
            'third_party/murmurhash/MurmurHash3.h',
        ],
        'libangle_d3d_shared_sources':
        [
            'libANGLE/renderer/d3d/BufferD3D.cpp',
            'libANGLE/renderer/d3d/BufferD3D.h',
            'libANGLE/renderer/d3d/CompilerD3D.cpp',
            'libANGLE/renderer/d3d/CompilerD3D.h',
            'libANGLE/renderer/d3d/copyimage.cpp',
            'libANGLE/renderer/d3d/copyimage.h',
            'libANGLE/renderer/d3d/copyimage.inl',
            'libANGLE/renderer/d3d/DeviceD3D.cpp',
            'libANGLE/renderer/d3d/DeviceD3D.h',
            'libANGLE/renderer/d3d/DisplayD3D.cpp',
            'libANGLE/renderer/d3d/DisplayD3D.h',
            'libANGLE/renderer/d3d/DynamicHLSL.cpp',
            'libANGLE/renderer/d3d/DynamicHLSL.h',
            'libANGLE/renderer/d3d/formatutilsD3D.cpp',
            'libANGLE/renderer/d3d/formatutilsD3D.h',
            'libANGLE/renderer/d3d/FramebufferD3D.cpp',
            'libANGLE/renderer/d3d/FramebufferD3D.h',
            'libANGLE/renderer/d3d/generatemip.h',
            'libANGLE/renderer/d3d/generatemip.inl',
            'libANGLE/renderer/d3d/HLSLCompiler.cpp',
            'libANGLE/renderer/d3d/HLSLCompiler.h',
            'libANGLE/renderer/d3d/ImageD3D.cpp',
            'libANGLE/renderer/d3d/ImageD3D.h',
            'libANGLE/renderer/d3d/imageformats.h',
            'libANGLE/renderer/d3d/IndexBuffer.cpp',
            'libANGLE/renderer/d3d/IndexBuffer.h',
            'libANGLE/renderer/d3d/IndexDataManager.cpp',
            'libANGLE/renderer/d3d/IndexDataManager.h',
            'libANGLE/renderer/d3d/loadimage.cpp',
            'libANGLE/renderer/d3d/loadimage.h',
            'libANGLE/renderer/d3d/loadimage.inl',
            'libANGLE/renderer/d3d/loadimageSSE2.cpp',
            'libANGLE/renderer/d3d/ProgramD3D.cpp',
            'libANGLE/renderer/d3d/ProgramD3D.h',
            'libANGLE/renderer/d3d/RenderbufferD3D.cpp',
            'libANGLE/renderer/d3d/RenderbufferD3D.h',
            'libANGLE/renderer/d3d/RendererD3D.cpp',
            'libANGLE/renderer/d3d/RendererD3D.h',
            'libANGLE/renderer/d3d/RenderTargetD3D.h',
            'libANGLE/renderer/d3d/RenderTargetD3D.cpp',
            'libANGLE/renderer/d3d/ShaderD3D.cpp',
            'libANGLE/renderer/d3d/ShaderD3D.h',
            'libANGLE/renderer/d3d/ShaderExecutableD3D.cpp',
            'libANGLE/renderer/d3d/ShaderExecutableD3D.h',
            'libANGLE/renderer/d3d/SurfaceD3D.cpp',
            'libANGLE/renderer/d3d/SurfaceD3D.h',
            'libANGLE/renderer/d3d/SwapChainD3D.h',
            'libANGLE/renderer/d3d/TextureD3D.cpp',
            'libANGLE/renderer/d3d/TextureD3D.h',
            'libANGLE/renderer/d3d/TextureStorage.h',
            'libANGLE/renderer/d3d/TransformFeedbackD3D.cpp',
            'libANGLE/renderer/d3d/TransformFeedbackD3D.h',
            'libANGLE/renderer/d3d/VertexBuffer.cpp',
            'libANGLE/renderer/d3d/VertexBuffer.h',
            'libANGLE/renderer/d3d/VertexDataManager.cpp',
            'libANGLE/renderer/d3d/VertexDataManager.h',
            'libANGLE/renderer/d3d/WorkaroundsD3D.h',
        ],
        'libangle_d3d9_sources':
        [
            'libANGLE/renderer/d3d/d3d9/Blit9.cpp',
            'libANGLE/renderer/d3d/d3d9/Blit9.h',
            'libANGLE/renderer/d3d/d3d9/Buffer9.cpp',
            'libANGLE/renderer/d3d/d3d9/Buffer9.h',
            'libANGLE/renderer/d3d/d3d9/DebugAnnotator9.cpp',
            'libANGLE/renderer/d3d/d3d9/DebugAnnotator9.h',
            'libANGLE/renderer/d3d/d3d9/Fence9.cpp',
            'libANGLE/renderer/d3d/d3d9/Fence9.h',
            'libANGLE/renderer/d3d/d3d9/formatutils9.cpp',
            'libANGLE/renderer/d3d/d3d9/formatutils9.h',
            'libANGLE/renderer/d3d/d3d9/Framebuffer9.cpp',
            'libANGLE/renderer/d3d/d3d9/Framebuffer9.h',
            'libANGLE/renderer/d3d/d3d9/Image9.cpp',
            'libANGLE/renderer/d3d/d3d9/Image9.h',
            'libANGLE/renderer/d3d/d3d9/IndexBuffer9.cpp',
            'libANGLE/renderer/d3d/d3d9/IndexBuffer9.h',
            'libANGLE/renderer/d3d/d3d9/Query9.cpp',
            'libANGLE/renderer/d3d/d3d9/Query9.h',
            'libANGLE/renderer/d3d/d3d9/Renderer9.cpp',
            'libANGLE/renderer/d3d/d3d9/Renderer9.h',
            'libANGLE/renderer/d3d/d3d9/renderer9_utils.cpp',
            'libANGLE/renderer/d3d/d3d9/renderer9_utils.h',
            'libANGLE/renderer/d3d/d3d9/RenderTarget9.cpp',
            'libANGLE/renderer/d3d/d3d9/RenderTarget9.h',
            'libANGLE/renderer/d3d/d3d9/ShaderCache.h',
            'libANGLE/renderer/d3d/d3d9/ShaderExecutable9.cpp',
            'libANGLE/renderer/d3d/d3d9/ShaderExecutable9.h',
            'libANGLE/renderer/d3d/d3d9/shaders/compiled/componentmaskps.h',
            'libANGLE/renderer/d3d/d3d9/shaders/compiled/flipyvs.h',
            'libANGLE/renderer/d3d/d3d9/shaders/compiled/luminanceps.h',
            'libANGLE/renderer/d3d/d3d9/shaders/compiled/passthroughps.h',
            'libANGLE/renderer/d3d/d3d9/shaders/compiled/standardvs.h',
            'libANGLE/renderer/d3d/d3d9/SwapChain9.cpp',
            'libANGLE/renderer/d3d/d3d9/SwapChain9.h',
            'libANGLE/renderer/d3d/d3d9/TextureStorage9.cpp',
            'libANGLE/renderer/d3d/d3d9/TextureStorage9.h',
            'libANGLE/renderer/d3d/d3d9/VertexArray9.h',
            'libANGLE/renderer/d3d/d3d9/VertexBuffer9.cpp',
            'libANGLE/renderer/d3d/d3d9/VertexBuffer9.h',
            'libANGLE/renderer/d3d/d3d9/vertexconversion.h',
            'libANGLE/renderer/d3d/d3d9/VertexDeclarationCache.cpp',
            'libANGLE/renderer/d3d/d3d9/VertexDeclarationCache.h',
        ],
        'libangle_d3d11_sources':
        [
            'libANGLE/renderer/d3d/d3d11/Blit11.cpp',
            'libANGLE/renderer/d3d/d3d11/Blit11.h',
            'libANGLE/renderer/d3d/d3d11/Buffer11.cpp',
            'libANGLE/renderer/d3d/d3d11/Buffer11.h',
            'libANGLE/renderer/d3d/d3d11/Clear11.cpp',
            'libANGLE/renderer/d3d/d3d11/Clear11.h',
            'libANGLE/renderer/d3d/d3d11/copyvertex.h',
            'libANGLE/renderer/d3d/d3d11/copyvertex.inl',
            'libANGLE/renderer/d3d/d3d11/DebugAnnotator11.cpp',
            'libANGLE/renderer/d3d/d3d11/DebugAnnotator11.h',
            'libANGLE/renderer/d3d/d3d11/dxgi_support_table.cpp',
            'libANGLE/renderer/d3d/d3d11/dxgi_support_table.h',
            'libANGLE/renderer/d3d/d3d11/Fence11.cpp',
            'libANGLE/renderer/d3d/d3d11/Fence11.h',
            'libANGLE/renderer/d3d/d3d11/formatutils11.cpp',
            'libANGLE/renderer/d3d/d3d11/formatutils11.h',
            'libANGLE/renderer/d3d/d3d11/Framebuffer11.cpp',
            'libANGLE/renderer/d3d/d3d11/Framebuffer11.h',
            'libANGLE/renderer/d3d/d3d11/Image11.cpp',
            'libANGLE/renderer/d3d/d3d11/Image11.h',
            'libANGLE/renderer/d3d/d3d11/IndexBuffer11.cpp',
            'libANGLE/renderer/d3d/d3d11/IndexBuffer11.h',
            'libANGLE/renderer/d3d/d3d11/InputLayoutCache.cpp',
            'libANGLE/renderer/d3d/d3d11/InputLayoutCache.h',
            'libANGLE/renderer/d3d/d3d11/NativeWindow.h',
            'libANGLE/renderer/d3d/d3d11/PixelTransfer11.cpp',
            'libANGLE/renderer/d3d/d3d11/PixelTransfer11.h',
            'libANGLE/renderer/d3d/d3d11/Query11.cpp',
            'libANGLE/renderer/d3d/d3d11/Query11.h',
            'libANGLE/renderer/d3d/d3d11/Renderer11.cpp',
            'libANGLE/renderer/d3d/d3d11/Renderer11.h',
            'libANGLE/renderer/d3d/d3d11/renderer11_utils.cpp',
            'libANGLE/renderer/d3d/d3d11/renderer11_utils.h',
            'libANGLE/renderer/d3d/d3d11/RenderStateCache.cpp',
            'libANGLE/renderer/d3d/d3d11/RenderStateCache.h',
            'libANGLE/renderer/d3d/d3d11/RenderTarget11.cpp',
            'libANGLE/renderer/d3d/d3d11/RenderTarget11.h',
            'libANGLE/renderer/d3d/d3d11/ShaderExecutable11.cpp',
            'libANGLE/renderer/d3d/d3d11/ShaderExecutable11.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/buffertotexture11_gs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/buffertotexture11_ps_4f.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/buffertotexture11_ps_4i.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/buffertotexture11_ps_4ui.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/buffertotexture11_vs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/clearfloat11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/clearfloat11vs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/clearsint11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/clearsint11vs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/clearuint11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/clearuint11vs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthrough2d11vs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthrough3d11gs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthrough3d11vs.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughdepth2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughlum2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughlum3d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughlumalpha2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughlumalpha3d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughr2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughr2di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughr2dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughr3d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughr3di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughr3dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrg2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrg2di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrg2dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrg3d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrg3di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrg3dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgb2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgb2di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgb2dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgb3d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgb3di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgb3dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgba2d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgba2di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgba2dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgba3d11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgba3di11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/passthroughrgba3dui11ps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzlef2darrayps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzlef2dps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzlef3dps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzlei2darrayps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzlei2dps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzlei3dps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzleui2darrayps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzleui2dps.h',
            'libANGLE/renderer/d3d/d3d11/shaders/compiled/swizzleui3dps.h',
            'libANGLE/renderer/d3d/d3d11/SwapChain11.cpp',
            'libANGLE/renderer/d3d/d3d11/SwapChain11.h',
            'libANGLE/renderer/d3d/d3d11/TextureStorage11.cpp',
            'libANGLE/renderer/d3d/d3d11/TextureStorage11.h',
            'libANGLE/renderer/d3d/d3d11/Trim11.cpp',
            'libANGLE/renderer/d3d/d3d11/Trim11.h',
            'libANGLE/renderer/d3d/d3d11/VertexArray11.h',
            'libANGLE/renderer/d3d/d3d11/VertexBuffer11.cpp',
            'libANGLE/renderer/d3d/d3d11/VertexBuffer11.h',
        ],
        'libangle_d3d11_win32_sources':
        [
            'libANGLE/renderer/d3d/d3d11/win32/NativeWindow.cpp',
            'third_party/systeminfo/SystemInfo.cpp',
            'third_party/systeminfo/SystemInfo.h',
        ],
        'libangle_d3d11_winrt_sources':
        [
            'libANGLE/renderer/d3d/d3d11/winrt/SwapChainPanelNativeWindow.cpp',
            'libANGLE/renderer/d3d/d3d11/winrt/SwapChainPanelNativeWindow.h',
            'libANGLE/renderer/d3d/d3d11/winrt/CoreWindowNativeWindow.cpp',
            'libANGLE/renderer/d3d/d3d11/winrt/CoreWindowNativeWindow.h',
            'libANGLE/renderer/d3d/d3d11/winrt/InspectableNativeWindow.cpp',
            'libANGLE/renderer/d3d/d3d11/winrt/InspectableNativeWindow.h',
        ],
        'libangle_gl_sources':
        [
            'libANGLE/renderer/gl/BufferGL.cpp',
            'libANGLE/renderer/gl/BufferGL.h',
            'libANGLE/renderer/gl/CompilerGL.cpp',
            'libANGLE/renderer/gl/CompilerGL.h',
            'libANGLE/renderer/gl/DisplayGL.cpp',
            'libANGLE/renderer/gl/DisplayGL.h',
            'libANGLE/renderer/gl/FenceNVGL.cpp',
            'libANGLE/renderer/gl/FenceNVGL.h',
            'libANGLE/renderer/gl/FenceSyncGL.cpp',
            'libANGLE/renderer/gl/FenceSyncGL.h',
            'libANGLE/renderer/gl/FramebufferGL.cpp',
            'libANGLE/renderer/gl/FramebufferGL.h',
            'libANGLE/renderer/gl/FunctionsGL.cpp',
            'libANGLE/renderer/gl/FunctionsGL.h',
            'libANGLE/renderer/gl/ProgramGL.cpp',
            'libANGLE/renderer/gl/ProgramGL.h',
            'libANGLE/renderer/gl/QueryGL.cpp',
            'libANGLE/renderer/gl/QueryGL.h',
            'libANGLE/renderer/gl/RenderbufferGL.cpp',
            'libANGLE/renderer/gl/RenderbufferGL.h',
            'libANGLE/renderer/gl/RendererGL.cpp',
            'libANGLE/renderer/gl/RendererGL.h',
            'libANGLE/renderer/gl/ShaderGL.cpp',
            'libANGLE/renderer/gl/ShaderGL.h',
            'libANGLE/renderer/gl/StateManagerGL.cpp',
            'libANGLE/renderer/gl/StateManagerGL.h',
            'libANGLE/renderer/gl/SurfaceGL.cpp',
            'libANGLE/renderer/gl/SurfaceGL.h',
            'libANGLE/renderer/gl/TextureGL.cpp',
            'libANGLE/renderer/gl/TextureGL.h',
            'libANGLE/renderer/gl/TransformFeedbackGL.cpp',
            'libANGLE/renderer/gl/TransformFeedbackGL.h',
            'libANGLE/renderer/gl/VertexArrayGL.cpp',
            'libANGLE/renderer/gl/VertexArrayGL.h',
            'libANGLE/renderer/gl/WorkaroundsGL.h',
            'libANGLE/renderer/gl/formatutilsgl.cpp',
            'libANGLE/renderer/gl/formatutilsgl.h',
            'libANGLE/renderer/gl/functionsgl_enums.h',
            'libANGLE/renderer/gl/functionsgl_typedefs.h',
            'libANGLE/renderer/gl/renderergl_utils.cpp',
            'libANGLE/renderer/gl/renderergl_utils.h',
        ],
        'libangle_gl_wgl_sources':
        [
            'libANGLE/renderer/gl/wgl/DisplayWGL.cpp',
            'libANGLE/renderer/gl/wgl/DisplayWGL.h',
            'libANGLE/renderer/gl/wgl/FunctionsWGL.cpp',
            'libANGLE/renderer/gl/wgl/FunctionsWGL.h',
            'libANGLE/renderer/gl/wgl/PbufferSurfaceWGL.cpp',
            'libANGLE/renderer/gl/wgl/PbufferSurfaceWGL.h',
            'libANGLE/renderer/gl/wgl/WindowSurfaceWGL.cpp',
            'libANGLE/renderer/gl/wgl/WindowSurfaceWGL.h',
            'libANGLE/renderer/gl/wgl/functionswgl_typedefs.h',
            'libANGLE/renderer/gl/wgl/wgl_utils.cpp',
            'libANGLE/renderer/gl/wgl/wgl_utils.h',
            'third_party/khronos/GL/wglext.h',
        ],
        'libangle_gl_glx_sources':
        [
            'libANGLE/renderer/gl/glx/DisplayGLX.cpp',
            'libANGLE/renderer/gl/glx/DisplayGLX.h',
            'libANGLE/renderer/gl/glx/FunctionsGLX.cpp',
            'libANGLE/renderer/gl/glx/FunctionsGLX.h',
            'libANGLE/renderer/gl/glx/PbufferSurfaceGLX.cpp',
            'libANGLE/renderer/gl/glx/PbufferSurfaceGLX.h',
            'libANGLE/renderer/gl/glx/WindowSurfaceGLX.cpp',
            'libANGLE/renderer/gl/glx/WindowSurfaceGLX.h',
            'libANGLE/renderer/gl/glx/functionsglx_typedefs.h',
            'libANGLE/renderer/gl/glx/platform_glx.h',
        ],
        'libangle_gl_nsgl_sources':
        [
            'libANGLE/renderer/gl/nsgl/DisplayNSGL.mm',
            'libANGLE/renderer/gl/nsgl/DisplayNSGL.h',
            'libANGLE/renderer/gl/nsgl/WindowSurfaceNSGL.mm',
            'libANGLE/renderer/gl/nsgl/WindowSurfaceNSGL.h',
        ],
        'libglesv2_sources':
        [
            'common/angleutils.h',
            'common/debug.h',
            'libGLESv2/entry_points_egl.cpp',
            'libGLESv2/entry_points_egl.h',
            'libGLESv2/entry_points_egl_ext.cpp',
            'libGLESv2/entry_points_egl_ext.h',
            'libGLESv2/entry_points_gles_2_0.cpp',
            'libGLESv2/entry_points_gles_2_0.h',
            'libGLESv2/entry_points_gles_2_0_ext.cpp',
            'libGLESv2/entry_points_gles_2_0_ext.h',
            'libGLESv2/entry_points_gles_3_0.cpp',
            'libGLESv2/entry_points_gles_3_0.h',
            'libGLESv2/entry_points_gles_3_0_ext.cpp',
            'libGLESv2/entry_points_gles_3_0_ext.h',
            'libGLESv2/global_state.cpp',
            'libGLESv2/global_state.h',
            'libGLESv2/libGLESv2.cpp',
            'libGLESv2/libGLESv2.def',
            'libGLESv2/libGLESv2.rc',
            'libGLESv2/resource.h',
        ],
        'libegl_sources':
        [
            'libEGL/libEGL.cpp',
            'libEGL/libEGL.def',
            'libEGL/libEGL.rc',
            'libEGL/resource.h',
        ],
    },
    # Everything below this is duplicated in the GN build. If you change
    # anything also change angle/BUILD.gn
    'targets':
    [
        {
            'target_name': 'libANGLE',
            'type': 'static_library',
            'dependencies':
            [
                'translator_static',
                'commit_id',
                'angle_common',
            ],
            'includes': [ '../build/common_defines.gypi', ],
            'include_dirs':
            [
                '.',
                '../include',
                'third_party/khronos',
            ],
            'sources':
            [
                '<@(libangle_sources)',
                '<@(libangle_includes)',
            ],
            'defines':
            [
                'LIBANGLE_IMPLEMENTATION',
            ],
            'direct_dependent_settings':
            {
                'include_dirs':
                [
                    '<(angle_path)/src',
                    '<(angle_path)/include',
                ],
                'defines':
                [
                    'GL_GLEXT_PROTOTYPES=',
                    'ANGLE_PRELOADED_D3DCOMPILER_MODULE_NAMES={ "d3dcompiler_47.dll", "d3dcompiler_46.dll", "d3dcompiler_43.dll" }',
                ],
                'conditions':
                [
                    ['OS=="win"', {
                        'defines':
                        [
                            'GL_APICALL=',
                            'EGLAPI=',
                        ],
                    }, {
                        'defines':
                        [
                            'GL_APICALL=__attribute__((visibility("default")))',
                            'EGLAPI=__attribute__((visibility("default")))',
                        ],
                    }],
                    ['OS == "mac"',
                    {
                        'xcode_settings':
                        {
                            'DYLIB_INSTALL_NAME_BASE': '@rpath',
                        },
                    }],
                    ['angle_enable_d3d9==1',
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_D3D9',
                        ],
                    }],
                    ['angle_enable_d3d11==1',
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_D3D11',
                        ],
                    }],
                    ['angle_enable_gl==1',
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_OPENGL',
                        ],
                        'conditions':
                        [
                            ['angle_use_glx==1',
                            {
                                'defines':
                                [
                                    'ANGLE_USE_X11',
                                ]
                            }],
                        ],
                    }],
                ],
            },
            'conditions':
            [
                ['angle_enable_d3d9==1 or angle_enable_d3d11==1',
                {
                    'sources':
                    [
                        '<@(libangle_d3d_shared_sources)',
                    ],
                }],
                ['angle_enable_d3d9==1',
                {
                    'sources':
                    [
                        '<@(libangle_d3d9_sources)',
                    ],
                    'defines':
                    [
                        'ANGLE_ENABLE_D3D9',
                    ],
                    'link_settings':
                    {
                        'msvs_settings':
                        {
                            'VCLinkerTool':
                            {
                                'AdditionalDependencies':
                                [
                                    'd3d9.lib',
                                ]
                            }
                        },
                    },
                }],
                ['angle_enable_d3d11==1',
                {
                    'sources':
                    [
                        '<@(libangle_d3d11_sources)',
                    ],
                    'defines':
                    [
                        'ANGLE_ENABLE_D3D11',
                    ],
                    'link_settings':
                    {
                        'msvs_settings':
                        {
                            'VCLinkerTool':
                            {
                                'conditions':
                                [
                                    ['angle_build_winrt==0',
                                    {
                                        'AdditionalDependencies':
                                        [
                                            'dxguid.lib',
                                        ],
                                    }],
                                    ['angle_build_winrt==1',
                                    {
                                        'AdditionalDependencies':
                                        [
                                            'dxguid.lib',
                                            'd3d11.lib',
                                            'd3dcompiler.lib',
                                        ],
                                    }],
                                ],
                            }
                        },
                    },
                    'conditions':
                    [
                        ['angle_build_winrt==1',
                        {
                            'sources':
                            [
                                '<@(libangle_d3d11_winrt_sources)',
                            ],
                        },
                        { # win32
                            'sources':
                            [
                                '<@(libangle_d3d11_win32_sources)',
                            ],
                        }],
                    ],
                }],
                ['angle_enable_gl==1',
                {
                    'sources':
                    [
                        '<@(libangle_gl_sources)',
                    ],
                    'defines':
                    [
                        'ANGLE_ENABLE_OPENGL',
                    ],
                    'conditions':
                    [
                        ['OS=="win"',
                        {
                            'sources':
                            [
                                '<@(libangle_gl_wgl_sources)',
                            ],
                        }],
                        ['angle_use_glx==1',
                        {
                            'defines':
                            [
                                'ANGLE_USE_X11',
                            ],
                            'sources':
                            [
                                '<@(libangle_gl_glx_sources)',
                            ],
                        }],
                        ['OS=="mac"',
                        {
                            'sources':
                            [
                                '<@(libangle_gl_nsgl_sources)',
                            ],
                        }],
                    ],
                }],
                ['angle_build_winrt==0 and OS=="win"',
                {
                    'dependencies':
                    [
                        'copy_compiler_dll'
                    ],
                }],
                ['angle_build_winrt==1',
                {
                    'defines':
                    [
                        'NTDDI_VERSION=NTDDI_WINBLUE',
                    ],
                    'msvs_enable_winrt' : '1',
                    'msvs_application_type_revision' : '<(angle_build_winrt_app_type_revision)',
                    'msvs_requires_importlibrary' : 'true',
                    'msvs_settings':
                    {
                        'VCLinkerTool':
                        {
                            'EnableCOMDATFolding': '1',
                            'OptimizeReferences': '1',
                        }
                    },
                }],
                ['angle_build_winphone==1',
                {
                    'msvs_enable_winphone' : '1',
                }],
                ['angle_link_glx==1',
                {
                    'link_settings':
                    {
                        'libraries':
                        [
                            '-lGL',
                        ],
                    },
                    'defines':
                    [
                        'ANGLE_LINK_GLX',
                    ],
                }],
            ],
        },
        {
            'target_name': 'libGLESv2',
            'type': '<(angle_gl_library_type)',
            'dependencies': [ 'libANGLE', 'angle_common' ],
            'includes': [ '../build/common_defines.gypi', ],
            'sources':
            [
                '<@(libglesv2_sources)',
            ],
            'defines':
            [
                'LIBGLESV2_IMPLEMENTATION',
            ],
            'conditions':
            [
                ['angle_build_winrt==1',
                {
                    'msvs_enable_winrt' : '1',
                    'msvs_application_type_revision' : '<(angle_build_winrt_app_type_revision)',
                    'msvs_requires_importlibrary' : 'true',
                    'msvs_settings':
                    {
                        'VCLinkerTool':
                        {
                            'EnableCOMDATFolding': '1',
                            'OptimizeReferences': '1',
                        }
                    },
                }],
                ['angle_build_winphone==1',
                {
                    'msvs_enable_winphone' : '1',
                }],
                ['angle_use_glx==1',
                {
                    'link_settings': {
                        'ldflags': [
                            '<!@(pkg-config --libs-only-L --libs-only-other x11 xi)',
                        ],
                        'libraries': [
                            '<!@(pkg-config --libs-only-l x11 xi) -ldl',
                        ],
                    },
                }],
                ['OS == "mac"',
                {
                    'all_dependent_settings':
                    {
                        'xcode_settings': {
                            'LD_RUNPATH_SEARCH_PATHS': ['@executable_path/.'],
                        },
                    }
                }]
            ],
        },
    ],
}
