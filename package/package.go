package xfile

import (
	"github.com/jurgen-kluft/ccode/denv"
	xbase "github.com/jurgen-kluft/xbase/package"
	xentry "github.com/jurgen-kluft/xentry/package"
	xunittest "github.com/jurgen-kluft/xunittest/package"
)

// GetPackage returns the package object of 'xfile'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := xunittest.GetPackage()
	entrypkg := xentry.GetPackage()
	xbasepkg := xbase.GetPackage()

	// The main (xfile) package
	mainpkg := denv.NewPackage("xfile")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(entrypkg)
	mainpkg.AddPackage(xbasepkg)

	// 'xfile' library
	mainlib := denv.SetupDefaultCppLibProject("xfile", "github.com\\jurgen-kluft\\xfile")
	mainlib.Dependencies = append(mainlib.Dependencies, xbasepkg.GetMainLib())

	// 'xfile' unittest project
	maintest := denv.SetupDefaultCppTestProject("xfile_test", "github.com\\jurgen-kluft\\xfile")
	maintest.Dependencies = append(maintest.Dependencies, unittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, entrypkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, xbasepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
