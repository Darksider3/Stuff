// main logic
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/DigestStream.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/MD5Engine.h"
#include "Poco/SHA1Engine.h"

// Application interface
#include "Poco/Util/Application.h"

// Options
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

// Output Help Formatting
#include "Poco/Util/HelpFormatter.h"

#include <iostream>
#include <variant>
#include <vector>

/*class SHA256Engine : public Poco::Crypto::DigestEngine {
public:
    enum {
        BLOCK_SIZE = 64,
        DIGEST_SIZE = 32
    };

    SHA256Engine()
        : Poco::Crypto::DigestEngine("SHA256")
    {
    }
};*/

void PrintMD5(const std::string& in, const std::string& Method)
{
    Poco::File iFile { in };
    std::unique_ptr<Poco::Crypto::DigestEngine> Engine;
    if (Method == "sha1") {
        Engine = std::make_unique<Poco::Crypto::DigestEngine>("SHA1");
    } else if (Method == "sha256") {
        Engine = std::make_unique<Poco::Crypto::DigestEngine>("SHA256");
    } else {
        Engine = std::make_unique<Poco::Crypto::DigestEngine>("MD5");
    }
    Poco::DigestOutputStream ds(*Engine.get());
    if (!iFile.exists() || !iFile.canRead()) {
        std::cout << "File: " << in << " could not be found or read.\n";
    }

    Poco::FileInputStream fiS { iFile.path() };

    std::string read;
    while (fiS.good())
        fiS >> read;
    Engine->update(read);
    std::cout << iFile.path() << ": " << Poco::DigestEngine::digestToHex(Engine->digest()) << "\n";

    return;
}

using Poco::Util::AbstractConfiguration;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionSet;

class DigestEncryptApp : public Poco::Util::Application {
public:
    DigestEncryptApp()
    {
        setUnixOptions(true);
    }

protected:
    void initialize(Application& self) override
    {
        loadConfiguration(); // load config files if present
        Application::initialize(self);
    }

    void uninitialize() override
    {
        Application::uninitialize();
    }

    void reinitialize(Application& self) override
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

        opts.addOption(Option("sha1", "s", "use SHA1 instead of MD5").repeatable(false).required(false).argument("sha1"));
        opts.addOption(Option("sha256", "6", "use SHA256 instead of MD5").repeatable(false).required(false).argument("sha256"));
        opts.addOption(Option("md5", "5", "use md5(default)").repeatable(false).required(false).argument("md5"));
        opts.addOption(Option("file", "f", "Path to file(s) to be encrypted. Repeatable.").repeatable(true).required(false).argument("file"));
    }

    void displayHelp() const
    {
        HelpFormatter HF(options());
        HF.setCommand(commandName());
        HF.setHeader("A simple file hashing application");
        HF.setUsage("-f file [OPTIONS]");
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
        } else if (name == "file") {
            _fileVec.push_back(value);
        }
    }

    void defineProperty(const std::string& def)
    {

        std::string name;
        std::string value;

        std::string::size_type pos = def.find('=');
        if (pos != std::string::npos) {
            name.assign(def, 0, pos);
            value.assign(def, pos + 1, def.length() - pos);
        } else
            name = def;

        if (options().hasOption(name))
            std::cout << ("Unexpected option argument:");
        else
            config().setString(name, value);
    }

    int main(const ArgVec& args) override
    {
        if (_needsHelp) {
            displayHelp();
            return 1;
        }

        for (auto& anonymous_args : args) {
            _fileVec.push_back(anonymous_args);
        }

        if (_fileVec.empty()) {
            displayHelp();
        }

        if (auto* detectedMD5 = std::get_if<_md5>(&RequestedDigest); detectedMD5 != nullptr) {
            for (auto& path : _fileVec) {
                std::cout << path;
                PrintMD5(path, "md5");
            }
        } else if (auto* detectedSHA1 = std::get_if<_sha1>(&RequestedDigest); detectedSHA1 != nullptr) {
            for (auto& path : _fileVec) {
                PrintMD5(path, "sha1");
            }
        } else if (auto* detectedSHA256 = std::get_if<_sha256>(&RequestedDigest); detectedSHA256 != nullptr) {
            for (auto& path : _fileVec) {
                PrintMD5(path, "sha256");
            }
        }
        return 0;
    };

private:
    struct _sha1 {
    };

    struct _sha256 {
    };

    struct _md5 {
    };

    std::variant<_sha1, _sha256, _md5> RequestedDigest { _md5 {} };

    std::vector<std::string> _fileVec {};
    bool _needsHelp { false };
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