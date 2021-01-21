// main logic
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/DigestStream.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include <fstream>

// Application interface
#include "Poco/Util/Application.h"

// Options
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

// Output Help Formatting
#include "Poco/Util/HelpFormatter.h"

#include <iostream>
#include <variant>
#include <vector>

constexpr char version_str[] = "Licensed under MIT. (c) 2021, fileHasher Version 0.2.7 by darksider3. ";

void PrintFileHash(const std::string& in, const std::string& Method, bool used_algorithm = false, std::ostream& output = std::cout)
{
    if (in.empty())
        std::cerr << "empty arg?!" << std::endl;

    Poco::File iFile { in };
    if (!iFile.exists() || !iFile.canRead()) {
        std::cerr << "File: " << in << " could not be found or read.\n";
        return;
    }

    std::unique_ptr<Poco::Crypto::DigestEngine> Engine;
    if (Method == "SHA1") {
        Engine = std::make_unique<Poco::Crypto::DigestEngine>("SHA1");
    } else if (Method == "SHA256") {
        Engine = std::make_unique<Poco::Crypto::DigestEngine>("SHA256");
    } else {
        Engine = std::make_unique<Poco::Crypto::DigestEngine>("MD5");
    }

    Poco::DigestOutputStream ds(*Engine.get());
    Poco::FileInputStream FileReadStream { iFile.path() };

    std::string read;

    FileReadStream.seekg(0, std::ios::end);
    size_t Size = FileReadStream.tellg();
    FileReadStream.seekg(0, std::ios::beg);
    read = std::string(Size, '\0');

    FileReadStream.read(&read[0], Size);

#ifdef DEBUG_DIFFER_OUTPUT
    std::ofstream debugFile;
    debugFile.write(&read[0], read.length());
    debugFile.close();
#endif
    Engine->update(read); // plug it in

    output << Poco::DigestEngine::digestToHex(Engine->digest()) << "  " << iFile.path() /* print 'em out */ << "\n";
    if (used_algorithm)
        output << "Method " << Engine->algorithm() << " -> ";
    output.flush();
    return;
}

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

        opts.addOption(Option("sha1", "s", "use SHA1 instead of MD5").repeatable(false).required(false));
        opts.addOption(Option("sha256", "6", "use SHA256 instead of MD5").repeatable(false).required(false));
        opts.addOption(Option("md5", "5", "use md5(default)").repeatable(false).required(false));
        opts.addOption(Option("file", "f", "Write to file instead of stdout").repeatable(false).required(false).argument("name", true));
        opts.addOption(Option("print", "p").required(false).repeatable(false).noArgument());
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
        } else if (name == "file") {
            if (value.empty())
                return;
            _file = Poco::File { value };
            if (!_file.exists())
                _file.createFile();
        } else if (name == "print") {
            _printAlgoUsed = true;
        } else {
            std::cout << "Option: " << name << ", Value: " << value << std::endl;
        }
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

    std::variant<_sha1, _sha256, _md5> RequestedDigest { _md5 {} };
    std::vector<std::string> _fileVec {};
    bool _needsHelp { false };
    bool _printAlgoUsed { false };

    /// file we gonna write to
    Poco::File _file {};
    /// buffer to redirect the writes to
    std::streambuf* buf;

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