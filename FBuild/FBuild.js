
if (args.build == undefined) args.build = "Release";
if (args.build != "Debug" && args.build != "Release") throw "build='<Release|Debug>' expected";

if (args.rebuild == undefined) args.rebuild = false;

var exe = new Exe;
exe.Build(args.build);
exe.Output("../" + args.build + "/FBuild.exe");
exe.Includes("../Boost");
exe.Defines("_CRT_SECURE_NO_WARNINGS");
exe.CRT("Static");
exe.WarningLevel(4).WarningAsError(true);
exe.Files(Glob("*.cpp"));
exe.PrecompiledHeader("Precompiled.h", "Precompiled.cpp");
exe.LibPath("../Boost/stage/lib", "../" + args.build);
exe.Libs("Duktape.lib", "Ws2_32.lib", "Winmm.lib", "Shlwapi.lib");
exe.DependencyCheck(!args.rebuild);

exe.Create();
