
if (args.build == undefined) args.build = "Release";
if (args.build != "Debug" && args.build != "Release") throw "build='<Release|Debug>' expected";

if (args.rebuild == undefined) args.rebuild = false;

ToolChain("x64");

var exe = new Exe;
exe.Build(args.build);
exe.Files(Glob("*.cpp"));
exe.DependencyCheck(!args.rebuild);
exe.CRT("Static");
exe.Includes("../Boost");
exe.Defines("_CRT_SECURE_NO_WARNINGS");
exe.PrecompiledHeader("Precompiled.h", "Precompiled.cpp");
exe.CompileArgs("-std:c++latest");
exe.WarningLevel(4).WarningAsError(true);

exe.Output("../" + args.build + "/FBuild.exe");
exe.LibPath("../Boost/stage/lib", "../" + args.build);
exe.Libs("Duktape.lib", "Ws2_32.lib", "Winmm.lib", "Shlwapi.lib");

exe.Create();
