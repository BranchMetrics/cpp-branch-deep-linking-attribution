// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "BranchIO/JSONObject.h"
#include "Util/StringUtils.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation::Collections;

namespace BranchIO {

JSONObject::JSONObject() {
    jObject = JsonObject();
}

JSONObject::JSONObject(const JsonObject& object){
        jObject = object;
}

bool
JSONObject::isEmpty() const {
    return jObject.Size() == 0;
}

JSONObject
JSONObject::parse(const std::string& jsonString) {
    // Can throw Exception
     JsonObject obj = JsonObject::Parse(to_hstring(jsonString));
     return JSONObject(obj);
}

JSONObject
JSONObject::parse(std::istream& s) {
    std::string str(std::istreambuf_iterator<char>(s), {});
    return parse(str);
}

JSONObject
JSONObject::load(const std::string& path) {
    // std::ifstream constructor throws std::runtime_error if file does not exist, etc.
    ifstream jsonFile(path.c_str());
    return parse(jsonFile);
}

std::string
JSONObject::stringify() const {
    winrt::hstring s = jObject.Stringify();
    return StringUtils::wstring_to_utf8(s.c_str());
}

JSONObject &
JSONObject::operator += (const JSONObject &rhs) {

    IIterator<IKeyValuePair<winrt::hstring, IJsonValue>> it;
     for (it = rhs.jObject.First(); it.HasCurrent(); it.MoveNext()) {
        IKeyValuePair< winrt::hstring, IJsonValue>  kvp = it.Current();
        jObject.SetNamedValue(kvp.Key(), kvp.Value());
    }
    return *this;
}

void JSONObject::set(const std::string& key, const std::string& value){
    JsonValue objValue = JsonValue::CreateStringValue(to_hstring(value));
    jObject.SetNamedValue(to_hstring(key), objValue);
}

void JSONObject::set(const std::string& key, const int& value){
    JsonValue objValue = JsonValue::CreateNumberValue(value);
    jObject.SetNamedValue(to_hstring(key), objValue);
}

void JSONObject::set(const std::string& key, const double& value) {
    JsonValue objValue = JsonValue::CreateNumberValue(value);
    jObject.SetNamedValue(to_hstring(key), objValue);
}

void JSONObject::set(const std::string& key, const JSONObject value) const{
    const JsonObject obj = value.getWinRTJsonObj();
    jObject.SetNamedValue(to_hstring(key), obj);
}

void JSONObject::set(const std::string& key, const std::vector<std::string> value) const {
    winrt::Windows::Data::Json::JsonArray arr;
    for each (std::string str in value) {
        arr.Append(JsonValue::CreateStringValue(to_hstring(str)));
    }
    jObject.SetNamedValue(to_hstring(key), arr);
}

void JSONObject::set(const JSONObject& jsonObject) const {
    IIterator<IKeyValuePair<winrt::hstring, IJsonValue>> it;
    for (it = jsonObject.jObject.First(); it.HasCurrent(); it.MoveNext()) {
        IKeyValuePair< winrt::hstring, IJsonValue>  kvp = it.Current();
        jObject.SetNamedValue(kvp.Key(), kvp.Value());
    }
}

std::string JSONObject::getNamedString(std::string const& name) const{
    winrt::hstring hStrValue = jObject.GetNamedString(to_hstring(name));
    return StringUtils::wstring_to_utf8(hStrValue.c_str());
}

void JSONObject::clear() const{
    jObject.Clear();
}

bool JSONObject::has(const std::string& key) const {
    return jObject.HasKey(to_hstring(key));
}

void JSONObject::remove(const std::string& key){
    jObject.Remove(to_hstring(key));
}

const JsonObject JSONObject::getWinRTJsonObj() const{
    return jObject;
}

const uint32_t JSONObject::size() const {
    return jObject.Size() ;
}

}  // namespace BranchIO
