#include <curl/curl.h>
#include <curl/easy.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

class Downloader {
private:
    string storage;
    string extract_file_name(const string &url) {
        int i = url.size();
        for (; i >= 0; i--) {
            if (url[i] == '/') {
                break;
            }
        }

        return url.substr(i + 1, url.size() - 1);
    }

public:
    Downloader() {
        storage = "storage/";
    }
    bool download(string url) {

        const string filename = extract_file_name(url);    
        cout << filename << endl;
        const string file_save_path = storage + filename;
        
        CURL *curl;
        FILE *fp;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            fp = fopen(file_save_path.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);
        }

        if (res == CURLE_OK)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};


int main() {
    Downloader d;
    string url = "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTWkKyHOLbYZU-yvcBbtMP9VE75QYizOjDxEQ&usqp=CAU";
    d.download(url);
}