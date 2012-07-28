/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>

class FileToCpp {
   bool        dependencyCheck;
   std::string infile;
   std::string outfile;
   std::string nameForNamespace;
   std::string nameForArray;
   std::string nameForPtr;
   std::string intro;
   std::string outro;
   std::string additional;
   bool        varConst;
   bool        terminatingNull;

   void CheckParams ();
   bool NeedsRebuild ();

public:
   FileToCpp () : dependencyCheck(true), varConst(true), terminatingNull(false) { }

   void DependencyCheck (bool v)    { dependencyCheck = v; }
   void Const (bool v)              { varConst = v; }
   void NullTerminator (bool v)    { terminatingNull = v; }
   void Infile (std::string v)      { infile = std::move(v); }
   void Outfile (std::string v)     { outfile = std::move(v); }
   void Namespace (std::string v)   { nameForNamespace = std::move(v); }
   void Array (std::string v)        { nameForArray = std::move(v); }
   void Ptr (std::string v)         { nameForPtr = std::move(v); }
   void Intro (std::string v)       { intro = std::move(v); }
   void Outro (std::string v)       { outro = std::move(v); }
   void Additional (std::string v)  { additional = std::move(v); }

   bool               DependencyCheck () const  { return dependencyCheck; }
   bool               Const () const            { return varConst; }
   bool               NullTerminator () const  { return terminatingNull; }
   const std::string& Infile () const           { return infile; }
   const std::string& Outfile () const          { return outfile; }
   const std::string& Namespace () const        { return nameForNamespace; }
   const std::string& Array () const            { return nameForArray; }
   const std::string& Ptr () const              { return nameForPtr; }
   const std::string& Intro () const            { return intro; }
   const std::string& Outro () const            { return outro; }
   const std::string& Additional () const       { return additional; }


   void Create ();
};

