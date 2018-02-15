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
const SYN_MODEL_NEARBY_PATH = "SYN_MODEL_NEARBY_PATH" in process.env ? process.env["SYN_MODEL_NEARBY_PATH"] : w2v_nearby_;

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

Word2vec.prototype.v = function(word) {
    let deferred = Q.defer();
    try {
        Q.fcall(() => {
                if (this.isInited)
                    return;
                return this.init();
            })
            .then(() => {
                this.R.getVector(word, (rc, v) => {
                    if (rc == 0) {
                        deferred.resolve(v);
                    } else if (rc == 1) {
                        deferred.reject({
                            rc: 1,
                            error: "word not found"
                        });
                    } else {
                        deferred.reject({
                            rc: 2,
                            error: "empty word"
                        });
                    }
                })
            }, (err) => {
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


exports = module.exports = new Word2vec();