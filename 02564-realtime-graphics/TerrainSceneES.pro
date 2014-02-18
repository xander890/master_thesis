# Add files and directories to ship with the application
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
# DEPLOYMENTFOLDERS = # file1 dir1

TEMPLATE = app

CONFIG += mobility
MOBILITY += sensors

QT += opengl

INCLUDEPATH += lib02564

HEADERS += exercises/TerrainScene_ES/Terrain.h\
        exercises/TerrainScene_ES/TerrainScene.h\
        exercises/TerrainScene_ES/auxil.h\
        exercises/TerrainScene_ES/detector.h

SOURCES += exercises/TerrainScene_ES/Terrain.cpp\
        exercises/TerrainScene_ES/auxil.cpp \
        exercises/TerrainScene_ES/TerrainScene.cpp \
        exercises/TerrainScene_ES/detector.cpp

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
lib02564/Mesh/Material.h\
lib02564/Mesh/Texmap.h\
lib02564/Mesh/ObjLoader.h\
lib02564/Mesh/TriangleMesh.h\
lib02564/Util/ArgExtracter.h\
lib02564/Util/Grid2D.h\

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

RESOURCES +=  \
    TerrainSceneES.qrc

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += shaders/TerrainSceneES/* \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog


