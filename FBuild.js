
/* Build Boost */
ChangeDirectory("Boost");
Run("bootstrap");
Run("b2 headers");
Run("b2");
ChangeDirectory("..");


/* Build  FBuild */
Build("FBuild");
