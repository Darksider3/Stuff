#include "controllers/TestCookieSetCtrl.h"

void TestCookieSetCtrl::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    std::string rspHead = "<html>"
                          "<head></head>"
                          "<body>";
    std::string rspBody = "<p>Am going to set some cookie here!</p>";
    std::string rspFoot = "</body>"
                          "</html>";
    auto resp = HttpResponse::newHttpResponse();

    std::string first, second;
    first = "Hello";
    second = "World";
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    //callback(resp);
    if (req->cookies().find(first) == req->cookies().end())
        resp->addCookie(first, second);
    else {

        rspBody.append("Already set!");
        auto cok = req->getSession()->get<Cookie>(first);
    }
    resp->setBody(rspHead + rspBody + rspFoot);
    callback(resp);
}