local common = {
  Env = {
    CPPDEFS = {
      "HAVE_CONFIG_H",
      "_CRT_SECURE_NO_WARNINGS",
    },
    CPPPATH = {
      ".",
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
