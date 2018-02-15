/* Copyright (C) 2018 Hai Liang Wang - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GPL 3.0 license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the GPL 3.0 license with
 * this file. If not, visit: https://www.gnu.org/licenses/gpl-howto.en.html
 */

#include <nan.h>
#include "reader.h"

using namespace synonyms::word2vec;

void InitAll(v8::Local<v8::Object> exports) {
  Reader::Init(exports);
}

NODE_MODULE(addon, InitAll)