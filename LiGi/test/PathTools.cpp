#include "FileTools.h"
#include "GeneralTools.h"
#include "Test.h"

#include <iostream>

bool test_absolutise(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "fs::absolutise";
    tcase->descr = "Are we gonna absolutise the given path?";

    std::string teststr = "./world.cpp";
    std::string resultstr = Li::GeneralTools::fs::absolutise(teststr);

    if (teststr == resultstr) {
        tcase->error = 1;
        returner = false;
        tcase->errorDesc = resultstr;
    }
    return returner;
}

bool test_file_exists(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "file_exists";
    tcase->descr = "Do we actually detect existence?";
    if (!Li::GeneralTools::fs::file_exists("/etc/passwd")) {
        tcase->error = 1;
        returner = false;
        tcase->errorDesc = "/etc/passwd not present! :o";
    }

    if (Li::GeneralTools::fs::file_exists("humbug")) {
        tcase->error = 1;
        returner = false;
        tcase->errorDesc = "The file 'humbug' really shouldnt exist";
    }
    return returner;
}

bool test_fp_or_error(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "fp_or_error";
    tcase->descr = "Test filepointer and errno coverage";

    auto f = Li::GeneralTools::fs::error_or_fp("./pathtools_test");
    if (!f.first.first) {
        tcase->error = 1;
        returner = false;
        tcase->errorDesc = "Somehow we couldn't open this exact binary and write to it despite building it!";
        if (f.second != nullptr) {
            tcase->errorDesc += "\nBut somehow we aquired a somewhat valid file pointer?!";
            tcase->error = 2;
        }

        tcase->errorDesc += "Error message: " + std::string(f.first.second);
    }

    f = Li::GeneralTools::fs::error_or_fp("/etc/pa/sodkasl/dk");
    if (f.first.first) {
        tcase->error = 1;
        tcase->errorDesc = "This file shouldn't exist ANYWHERE on this planet. Means, the function is misguided...";
        returner = false;
    }

    return returner;
}

bool testPathLib(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "Test Path Lib";
    tcase->descr = "Testing various functions in the Path lib";

    auto F = Li::GeneralTools::fs::Path("/non_existant/path");
    if ((std::string)F != "/non_existant/path") {
        tcase->errorDesc += (std::string)F + "\n";
        tcase->error = 1;
        returner = false;
    }

    if (F.exists()) {
        tcase->errorDesc += "F is somehow valid albeit it really should not!\n";
        tcase->error = 1;
        returner = false;
    }

    F = Li::GeneralTools::fs::Path("/usr/bin");
    if (!F.exists()) {
        tcase->errorDesc += "/usr/bin isn't existant? It MUST be.\n";
        tcase->error = 2;
        returner = false;
    }

    return returner;
}

int main()
{
    Li::Test* tester = Li::Test::instance();

    std::shared_ptr<Li::TestCase> TestFileExists = std::make_shared<Li::TestCase>();
    std::shared_ptr<Li::TestCase> TestAbsolutise = std::make_shared<Li::TestCase>();
    std::shared_ptr<Li::TestCase> TestFPOrError = std::make_shared<Li::TestCase>();
    std::shared_ptr<Li::TestCase> TestPathLib = std::make_shared<Li::TestCase>();

    TestFileExists->func = *test_file_exists;
    TestFPOrError->func = *test_fp_or_error;
    TestAbsolutise->func = *test_absolutise;
    TestPathLib->func = *testPathLib;

    tester->append(TestFileExists.get());
    tester->append(TestFPOrError.get());
    tester->append(TestAbsolutise.get());
    tester->append(TestPathLib.get());
    tester->exec();

    Li::GeneralTools::fs::Path rund("/etc/./world/has/gone/over/lol.cpp/../");
    std::cout << "\n-----\n get() -> " << rund.get() << "\n------\n";

    std::cout << "\nParent: " << Li::GeneralTools::strip(rund.Parent(), '/') << "\n";
    std::cout << tester->errors();
    return 0;
}
