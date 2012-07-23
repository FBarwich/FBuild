#pragma once

#include <string>

class FileToCpp {
   bool        checkDependency;
   std::string infile;
   std::string outfile;
   std::string nameForNamespace;
   std::string nameForArray;
   std::string nameForPtr;
   std::string intro;
   std::string outro;
   std::string additional;
   bool        varConst;
   bool        addTerminatingNull;
   
   void CheckParams ();
   bool CheckDependency ();

public:
   FileToCpp () : checkDependency(true), varConst(true) { }

   void CheckDependency (bool v)          { checkDependency = v; }
   void Infile (const std::string& v)     { infile = v; }
   void Outfile (const std::string& v)    { outfile = v; }
   void Namespace (const std::string& v)  { nameForNamespace = v; }
   void Arry (const std::string& v)       { nameForArray = v; }
   void Ptr (const std::string& v)        { nameForPtr = v; }
   void Intro (const std::string& v)      { intro = v; }
   void Outro (const std::string& v)      { outro = v; }
   void Additional (const std::string& v) { additional = v; }
   void Const (bool v)                    { varConst = v; }
   void AddTerminatingNull (bool v)       { addTerminatingNull = v; }

   void Go ();
};

