//
// Created by darksider3 on 28.01.21.
//

#ifndef POCO_FILE_HASHER_DIGESTENCRYPTAPP_HPP
#define POCO_FILE_HASHER_DIGESTENCRYPTAPP_HPP

// main logic
#include "Poco/File.h"
#include "Poco/RecursiveDirectoryIterator.h"
#include "Print/Adapters/FileHashPrinter.hpp"
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
     * @brief CRTP-Class for the hash methods
     *
     * @tparam T Class to attach to
     */
    template<typename T>
    struct m_HashMethod {

        /**
         * @brief Set CRTP-var `method`.
         *
         * @param const std::string& n String to set
         */
        void setMethod(const std::string& n) { static_cast<T*>(this)->method = n; }

        /**
         * @brief get CRTPs-var `method` contents
         *
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

    using DigestVariant = std::variant<_sha1, _sha256, _md5, _ownName>;

    /**
     * @brief Get's called by `this->run()` by POCO itself
     *
     * @param self This exact application object.
     */
    [[maybe_unused]] void initialize(Application& self) override
    {
        loadConfiguration(); // load config files if present
        Application::initialize(self);

        return;
    }

    /**
     * @brief uninitialized this object. Called by `this->run()` on exit
     *
     */
    [[maybe_unused]] void uninitialize() override
    {
        Application::uninitialize();

        return;
    }

    /**
     * @brief Reinitialises the current application object. Called by `this->run()`.
     *
     * @param self This exact Application object.
     */
    [[maybe_unused]] void reinitialize(Application& self) override
    {
        Application::reinitialize(self);

        return;
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
            Option()
                .fullName("sha1")
                .description("SHA1 instead of MD5")
                .repeatable(false)
                .required(false)
                .noArgument());

        opts.addOption(
            Option()
                .fullName("sha256")
                .description("use SHA256 instead of MD5")
                .repeatable(false)
                .required(false)
                .noArgument());

        opts.addOption(
            Option()
                .fullName("md5")
                .description("use md5(default)")
                .repeatable(false)
                .required(false)
                .noArgument());

        opts.addOption(
            Option()
                .fullName("own")
                .description("Select your own hashing method your system supports through OpenSSL. Note: combine with -c in case you need multiple hashes for the same file.")
                .required(false)
                .repeatable(true)
                .argument("digestname")
                .callback(OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleDefine)));

        opts.addOption(
            Option()
                .fullName("cmpr")
                .description("Compare two hashes. Note: Specify twice. e.g. -c $hash1 -c $hash2! Currently legal only when supplying your own hashes!")
                .required(false)
                .repeatable(true)
                .argument("hash1 --cmpr=hash2")
                .callback(OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleCmpr)));

        opts.addOption(
            Option("file", "f", "Write hashes to file instead of standard out.")
                .repeatable(false)
                .required(false)
                .argument("name", true));

        opts.addOption(
            Option("print", "p", "Print used hash alongside the files name. Format: '$hash  $filename; $HashMethod.")
                .required(false)
                .repeatable(false)
                .noArgument());

        opts.addOption(
            Option("combined-output", "c", "Print multiple, given, Digests combined for each given file. If not specified, the last given(or md5, as default, when nothing supplied) hash name will be used.")
                .required(false)
                .repeatable(false)
                .noArgument()
                .callback(OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleCombined)));

        opts.addOption(
            Option("recursive", "R", "Recursivly scan through directories instead of manual files")
                .required(false)
                .repeatable(false)
                .noArgument()
                .callback(OptionCallback<DigestEncryptApp>(this, &DigestEncryptApp::handleRecursive)));

#ifndef NO_HASH_LISTINGS
        opts.addOption(
            Option("list-digests", "l", "Print supported hashes/digest algorithm by OpenSSL")
                .repeatable(false)
                .required(false)
                .noArgument());
#endif

        return;
    }

    /**
     * @brief Input-Digest-Handling
     *
     * @param DigestVariant&& dig  Moving Object into vector
     *
     */
    void inputDigest(DigestVariant&& dig)
    {
        m_RequestedDigests.push_back(std::move(dig));
        return;
    }

    /**
     * @brief -c, --combined-output Handler. Set the Flag!
     *
     */
    void handleCombined(const std::string&, const std::string&)
    {
        m_Flags._combinedOutput = true;
        return;
    }

    void handleRecursive(const std::string&, const std::string&)
    {
        m_Flags._recursive = true;
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
        m_Flags._needsHelp = true;
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

        return;
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
        if (m_CmprHashes.First.empty()) {
            m_CmprHashes.First = value;
        } else if (m_CmprHashes.Second.empty()) {
            m_CmprHashes.Second = value;
            m_CmprHashes.CmprResult = Formatting::CompareHash(m_CmprHashes.First, m_CmprHashes.Second);
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
        } else {
            name = def;
        }
        config().setString(name, value);

        return;
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
            m_Flags._needsHelp = true;
        } else if (name == "sha1") {
            inputDigest(_sha1 {});
        } else if (name == "sha256") {
            inputDigest(_sha256 {});
        } else if (name == "md5") {
            inputDigest(_md5 {});
        } else if (name == "own") {
            inputDigest(_ownName { value });
        } else if (name == "file") {
            if (value.empty())
                return;
            m_file = Poco::File { value };
            if (!m_file.exists())
                m_file.createFile();

            assert(m_file.exists() && "Somehow we fucked up to create the file and got here!");
        } else if (name == "print") {
            m_Flags._printAlgoUsed = true;
        }
#ifndef NO_HASH_LISTINGS
        else if (name == "list-digests") {
            m_Flags._listAvailable = true;
        }
#endif
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
        if (!m_CmprHashes.First.empty() || !m_CmprHashes.Second.empty()) {
            return m_CmprHashes.CmprResult;
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
        if (m_Flags._listAvailable) {
            list_avail();
            return 0;
        }
#endif

        for (auto& anonymous_args : args) {
            m_fileVec.push_back(anonymous_args);
        }

        cleanup_vec(m_fileVec);
        if (m_RequestedDigests.empty())
            inputDigest(_md5 {});

        if (m_Flags._needsHelp) {
            // It's not an error when user requests help
            displayHelp(Application::EXIT_OK);
            ASSERT_NOT_REACHED();
        }

        if (m_fileVec.empty() || args.empty()) {
            // it is indeed an usage error when user doesn't supply anything at all
            displayHelp(Application::EXIT_USAGE);
            ASSERT_NOT_REACHED();
        }

        // write either to specified file or std::cout
        if (!m_file.path().empty() && !m_file.exists()) {
            m_of.rdbuf()->close();
            m_of.open(m_file.path(), std::ios::in);
            m_sbuf = m_of.rdbuf();
        } else {
            m_sbuf = std::cout.rdbuf();
        }

        assert(m_of.good() && "File isn't good!");

        std::ostream output(m_sbuf);
        assert(output.good() && "This stream has to stay valid.");

        Poco::File tmpF { m_fileVec.back() };
        std::vector<unsigned char> tmpVec {};

        Formatting::PrintFormat Formatter { tmpVec, tmpF, "", "" };

        auto digestSelect = [&](DigestVariant& dig, const std::string& path) {
            if (auto* detectedMD5 = std::get_if<_md5>(&dig); detectedMD5 != nullptr) {
                printFileHashAdapter(path, detectedMD5->getMethod(), Formatter, m_Flags._printAlgoUsed, output);
            } else if (auto* detectedSHA1 = std::get_if<_sha1>(&dig); detectedSHA1 != nullptr) {
                printFileHashAdapter(path, detectedSHA1->getMethod(), Formatter, m_Flags._printAlgoUsed, output);
            } else if (auto* detectedSHA256 = std::get_if<_sha256>(&dig); detectedSHA256 != nullptr) {
                printFileHashAdapter(path, detectedSHA256->getMethod(), Formatter, m_Flags._printAlgoUsed, output);
            } else if (auto* detectOwnMethod = std::get_if<_ownName>(&dig); detectOwnMethod != nullptr) {
                printFileHashAdapter(path, detectOwnMethod->getMethod(), Formatter, m_Flags._printAlgoUsed, output);
            }
        };

        /**
         * @brief Combined ouitput by file instead of digest
         */
        auto combined_out_by_file = [&]() {
            for (auto& path : m_fileVec) {
                for (auto& buck : m_RequestedDigests) {
                    digestSelect(buck, path);
                }
            }
        };

        /**
         * @brief Combined output by digest instead of file
         */
        [[maybe_unused]] auto combined_out_by_digest = [&]() {
            for (auto& buck : m_RequestedDigests) {
                for (auto& path : m_fileVec) {
                    digestSelect(buck, path);
                }
            }
        };

        /**
         * @brief Prints just the last given file, nothing else
         */
        auto singleOut = [&]() {
            auto Element = m_RequestedDigests.back(); // just the last file given, always.
            for (auto& path : m_fileVec) {
                digestSelect(Element, path);
            }
        };

        /**
         * @brief Recursive print of a combined output
         */
        [[flatten]] auto recursiveComined = [&digestSelect](decltype(m_fileVec)& Vector, decltype(m_RequestedDigests)& Digests) {
            for (auto& path : Vector) {
                try {
                    auto end = Poco::RecursiveDirectoryIterator {};
                    auto it = Poco::RecursiveDirectoryIterator(path);
                    for (; it != Poco::RecursiveDirectoryIterator {}; ++it) {
                        if (it->isFile()) {
                            for (auto& holyDigest : Digests) {
                                digestSelect(holyDigest, it->path());
                            }
                        }
                    }
                } catch (std::exception& e) {
                    std::cerr << e.what() << "\n";
                    std::exit(0);
                }
            }
        };

        if (!m_Flags._recursive) {
            if (!m_Flags._combinedOutput) {
                singleOut();
            } else {
                combined_out_by_file();
            }
        } else {
            recursiveComined(m_fileVec, m_RequestedDigests);
        }

        if (m_of.is_open())
            m_of.close();
        m_sbuf = std::cout.rdbuf();

        return Application::EXIT_OK;
    };

private:
    /// SFINAE variant to select correct algorithm
    std::vector<DigestVariant> m_RequestedDigests {};

    /// Files given by Argument/Options(-f)
    std::vector<std::string> m_fileVec {};

    struct BoolFlags {
        /// -h, --help Flag
        bool _needsHelp { false };

        /// -p, --print Flag
        bool _printAlgoUsed { false };

        /// -l, --list-digests Flag
        bool _listAvailable { false };

        /// -c, --combined-output
        bool _combinedOutput { false };

        /// -R, --recursive
        bool _recursive { false };
    } m_Flags {};

    /// Output File
    Poco::File m_file {};

    /// Streambuffer to redirect files to(e.g. `std::cout` or some file on the disk)
    std::streambuf* m_sbuf {};

    /// Open File(or Output Device(e.g. `std::cout`))
    std::ofstream m_of;

    struct CmprStruct {
        std::string First {};
        std::string Second {};
        bool CmprResult = false;
    } m_CmprHashes;
};
#endif //POCO_FILE_HASHER_DIGESTENCRYPTAPP_HPP
