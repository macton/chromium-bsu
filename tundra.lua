local common = {
  Env = {
    CPPDEFS = {
      "ZLIB_DLL",
      "HAVE_CONFIG_H",
      "_CRT_SECURE_NO_WARNINGS",
      "FT2_INCLUDE=3pp/freetype2/include",
      'FT_CONFIG_CONFIG_H=\\"3pp/freetype2-custom/ftconfig.h\\"',
    },
    CPPPATH = {
      ".",
      "3pp/zlib",
      "3pp/freetype2/include",
      "../chromium-bsu-3pp/sdl/include",
      "../chromium-bsu-3pp/sdl_image/include",
    },
    LIBPATH = {
      "../chromium-bsu-3pp/sdl/lib/x64",
      "../chromium-bsu-3pp/sdl_image/lib/x64",
    },
    CXXOPTS = {
      "/EHsc",    -- exceptions
      "/wd4127",  -- conditional expression is constant
      "/W4",
      "/WX",
      { "/O2"; Config = "*-*-release" },
    },
  },
}

Build {

  Units = function()
    local zlib = SharedLibrary {
      Name = "zlib",
      SourceDir = "3pp/zlib/",
      Sources = {
        "adler32.c", "crc32.c", "crc32.h", "deflate.c", "deflate.h",
        "gzclose.c", "gzguts.h", "gzlib.c", "gzread.c", "gzwrite.c",
        "infback.c", "inffast.c", "inffast.h", "inffixed.h", "inflate.c",
        "inflate.h", "inftrees.c", "inftrees.h", "trees.c", "trees.h",
        "zconf.h", "zlib.h", "zutil.c", "zutil.h",
      },
    }

    local freetype2 = SharedLibrary {
      Name = "freetype2",
      Defines = {
        "FT2_BUILD_LIBRARY",
        'FT_CONFIG_MODULES_H=\\"3pp/freetype2-custom/ftmodule.h\\"',
      },
      Includes = { "3pp/freetype2/include", },
      SourceDir = "3pp/freetype2/src/",
      Sources = {
        "base/ftsystem.c",
        "base/ftinit.c",
        "base/ftdebug.c",
        "base/ftbase.c",
        "base/ftbbox.c",
        "base/ftglyph.c",
        "base/ftbitmap.c",   -- optional, see <ftbitmap.h>
        "truetype/truetype.c", -- TrueType font driver
        "raster/raster.c",   -- monochrome rasterizer
        "smooth/smooth.c",   -- anti-aliasing rasterizer
        "autofit/autofit.c", -- auto hinting module
        "sfnt/sfnt.c",       -- SFNT files support
        "psnames/psnames.c", -- PostScript glyph names support
        "gzip/ftgzip.c",     -- support for compressed fonts (.gz)
      },
    }

    local ftgl = SharedLibrary {
      Name = "ftgl",
      Depends = { "freetype2" },
      Libs = { "opengl32.lib", "glu32.lib" },
      Env = {
        CXXOPTS = {
          "/wd4996", -- The POSIX name for this item is deprecated Instead, use the ISO C++ conformant name: _strdup. See online help for details.
          "/wd4512", -- Assignment operator could not be generated
          "/wd4100", -- Unreferenced formal
          "/wd4244", -- Integer truncation
        },
      },
      Defines = {
        "FTGL_LIBRARY", -- building it
        '__FUNC__=\\"function\\"',  -- lame
      },
      Includes = {
        "3pp/ftgl/src",
        "3pp/ftgl-custom",
      },
      SourceDir = "3pp/ftgl/src/",
      Sources = {
        "FTBuffer.cpp",
        "FTCharmap.cpp",
        "FTCharmap.h",
        "FTCharToGlyphIndexMap.h",
        "FTCleanup.cpp",
        "FTCleanup.h",
        "FTContour.cpp",
        "FTContour.h",
        "FTFace.cpp",
        "FTFace.h",
        "FTGL.cpp",
        "FTGlyphContainer.cpp",
        "FTGlyphContainer.h",
        "FTInternals.h",
        "FTLibrary.cpp",
        "FTLibrary.h",
        "FTList.h",
        "FTPoint.cpp",
        "FTSize.cpp",
        "FTSize.h",
        "FTUnicode.h",
        "FTVector.h",
        "FTVectoriser.cpp",
        "FTVectoriser.h",
        "FTFont/FTBitmapFont.cpp",
        "FTFont/FTBitmapFontImpl.h",
        "FTFont/FTBufferFont.cpp",
        "FTFont/FTBufferFontImpl.h",
        "FTFont/FTExtrudeFont.cpp",
        "FTFont/FTExtrudeFontImpl.h",
        "FTFont/FTFont.cpp",
        "FTFont/FTFontGlue.cpp",
        "FTFont/FTFontImpl.h",
        "FTFont/FTOutlineFont.cpp",
        "FTFont/FTOutlineFontImpl.h",
        "FTFont/FTPixmapFont.cpp",
        "FTFont/FTPixmapFontImpl.h",
        "FTFont/FTPolygonFont.cpp",
        "FTFont/FTPolygonFontImpl.h",
        "FTFont/FTTextureFont.cpp",
        "FTFont/FTTextureFontImpl.h",
        "FTGL/FTBBox.h",
        "FTGL/FTBitmapGlyph.h",
        "FTGL/FTBuffer.h",
        "FTGL/FTBufferFont.h",
        "FTGL/FTBufferGlyph.h",
        "FTGL/FTExtrdGlyph.h",
        "FTGL/FTFont.h",
        "FTGL/ftgl.h",
        "FTGL/FTGLBitmapFont.h",
        "FTGL/FTGLExtrdFont.h",
        "FTGL/FTGLOutlineFont.h",
        "FTGL/FTGLPixmapFont.h",
        "FTGL/FTGLPolygonFont.h",
        "FTGL/FTGLTextureFont.h",
        "FTGL/FTGlyph.h",
        "FTGL/FTLayout.h",
        "FTGL/FTOutlineGlyph.h",
        "FTGL/FTPixmapGlyph.h",
        "FTGL/FTPoint.h",
        "FTGL/FTPolyGlyph.h",
        "FTGL/FTSimpleLayout.h",
        "FTGL/FTTextureGlyph.h",
        "FTGlyph/FTBitmapGlyph.cpp",
        "FTGlyph/FTBitmapGlyphImpl.h",
        "FTGlyph/FTBufferGlyph.cpp",
        "FTGlyph/FTBufferGlyphImpl.h",
        "FTGlyph/FTExtrudeGlyph.cpp",
        "FTGlyph/FTExtrudeGlyphImpl.h",
        "FTGlyph/FTGlyph.cpp",
        "FTGlyph/FTGlyphGlue.cpp",
        "FTGlyph/FTGlyphImpl.h",
        "FTGlyph/FTOutlineGlyph.cpp",
        "FTGlyph/FTOutlineGlyphImpl.h",
        "FTGlyph/FTPixmapGlyph.cpp",
        "FTGlyph/FTPixmapGlyphImpl.h",
        "FTGlyph/FTPolygonGlyph.cpp",
        "FTGlyph/FTPolygonGlyphImpl.h",
        "FTGlyph/FTTextureGlyph.cpp",
        "FTGlyph/FTTextureGlyphImpl.h",
        "FTLayout/FTLayout.cpp",
        "FTLayout/FTLayoutGlue.cpp",
        "FTLayout/FTLayoutImpl.h",
        "FTLayout/FTSimpleLayout.cpp",
        "FTLayout/FTSimpleLayoutImpl.h",
      },
    }

    local chromium = Program {
      Name = "chromium",
      Libs = { "SDL" },
      Sources = {
        'chromium-bsu-config.h',
        'src/Ammo.cpp', 'src/Ammo.h', 'src/Audio.cpp', 'src/Audio.h',
        'src/AudioOpenAL.cpp', 'src/AudioOpenAL.h', 'src/AudioSDLMixer.cpp',
        'src/AudioSDLMixer.h', 'src/compatibility.h', 'src/Config.cpp',
        'src/Config.h', 'src/define.h', 'src/EnemyAircraft.cpp',
        'src/EnemyAircraft.h', 'src/EnemyAircraft_Boss00.cpp',
        'src/EnemyAircraft_Boss00.h', 'src/EnemyAircraft_Boss01.cpp',
        'src/EnemyAircraft_Boss01.h', 'src/EnemyAircraft_Gnat.cpp',
        'src/EnemyAircraft_Gnat.h', 'src/EnemyAircraft_Omni.cpp',
        'src/EnemyAircraft_Omni.h', 'src/EnemyAircraft_RayGun.cpp',
        'src/EnemyAircraft_RayGun.h', 'src/EnemyAircraft_Straight.cpp',
        'src/EnemyAircraft_Straight.h', 'src/EnemyAircraft_Tank.cpp',
        'src/EnemyAircraft_Tank.h', 'src/EnemyAmmo.cpp', 'src/EnemyAmmo.h',
        'src/EnemyFleet.cpp', 'src/EnemyFleet.h', 'src/Explosions.cpp',
        'src/Explosions.h', 'src/extern.h', 'src/gettext.h', 'src/Global.cpp',
        'src/Global.h', 'src/Ground.cpp', 'src/Ground.h',
        'src/GroundMetal.cpp', 'src/GroundMetal.h',
        'src/GroundMetalSegment.cpp', 'src/GroundMetalSegment.h',
        'src/GroundSea.cpp', 'src/GroundSea.h', 'src/GroundSeaSegment.cpp',
        'src/GroundSeaSegment.h', 'src/GroundSegment.cpp',
        'src/GroundSegment.h', 'src/HeroAircraft.cpp', 'src/HeroAircraft.h',
        'src/HeroAmmo.cpp', 'src/HeroAmmo.h', 'src/HiScore.cpp',
        'src/HiScore.h', 'src/Image.cpp', 'src/Image.h', 'src/main.cpp',
        'src/main.h', 'src/MainGL.cpp', 'src/MainGL.h', 'src/MainGLUT.cpp',
        'src/MainGLUT.h', 'src/MainSDL.cpp', 'src/MainSDL.h',
        'src/MainSDL_Event.cpp', 'src/MainToolkit.cpp', 'src/MainToolkit.h',
        'src/Makefile.am', 'src/MenuGL.cpp', 'src/MenuGL.h',
        'src/PowerUps.cpp', 'src/PowerUps.h', 'src/ScreenItem.cpp',
        'src/ScreenItem.h', 'src/ScreenItemAdd.cpp', 'src/ScreenItemAdd.h',
        'src/StatusDisplay.cpp', 'src/StatusDisplay.h', 'src/Text.cpp',
        'src/Text.h', 'src/TextFTGL.cpp', 'src/TextFTGL.h',
        'src/textGeometry.h', 'src/textGeometryBSU.cpp',
        'src/textGeometryChromium.cpp', 'src/TextGLC.cpp', 'src/TextGLC.h',
      },
    }
    Default(chromium)
  end,

  Configs = {
    Config {
      Name = "win64-vs2012",
      Tools = { { "msvc-vs2012"; TargetArch = "x64" } },
      Inherit = common,
      DefaultOnHost = "windows",
    },
  },

}
