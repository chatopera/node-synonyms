const test = require("ava");
/**
 * Test
 */
const curdir = __dirname;
const path = require("path");
const debug = require("debug")("synonyms:test:word2vec");
const word2vec = require(path.resolve(curdir, "..", "word2vec"));

test.only("word2vec#init", async (t)=>{
    debug("word2vec#init");
    let v = await word2vec.v("飞机")
    debug("word2vec#v", v)
    t.pass();
})