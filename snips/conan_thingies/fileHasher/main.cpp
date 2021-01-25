// main logic
#include "FileHashPrinter.hpp"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/File.h"
#include "common.hpp"

// Application interface
#include "Poco/Util/Application.h"
// Options
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

// Output Help Formatting
#include "Poco/Util/HelpFormatter.h"

// Output and helpers
#include <fstream>
#include <iostream>
#include <variant>
#include <vector>

// Listing functionality
#ifndef NO_HASH_LISTINGS
#    include <openssl/evp.h>
#    include <openssl/objects.h>

/**
 * @brief Print out given OBJ_NAME.
 *
 * @param const OBJ_NAME* obj
 */
void ssl_callback_print(const OBJ_NAME* obj, void* /*unused*/)
{
    std::cout << obj->name << " ";
}

/**
 * @brief Registers a callback to OpenSSL for listing current installed & enabled hashing/digest algorithms supported.
 */
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
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
}

// main app!
class DigestEncryptApp : public Application {
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
                .required(false)
                .repeatable(false)
                .callback(Poco::Util::OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleHelp)));

        opts.addOption(
            Option("sha1", "s", "use SHA1 instead of MD5")
                .repeatable(false)
                .required(false)
                .noArgument());
        opts.addOption(
            Option("sha256", "6", "use SHA256 instead of MD5")
                .repeatable(false)
                .required(false)
                .noArgument());
        opts.addOption(
            Option("md5", "5", "use md5(default)")
                .repeatable(false)
                .required(false)
                .noArgument());
        opts.addOption(
            Option("file", "f", "Write to file instead of stdout")
                .repeatable(false)
                .required(false)
                .argument("name", true));
        opts.addOption(
            Option("print", "p", "Print used hash alongside the files name.")
                .required(false)
                .repeatable(false)
                .noArgument());
        opts.addOption(
            Option("own", "o", "Select your own hashing method your system supports through OpenSSL.")
                .required(false)
                .repeatable(false)
                .argument("digestname")
                .callback(OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleDefine)));

        opts.addOption(
            Option("cmpr", "c", "Compare two hashes. Note: Specify twice. e.g. -c $hash1 -c $hash2!")
                .required(false)
                .repeatable(true)
                .argument("hash")
                .callback(OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleCmpr)));
#ifndef NO_HASH_LISTINGS
        opts.addOption(
            Option("list-digests", "l", "Print supported hashes/digest algorithm by OpenSSL")
                .repeatable(false)
                .required(false)
                .noArgument());
#endif
    }

    /**
     * @brief Handle given -h/--help flag.
     *
     * It specificially needs it, because we don't have to process any of the given Options anymore, if we got some. We just
     * print the help text and exit the program while cleaning up after ourselfs. Does take two unused values due to POCO
     *
     */
    void handleHelp(const std::string& /* unused */, const std::string& /* unused */)
    {
        _needsHelp = true;
        stopOptionsProcessing(); // stop's processing because we won't need any - printing is anything we can do here
        displayHelp(Application::EXIT_OK);
        ASSERT_NOT_REACHED();
    }

    /**
     * @brief Handles property in name=value style arguments, delegates to `definePropery()`.
     *
     * @param const std::string& Value to treat as a property(in name=value style).
     */
    void handleDefine(const std::string& /* unused name */, const std::string& value)
    {
        defineProperty(value);
    }

    /**
     * @brief Handle --cmpr/-c Flag. Compares two hashes against each other. **Must** be called twice, when used.
     *
     * @param const std::string& Inputhash.
     */
    void handleCmpr(const std::string& /*name*/, const std::string& value)
    {
        if (value.empty())
            displayHelp();
        if (CmprHashes.First.empty()) {
            CmprHashes.First = value;
        } else if (CmprHashes.Second.empty()) {
            CmprHashes.Second = value;
            CmprHashes.CmprResult = Formatting::CompareHash(CmprHashes.First, CmprHashes.Second);
            std::cout << "File hashes match!" << std::endl;
        } else {
            displayHelp(Application::EXIT_USAGE);
        }

        return;
    }

    /**
     * @brief Handle name=value commands!
     *
     * @param const std::string& def String formatted like name=value
     */
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
        config().setString(name, value);
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
    __attribute__((flatten)) void handleOption(const std::string& name, const std::string& value) override
    {
        Application::handleOption(name, value);
        assert(!name.empty() && "Never give this an empty arguments name!");

        if (name == "help") {
            _needsHelp = true;
        } else if (name == "sha1") {
            RequestedDigest = _sha1 {};
        } else if (name == "sha256") {
            RequestedDigest = _sha256 {};
        } else if (name == "md5") {
            RequestedDigest = _md5 {};
        } else if (name == "own") {
            RequestedDigest.emplace<_ownName>(_ownName { value });
        } else if (name == "file") {
            if (value.empty())
                return;
            _file = Poco::File { value };
            if (!_file.exists())
                _file.createFile();

            assert(_file.exists() && "Somehow we fucked up to create the file and got here!");
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
        if (!CmprHashes.First.empty() || !CmprHashes.Second.empty()) {
            return CmprHashes.CmprResult;
        }
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

        for (auto& anonymous_args : args) {
            _fileVec.push_back(anonymous_args);
        }

        cleanup_vec(_fileVec);

        if (_needsHelp) {
            // It's not an error when user requests help
            displayHelp(Application::EXIT_OK);
            ASSERT_NOT_REACHED();
        }

        if (_fileVec.empty() || args.empty()) {
            // it is indeed an usage error when user doesn't supply anything at all
            displayHelp(Application::EXIT_USAGE);
            ASSERT_NOT_REACHED();
        }

        // write either to specified file or std::cout
        if (!_file.path().empty() && _file.exists()) {
            of.rdbuf()->close();
            of.open(_file.path(), std::ios::in);
            buf = of.rdbuf();
        } else {
            buf = std::cout.rdbuf();
        }
        assert(of.good() && "File isn't good!");

        std::ostream output(buf);
        assert(output.good() && "This stream has to stay valid.");

        if (auto* detectedMD5 = std::get_if<_md5>(&RequestedDigest); detectedMD5 != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectedMD5->getMethod(), _printAlgoUsed, output);
            }
        } else if (auto* detectedSHA1 = std::get_if<_sha1>(&RequestedDigest); detectedSHA1 != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectedSHA1->getMethod(), _printAlgoUsed, output);
            }
        } else if (auto* detectedSHA256 = std::get_if<_sha256>(&RequestedDigest); detectedSHA256 != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectedSHA256->getMethod(), _printAlgoUsed, output);
            }
        } else if (auto* detectOwnMethod = std::get_if<_ownName>(&RequestedDigest); detectOwnMethod != nullptr) {
            for (auto& path : _fileVec) {
                PrintFileHash(path, detectOwnMethod->getMethod(), _printAlgoUsed, output);
            }
        }

        return Application::EXIT_OK;
    };

private:
    /**
     * @brief CRTP-Class for the hash methods
     *
     * @tparam T Class to attach to
     */
    template<typename T>
    struct m_HashMethod {
        /**
         * @brief Set CRTP-var `method`.
         * @param const std::string& n String to set
         */
        void setMethod(const std::string& n) { static_cast<T*>(this)->method = n; }

        /**
         * @brief get CRTPs-var `method` contents
         * @return std::string `method` variable
         */
        [[nodiscard]] std::string getMethod() const { return static_cast<const T*>(this)->method; }

    private:
        /**
         * @brief Datavariable
         */
        std::string method {};
    };

    /// used for SFINAE-Handling of options
    struct _sha1 : public m_HashMethod<_sha1> {
        _sha1() { this->setMethod("SHA1"); }
    };

    /// used for SFINAE-Handling of options
    struct _sha256 : public m_HashMethod<_sha256> {
        _sha256() { this->setMethod("SHA256"); }
    };

    /// used for SFINAE-Handling of options
    struct _md5 : public m_HashMethod<_md5> {
        _md5() { this->setMethod("MD5"); }
    };

    /// used for SFINAE-Handling of options
    struct _ownName : public m_HashMethod<_ownName> {
        _ownName(const std::string& name) { this->setMethod(name); }
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

    struct CmprStruct {
        std::string First {};
        std::string Second {};
        bool CmprResult = false;
    } CmprHashes;
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

    int ret = pApp->run();
    std::flush(std::cout);

    return ret;
}