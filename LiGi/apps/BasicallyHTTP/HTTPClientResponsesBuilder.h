//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_HTTPCLIENTRESPONSESBUILDER_H
#define LIGI_APPS_HTTPCLIENTRESPONSESBUILDER_H

#include "Responses.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <istream>
#include <string>
#include <string_view>
#include <vector>

template<typename StrT = std::string>
class HTTPClientResponseBuilder {
private:
    /**
     * @brief Trims a string(remove Spaces from the immediate beginning and end)
     * @param std::string_view String to trim
     * @return trimmed String
     */
    static StrT TrimField(std::string_view in)
    {
        auto begin = in.begin();
        auto end = in.end();
        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        return StrT(begin, end);
    }

    /**
     * @brief Skip whitespace in a istream
     * @param std::istream& Stream to skip in
     */
    void SkipWhitespace(std::istream& in)
    {
        while (std::isspace(in.peek())) {
            in.ignore(1);
        }
    }

    /**
     * @brief Read HTTP Mehtod and Version from a stream, and return a accordingly filled HTTPClientResponse
     * @note This always should be given a "untouched" stream, e.g. always a stream set at position 0
     * @param std::istream& Inputstream to process
     * @return HTTPClientResponse Empty on failure, filled on success.
     */
    static HTTPClientResponse ReadHTTPMethodAndVersion(std::istream& in)
    {
        HTTPClientResponse response;
        std::string line;
        std::string tmp;
        if (in.tellg() > 0) {
            return {}; // @TODO: ERROR! Shall not pass in
            // some stream in useless state
            // because HTTP headers start in the first char...
        }

        // ORDER is important here! Ideally, we should be past
        // the HTTP verb(GET, POST, PATCH, etc) after this call
        // so dont mess up order!

        std::getline(in, line);
        if (line.empty()) {
            return {}; // @TODO: ERROR! Line MUST have words!
        }

        // Step 1: get the method
        auto begin = line.begin();
        auto end = line.end();
        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });
        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        if (end == begin) {
            // matching error
        } else {
            // here we should have the method - store it!
            response.Method = std::string(begin, end);
        }

        // Step 2: get Destination URL

        // we recycle the begin as our end because it already points to the space behind the verb

        begin = end;
        end = line.end();

        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        if (end == begin) {
            // @TODO: ERROR! somehow we read 0 size!
        } else {
            /*
			 * Because we already know where the URL begins, we just have to search for its end -
			 * which is also the beginning of the Query portion.
			 * So
			 * begin+end == whole URL
			 * begin+URI_end = /path/to/whatever
			 * URI_end+end = ?thisWhole=Section&of=Fluff
			*/
            auto URI_end = std::find_if(begin, end, [](char cur) -> bool {
                return cur == '?';
            });

            response.URI = std::string(begin, URI_end);

            // @TODO: Query parsing - not the right place to throw out the ?
            response.Query = std::string(URI_end, end);

            std::cout << "Split up URL and Query: URL: " << response.URI << ", Query: " << response.Query << "\n";
        }
        // Step 3: get HTTP-Version

        begin = end;
        end = line.end();

        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        if (end == begin) {
            // @TODO: ERROR! Still shouldn't be 0!
        } else {
            response.Version = std::string(begin, end);
        }

        return response;
    }

    /**
     * @brief Reads in HTTP fields - ideally called directly after HTTPReadVersionAndMethod - and maps them to a given StringMap
     * @param std::istream& Inputstream
     * @param Map& Map to write the fields in
     */
    static void ReadFields(std::istream& in, Map& map)
    {
        std::string line;
        std::getline(in, line);

        while (!line.empty() && line != "\r") /* In HTTP Header */ {
            auto [name, value] = Li::common::SplitPair(line, ':');
            if (name.empty() || value.empty()) {
                // @TODO: ERROR! Invalid HTTP Header Field!
            }

            name = TrimField(name);
            value = TrimField(value);

            // any field may also be just lowercase!
            std::transform(name.begin(), name.end(), name.begin(), ToLower);
            map[std::move(name)] = std::move(value);
            std::getline(in, line);
        }
    }

    /**
     * @brief Process possible HTTP body, return it as a string on success
     * @param std::istream& Inputstream to process
     * @param Map Map of strings to read relevant informations from
     * @param ssize_t max_streamlen  maximum length per body
     * @return std::string On success a string with a length greater than 0, otherwise a string with length 0.
     */
    static std::string processPossibleContent(std::istream& in, Map& map, ssize_t max_streamlen)
    {
        std::string ret {};
        if (!has<StringMap, std::string>(map, "content-length"))
            return ret; // no content to possibly read

        ssize_t len = std::stol(map.at("content-length"));

        if (len > max_streamlen) {
            return ret; // @TODO: ERROR! Size given is bigger then what we accept
            // @TODO: Shall we force the non-zero check? Actually i think the user should know that
            // when he wants to read that the content is bigger then 0...
        }
        for (ssize_t i = 0; i < len && in.good(); ++i) {
            ret += static_cast<char>(in.get());
        }

        return ret;
    }

    /**
     *
     * @return
     */
    static StringMap parsedResponseBody()
    {
        StringMap ret {};
        return ret;
    }

public:
    /**
     * @brief Parses a request
     * @param std::string_view Input to parse
     * @return A filled HTTPClientResponse. Empty when not.
     */
    HTTPClientResponse parse(std::string_view in, ClientConnection& con)
    {
        std::string constr { in };
        std::istringstream x(constr);
        auto r = ReadHTTPMethodAndVersion(x);
        ReadFields(x, r.Fields);

        std::cout << "Parsed! "
                  << "Method: " << r.Method << "\n"
                  << "URL: " << r.URI << "\n"
                  << "Version: " << r.Version << "\n"
                  << std::endl;

        for (auto& debug : r.Fields) {
            std::cout << "Field: " << debug.first << " "
                      << "Value: " << debug.second
                      << "\n";
        }

        if (r.Fields.find("content-length") != r.Fields.end()) {
            auto out = con.outResp();
            if (std::size_t len = std::stoul(r.Fields.at("content-length")); len > 0) {
                std::cout << "LEN: " << len << std::endl;
                std::vector<char> vec = std::vector<char>(max_buf_len);
                std::string possible_content {}; // @TODO: Magic number(4096)
                possible_content.reserve(len);
                size_t already_read = 0;
                auto it = std::make_move_iterator(constr.begin() + x.tellg());
                possible_content.append(it, std::make_move_iterator(constr.end()));
                already_read = (possible_content.length());
                // old method: Not looking at iterators at all
                //                while (x.good()) {
                //                    /*
                //                         * We have to cast here because just get an "int_like" type
                //                         */
                //                    possible_content += static_cast<char>(x.get());
                //
                //                    ++already_read;
                //                }
                if (!(already_read >= len)) {
                    // @TODO: REFACTOR THIS SHIT OMG
                    possible_content.append(con.ReadUntilN(vec, max_buf_len));
                    std::istringstream Content(possible_content);
                    possible_content = processPossibleContent(Content, r.Fields, len);
                }
                if (!possible_content.empty()) {
                    std::cout << "Got a file! Full return size: " << possible_content.size() << "\n";
                    std::cout << "------------------------------------------------------------------\n"
                              << possible_content
                              << std::endl;
                } else {
                }
            }
        }
        // @TODO: FIELDS PARSING
        // @TODO: BODY PARSING
        return r;
    }
};

#endif //LIGI_APPS_HTTPCLIENTRESPONSESBUILDER_H
