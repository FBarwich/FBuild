
if Build == nil then
   Build = "Release" 
end

if Build ~= "Release" and Build ~= "Debug" then 
   error("Build='<Release|Debug>' expected");
end;

FBuild.BuildExe({
                   Output = "../" .. Build .. "/FBuild.exe";
                   Config   = Build;
                   Outdir   = Build;
                   Includes = { "/boost_1_50_0" };
                   CRT      = 'Static';
                   Files    = FBuild.Glob("*.cpp");
                   PrecompiledHeader = {         
                      Header = 'Precompiled.h';  
                      Cpp    = 'Precompiled.cpp';
                   };
                   Libpath = { "/boost_1_50_0/stage/lib", "../" .. Build };
                   Libs = { "Lua.lib", "Shlwapi.lib" };
                });

