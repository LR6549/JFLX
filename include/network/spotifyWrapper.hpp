#ifndef SPOTIFYWRAPPER
#define SPOTIFYWRAPPER

#include <string>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>

using json = nlohmann::json;

inline namespace Spotify {
    class SpotifyWrapper {
    public:
        SpotifyWrapper(const std::string& clientId,
                       const std::string& clientSecret,
                       const std::string& refreshToken)
            : client_id(clientId), client_secret(clientSecret), refresh_token(refreshToken) {
            refreshAccessToken();
        }

        struct PlaybackInfo {
            std::string artist;
            std::string album;
            std::string title;
            std::string coverUrl;
            int positionMs = 0;
            int durationMs = 0;
            bool isPlaying = false;
        };

        PlaybackInfo getPlaybackInfo() {
            refreshAccessToken();
            json j = get("/v1/me/player");
            PlaybackInfo info;
            if (j.is_null() || j.find("item") == j.end()) return info;

            const auto& item = j["item"];
            info.title = item["name"];
            info.album = item["album"]["name"];
            info.coverUrl = item["album"]["images"][0]["url"];
            info.positionMs = j["progress_ms"];
            info.durationMs = item["duration_ms"];
            info.isPlaying = j["is_playing"];

            const auto& artists = item["artists"];
            for (size_t i = 0; i < artists.size(); ++i) {
                if (i > 0) info.artist += ", ";
                info.artist += artists[i]["name"].get<std::string>();
            }

            return info;
        }

        void play()     { refreshAccessToken(); put("/v1/me/player/play");  }
        void pause()    { refreshAccessToken(); put("/v1/me/player/pause"); }
        void next()     { refreshAccessToken(); post("/v1/me/player/next"); }
        void previous() { refreshAccessToken(); post("/v1/me/player/previous"); }

    private:
        std::string client_id;
        std::string client_secret;
        std::string refresh_token;
        std::string access_token;

        static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }

        void refreshAccessToken() {
            CURL* curl = curl_easy_init();
            std::string readBuffer;

            if (curl) {
                std::string credentials = client_id + ":" + client_secret;
                std::string encodedCredentials = base64_encode(credentials);

                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, ("Authorization: Basic " + encodedCredentials).c_str());
                headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

                std::string postFields = "grant_type=refresh_token&refresh_token=" + refresh_token;

                curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }

            json j = json::parse(readBuffer, nullptr, false);
            if (j.contains("access_token")) {
                access_token = j["access_token"];
            } else {
                std::cerr << "Failed to refresh access token:\n" << readBuffer << std::endl;
            }
        }

        json get(const std::string& endpoint) {
            CURL* curl = curl_easy_init();
            std::string readBuffer;
            if (curl) {
                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
                curl_easy_setopt(curl, CURLOPT_URL, ("https://api.spotify.com" + endpoint).c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
            return json::parse(readBuffer, nullptr, false);
        }

        void put(const std::string& endpoint) {
            CURL* curl = curl_easy_init();
            if (curl) {
                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
                headers = curl_slist_append(headers, "Content-Length: 0");
                curl_easy_setopt(curl, CURLOPT_URL, ("https://api.spotify.com" + endpoint).c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
        }

        void post(const std::string& endpoint) {
            CURL* curl = curl_easy_init();
            if (curl) {
                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
                headers = curl_slist_append(headers, "Content-Length: 0");
                curl_easy_setopt(curl, CURLOPT_URL, ("https://api.spotify.com" + endpoint).c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
        }

        static std::string base64_encode(const std::string& in) {
            static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string out;
            int val = 0, valb = -6;
            for (uint8_t c : in) {
                val = (val << 8) + c;
                valb += 8;
                while (valb >= 0) {
                    out.push_back(b[(val >> valb) & 0x3F]);
                    valb -= 6;
                }
            }
            if (valb > -6) out.push_back(b[((val << 8) >> (valb + 8)) & 0x3F]);
            while (out.size() % 4) out.push_back('=');
            return out;
        }
    };
}

#endif