
/* Build Boost */
ChangeDirectory("Boost");
Run("bootstrap");
Run("b2 headers");
Run("b2 threading=multi link=static runtime-link=static address-model=64 toolset=msvc-14.0");
ChangeDirectory("..");

/* Build Duktape */
Build("Duktape");

/* Build  FBuild */
Build("FBuild");
