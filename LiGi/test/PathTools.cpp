#include "GeneralTools.h"
#include "Test.h"

#include <iostream>

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
    return returner;
}

bool test_file_doesnt_exists(Li::TestCase* tcase)
{
    bool returner = true;
    tcase->name = "File_exists but intentionally failing";
    tcase->descr = "See name";
    if (Li::GeneralTools::fs::file_exists("humbug")) {
        tcase->error = 1;
        returner = false;
        tcase->errorDesc = "The file 'humbug' really shouldnt exist";
    }
    return returner;
}

int main()
{
    Li::Test* tester = Li::Test::instance();

    std::shared_ptr<Li::TestCase> TestFileExists = std::make_shared<Li::TestCase>();
    std::shared_ptr<Li::TestCase> TestFileNotExists = std::make_shared<Li::TestCase>();

    TestFileExists->func = *test_file_exists;
    TestFileNotExists->func = *test_file_doesnt_exists;

    tester->append(TestFileExists.get());
    tester->append(TestFileNotExists.get());

    tester->exec();

    std::cout << tester->errors();
    return 0;
}
