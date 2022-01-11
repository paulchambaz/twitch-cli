#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <pwd.h>
#include "config.h"

#define MAX_ANSWER 256
#define KW_LIVE "live_user"
#define KW_EXIST "www.twitch.tv/"

#define RESET "\033[0;0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define GREY "\033[0;37m"
#define BOLD "\033[1m"
#define UNDER "\033[4m"


static size_t WriteMemoryCallback (void *content, size_t size, size_t nmemb, void *userp);
void FetchStreamerData (int count, char *streamer[], char *status);

struct MemoryStruct {
  char *memory;
  size_t size;
};

int main (int argc, char *argv[]) {
  int count = sizeof(streamers) / sizeof(char *);
  // now that we have a list of all streamers, we use curl to get the html of the page
  // we will then search some special characters in the html to determine if the user is live
  char *status = (char *) malloc(sizeof(char) * count);
  FetchStreamerData(count, streamers, status);
  if (argc == 2 && !strcmp(argv[1], "-d")) {
    while (1) {
      char *new_status = (char *) malloc(sizeof(char) * count);
      FetchStreamerData(count, streamers, new_status);
      for (int i = 0; i != count; ++i) {
        if (status[i] != 0 &&  new_status[i] == 0) {
          char *command = "notify-send \"";
          char *message = " is live\"";
          char notification[strlen(command) + strlen(streamers[i]) + strlen(message)]; 
          // build notification
          strcpy(notification, command);
          strcat(notification, streamers[i]);
          strcat(notification, message);
          // send it
          system(notification);
        }
      }
      sleep(60);
    }
  } else {
    // we print the result of the curl to the user
    printf("%sYour twitch channels : %s\n", BOLD, RESET);
    for (int i = 0; i != count; i++) {
      if (status[i] == 0) {
        printf(" %d - %s : %slive%s\n", i + 1, streamers[i], GREEN, RESET);
      } else if (status[i] == 1) {
        printf(" %d - %s : %soffline%s\n", i + 1, streamers[i], GREY, RESET);
      } else {
        printf(" %d - %s : %serror%s\n", i + 1, streamers[i], RED, RESET);
      }
    }
    printf(" %d - Exit\n", count + 1);
    // we setup a response
    int answer = -1;
    char str_answer[MAX_ANSWER];
    do {
      // if the user has chosen an out of range option, or a streamer who is not live, we print an error.
      if (answer != -1 && answer <= 0 || answer > count + 1 || status[answer - 1] != 0) {
        printf("Error. ");
      }
      printf("What do you want to do (1-%s%d%s)? : ", UNDER, count + 1, RESET);
      fgets(str_answer, MAX_ANSWER, stdin);
      str_answer[strlen(str_answer) - 1] = '\0';
      if (strlen(str_answer) == 0) {
        // we autoexit if the user has chosen nothing (default exit)
        exit(EXIT_SUCCESS);
      }
      answer = atoi(str_answer);
      if (answer == count + 1) {
        // we also exit if the user has chosen the exit option
        exit(EXIT_SUCCESS);
      }
    } while (answer <= 0 || answer > count + 1 || status[answer - 1] != 0);
    printf("Starting stream...\n");
    // we construct the string for the command with mpv
    char *start = "mpv --force-seekable=yes --speed=1 --really-quiet \"https://www.twitch.tv/";
    char *out = (char *) malloc(sizeof(char) * (strlen(start) + strlen(streamers[answer - 1]) + 1));
    for (int i = 0; i != strlen(start); ++i) {
      out[i] = start[i];
    }
    for (int i = 0; i != strlen(streamers[answer - 1]); ++i) {
      out[i + strlen(start)] = streamers[answer - 1][i];
    }
    out[strlen(start) + strlen(streamers[answer - 1])] = '"';
    out[strlen(start) + strlen(streamers[answer - 1]) + 1] = '\0';
    // then run the command
    system(out);
    free(out);
  }
}

void FetchStreamerData (int count, char *streamer[], char *status) {
  CURL *curl_handle;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = (char *) malloc(1);
  chunk.size = 0;
  char *address = "https://www.twitch.tv/";
  for (int i = 0; i != count; ++i) {
    // default settings for a curl request
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    // we build the url of the curl request manually
    char *url = (char *) malloc(sizeof(char) * (strlen(address) + strlen(streamers[i])));
    for (int j = 0; j != strlen(address); ++j) {
      url[j] = address[j];
    }
    for (int j = 0; j != strlen(streamers[i]); ++j) {
      url[j + strlen(address)] = streamers[i][j];
    }
    url[strlen(address) + strlen(streamers[i])] = '\0';
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    free(url);
    // we throw a call to WriteMemoryCallback, the function is defined below
    // it will then be stored on our chunk structure
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    //we finally perform the request and deal with the result
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
      status[i] = 2;
      free(chunk.memory);
    } else {
      char *output = chunk.memory;
      // we search in the html of the page for the pattern
      if (strstr(output, KW_LIVE)) {
        status[i] = 0;
      } else if (strstr(output, KW_EXIST)) {
        status[i] = 1;
      } else {
        status[i] = 2;
      }
      free(chunk.memory);
    }
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;
  }
  free(chunk.memory);
}

/* this function returns the string content of a website to a memorystruct */
static size_t WriteMemoryCallback (void *content, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *) userp;
  char *ptr = (char *) realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    printf("Error, no memory available!");
    exit(EXIT_FAILURE);
  }
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), content, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}
