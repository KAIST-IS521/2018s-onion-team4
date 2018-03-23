#include "Features.hh"
#include <fstream>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <curl/curl.h>
#include <unistd.h>

namespace Features {
    void Exemusic(const char *filepath);
    void Asciiart(const char *filepath);

    size_t callbackfunction(void *ptr, size_t size,
            size_t nmemb, void* userdata) {
        ofstream *file = static_cast<ofstream *>(userdata);
        if (file->is_open()) {
            file->write((const char *)ptr, size * nmemb);
            return size *nmemb;
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

    void Downloadmusic(string id, string url) {
        //just wave file
        string filepath = "/tmp/";
        filepath.append(id).append("_out.wav");
        ofstream file(filepath);
        if (file.is_open() && download(url, &file)) {
            Exemusic(filepath.c_str());
        }
    }

    void Downloadimage(string id, string url) {
        string filepath = "/tmp/";
        filepath.append(id).append("_out.jpg");
        ofstream file(filepath);

        if (file.is_open() && download(url, &file)) {
            Asciiart(filepath.c_str());
        }
    }

    void Exemusic(const char *filepath) {
        pid_t pid;
        int status;

        pid = fork();
        if (pid == -1) { /* Error occured, Exception required */ }
        else if(pid == 0) { // child
            //ADD A VOICE PROGRAM
        }
        // parent
        wait(&status);
    }

    void Asciiart(const char *filepath) {
        pid_t pid;
        int status;

        pid = fork();
        if (pid == -1) { /* Error occured, Exception required */ }
        else if(pid == 0) { // child
            //execlp("image2ascii", "image2ascii","-h","60", filepath, NULL); //docker
            const char *path="/bin/image2ascii" ;
            char *const argv_A[] = {"image2ascii","-h","60",filepath,NULL};
            execute(path, argv_A);
        }
        // parent
        wait(&status);

        // Done
        // verification is required
    }
    int execute(const char *path, char *const argv[]) {
        pid_t pid;
        int ret = posix_spawn(&pid, path, NULL, NULL, argv, NULL);

        int status;
        waitpid(pid, &status, 0);

        return ret;
    }

}
