workspace "OpenGL"
   filename "OpenGL_Workspace"
   location("builds/" .. _ACTION)

   configurations { "Debug", "Release" }
   platforms { "Win64" }

project "OpenGL"
   filename "OpenGL_Project"
   location("builds/" .. _ACTION .. "/project")

   kind "ConsoleApp"
   language "C++"
   targetdir "%{prj.location}/%{cfg.buildcfg}_%{cfg.platform}"
   targetname "program_vs2022"
   
   prebuildcommands { "{COPYDIR} %[Assets] %[%{cfg.buildtarget.directory}/Assets]" }

   files { "include/*.hpp", "src/*.cpp", _WORKING_DIR .. "/external/glad/src/glad.c" }

   links { "glfw3", "gdi32", "user32", "kernel32" }

   libdirs { _WORKING_DIR .. "/external/glfw-3.4-win64/lib-vc2022" }

   includedirs { "include", _WORKING_DIR .. "/external/glad/include", _WORKING_DIR .. "/external/glfw-3.4-win64/include", _WORKING_DIR .. "/external/glm-1.0.1" }

   debugdir "%{cfg.buildtarget.absolutepath}"

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"