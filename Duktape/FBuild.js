
if (args.build == undefined) args.build = "Release";
if (args.build != "Debug" && args.build != "Release") throw "Build='<Release|Debug>' expected";

if (args.rebuild == undefined) args.rebuild = false;

ToolChain("x64");

var lib = new Lib;
lib.Build(args.build);
lib.Files(Glob("*.c"));
lib.DependencyCheck(!args.rebuild);
lib.Output("../" + args.build + "/Duktape.lib");
lib.CRT("Static");
lib.Defines("DUK_USE_DATE_NOW_WINDOWS");
lib.Create();
