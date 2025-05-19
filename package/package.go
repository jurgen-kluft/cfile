package cfile

import (
	"github.com/jurgen-kluft/ccode/denv"
	ccore "github.com/jurgen-kluft/ccore/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'cfile'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := cunittest.GetPackage()
	ccorepkg := ccore.GetPackage()

	// The main (cfile) package
	mainpkg := denv.NewPackage("cfile")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(ccorepkg)

	// 'cfile' library
	mainlib := denv.SetupCppLibProject("cfile", "github.com\\jurgen-kluft\\cfile")
	mainlib.AddDependencies(ccorepkg.GetMainLib()...)

	// 'cfile' unittest project
	maintest := denv.SetupDefaultCppTestProject("cfile_test", "github.com\\jurgen-kluft\\cfile")
	maintest.AddDependencies(unittestpkg.GetMainLib()...)
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
