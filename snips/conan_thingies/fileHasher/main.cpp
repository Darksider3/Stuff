// main logic
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/File.h"
#include <fstream>

// Application interface
#include "Poco/Util/Application.h"

// Options
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

// Output Help Formatting
#include "Poco/Util/HelpFormatter.h"

// Output and helpers
#include <iostream>
#include <variant>
#include <vector>

#include "FileHashPrinter.hpp"
#include "common.hpp"

// Listing functionality
#ifndef NO_HASH_LISTINGS
#    include <openssl/evp.h>
#    include <openssl/objects.h>

void ssl_callback_print(const OBJ_NAME* obj, void* /*unused*/)
{
    std::cout << obj->name << " ";
}

void list_avail()
{
    void* arg = nullptr;
    std::cout << "(Space separated)Available digests: ";
    OpenSSL_add_all_digests();
    OBJ_NAME_do_all(OBJ_NAME_TYPE_MD_METH, ssl_callback_print, arg);
    std::cout << ";"
              << LN;
    return;
}

#endif

// dont clutter global namespace
namespace {
using Poco::Util::AbstractConfiguration;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionSet;
}

// main app!
class DigestEncryptApp : public Poco::Util::Application {
public:
    DigestEncryptApp()
    {
        setUnixOptions(true);
    }

protected:
    /**
     * @brief Get's called by `this->run()` by POCO itself
     *
     * @param self This exact application object.
     */
    [[maybe_unused]] void initialize(Application& self) override
    {
        loadConfiguration(); // load config files if present
        Application::initialize(self);
    }

    /**
     * @brief uninitialized this object. Called by `this->run()` on exit
     *
     */
    [[maybe_unused]] void uninitialize() override
    {
        Application::uninitialize();
    }

    /**
     * @brief Reinitialises the current application object. Called by `this->run()`.
     *
     * @param self This exact Application object.
     */
    [[maybe_unused]] void reinitialize(Application& self) override
    {
        Application::reinitialize(self);
    }

    /**
     * @brief Defines Options this Application object takes/needs.
     *
     * @param opts OptionSet provided by the parent class(`Poco::Util::Application`)
     */
    void defineOptions(OptionSet& opts) override
    {
        Application::defineOptions(opts);
        opts.addOption(
            Option("help", "h", "display this help")
                .repeatable(false)
                .required(false));

        opts.addOption(Option("sha1", "s", "use SHA1 instead of MD5").repeatable(false).required(false).noArgument());
        opts.addOption(Option("sha256", "6", "use SHA256 instead of MD5").repeatable(false).required(false).noArgument());
        opts.addOption(Option("md5", "5", "use md5(default)").repeatable(false).required(false).noArgument());
        opts.addOption(Option("file", "f", "Write to file instead of stdout").repeatable(false).required(false).argument("name", true));
        opts.addOption(Option("print", "p").required(false).repeatable(false).noArgument());
        opts.addOption(Option("own", "o", "Select your own hashing method your system supports through OpenSSL.").required(false).repeatable(false).argument("own"));
#ifndef NO_HASH_LISTINGS
        opts.addOption(Option("list-digests", "l", "Print supported hashes/digest algorithm by OpenSSL").repeatable(false).required(false).noArgument());
#endif
    }

    /**
     * @brief View Help and exit with given return code(by default EXIT_OK)
     *
     * @param int8_t return_code=EXIT_OK Return code the application should give on exit through std::exit.
     */
    void displayHelp(int8_t return_code = EXIT_OK) const
    {
        HelpFormatter HF(options());
        HF.setCommand(commandName());
        HF.setHeader("A simple file hashing application");
        HF.setUsage("[OPTIONS] file1 file2 file3...");
        HF.setFooter(version_str);
        HF.setUnixStyle(true);
        HF.format(std::cout);

        std::exit(return_code);
    }

    /**
     * @brief Handles all possible input options/Arguments defined in `defineOptions`.
     *
     * @param std::string& name Name of the given argument
     * @param std::string& value Value of arguemnt
     */
    void handleOption(const std::string& name, const std::string& value) override
    {
        Application::handleOption(name, value);
        if (name == "help")
            _needsHelp = true;
        else if (name == "sha1") {
            RequestedDigest = _sha1 {};
        } else if (name == "sha256") {
            RequestedDigest = _sha256 {};
        } else if (name == "md5") {
            RequestedDigest = _md5 {};
        } else if (name == "own") {
            RequestedDigest = _ownName { .method = value };
        } else if (name == "file") {
            if (value.empty())
                return;
            _file = Poco::File { value };
            if (!_file.exists())
                _file.createFile();
        } else if (name == "print") {
            _printAlgoUsed = true;
        }
#ifndef NO_HASH_LISTINGS
        else if (name == "list-digests") {
            _listAvailable = true;
        }
#endif
        /*        else {
            std::cout << "Option: " << name << ", Value: " << value << std::endl;
        }*/
    }

    /**
     * @brief Main application entrypoint. Get's called by `this->run()`
     *
     * @param const ArgVec& args Arguments.
     *
     * @return int8_t return value
     */
    int main(const ArgVec& args) override
    {

        auto cleanup_vec = [](std::vector<std::string>& in) {
            in.erase(std::remove_if(in.begin(), in.end(), [](std::string& in) {
                if (in.empty())
                    return true;
                if (in == " ")
                    return true;

                return false;
            }),
                in.end());
        };
#ifndef NO_HASH_LISTINGS
        if (_listAvailable) {
            list_avail();
            return 0;
        }
#endif
        if (_needsHelp) {
            displayHelp(0);
            return 1;
        }

        for (auto& anonymous_args : args) {
            _fileVec.push_back(anonymous_args);
        }

        cleanup_vec(_fileVec);

        if (_fileVec.empty() && args.empty()) {
            displayHelp();
            return 1;
        }

        // write either to specified file or std::cout
        if (!_file.path().empty() && _file.exists()) {
            of.rdbuf()->close();
            of.open(_file.path(), std::ios::in);
            buf = of.rdbuf();
        } else {
            buf = std::cout.rdbuf();
        }

        std::ostream output(buf);

        if (auto* detectedMD5 = std::get_if<_md5>(&RequestedDigest); detectedMD5 != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectedMD5->method, _printAlgoUsed, output);
            }
        } else if (auto* detectedSHA1 = std::get_if<_sha1>(&RequestedDigest); detectedSHA1 != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectedSHA1->method, _printAlgoUsed, output);
            }
        } else if (auto* detectedSHA256 = std::get_if<_sha256>(&RequestedDigest); detectedSHA256 != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectedSHA256->method, _printAlgoUsed, output);
            }
        } else if (auto* detectOwnMethod = std::get_if<_ownName>(&RequestedDigest); detectOwnMethod != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectOwnMethod->method, _printAlgoUsed, output);
            }
        }

        std::flush(std::cout);
        return 0;
    };

private:
    struct m_HashMethod {
        [[maybe_unused]] std::string Method;
    };
    /// used for SFINAE-Handling of options
    struct _sha1 : m_HashMethod {
        std::string method { "SHA1" }; /// Method Name
    };
    /// used for SFINAE-Handling of options
    struct _sha256 : m_HashMethod {
        std::string method { "SHA256" }; /// See parent
    };
    /// used for SFINAE-Handling of options
    struct _md5 : m_HashMethod {
        std::string method { "MD5" }; /// See parent
    };

    /// used for SFINAE-Handling of options
    struct _ownName : public m_HashMethod {
        std::string method { "" }; /// See parent
    };

    /// SFINAE variant to select correct algorithm
    std::variant<_sha1, _sha256, _md5, _ownName> RequestedDigest { _md5 {} };

    /// Files given by Argument/Options(-f)
    std::vector<std::string> _fileVec {};

    /// -h, --help Flag
    bool _needsHelp { false };

    /// -p, --print Flag
    bool _printAlgoUsed { false };

    /// -l, --list-digests Flag
    bool _listAvailable { false };

    /// Output File
    Poco::File _file {};

    /// Streambuffer to redirect files to(e.g. `std::cout` or some file on the disk)
    std::streambuf* buf {};

    /// Open File(or Output Device(e.g. `std::cout`))
    std::ofstream of;
};

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
    std::ios_base::sync_with_stdio(false);
    std::shared_ptr<DigestEncryptApp> pApp = std::make_shared<DigestEncryptApp>();
    try {
        pApp->init(argc, argv);
    } catch (Poco::Exception& e) {
        pApp->logger().log(e);
        return Application::EXIT_CONFIG;
    }

    return pApp->run();
}