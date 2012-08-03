
if (args.build == undefined) args.build = "Release";
if (args.build != "Debug" && args.build != "Release") throw "build='<Release|Debug>' expected";

if (args.rebuild == undefined) args.rebuild = false;

var exe = new Exe;
exe.Build(args.build);
exe.Output("../" + args.build + "/FBuild.exe");
exe.Includes("/boost_1_50_0", "../V8/Include");
exe.CRT("Static");
exe.WarningLevel(4).WarningAsError(true);
exe.Files(Glob("*.cpp"));
exe.PrecompiledHeader("Precompiled.h", "Precompiled.cpp");
exe.LibPath("/boost_1_50_0/stage/lib", "../" + args.build);
exe.Libs("v8_base.lib", "preparser_lib.lib", "v8_nosnapshot.lib", "v8_snapshot.lib", "Ws2_32.lib", "Winmm.lib", "Shlwapi.lib");
exe.DependencyCheck(!args.rebuild);
if (args.build == "debug") exe.LinkArgs("-ignore:4099"); // Gnah, Fuck you Microsoft. Why is this Warning not ignorable?

exe.Create();
