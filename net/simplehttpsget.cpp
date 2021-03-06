#include <iostream>
#include <curl/curl.h>


static std::string buffer;
static char errorBuffer[CURL_ERROR_SIZE];

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
  if(writerData == NULL)
    return 0;

  writerData->append(data, size*nmemb);

  return size*nmemb;
}

static bool init(CURL *&conn, char *url)
{
  CURLcode code;
 
  conn = curl_easy_init();
 
  if(conn == NULL) {
    fprintf(stderr, "Failed to create CURL connection\n");
    exit(EXIT_FAILURE);
  }
 
  code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set error buffer [%d]\n", code);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_URL, url);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
    return false;
  }
 
  code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
    return false;
  }
 
  return true;
}
//@TODO Redirect 
//@TODO Simple XML parser
//@TODO very simple json parser

int main(int argc, char *argv[])
{
  CURL *curl;
  std::string url = "https://darksider3.de";
  curl_global_init(CURL_GLOBAL_DEFAULT);
  if(!init(curl, argv[1]))
  {
    fprintf(stderr, "Sorry, failure.. %s \n", errorBuffer);
    exit(EXIT_FAILURE);
  }
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  std::cout << buffer;
}

