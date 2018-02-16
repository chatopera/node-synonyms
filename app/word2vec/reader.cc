/* Copyright (C) 2018 Hai Liang Wang<hailiang.hl.wang@gmail.com> - All Rights
 * Reserved
 * You may use, distribute and modify this code under the
 * terms of the GPL 3.0 license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the GPL 3.0 license with
 * this file. If not, visit: https://www.gnu.org/licenses/gpl-howto.en.html
 */
#include "reader.h"
#include <algorithm>
#include <functional>
#include "vector_utils.h"

using namespace msa::vector_utils;

// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline bool exists(const std::string& name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

namespace synonyms {
namespace word2vec {

Nan::Persistent<v8::Function> Reader::constructor;

Reader::Reader() {
  word_vectors = new WordVectorMap();
  word_vectors_norm = new WordVectorMap();
}

Reader::~Reader() {
  delete word_vectors;
  delete word_vectors_norm; 
}

void Reader::clear() {
  num_words = 0;
  num_dims = 0;
  word_vectors->clear();
  word_vectors_norm->clear();
}

void Reader::updateNormVectors() {
    word_vectors_norm->clear();

    for(auto&& word : *word_vectors) {
        (*word_vectors_norm)[word.first] = msa::vector_utils::normalized(word.second);
    }
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
  Nan::SetPrototypeMethod(tpl, "findClosestWords", FindClosestWords);
  Nan::SetPrototypeMethod(tpl, "getVocabSize", GetVocabSize);
  Nan::SetPrototypeMethod(tpl, "getEmbeddingDim", GetEmbeddingDim);

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
void Reader::Initialize(const Nan::FunctionCallbackInfo<v8::Value>& info) {
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

  if (!exists(modelPath)) {
    std::cout << "Word2Vec Model File does not exist: " << modelPath << endl;
    Nan::ThrowTypeError("Word2Vec Model File does not exist. ");
    return;
  }

  std::cout << "reader.cc << init with file: " << modelPath << endl;
  FILE* f = fopen(modelPath.c_str(), "rb");
  if (f == NULL) {
    Nan::ThrowTypeError("Word2Vec Model can not open file.");
  }

  obj->clear();
  fscanf(f, "%lld", &(obj->num_words));
  fscanf(f, "%lld", &(obj->num_dims));
  std::cout << "reader.cc << num_words: " << obj->num_words
            << ", num_dims: " << obj->num_dims << endl;
  for (int word_index = 0; word_index < obj->num_words; word_index++) {
    // read characters into word until ' ' is encountered
    string word;
    while (1) {
      char c = fgetc(f);
      if (feof(f) || (c == ' ')) break;
      word += c;
    }

    word = msa::vector_utils::trim(word);

    (*obj->word_vectors)[word] = std::vector<float>(obj->num_dims);
    std::vector<float>& vec = (*obj->word_vectors)[word];

    // std::cout << "push word: " << word << endl;

    // read all floats
    for (int a = 0; a < obj->num_dims; a++) fread(&vec[a], sizeof(float), 1, f);
  }
  fclose(f);
  obj->updateNormVectors();

  std::cout << "reader.cc << dict size " << (obj->word_vectors)->size() << endl;
  std::cout << "reader.cc << init done." << endl;

  v8::Local<v8::Function> cb = info[1].As<v8::Function>();
  const unsigned argc = 0;
  v8::Local<v8::Value> argv[argc] = {};
  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
};

void Reader::GetEmbeddingDim(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Reader* obj = ObjectWrap::Unwrap<Reader>(info.Holder());
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }
  v8::Local<v8::Function> cb = info[0].As<v8::Function>();
  const unsigned argc = 1;
  v8::Local<v8::Value> argv[argc] = {Nan::New<Number>(obj->num_dims)};
  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
}

void Reader::GetVocabSize(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Reader* obj = ObjectWrap::Unwrap<Reader>(info.Holder());
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }
  v8::Local<v8::Function> cb = info[0].As<v8::Function>();
  const unsigned argc = 1;
  v8::Local<v8::Value> argv[argc] = {Nan::New<Number>(obj->num_words)};
  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
}

void Reader::GetVector(const Nan::FunctionCallbackInfo<v8::Value>& info) {
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

  if (word.empty()) {
    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = {Nan::New(2)};  // empty word
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
  } else {
    vector<float> v;
    int rc = 0;  // normal case
    std::string vector_str;
    std::map<string, vector<float> >::iterator it =
        (obj->word_vectors_norm)->find(word);
    if (it != (obj->word_vectors_norm)->end()) {
      v = it->second;
      vector_str = to_string(v);
      // std::cout << "Resolve vector:" << vector_str << endl;
    } else {
      // std::cout << "Could not resolve vector, oov." << endl;
      vector_str = "[]";
      rc = 1;  // OOV
    }

    v8::Local<v8::Value> vectorJsonArray;
    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::Value> vectorJsonValue =
        NanJSON.Parse(Nan::New(vector_str).ToLocalChecked());
    vectorJsonArray = vectorJsonValue.ToLocalChecked();

    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    const unsigned argc = 2;
    v8::Local<v8::Value> argv[argc] = {Nan::New(rc), vectorJsonArray};
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
  }
};

void Reader::FindClosestWords(
    const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Reader* obj = ObjectWrap::Unwrap<Reader>(info.Holder());

  if (info.Length() < 3) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Invalid argument type of word");
    return;
  }

  if (!info[1]->IsNumber()) {
    Nan::ThrowTypeError("Invalid argument type of topK");
    return;
  }

  v8::String::Utf8Value param0(info[0]->ToString());
  // convert it to string
  std::string word = std::string(*param0);

  // convert it to int
  int top_k = info[1]->NumberValue();

  int rc = 0;  // normal case
  std::string result;
  std::map<string, vector<float> >::iterator it =
      (obj->word_vectors_norm)->find(word);
  if (it != (obj->word_vectors_norm)->end()) {
    vector<float> vec0 = it->second;

    // will store all distances
    vector<pair<string, float> > dists;  //(num_words);

    // iterate all words and save distances
    for (auto&& word1 : *(obj->word_vectors_norm)) {
      const vector<float>& vec1 = word1.second;
      float dist = msa::vector_utils::dot(vec0, vec1);
      dists.push_back(make_pair(word1.first, dist));
    }

    // sort distances list by distance
    stable_sort(dists.rbegin(), dists.rend(),
                [](decltype(*dists.cbegin()) lhs, decltype(lhs) rhs) {
                  return lhs.second > rhs.second;
                });

    // return top_k results
    vector<pair<string, float> > top_dists(dists.begin(),
                                           dists.begin() + top_k);

    std::vector<std::string> words;
    std::vector<float> scores;
    for (auto&& tmp : top_dists) {
        words.push_back(tmp.first);
        scores.push_back(tmp.second);
        // std::cout << "nearest word: " << tmp.first << ", score: " <<
        // tmp.second
        //           << endl;
    }
    std::stringstream ss;
    ss << "[";
    ss << msa::vector_utils::to_string(words);
    ss << ",";
    ss << msa::vector_utils::to_string(scores);
    ss << "]";
    result = ss.str();
    // std::cout << "FindClosestWords result: " << result << endl;
  } else {
    // std::cout << "Could not resolve vector, oov." << endl;
    result = "[[],[]]";
    rc = 1;  // OOV
  }

  v8::Local<v8::Value> vectorJsonArray;
  Nan::JSON NanJSON;
  Nan::MaybeLocal<v8::Value> vectorJsonValue =
      NanJSON.Parse(Nan::New(result).ToLocalChecked());
  vectorJsonArray = vectorJsonValue.ToLocalChecked();

  v8::Local<v8::Function> cb = info[2].As<v8::Function>();
  const unsigned argc = 2;
  v8::Local<v8::Value> argv[argc] = {Nan::New(rc), vectorJsonArray};
  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
}
}  // namespace word2vec
}  // namespace synonyms