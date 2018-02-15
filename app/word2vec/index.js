'use strict';
/**
 * Word2Vec Module
 */
const curdir = __dirname;
const Reader = require('bindings')('word2vec').Reader;
const _ = require("lodash");
const path = require("path");
const debug = require('debug')('synonyms:word2vec');
const Q = require('q');
const w2v_model_ = path.resolve(curdir, "..", "..", "data", "words.vector")
const w2v_nearby_ = path.resolve(curdir, "..", "..", "data", "words.nearby.json.gz")
const SYN_MODEL_W2V_PATH = "SYN_MODEL_W2V_PATH" in process.env ? process.env["SYN_MODEL_W2V_PATH"] : w2v_model_;
const tokenizer = require("../common/tokenizer");

function Word2vec() {
    this.isInited = false;
    this.R = new Reader();
}

Word2vec.prototype.init = function() {
    let deferred = Q.defer();
    if (this.isInited)
        return deferred.resolve();
    try {
        this.R.initialize(SYN_MODEL_W2V_PATH, () => {
            this.isInited = true;
            deferred.resolve();
        }, (err) => {
            deferred.reject(err);
        });
    } catch (err) {
        deferred.reject(err);
    }
    return deferred.promise;
}

Word2vec.prototype.nearby = function(word, topk = 10) {
    let deferred = Q.defer();
    try {
        Q.fcall(() => {
                if (this.isInited)
                    return;
                return this.init();
            })
            .then(() => {
                this.R.findClosestWords(word, topk, (rc, data) => {
                    if (rc === 0) {
                        deferred.resolve(data);
                    } else if (rc === 1) {
                        deferred.reject({
                            rc: rc,
                            error: "Exception: Out of vocabulary."
                        })
                    } else {
                        deferred.reject({
                            rc: 2,
                            error: "unknown error."
                        })
                    }
                });
            }, (err) => {
                deferred.reject({
                    rc: 3,
                    error: "empty word."
                });
            });
    } catch (e) {
        deferred.reject({
            rc: 4,
            error: "init fails."
        });
    }
    return deferred.promise;
}

Word2vec.prototype.v = function(word) {
    let deferred = Q.defer();
    debug("vector: %s", word);
    try {
        Q.fcall(() => {
                if (this.isInited)
                    return;
                return this.init();
            })
            .then(() => {
                this.R.getVector(word, (rc, v) => {
                    debug("vector getVector: %s, %j", rc, v)
                    if (rc == 0) {
                        deferred.resolve(v);
                    } else if (rc == 1) {
                        deferred.reject({
                            rc: 1,
                            error: `warning: ${word} word not found`
                        });
                    } else {
                        deferred.reject({
                            rc: 2,
                            error: "empty word"
                        });
                    }
                })
            }, (err) => {
                debug("vector error:", err)
                deferred.reject({
                    rc: 3,
                    error: "unknown error."
                })
            })
    } catch (e) {
        deferred.reject({
            rc: 4,
            error: "init fails."
        });
    }
    return deferred.promise;
}


/**
 * Get vectors for words
 * @param  {[type]} words [description]
 * @return {[type]}       [description]
 */
Word2vec.prototype.vectors = function(words) {
    var deferred = Q.defer();
    var promises = [];
    var vectors = [];
    for (let x in words) {
        promises.push(this.v(words[x]));
    }
    Q.allSettled(promises)
        .then((results) => {
            for (let x in results) {
                if (results[x]["state"] === "fulfilled") {
                    vectors.push(results[x]["value"]);
                } else {
                    console.warn("_vectors error:", JSON.stringify(results[x]));
                }
            }

            if (vectors.length > 0)
                return deferred.resolve(vectors);
            deferred.reject("Invalid vectors length: " + JSON.stringify(words));
        }, function(err) {
            deferred.reject(err);
        })

    return deferred.promise;
}

Word2vec.prototype.sentence_vector = function(sen) {
    let deferred = Q.defer();
    tokenizer.seg(sen, false, false)
        .then((words) => {
            if (words.length > 0) {
                return this.vectors(words);
            } else {
                throw new Error("Blank sentence after wordseg:" + sen);
            }
        }, (err) => {
            deferred.reject(err);
        })
        .then((vectors) => {
            // bag of words
            let v = [];
            for (let x in vectors) {
                _.each(vectors[x], (val, index) => {
                    if(v[index] !== undefined){
                        v[index] += val;
                    } else {
                        v.push(val);
                    }
                })
            }
            if(v.length > 0)
                return deferred.resolve(v);
            deferred.reject("Empty vector after Bag of Words:" + sen);
        }, (err) => {
            deferred.reject(err);
        });

    return deferred.promise;
}

/**
 * Get vocal size
 * @return {Promise} [description]
 */
Word2vec.prototype.getVocabSize = function() {
    let deferred = Q.defer();
    try {
        Q.fcall(() => {
                if (this.isInited)
                    return;
                return this.init();
            })
            .then(() => {
                this.R.getVocabSize((num) => {
                    deferred.resolve(num);
                })
            })
    } catch (e) {
        deferred.reject(e);
    }
    return deferred.promise;
}

/**
 * Get embedding dim
 * @return {Promise} [description]
 */
Word2vec.prototype.getEmbeddingDim = function() {
    let deferred = Q.defer();
    try {
        Q.fcall(() => {
                if (this.isInited)
                    return;
                return this.init();
            })
            .then(() => {
                this.R.getEmbeddingDim((num) => {
                    deferred.resolve(num);
                })
            })
    } catch (e) {
        deferred.reject(e);
    }
    return deferred.promise;
}


exports = module.exports = new Word2vec();