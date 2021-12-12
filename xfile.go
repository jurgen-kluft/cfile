package main

import (
	"github.com/jurgen-kluft/xcode"
	xfileio "github.com/jurgen-kluft/xfileio/package"
)

func main() {
	xcode.Init()
	xcode.Generate(xfileio.GetPackage())
}
