//
// Created by lr6549 on 18.08.26.
//

#ifndef JFLX_MANAGEMENT_SYSTEM_CURLWRAPPER_HPP
#define JFLX_MANAGEMENT_SYSTEM_CURLWRAPPER_HPP

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>

namespace cWrap {
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    inline std::string httpGet(const std::string& url) {
        CURL* curl = curl_easy_init();
        std::string response;

        if (!curl) {
            throw std::runtime_error("curl_easy_init() failed");
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            throw std::runtime_error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res));
        }

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (httpCode != 200) {
            throw std::runtime_error("HTTP-Fehler, Code: " + std::to_string(httpCode) + " Body: " + response);
        }

        return response;
    }
}
#endif //JFLX_MANAGEMENT_SYSTEM_CURLWRAPPER_HPP
