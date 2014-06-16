QT += core gui widgets opengl

CONFIG += c++11
TARGET = TranslucentMaterials
TEMPLATE = app

win32 {
    INCLUDEPATH +=  "$${PWD}/glew-1.10.0/include/"
    !contains(QMAKE_TARGET.arch, x86_64) {

        LIBS *=  "$${PWD}/glew-1.10.0/lib/Release/Win32/glew32s.lib"
    } else {

        LIBS *=  "$${PWD}/glew-1.10.0/lib/Release/x64/glew32s.lib"
    }
}

mac{
    eval(QT_MAJOR_VERSION = "4") {
        LIBS += -framework Foundation -framework Cocoa
        OBJECTIVE_SOURCES  +=  lib02564/GLGraphics/core_profile_attributes.mm
    }
}

INCLUDEPATH += lib02564
INCLUDEPATH += exercises/TranslucentMaterials

QMAKE_CXXFLAGS += /wd4996

HEADERS += exercises/TranslucentMaterials/GBuffer.h\
        exercises/TranslucentMaterials/ShadowBuffer.h\
        exercises/TranslucentMaterials/Terrain.h\
    lib02564/Mesh/scatteringmaterial.h \
    lib02564/GLGraphics/light.h \
    lib02564/GLGraphics/lightmanager.h \
    lib02564/Mesh/proceduralsphere.h \
    lib02564/Objects/threedplane.h \
    lib02564/Objects/threedsphere.h \
    lib02564/Utils/areaestimator.h \
    lib02564/Objects/threedcube.h \
    lib02564/Utils/miscellaneous.h \
    lib02564/Dipoles/directionaldipole.h \
    lib02564/Dipoles/dipolecpu.h \
    lib02564/Utils/cglautils.h \
    lib02564/Dipoles/bssrdf.h \
    lib02564/Dipoles/jensendipole.h \
    lib02564/Mesh/material.h \
    lib02564/Dipoles/betterdipole.h \
    lib02564/Dipoles/singlescattering.h \
    lib02564/Mesh/texture.h \
    lib02564/Dipoles/dipolegpu.h \
    exercises/TranslucentMaterials/translucentmaterials.h \
    exercises/TranslucentMaterials/GUI/mainwindow.h \
    exercises/TranslucentMaterials/GUI/vectorform.h \
    exercises/TranslucentMaterials/GUI/onelinetextedit.h \
    exercises/TranslucentMaterials/GUI/colorform.h \
    lib02564/Utils/defaultmaterials.h \
    lib02564/Objects/threedline.h \
    lib02564/Objects/threedgrid.h \
    exercises/TranslucentMaterials/GUI/threedobjectgui.h \
    lib02564/Objects/threedwirecube.h \
    exercises/TranslucentMaterials/vertexnormalbuffer.h \
    exercises/TranslucentMaterials/cubemapbuffer.h \
    exercises/TranslucentMaterials/GUI/sliderlabel.h \
    exercises/TranslucentMaterials/GUI/scatteringmaterialgui.h \
    exercises/TranslucentMaterials/arraytexturebuffer.h \
    exercises/TranslucentMaterials/mipmapgenerator.h \
    exercises/TranslucentMaterials/arrayvertexnormalbuffer.h \
    exercises/TranslucentMaterials/depthonlybuffer.h \
    lib02564/Mesh/imagetexture.h \
    exercises/TranslucentMaterials/arrayimagebuffer.h \
    lib02564/GLGraphics/computeshader.h \
    lib02564/Utils/performancetimer.h \
    exercises/TranslucentMaterials/shaderpreprocessor.h \
    exercises/TranslucentMaterials/mipmapgeneratorview.h \
    lib02564/GLGraphics/infinitearealight.h

SOURCES += exercises/TranslucentMaterials/GBuffer.cpp\
        exercises/TranslucentMaterials/ShadowBuffer.cpp\
        exercises/TranslucentMaterials/Terrain.cpp\
    lib02564/Mesh/scatteringmaterial.cpp \
    lib02564/GLGraphics/light.cpp \
    lib02564/GLGraphics/lightmanager.cpp \
    lib02564/Mesh/proceduralsphere.cpp \
    lib02564/Objects/threedplane.cpp \
    lib02564/Objects/threedsphere.cpp \
    lib02564/Utils/areaestimator.cpp \
    lib02564/Objects/threedcube.cpp \
    lib02564/Utils/miscellaneous.cpp \
    lib02564/Dipoles/directionaldipole.cpp \
    lib02564/Dipoles/dipolecpu.cpp \
    lib02564/Utils/cglautils.cpp \
    lib02564/Dipoles/jensendipole.cpp \
    lib02564/Mesh/material.cpp \
    lib02564/Dipoles/betterdipole.cpp \
    lib02564/Dipoles/singlescattering.cpp \
    lib02564/Mesh/texture.cpp \
    lib02564/Dipoles/dipolegpu.cpp \
    exercises/TranslucentMaterials/translucentmaterials.cpp \
    exercises/TranslucentMaterials/GUI/mainwindow.cpp \
    exercises/TranslucentMaterials/GUI/vectorform.cpp \
    exercises/TranslucentMaterials/GUI/onelinetextedit.cpp \
    exercises/TranslucentMaterials/GUI/colorform.cpp \
    lib02564/Utils/defaultmaterials.cpp \
    lib02564/Objects/threedline.cpp \
    lib02564/Objects/threedgrid.cpp \
    exercises/TranslucentMaterials/GUI/threedobjectgui.cpp \
    lib02564/Objects/threedwirecube.cpp \
    exercises/TranslucentMaterials/vertexnormalbuffer.cpp \
    exercises/TranslucentMaterials/cubemapbuffer.cpp \
    exercises/TranslucentMaterials/GUI/sliderlabel.cpp \
    exercises/TranslucentMaterials/GUI/scatteringmaterialgui.cpp \
    exercises/TranslucentMaterials/arraytexturebuffer.cpp \
    exercises/TranslucentMaterials/mipmapgenerator.cpp \
    exercises/TranslucentMaterials/arrayvertexnormalbuffer.cpp \
    exercises/TranslucentMaterials/depthonlybuffer.cpp \
    lib02564/Mesh/imagetexture.cpp \
    exercises/TranslucentMaterials/arrayimagebuffer.cpp \
    lib02564/GLGraphics/computeshader.cpp \
    lib02564/Utils/performancetimer.cpp \
    exercises/TranslucentMaterials/shaderpreprocessor.cpp \
    exercises/TranslucentMaterials/mipmapgeneratorview.cpp \
    lib02564/GLGraphics/infinitearealight.cpp


HEADERS += lib02564/CGLA/ArithMatFloat.h\
lib02564/CGLA/ArithQuat.h\
lib02564/CGLA/ArithSqMat2x2Float.h\
lib02564/CGLA/ArithSqMat3x3Float.h\
lib02564/CGLA/ArithSqMat4x4Float.h\
lib02564/CGLA/ArithSqMatFloat.h\
lib02564/CGLA/ArithVec.h\
lib02564/CGLA/ArithVec2Float.h\
lib02564/CGLA/ArithVec3Float.h\
lib02564/CGLA/ArithVec3Int.h\
lib02564/CGLA/ArithVec4Float.h\
lib02564/CGLA/ArithVec4Int.h\
lib02564/CGLA/ArithVecFloat.h\
lib02564/CGLA/ArithVecInt.h\
lib02564/CGLA/BitMask.h\
lib02564/CGLA/CGLA.h\
lib02564/CGLA/eigensolution.h\
lib02564/CGLA/ExceptionStandard.h\
lib02564/CGLA/Mat2x2d.h\
lib02564/CGLA/Mat2x2f.h\
lib02564/CGLA/Mat2x3d.h\
lib02564/CGLA/Mat2x3f.h\
lib02564/CGLA/Mat3x3d.h\
lib02564/CGLA/Mat3x3f.h\
lib02564/CGLA/Mat4x4d.h\
lib02564/CGLA/Mat4x4f.h\
lib02564/CGLA/Quatd.h\
lib02564/CGLA/Quaternion.h\
lib02564/CGLA/Quatf.h\
lib02564/CGLA/statistics.h\
lib02564/CGLA/TableTrigonometry.h\
lib02564/CGLA/UnitVector.h\
lib02564/CGLA/Vec2d.h\
lib02564/CGLA/Vec2f.h\
lib02564/CGLA/Vec2i.h\
lib02564/CGLA/Vec2ui.h\
lib02564/CGLA/Vec3d.h\
lib02564/CGLA/Vec3f.h\
lib02564/CGLA/Vec3Hf.h\
lib02564/CGLA/Vec3i.h\
lib02564/CGLA/Vec3uc.h\
lib02564/CGLA/Vec3usi.h\
lib02564/CGLA/Vec4d.h\
lib02564/CGLA/Vec4f.h\
lib02564/CGLA/Vec4i.h\
lib02564/CGLA/Vec4uc.h\
lib02564/GLGraphics/Core3_2_context.h\
lib02564/GLGraphics/GLHeader.h\
lib02564/GLGraphics/ShaderProgram.h\
lib02564/GLGraphics/ThreeDObject.h\
lib02564/GLGraphics/ResourceLoader.h\
lib02564/GLGraphics/User.h\
lib02564/Mesh/Texmap.h\
lib02564/Mesh/ObjLoader.h\
lib02564/Mesh/TriangleMesh.h\

SOURCES += lib02564/CGLA/ArithSqMat3x3Float.cpp\
lib02564/CGLA/ArithSqMat4x4Float.cpp\
lib02564/CGLA/ArithVec2Float.cpp\
lib02564/CGLA/ArithVec3Float.cpp\
lib02564/CGLA/eigensolution.cpp\
lib02564/CGLA/gel_rand.cpp\
lib02564/CGLA/Mat3x3d.cpp\
lib02564/CGLA/Mat3x3f.cpp\
lib02564/CGLA/Mat4x4d.cpp\
lib02564/CGLA/Mat4x4f.cpp\
lib02564/CGLA/statistics.cpp\
lib02564/CGLA/TableTrigonometry.cpp\
lib02564/CGLA/Vec2i.cpp\
lib02564/CGLA/Vec3f.cpp\
lib02564/CGLA/Vec3i.cpp\
lib02564/GLGraphics/GLHeader.cpp\
lib02564/GLGraphics/ShaderProgram.cpp\
lib02564/GLGraphics/ThreeDObject.cpp\
lib02564/GLGraphics/ResourceLoader.cpp\
lib02564/GLGraphics/User.cpp\
lib02564/Mesh/Texmap.cpp\
lib02564/Mesh/ObjLoader.cpp\
lib02564/Mesh/TriangleMesh.cpp

RESOURCES +=

OTHER_FILES += shaders/TranslucentMaterials/* \
    shaders/TranslucentMaterials/red.frag \
    shaders/TranslucentMaterials/red.vert \
    shaders/TranslucentMaterials/plane.vert \
    shaders/TranslucentMaterials/plane.frag \
    shaders/TranslucentMaterials/jensen_dipole_gpu.vert \
    shaders/TranslucentMaterials/jensen_dipole_gpu.frag \
    shaders/TranslucentMaterials/better_dipole_gpu.vert \
    shaders/TranslucentMaterials/better_dipole_gpu.frag \
    shaders/TranslucentMaterials/directional_dipole_gpu.vert \
    shaders/TranslucentMaterials/directional_dipole_gpu.frag \
    shaders/TranslucentMaterials/jensen_dipole_gpu_vertex.vert \
    shaders/TranslucentMaterials/jensen_dipole_gpu_vertex.frag \
    shaders/TranslucentMaterials/better_dipole_gpu_vertex.frag \
    shaders/TranslucentMaterials/better_dipole_gpu_vertex.vert \
    shaders/TranslucentMaterials/directional_dipole_gpu_vertex.vert \
    shaders/TranslucentMaterials/directional_dipole_gpu_vertex.frag \
    shaders/TranslucentMaterials/color.vert \
    shaders/TranslucentMaterials/color.frag \
    shaders/TranslucentMaterials/ss_cubemap_gbuffer.vert \
    shaders/TranslucentMaterials/ss_cubemap_gbuffer.frag \
    shaders/TranslucentMaterials/ss_cubemap_test_gbuffer.vert \
    shaders/TranslucentMaterials/ss_cubemap_test_gbuffer.frag \
    shaders/TranslucentMaterials/ss_cubemap_test_wrap_gbuffer.vert \
    shaders/TranslucentMaterials/ss_cubemap_test_wrap_gbuffer.frag \
    shaders/TranslucentMaterials/ss_cubemap_render_to_cubemap.frag \
    shaders/TranslucentMaterials/ss_cubemap_render_to_cubemap.vert \
    shaders/TranslucentMaterials/ss_cubemap_test_render_to_cubemap_screen.vert \
    shaders/TranslucentMaterials/ss_cubemap_test_render_to_cubemap_screen.frag \
    shaders/TranslucentMaterials/ss_cubemap_test_render_to_cubemap_cube.vert \
    shaders/TranslucentMaterials/ss_cubemap_test_render_to_cubemap_cube.frag \
    shaders/TranslucentMaterials/ss_cubemap_combination.vert \
    shaders/TranslucentMaterials/ss_cubemap_combination.frag \
    shaders/TranslucentMaterials/ss_cubemap_render_to_cubemap_jensen.frag \
    shaders/TranslucentMaterials/ss_cubemap_render_to_cubemap_jeppe.vert \
    shaders/TranslucentMaterials/ss_cubemap_render_to_cubemap_jeppe.frag \
    shaders/TranslucentMaterials/ss_array_combination.vert \
    shaders/TranslucentMaterials/ss_array_combination.frag \
    shaders/TranslucentMaterials/ss_cubemap_render_to_mipmap.vert \
    shaders/TranslucentMaterials/ss_cubemap_render_to_mipmap.frag \
    shaders/TranslucentMaterials/ss_cubemap_render_to_mipmap.geom \
    shaders/TranslucentMaterials/ss_render_to_arraymap_imageStore_multilight_jensen.frag \
    shaders/TranslucentMaterials/ss_render_to_arraymap_imageStore_clear.frag \
    shaders/TranslucentMaterials/ss_aincludes_jensen_bssrdf.glinc \
    shaders/TranslucentMaterials/ss_aincludes_optics.glinc \
    shaders/TranslucentMaterials/ss_aincludes_random.glinc \
    shaders/TranslucentMaterials/ss_aincludes_ss_uniforms.glinc \
    shaders/TranslucentMaterials/ss_aincludes_directional_bssrdf.glinc \
    shaders/TranslucentMaterials/ss_aincludes_directional_bssrdf_opt.glinc \
    shaders/TranslucentMaterials/ss_skybox.vert \
    shaders/TranslucentMaterials/ss_skybox.frag \
    shaders/TranslucentMaterials/ss_aincludes_constants.glinc \
    shaders/TranslucentMaterials/ss_array_gbuffer_multilight.vert \
    shaders/TranslucentMaterials/ss_array_gbuffer_multilight.frag \
    shaders/TranslucentMaterials/ss_array_gbuffer_multilight.geom \
    shaders/TranslucentMaterials/ss_old_array_generate_mips.compute \
    shaders/TranslucentMaterials/ss_old_array_generate_mips_single.compute \
    shaders/TranslucentMaterials/ss_array_generate_mipmaps.vert \
    shaders/TranslucentMaterials/ss_array_generate_mipmaps.geom \
    shaders/TranslucentMaterials/ss_array_generate_mipmaps.frag \
    shaders/TranslucentMaterials/ss_array_render_to_arraymap_multilight_directional.frag \
    shaders/TranslucentMaterials/ss_array_render_to_arraymap_multilight_standard.frag \
    shaders/TranslucentMaterials/ss_array_render_to_arraymap.vert \
    shaders/TranslucentMaterials/ss_array_render_to_arraymap.geom \
    shaders/TranslucentMaterials/ss_array_debug_tex.vert \
    shaders/TranslucentMaterials/ss_array_debug_tex.frag \
    shaders/TranslucentMaterials/ss_array_depth_pass.vert \
    shaders/TranslucentMaterials/ss_array_depth_pass.geom \
    shaders/TranslucentMaterials/ss_array_depth_pass.frag

FORMS += \
    exercises/TranslucentMaterials/mainwindow.ui \
    exercises/TranslucentMaterials/GUI/vectorform.ui \
    exercises/TranslucentMaterials/GUI/colorform.ui \
    exercises/TranslucentMaterials/GUI/threedobject.ui \
    exercises/TranslucentMaterials/GUI/sliderlabel.ui \
    exercises/TranslucentMaterials/GUI/scatteringmaterialgui.ui
