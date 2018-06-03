/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>


class Uic {
public:
   Uic(const std::string& qtBinDirectory);

   const std::string& OutDir () const { return outDir_; }
   const std::vector<std::string>& Files () const { return files_; }
   bool DependencyCheck () const { return dependencyCheck_; }

   void OutDir (const std::string& outDir) { outDir_ = outDir; }
   void Files (const std::vector<std::string>& files) { files_ = files; }
   void Files (std::vector<std::string>&& files) { files_ = std::move(files); }
   void DependencyCheck (bool dependencyCheck) { dependencyCheck_ = dependencyCheck; }

   void Compile ();

   const std::vector<std::string>& OutFiles () const { return outFiles_; }

private:
   std::string              uicExe_;
   std::string              outDir_;
   std::vector<std::string> files_;
   std::string              prefix_{"ui_"};
   std::vector<std::string> outFiles_;

   bool dependencyCheck_{true};

   std::string OutFile (const std::string& inFile) const;
   bool NeedsRebuild (const std::string& inFile, const std::string& outFile) const;
};

