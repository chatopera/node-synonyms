/**
 * Tokenizer
 */
const debug = require("debug")("synonyms:tokenizer");
const jieba = require("nodejieba");
const curdir = __dirname;
const path = require("path");
const _ = require("lodash");
let userdict_ = path.resolve(curdir, "..", "data", "tokenizer", "user.dict.utf8");
let stopdict_ = path.resolve(curdir, "..", "data", "tokenizer", "stop_words.utf8");
let puntdict_ = path.resolve(curdir, "..", "data", "tokenizer", "punctuation.utf8");
const USER_DICT = "SYN_WORDSEG_CUSTOM_DICT" in process.env ? process.env["SYN_WORDSEG_CUSTOM_DICT"] : userdict_;
const STOP_DICT = "SYN_WORDSEG_STOPWORD_DICT" in process.env ? process.env["SYN_WORDSEG_STOPWORD_DICT"] : stopdict_;
const PUNT_DICT = "SYN_WORDSEG_PUNCT_DICT" in process.env ? process.env["SYN_WORDSEG_PUNCT_DICT"] : puntdict_;
const readlineq = require('readlineq');
const STOPWORDS = new Set();
const PUNCT = new Set();
const Q = require("q");

jieba.load({
    userDict: USER_DICT,
    stopWordDict: STOP_DICT
});

function Tokenizer() {
    this.isInit = false;
}

Tokenizer.prototype.init = function() {
    debug("Tokenizer#init");
    let deferred = Q.defer();
    if (this.isInit) {
        deferred.resolve();
    } else {
        readlineq(STOP_DICT)
            .then((lines) => {
                _.each(lines, (val) => {
                    STOPWORDS.add(val.trim());
                });
                debug("load stopword size: %s", STOPWORDS.size);
                return readlineq(PUNT_DICT);
            }, (err) => {
                debug("load stopword err", err);
                deferred.reject(err);
            })
            .then((lines) => {
                _.each(lines, (val)=>{
                    PUNCT.add(val.trim());
                });
                debug("load punctuation size: %s", PUNCT.size);

                this.isInit = true;
                deferred.resolve();
            }, (err) => {
                debug("load punctuation err", err);
                deferred.reject(err);
            })
    }
    return deferred.promise;
}

/**
 * 分词
 * @param  {String}  sen      待分词句子
 * @param  {Boolean} stopword 是否允许停用此
 * @param  {Boolean} punt     是否允许标点符号
 * @return {Promise}           JSONArray[word1, word2, ...]
 */
Tokenizer.prototype.seg = function(sen, stopword = true, punt = true) {
    let deferred = Q.defer();
    this.init()
        .then(() => {
            let data = this.tag(sen);
            let result = [];
            for(let x in data){
                let {word, tag} = data[x];
                debug("word: %s, tag: %s", word, tag);
                if((!stopword) && (STOPWORDS.has(word))){
                    continue;
                }
                if((!punt) && (PUNCT.has(word))){
                    continue;
                }
                result.push(word);
            }
            deferred.resolve(result);
        }, (err) => {
            deferred.reject(err);
        });
    return deferred.promise;
};


Tokenizer.prototype.tag = jieba.tag;

exports = module.exports = new Tokenizer();