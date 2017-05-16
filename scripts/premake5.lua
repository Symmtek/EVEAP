Python_inc             = "C:/Miatar/Soft/Python27/include"

Python_lib             = "C:/Miatar/Soft/Python27/libs"

workspace "EVEMP"
   configurations { "Debug", "Release" }
   location "../"

project "EVEMP"
   kind "SharedLib"
   language "C++"
   location "../"
   targetdir "bin/%{cfg.buildcfg}"

   files {
        "../**.cpp",
        "../**.cxx",
        "../**.c",
        "../**.hpp",
        "../**.hxx",
        "../**.h",
    }

   flags {"StaticRuntime", "Unicode"}
   includedirs { Python_inc }
   libdirs { Python_lib }

   filter "configurations:Debug"
      defines { "WIN32", "_DEBUG", "_WINDOWS", "_USRDLL", "EVEMP_EXPORTS" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "WIN32", "NDEBUG", "_WINDOWS", "_USRDLL", "EVEMP_EXPORTS"}
      optimize "Full"