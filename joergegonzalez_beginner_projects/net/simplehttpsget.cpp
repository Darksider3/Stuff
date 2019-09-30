#include <iostream>
#include <curl/curl.h>

int main()
{
  CURL *curl;
  int err;
  std::string url = "https://darksider3.de";

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());


}

