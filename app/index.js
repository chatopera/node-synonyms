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
const tokenizer = require("./common/tokenizer");
const Q = require("q");

function Synonyms(){
    this.init();
}

// mount two inferfaces from tokenizer
Synonyms.prototype.seg = tokenizer.seg;
Synonyms.prototype.tag = tokenizer.tag;

Synonyms.prototype.nearby = function(word){

}

Synonyms.prototype.init = function(){
    
}

Synonyms.prototype.compare = function(sen1, sen2){

}

Synonyms.prototype.display = function(word){

}

exports = module.exports = new Synonyms();
