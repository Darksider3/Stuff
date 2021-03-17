#include "SetCookie.h"
void SetCookie::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    std::string CookieName = "settercookie";
    bool found = false;
    if (req->cookies().find(CookieName) != req->cookies().end()) {
        found = true;
    } else {
        auto resp = HttpResponse::newRedirectionResponse("/setcookie");
        resp->addCookie(CookieName, "Some Content here i guess");
        callback(resp);
    }

    HttpViewData data;
    data.insert("title", "Set cookies!");
    data.insert("gotSet", found);
    auto resp = HttpResponse::newHttpViewResponse("setcookie.csp", data);
    callback(resp);
}