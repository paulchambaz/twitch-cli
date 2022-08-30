/*
 * @file main.c
 * @author Paul Chambaz
 * @date 29 Aug 2022
 * @brief This file contains the entire code for twitch-cli including all functions to fetch twitch and print it to user
 * @license GPLv2
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <curl/curl.h>
#include <pthread.h>

#define MIN(x, y) (x < y) ? x : y

#define MAX_NUMBER_THREAD 64

/*
 * @breif Simple enum used to get the status codes from twitch
 */
typedef enum {
  STATUS_LIVE,
  STATUS_OFFLINE,
  STATUS_ERROR,
  STATUS_NULL,
} twitch_status;

/*
 * @brief Simple struct used to store the result of the curl request in memory
 */
typedef struct {
  char *content;
  size_t size;
} curl_data;

/*
 * @brief Used to send and recieve data to the threads
 */
typedef struct {
  char *streamer;
  twitch_status *status;
} thread_fetch_data;

void help_mode ( void );
void daemon_mode ( bool verbose );
void list_mode ( bool verbose );
void start_mode ( char *streamer );

void load_streamers ( const char *path, char ***streamers, size_t *number_streamers );
void copy_file_content ( FILE *file, char **str, size_t *len );
void copy_to_line ( char *str, size_t len, char ***lines, size_t *number_lines );

void start_stream ( char *streamer );
void fetch_streamer_data ( size_t count, char **streamer, twitch_status **status, bool verbose );
void *fetch_thread ( void *pargs );
static size_t write_memory_callback (void *content, size_t size, size_t nmemb, void *user_pointer);

bool is_arg ( const char *val, int argc, char *argv[] );
void exit_error ( char *message );

/*
 * @brief main function - only args are parsed here then sent to other functions
 * @param argc Number of arguments
 * @param argv Arguments
 */
int
main ( int argc, char *argv[] )
{
  bool verbose = is_arg("-v", argc, argv);
  if (is_arg("-h", argc, argv)) {
    help_mode();
  } else if (is_arg("-l", argc, argv)) {
    list_mode(verbose);
  } else if (is_arg("-d", argc, argv)) {
    daemon_mode(verbose);
  } else if (argc == 2) {
    start_mode(argv[1]);
  } else {
    help_mode();
  }
}

/*
 * @brief Prints a help message for the user
 */
void
help_mode ( void )
{
  printf("\e[1mUsage twitch-cli v2.0.0:\e[0m\n");
  printf("  twitch-cli [arg]\n");
  printf("  List of arguments:\n");
  printf("    -d         : starts twitch-cli in daemon mode.\n");
  printf("    -l         : list all available streamers to stdout.\n");
  printf("    <streamer> : start the stream of <streamer>.\n");
  printf("    -h         : prints this help message.\n");
}

/*
 * @brief Starts a daemon for twitch-cli that sends a notification every time a streamer goes live
 */
void
daemon_mode ( bool verbose )
{
  char **streamers;
  size_t number_streamers;
  twitch_status *prev_statuses;

  // constructs the file path of the list of streamers
  if (verbose)
    printf("Getting file path.\n");
  char *home_path = getenv("HOME");
  char path[256];
  snprintf(path, 255, "%s/.config/twitch-cli/twitchrc", home_path);

  // loads them into memory
  if (verbose)
    printf("Opening streamer file.\n");
  load_streamers(path, &streamers, &number_streamers);

  // sets up the statuses - this daemon registers change between statuses
  prev_statuses = (twitch_status *) malloc(sizeof(twitch_status) * number_streamers);
  if (!prev_statuses)
    exit_error("Error, not enough memory.");
  for (int i = 0; i < number_streamers; i++) {
    prev_statuses[i] = STATUS_NULL;
  }

  // starts the daemon loop
  printf("twitch-cli daemon started - press CTRL+C to stop it:\n");
  while (true) {
    // fetches the current statuses
    if (verbose)
      printf("Fetching streamer status\n");
    twitch_status *new_statuses;
    fetch_streamer_data(number_streamers, streamers, &new_statuses, verbose);

    for (int i = 0; i < number_streamers; i++) {
      // registers a streamer going live
      if (prev_statuses[i] != STATUS_LIVE && new_statuses[i] == STATUS_LIVE) {
        // creates the notification command
        char notification_cmd[256];
        snprintf(notification_cmd, 255, "notify-send \"%s is live\"", streamers[i]);
        if (verbose)
          printf("%s is live.\n", streamers[i]);

        // system calls the notification command
        system(notification_cmd);
      }
      // updates the statuses
      prev_statuses[i] = new_statuses[i];
    }

    free(new_statuses);
    sleep(60);
  }

  // this will in theory never be run - i keep it so it is theorically freed but it is useless
  free(prev_statuses);
}

/*
 * @brief Lists all live streamers
 */
void
list_mode ( bool verbose )
{
  char **streamers;
  size_t number_streamers;
  twitch_status *statuses;

  // constructs the file path of the list of streamers
  if (verbose)
    printf("Getting file path.\n");
  char *home_path = getenv("HOME");
  char path[256];
  snprintf(path, 255, "%s/.config/twitch-cli/twitchrc", home_path);

  // loads them into memory
  if (verbose)
    printf("Opening streamer file.\n");
  load_streamers(path, &streamers, &number_streamers);

  // fetches the current statuses
  if (verbose)
    printf("Fetching streamer status\n");
  fetch_streamer_data(number_streamers, streamers, &statuses, verbose);

  for (int i = 0; i < number_streamers; i++) {
    // prints the list of all live streamers
    if (statuses[i] == STATUS_LIVE)
      printf("%s\n", streamers[i]);
  }

  free(streamers);
  free(statuses);
}

/*
 * @brief Starts a stream
 */
void
start_mode ( char *streamer )
{
  printf("Starting stream...\n");
  start_stream(streamer);
}

/* @brief Simple bool function to simplify main
 * @param i The position of the argument
 * @param val The argument string to be matched against
 * @param argc Main's argc
 * @parma argv Main's argv
 * @return true if the argument exists and matches the value
 * @return false otherwise
 */
bool
is_arg ( const char *val, int argc, char *argv[] )
{
  if (argc < 2)
    return false;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], val))
      return true;
  }
  return false;
}

/*
 * @brief Loads the list of all streamers from a file
 * @param path Path of the file
 * @param streamers Address of the streamer's name list
 * @param number_streamers Address of the number of streamer
 */
void
load_streamers ( const char *path, char ***streamers, size_t *number_streamers )
{
  // opens the file
  FILE *file = fopen(path, "r");
  if (!file) {
    exit_error("Error, could not open twitchrc file.");
  }
  char *str;
  size_t len;
  copy_file_content(file, &str, &len);
  copy_to_line(str, len, streamers, number_streamers);
}

/*
 * @brief Copies the content of a file to a string
 * @param file The file to copy
 * @param str The address of the string to copy the file content to
 * @param len The address of the length of the above string
 */
void
copy_file_content ( FILE *file, char **str, size_t *len )
{
  char c;
  size_t length = 1;
  // intialize the str to the smallest possible size
  *str = (char *) malloc(length * sizeof(char));
  if (!*str)
    exit_error("Error, not enough memory.");
  *len = 0;
  while ((c = fgetc(file)) != EOF) {
    if (*len >= length) {
      // every time we are about to overflow - double the size of the string to fit more
      length *= 2;
      *str = (char *) realloc(*str, length * sizeof(char));
      if (!*str)
        exit_error("Error, not enough memory.");
    }
    // then simply copy the content of the file char by char
    (*str)[*len] = c;
    (*len)++;
  }
  // at the end do a realloc to have correct sized string
  *str = (char *) realloc(*str, *len * sizeof(char));
  if (!*str)
    exit_error("Error, not enough memory.");
}

/*
 * @breif Takes a string and formats it into a list of string seperated by new lines
 * @param str Source string to copy from
 * @param len Length of the source string
 * @param lines Address of the destination list of strings
 * @param number_lines Address of the number of above line of strings
 */
void
copy_to_line ( char *str, size_t len, char ***lines, size_t *number_lines )
{
  size_t nb_lines = 1;
  // intialize the str to the smallest possible size
  *lines = (char **) malloc(nb_lines * sizeof(char *));
  if (!*lines)
    exit_error("Error, not enough memory.");
  *number_lines = 0;
  int line_break = 0;
  for (int i = 0; i < len; i++) {
    if (*number_lines >= nb_lines) {
      // every time we are about to overflow - double the size of the string to fit more
      nb_lines *= 2;
      *lines = (char **) realloc(*lines, nb_lines * sizeof(char *));
      if (!*lines)
        exit_error("Error, not enough memory.");
    }
    if (str[i] == '\n' || str[i] == '\0') {
      // once we hit the end of a line we copy this line to memory
      (*lines)[*number_lines] = (char *) malloc((i - line_break) * sizeof(char));
      if (!(*lines)[*number_lines])
        exit_error("Error, not enough memory.");
      strncpy((*lines)[*number_lines], str + line_break, (i - line_break));
      (*lines)[*number_lines][i] = '\0';

      // increase the number of line
      (*number_lines)++;

      // then reset the pivot for the current line
      line_break = i + 1;
    }
  }
}

/*
 * @brief Starts a stream from a streamer's name
 * @param streamer The streamer's name
 */
void
start_stream ( char *streamer )
{
  char mpv[256];
  snprintf(mpv, 255, "mpv --force-seekable=yes --speed=1 --really-quiet --profile=low-latency \"https://www.twitch.tv/%s\"", streamer);
  system(mpv);
}

/*
 * @brief Callback function for curl
 * @param content The content recieved from curl
 * @param size The size of the content recieved from curl
 * @param nmemb The number of the content recieved from curl
 * @parma user_pointer The user pointer used to return the content
 */
static size_t
write_memory_callback (void *content, size_t size, size_t nmemb, void *user_pointer)
{
   size_t real_size = size * nmemb;
   curl_data *memory = (curl_data *) user_pointer;
   char *ptr = (char *) realloc(memory->content, memory->size + real_size + 1);
   if (!ptr) {
     exit_error("Error, not enough memory.");
   }
   memory->content = ptr;
   memcpy(&(memory->content[memory->size]), content, real_size);
   memory->size += real_size;
   memory->content[memory->size] = 0;
   return real_size;
}

/*
 * @brief Thread function to fetch data from twitch to see if a streamer is live or not
 * @param pargs Is a thread_fetch_data structure where streamer is the name of the streamer and status the return status
 */
void *
fetch_thread ( void *pargs )
{
  thread_fetch_data *fetch_data = (thread_fetch_data *) pargs;
  CURL *curl_handle;
  CURLcode res;
  curl_data data;
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  char url[256];
  snprintf(url, 255, "https://www.twitch.tv/%s", fetch_data->streamer);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  data.content = (char *) malloc(1);
  if (!data.content)
    exit_error("Error, not enough memory.");
  data.size = 0;
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &data); 
  res = curl_easy_perform(curl_handle);
  if (res == CURLE_OK) {
    if (strstr(data.content, "live_user")) {
      *fetch_data->status = STATUS_LIVE;
    } else if (strstr(data.content, "www.twitch.tv/")) {
      *fetch_data->status = STATUS_OFFLINE;
    } else {
      *fetch_data->status = STATUS_ERROR;
    }
  } else {
    *fetch_data->status = STATUS_ERROR;
  }
  free(data.content);
}

/*
 * @breif Fetches the status of each streamer
 * @param count Number of streamer to fetch content from
 * @param streamer List of streamers to fetch content from
 * @parma status Address of the list of status to return
 */
void
fetch_streamer_data ( size_t count, char **streamer, twitch_status **status, bool verbose )
{
  *status = (twitch_status *) malloc(sizeof(twitch_status) * count);
  if (!*status) {
    exit_error("Error, not enough memory.");
  }
  pthread_t thread[count];
  thread_fetch_data fetch_data[count];
  int current_thread = 0;
  while (current_thread < count) {
    int max = MIN(count, current_thread + MAX_NUMBER_THREAD);
    for (int i = current_thread; i < max; i++) {
      fetch_data[i].streamer = streamer[i];
      fetch_data[i].status = &(*status)[i];
      pthread_create(&thread[i], NULL, fetch_thread, (void *) &fetch_data[i]);
      if (verbose)
        printf("Started thread for %s.\n", streamer[i]);
    }
    for (int i = current_thread; i < max; i++) {
      pthread_join(thread[i], NULL);
      if (verbose)
        printf("Ended thread for %s.\n", streamer[i]);
    }
    current_thread += MAX_NUMBER_THREAD;
  }
}

void
exit_error ( char *message )
{
  if (message)
    fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}
