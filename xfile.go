package main

import (
	"github.com/jurgen-kluft/xcode"
	xpkg "github.com/jurgen-kluft/xfile/package"
)

func main() {
	xcode.Init()
	xcode.Generate(xpkg.GetPackage())
}
