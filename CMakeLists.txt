cmake_minimum_required (VERSION 2.8)
project(ogreTrees)

add_definitions(-DINCLUDE_RTSHADER_SYSTEM)

## [discover_ogre]
# specify which version and components you need
set(OGRE_DIR /usr/local/lib/OGRE/cmake)
#find_package(OGRE 1.11 REQUIRED COMPONENTS Bites RTShaderSystem)
find_package(OGRE 1.11 REQUIRED COMPONENTS Bites)

set(MEDIA_DIR ${CMAKE_BINARY_DIR}/media/)

include_directories(../../ogre/Samples/Common/include)
#include_directories(../../ogre/Samples/Character/include)
include_directories(../../ogre/OgreMain/src)
include_directories(src)
include_directories(tools)
include_directories(src/btOgre)
include_directories(../Common)
include_directories(../../Bullet)


link_directories(../../../ogre/build/Dependencies/lib)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib/OGRE)

set(CMAKE_EXECUTABLE_SUFFIX ".js")
set(CMAKE_EXE_LINKER_FLAGS "--preload-file ${MEDIA_DIR}/@. -s EXPORTED_FUNCTIONS=\"['_passAssetAsArrayBuffer', '_clearScene', '_main']\" -s USE_SDL=2 -s WASM=1 -std=c++1z -s ALLOW_MEMORY_GROWTH=1  -s USE_BULLET=1") # -s USE_LIBPNG=1 -s USE_ZLIB=1

add_definitions(-s USE_SDL=2 -s WASM=1 -std=c++1z -s USE_BULLET=1)

add_executable(ogreTrees Main.cpp src/GameOffContext.cpp src/Gem.cpp src/Projectile.cpp src/CoinScene.cpp src/BulletScene.cpp src/Tail.cpp src/ConvoManager.cpp src/StatsManager.cpp src/btOgre/btOgreSoftBody.cpp)

target_link_libraries(
ogreTrees
OgreBitesStatic
OgreOverlayStatic
#OgreRTShaderSystemStatic
#Plugin_ParticleFXStatic
#Plugin_OctreeSceneManagerStatic  #!
RenderSystem_GLES2Static
OgreGLSupportStatic
#Codec_STBIStatic
z
zzip
freetype
OgrePropertyStatic
#OgreHLMSStatic
${OGRE_LIBRARIES})
