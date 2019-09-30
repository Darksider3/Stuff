#include <iostream>
#include <curl/curl.h>

int main()
{
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers=NULL;
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if(!curl)
  {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    std::cout << "Nope..\n";
    return 0;
  }

  headers = curl_slist_append(headers, "");
  curl_easy_setopt(curl, CURLOPT_URL, "https://reddit.com/api/v1/access_token");
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "ScrapeThingy by /u/darksider3");
  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  
  //AUTH CREDS
  res = curl_easy_perform(curl);
  if(res != CURLE_OK)
  {
    std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << "\n";
  }
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}
