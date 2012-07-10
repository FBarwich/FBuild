
if Build == nil then
   Build = "Release" 
end

if Build ~= "Release" and Build ~= "Debug" then 
   error("Build='<Release|Debug> expected");
end;

FBuild.BuildStaticLib({
                        Output = "../" .. Build .. "/lua.lib";
                        Outdir = Build;
                        Config = Build;
                        CRT    = 'Static';
                        CC     = '-arch:SSE2';
                        Files  = { 
                           "src/lauxlib.c",
                           "src/lbaselib.c",
                           "src/lbitlib.c",
                           "src/lcode.c",
                           "src/ldblib.c",
                           "src/lcorolib.c",
                           "src/lapi.c",
                           "src/lfunc.c",
                           "src/linit.c",
                           "src/lgc.c",
                           "src/ldo.c",
                           "src/lmathlib.c",
                           "src/llex.c",
                           "src/lmem.c",
                           "src/loadlib.c",
                           "src/lobject.c",
                           "src/lopcodes.c",
                           "src/loslib.c",
                           "src/lparser.c",
                           "src/lstate.c",
                           "src/lstrlib.c",
                           "src/ldebug.c",
                           "src/ltablib.c",
                           "src/lctype.c",
                           "src/liolib.c",
                           "src/ldump.c",
                           "src/luac.c",
                           "src/ltm.c",
                           "src/ltable.c",
                           "src/lstring.c",
                           "src/lzio.c",
                           "src/lundump.c",
                           "src/lvm.c"
                        };
                      });

