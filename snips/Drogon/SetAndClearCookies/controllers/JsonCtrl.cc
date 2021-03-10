#include "JsonCtrl.h"

void JsonCtrl::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    Json::Value Val {};
    Val.insert(0, Json::String("Hello World"));
    Val.insert(1, Json::Int(15));
    auto resp = HttpResponse::newHttpJsonResponse(Val);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}