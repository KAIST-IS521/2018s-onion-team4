#include "Features.hh"
#include <fstream>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <curl/curl.h>
#include <string>
#include <spawn.h>
#include <unistd.h>
#include <iostream>

using namespace std;

namespace Features {
    string Asciiart(const char *filepath);

    int execute(const char *path, char *const argv[]) {
        pid_t pid;
        int ret = posix_spawn(&pid, path, NULL, NULL, argv, NULL);

        int status;
        waitpid(pid, &status, 0);

        return ret;
    }

    size_t callbackfunction(void *ptr, size_t size,
            size_t nmemb, void* userdata) {
        ofstream *file = static_cast<ofstream *>(userdata);
        if (file->is_open()) {
            file->write((const char *)ptr, size * nmemb);
            return size * nmemb;
        }
        return 0;
    }

    bool download(string url, ofstream *file) {
        CURL* curlCtx = curl_easy_init();
        curl_easy_setopt(curlCtx, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
        curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

        CURLcode rc = curl_easy_perform(curlCtx);
        if (rc) {
            //failed to download
            return false;
        }
        long res_code = 0;
        curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
        if (!((res_code == 200 || res_code == 201) &&
            rc != CURLE_ABORTED_BY_CALLBACK)) {
            //page error
            return false;
        }
        curl_easy_cleanup(curlCtx);
        file->close();
        return true;
    }

    string DisplayAArt(string url) {
        char fname[15] = "out_XXXXXX\0";
        mkstemp(fname);
        string filepath = "/root/image/";
        filepath.append(fname).append(".jpg");
        ofstream file(filepath);

        if (file.is_open() && download(url, &file)) {
            return Asciiart(filepath.c_str());
        }
        return "";
    }

    string Asciiart(const char *filepath) {
        string path = "/root/bin/goasciiart " ;
        string result= "";
        path.append(" -p ").append(filepath).append(" -w 80");
        char buf[1024];
        FILE *fp=NULL;
        cout << path << endl;
        fp=popen((char*)path.c_str(),"r");
        if (fp) {
            while ((fgets(buf,1024,fp)) != NULL) {
                result.append(string(buf));
            }
            pclose(fp);
        }
        return result;
    }
}
