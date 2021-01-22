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
    [[maybe_unused]] void initialize(Application& self) override
    {
        loadConfiguration(); // load config files if present
        Application::initialize(self);
    }

    [[maybe_unused]] void uninitialize() override
    {
        Application::uninitialize();
    }

    [[maybe_unused]] void reinitialize(Application& self) override
    {
        Application::reinitialize(self);
    }

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

    void displayHelp() const
    {
        HelpFormatter HF(options());
        HF.setCommand(commandName());
        HF.setHeader("A simple file hashing application");
        HF.setUsage("[OPTIONS] file1 file2 file3...");
        HF.setFooter(version_str);
        HF.setUnixStyle(true);
        HF.format(std::cout);
    }

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
            displayHelp();
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
        return 0;
    };

private:
    // used for SFINAE-Handling of options
    struct _sha1 {
        std::string method { "SHA1" };
    };
    struct _sha256 {
        std::string method { "SHA256" };
    };
    struct _md5 {
        std::string method { "MD5" };
    };

    struct _ownName {
        std::string method { "" };
    };

    std::variant<_sha1, _sha256, _md5, _ownName> RequestedDigest { _md5 {} };
    std::vector<std::string> _fileVec {};
    bool _needsHelp { false };
    bool _printAlgoUsed { false };
    bool _listAvailable { false };

    /// file we gonna write to
    Poco::File _file {};
    /// buffer to redirect the writes to
    std::streambuf* buf {};

    /// in case we gonna redirect std::cout, this is our opened file
    std::ofstream of;
};

int main(int argc, char** argv)
{
    std::shared_ptr<DigestEncryptApp> pApp = std::make_shared<DigestEncryptApp>();
    try {
        pApp->init(argc, argv);
    } catch (Poco::Exception& e) {
        pApp->logger().log(e);
        return Application::EXIT_CONFIG;
    }

    return pApp->run();
}