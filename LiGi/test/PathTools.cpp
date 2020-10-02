#include "FileTools.h"
#include "GeneralTools.h"
#include "Test.h"

#include <iostream>

#define ADD_TEST(suite, x)                                                  \
    std::shared_ptr<Li::TestCase> x##_t = std::make_shared<Li::TestCase>(); \
    x##_t->func = *x;                                                       \
    suite->append(std::move(x##_t.get()));

#define FAIL_TEST_IF(expr, code, descr, tcase, returner) \
    do {                                                 \
        if (!expr) {                                     \
            tcase->error = code;                         \
            tcase->errorDesc = descr;                    \
            returner = false;                            \
        }                                                \
    } while (false)

Li::Test* Global = Li::Test::instance();

bool test_absolutise(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "fs::absolutise";
    tcase->descr = "Are we gonna absolutise the given path?";

    std::string teststr = "./world.cpp";
    std::string resultstr = Li::fs::absolutise(teststr);

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
    if (!Li::fs::file_exists("/etc/passwd")) {
        tcase->error = 1;
        returner = false;
        tcase->errorDesc = "/etc/passwd not present! :o";
    }

    if (Li::fs::file_exists("humbug")) {
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

    auto f = Li::fs::error_or_fp("./pathtools_test");
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

    f = Li::fs::error_or_fp("/etc/pa/sodkasl/dk");
    if (f.first.first) {
        tcase->error = 1;
        tcase->errorDesc = "This file shouldn't exist ANYWHERE on this planet. Means, the function is misguided...";
        returner = false;
    }

    return returner;
}

bool testTestingMacros(Li::TestCase* tcase)
{
    bool returner = true;
    FAIL_TEST_IF(false, 0, "intentional fail", tcase, returner);
    return returner;
}

bool testPathLib(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "Test Path Lib";
    tcase->descr = "Testing various functions in the Path lib";

    auto F = Li::fs::Path("/non_existant/path");
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

    F = Li::fs::Path("/usr/bin");
    if (!F.exists()) {
        tcase->errorDesc += "/usr/bin isn't existant? It MUST be.\n";
        tcase->error = 2;
        returner = false;
    }

    return returner;
}

int main()
{
    ADD_TEST(Global, test_absolutise);
    ADD_TEST(Global, test_file_exists);
    ADD_TEST(Global, test_fp_or_error);
    ADD_TEST(Global, testPathLib);
    ADD_TEST(Global, testTestingMacros);

    Global->exec();

    Li::fs::Path rund("/etc/./;/world/has/gone/over/lol.cpp/../");
    std::cout << "\n-----\n get() -> " << rund.get() << "\n------\n";

    std::cout << "\nParent: " << Li::GeneralTools::strip(rund.Parent(), '/') << "\n";
    std::cout << Global->errors();
    return 0;
}
