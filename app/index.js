#!/usr/bin/env node
// -*- coding: utf-8 -*-
//===============================================================================
//
// Copyright (c) 2017 <> All Rights Reserved
//
//
// Module: node-synonyms
// Project: https://github.com/Samurais/node-synonyms
// File: /Users/hain/ai/node-synonyms/app/index.js
// Author: Hai Liang Wang
// Date: 2018-02-15:09:02:32
//
//===============================================================================
const argv = process.argv;
const curdir = __dirname;
const debug = require("debug")("synonyms:index")
const SYN_MODEL_W2V_PATH = "SYN_MODEL_W2V_PATH" in process.env?process.env["SYN_MODEL_W2V_PATH"]:"";
const SYN_MODEL_NEARBY_PATH = "SYN_MODEL_NEARBY_PATH" in process.env?process.env["SYN_MODEL_NEARBY_PATH"]:"";

function Synonyms(){
    this.init();
}

Synonyms.prototype.nearby = function(word){

}

Synonyms.prototype.init = function(){
    debug("init");
}

Synonyms.prototype.compare = function(sen1, sen2){

}

Synonyms.prototype.display = function(word){

}

exports = module.exports = new Synonyms();
