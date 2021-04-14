#include "SetCookie.h"
void SetCookie::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    std::string CookieName = "settercookie";
    bool found = false;
    if (req->cookies().find(CookieName) != req->cookies().end()) {
        found = true;
    } else {
        auto somedate = trantor::Date {};
        somedate = somedate.after(60 * 60);
        auto Cook = Cookie(CookieName, "Some content here i guess");
        Cook.setExpiresDate(somedate);
        auto resp = HttpResponse::newRedirectionResponse("/setcookie");
        resp->addCookie(CookieName, "Some Content here i guess");
        //resp->addCookie(Cook);
        callback(resp);
    }

    HttpViewData data;
    data.insert("title", "Set cookies!");
    data.insert("gotSet", found);
    auto resp = HttpResponse::newHttpViewResponse("setcookie.csp", data);
    callback(resp);
}