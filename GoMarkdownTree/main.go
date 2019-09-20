package main

import (
	"flag"
	"fmt"
	"github.com/monochromegane/go-gitignore"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"strings"
)

func MkStr(path string, name string) string {
	var MDStr string
	//path=strings.ReplaceAll(path, "_", "\\_") //Escape links
	name = strings.ReplaceAll(name, "_", " ") // strip _ off
	name = strings.TrimSuffix(name, ".md")
	var Depth = strings.Count(path, "/") // Every / means, we're one subdir higher
	for i := 0; i != Depth; i++ {
		MDStr += "  "
	}
	MDStr += "  * [" + name + "](" + path + ")\n"
	return MDStr
}

func MkStrEscapedStr(path string, name string) string {
	var MDStr string
	path = strings.ReplaceAll(path, "_", "\\_") //Escape links
	MDStr = MkStr(path, name)
	return MDStr
}

func main() {
	var includeDot bool
	var OutputFile string
	var AppendFile string
	var Dir string
	var IgnoreFile string
	var DirCounter int
	var FileCounter int
	var Stats bool
	var Escape bool

	FileCounter = 0
	DirCounter = 0
	var MDStr string
	flag.BoolVar(&includeDot, "dot", false, "True: Display display dotfiles. Default is false,")
	flag.BoolVar(&Stats, "stats", false, "True: Append Stats to output")
	flag.BoolVar(&Escape, "escape", true, "False: Dont escape pathes with _")
	flag.StringVar(&OutputFile, "o", "", "Output to given file")
	flag.StringVar(&AppendFile, "A", "", "Append to given file")
	flag.StringVar(&Dir, "d", "", "run in specified directory")
	flag.StringVar(&IgnoreFile, "i", "./.gitignore", "use specified gitignore")
	flag.Parse()
	if Dir == "" {
		Dir = "."
	}
	Gitignore, err := gitignore.NewGitIgnore(IgnoreFile)
	if err != nil {
		fmt.Println("Not using any gitignore")
	}
	if err == nil {
		_, err = ioutil.ReadFile(IgnoreFile)
		if err != nil {
			fmt.Println("Not using any gitignore")
		}
	}

	err = filepath.Walk(Dir,
		func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			// when dotfiles are excluded
			if !includeDot {
				if strings.HasPrefix(path, ".") != true && !Gitignore.Match(path, info.IsDir()) {
					if info.IsDir() {
						DirCounter++
						MDStr += MkStrEscapedStr(path, info.Name()+"/")
					} else {
						MDStr += MkStrEscapedStr(path, info.Name())
						FileCounter++
					}
				}
			} else { // in case when dotfiles are not excluded
				if info.IsDir() {
					DirCounter++
					MDStr += MkStrEscapedStr(path, info.Name()+"/")
				} else {
					MDStr += MkStrEscapedStr(path, info.Name())
					FileCounter++
				}
			}
			return nil
		})
	if err != nil {
		log.Println(err)
	}
	if Stats {
		statstr := "\n\nFiles: " + strconv.Itoa(FileCounter) + ", Directories: " + strconv.Itoa(DirCounter) + "\n"
		MDStr += statstr
	}
	if OutputFile != "" {
		err := ioutil.WriteFile(OutputFile, []byte(MDStr), 0644)
		if err != nil {
			log.Fatal(err)
		}
	} else if AppendFile != "" {
		f, err := os.OpenFile(AppendFile, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 644)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		_, err = f.WriteString(MDStr)
		if err != nil {
			log.Fatal(err)
		}
	} else {
		fmt.Println(MDStr)
	}
}
