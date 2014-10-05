
/* Build Boost */
ChangeDirectory("Boost");
Run("bootstrap");
Run("b2 headers");
Run("b2 threading=multi link=static runtime-link=static");
ChangeDirectory("..");

/* Build Duktape */
Build("Duktape");

/* Build  FBuild */
Build("FBuild");
