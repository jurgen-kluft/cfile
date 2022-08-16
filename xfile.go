package main

import (
	xpkg "github.com/jurgen-kluft/xfile/package"
)

func main() {
	xcode.Init()
	xcode.Generate(xpkg.GetPackage())
}
