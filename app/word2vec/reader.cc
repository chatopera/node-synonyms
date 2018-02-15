/* Copyright (C) 2018 Hai Liang Wang - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GPL 3.0 license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the GPL 3.0 license with
 * this file. If not, visit: https://www.gnu.org/licenses/gpl-howto.en.html
 */
#include "reader.h"
#include "vector_utils.h"

using namespace msa::vector_utils;

// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline bool exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

namespace synonyms {
namespace word2vec {

Nan::Persistent<v8::Function> Reader::constructor;

Reader::Reader() {
    word_vectors = new WordVectorMap();
}

Reader::~Reader() {
    delete word_vectors;
}


void Reader::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Reader").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "initialize", Initialize);
  Nan::SetPrototypeMethod(tpl, "getVector", GetVector);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Reader").ToLocalChecked(), tpl->GetFunction());
}

void Reader::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    Reader* obj = new Reader();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance());
  }
}

/**
 * [Reader::Initialize description]
 * @param info [description]
 */
void Reader::Initialize(const Nan::FunctionCallbackInfo<v8::Value>& info){
  // https://stackoverflow.com/questions/9437109/serialization-using-boost-serialization
  Reader* obj = ObjectWrap::Unwrap<Reader>(info.Holder());
  
  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Invalid argument type");
    return;
  }

  v8::String::Utf8Value param0(info[0]->ToString());
  // convert it to string
  std::string modelPath = std::string(*param0);

  if(!exists(modelPath)){
    std::cout << "Word2Vec Model File does not exist: " << modelPath <<endl;
    Nan::ThrowTypeError("Word2Vec Model File does not exist. ");
    return;
  }

  std::cout  << "reader.cc << init with file: " << modelPath << endl;
  FILE *f = fopen(modelPath.c_str(), "rb");
  if (f == NULL) {
    Nan::ThrowTypeError("Word2Vec Model can not open file.");
  }

  obj->clear();
  fscanf(f, "%lld", &(obj->num_words));
  fscanf(f, "%lld", &(obj->num_dims));
  std::cout << "reader.cc << num_words: " << obj->num_words << ", num_dims: "<< obj->num_dims << endl;
  for(int word_index = 0; word_index < obj->num_words; word_index++) {
    // read characters into word until ' ' is encountered
    string word;
    while (1) {
        char c = fgetc(f);
        if (feof(f) || (c == ' ')) break;
        word += c;
    }

    word = msa::vector_utils::trim(word);

    (*obj->word_vectors)[word] = std::vector<float>(obj->num_dims);
    std::vector<float> &vec = (*obj->word_vectors)[word];

    // std::cout << "push word: " << word << endl;

    // read all floats
    for(int a = 0; a < obj->num_dims; a++) fread(&vec[a], sizeof(float), 1, f);
  }
  fclose(f);

  std::cout << "reader.cc << dict size " << (obj->word_vectors)->size() <<endl;
  std::cout << "reader.cc << init done." <<endl;

  v8::Local<v8::Function> cb = info[1].As<v8::Function>();
  const unsigned argc = 0;
  v8::Local<v8::Value> argv[argc] = {};
  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
};

void Reader::clear()
{
    num_words = 0;
    num_dims = 0;
    word_vectors->clear();
};

void Reader::GetVector(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Reader* obj = ObjectWrap::Unwrap<Reader>(info.Holder());

  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Invalid argument type");
    return;
  }

  v8::String::Utf8Value param0(info[0]->ToString());
  // convert it to string
  std::string word = std::string(*param0);
  std::cout << "Resolve word:" << word << endl;

  if(word.empty()){
    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { Nan::New(2)}; // empty word
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
  } else {
    vector<float> v;
    int rc = 0; // normal case
    std::string vector_str;
    std::map<string, vector<float> >::iterator it = (obj->word_vectors)->find(word);
    if(it != (obj->word_vectors)->end()){
        v = it->second;
        to_string(v, vector_str);
        std::cout << "Resolve vector:" << vector_str << endl;
    } else {
        std::cout << "Could not resolve vector, oov." << endl;
        vector_str = "[]";
        rc = 1; // OOV
    }

    v8::Local<v8::Value> vectorJsonArray;
    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::Value> vectorJsonValue = NanJSON.Parse(Nan::New(vector_str).ToLocalChecked());
    vectorJsonArray = vectorJsonValue.ToLocalChecked();

    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    const unsigned argc = 2;
    v8::Local<v8::Value> argv[argc] = { Nan::New(rc), vectorJsonArray};
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);

  }
};
}
}