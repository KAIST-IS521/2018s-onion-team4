#include "Features.hh"
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
        FILE* stream = (FILE*)userdata;
        if (!stream) {
            return 0;
        }

        return fwrite((FILE*)ptr, size, nmemb, stream);
    }

    void Downloadmusic(string urls) {
        //just wave file
        const char *filepath = "out.wav";
        FILE* fp = fopen(filepath, "wb");
        char *url = &urls[0];
        if (!fp) {
            //file open error
            return;
        }

        CURL* curlCtx = curl_easy_init();
        curl_easy_setopt(curlCtx, CURLOPT_URL, url);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
        curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

        CURLcode rc = curl_easy_perform(curlCtx);
        if (rc) {
            //failed to download
            return;
        }

        long res_code = 0;
        curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
        if (!((res_code == 200 || res_code == 201) &&
            rc != CURLE_ABORTED_BY_CALLBACK)) {
            //page error
        }

        curl_easy_cleanup(curlCtx);

        fclose(fp);
        Exemusic(filepath);
    }

    void Downloadimage(string urls) {
        const char *filepath = "out.jpg";
        FILE* fp = fopen(filepath, "wb");
        char *url=&urls[0];
        if (!fp) {
            //file open error
            return;
        }

        CURL* curlCtx = curl_easy_init();
        curl_easy_setopt(curlCtx, CURLOPT_URL, url);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
        curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

        CURLcode rc = curl_easy_perform(curlCtx);
        if (rc) {
            //failed to download
            return;
        }

        long res_code = 0;
        curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
        if (!((res_code == 200 || res_code == 201) &&
                    rc != CURLE_ABORTED_BY_CALLBACK)) {
            //page error
        }

        curl_easy_cleanup(curlCtx);

        fclose(fp);
        Asciiart(filepath);
    }


    void Exemusic(const char *filepath) {
        pid_t pid;
        int status;

        pid = fork();
        if (pid == -1) { /* Error occured, Exception required */ }
        else if(pid == 0) { // child
            execlp("aplay", "aplay", filepath, NULL); //docker
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
            execlp("image2ascii", "image2ascii","-h","60", filepath, NULL); //docker
        }
        // parent
        wait(&status);

        // Done
        // verification is required
    }

}
