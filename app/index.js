#!/usr/bin/env node
 // -*- coding: utf-8 -*-
//===============================================================================
//
// Copyright (c) 2017 Hai Liang Wang<hailiang.hl.wang@gmail.com> All Rights Reserved
//
//
// Module: node-synonyms
// Project: https://github.com/Samurais/node-synonyms
// File: /Users/hain/ai/node-synonyms/app/index.js
// Author: Hai Liang Wang
// Date: 2018-02-15:09:02:32
//
//===============================================================================
const curdir = __dirname;
const argv = process.argv;
const Q = require("q");
const _ = require("lodash");
const debug = require("debug")("synonyms:index");
const Word2vec = require("node-word2vec-reader");
const tokenizer = require("./tokenizer");
const distance_levenshtein = require("fast-levenshtein");
const distance_cosine = require("compute-cosine-similarity");
const path = require("path");
const w2v_model_ = path.resolve(curdir, "..", "data", "words.vector")
const SYN_MODEL_W2V_PATH = "SYN_MODEL_W2V_PATH" in process.env ? process.env["SYN_MODEL_W2V_PATH"] : w2v_model_;

/**
 * word2vec class instance
 */
const word2vec = new Word2vec();
word2vec.init(SYN_MODEL_W2V_PATH);

function Synonyms() {}

// mount seg, tag inferfaces from tokenizer
Synonyms.prototype.seg = tokenizer.seg.bind(tokenizer);
Synonyms.prototype.tag = tokenizer.tag.bind(tokenizer);


// mount vector interface from word2vec
Synonyms.prototype.vector = word2vec.v.bind(word2vec);

/**
 * Nearby
 * @param  {[type]} word [description]
 * @param  {Number} topk [description]
 * @return {[type]}      [description]
 */
Synonyms.prototype.nearby = function(word, topk = 10) {
    return word2vec.nearby(word, topk);
}


/**
 * Compare two sentences similarity
 * @param  {[type]} sen1 [description]
 * @param  {[type]} sen2 [description]
 * @return {[type]}      [description]
 */
Synonyms.prototype.compare = function(sen1, sen2) {
    let deferred = Q.defer();
    let lev_max = _.max([sen1.length, sen2.length]);
    let lev = _.floor((lev_max - distance_levenshtein.get(sen1, sen2)) / lev_max, 5);
    debug("synonyms#compare distance_levenshtein: %s", lev);

    try {
        tokenizer.seg(sen1, false, false)
            .then((word1) => {
                tokenizer.seg(sen2, false, false)
                    .then((word2) => {
                        word2vec.bow(word1)
                            .then((vec1) => {
                                word2vec.bow(word2)
                                    .then((vec2) => {
                                        let cos = distance_cosine(vec1, vec2);
                                        debug("synonyms#compare distance_cosine: %s", cos);
                                        let similarity = _.min([1, (cos * 0.5 + lev * 0.8)]);
                                        debug("synonyms#compare similarity: %s", similarity);
                                        deferred.resolve(similarity);
                                    }, (err) => {
                                        console.error("synonyms#compare", err);
                                        deferred.reject(err);
                                    })
                            }, (err) => {
                                console.error("synonyms#compare", err);
                                deferred.reject(err);
                            });
                    }, (err) => {
                        throw new Error("wordseg error:" + sen2);
                    })
            }, (err) => {
                throw new Error("wordseg error:" + sen1);
            })
    } catch (err) {
        debug("compare error:", err);
        deferred.reject(err);
    }
    return deferred.promise;
}

/**
 * Display word nearbys
 * @param  {[type]} word [description]
 * @param  {Number} topk [description]
 * @return {[type]}      [description]
 */
Synonyms.prototype.display = function(word, topk = 10) {
    this.nearby(word, topk)
        .then((data) => {
            console.log(`'${word}'近义词:`)
            let [words, scores] = data;
            _.each(_.zip(words, scores), (i, j) => {
                console.log(`  ${j+1}. ${i[0]}: ${i[1]}`)
            })
        }, (err) => {
            console.log(err)
        });
}

exports = module.exports = new Synonyms();