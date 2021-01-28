#include "DigestEncryptApp.hpp"
#include "common.hpp"

/**
 * @brief Entry Point
 *
 * @param argc Number of Arguments given by the OS
 * @param argv Arguments given by the OS
 *
 * @return Return Code
 */
int main(int argc, char** argv)
{
    // disable(set to True) if we should really deallocate **everything** on exit.
    std::ios_base::sync_with_stdio(false);
    std::shared_ptr<DigestEncryptApp> pApp = std::make_shared<DigestEncryptApp>();
    try {
        pApp->init(argc, argv);
    } catch (Poco::Exception& e) {
        pApp->logger().log(e);
        return Application::EXIT_CONFIG;
    }

    int ret = pApp->run();
    std::flush(std::cout);

    pApp.reset();
    return ret;
}