const test = require("ava");
/**
 * Test
 */
const curdir = __dirname;
const path = require("path");
const debug = require("debug")("synonyms:test:word2vec");
const word2vec = require(path.resolve(curdir, "..", "word2vec"));

test("word2vec#vector", async (t)=>{
    debug("word2vec#vector");
    let v = await word2vec.v("飞机")
    debug("word2vec#v", v)
    let nearby = await word2vec.nearby("飞机", 10);
    debug("word2vec#nearby %j", nearby);
    t.pass();
})

test("word2vec#info", async (t)=>{
    debug("word2vec#info");
    let v = await word2vec.getEmbeddingDim();
    debug("word2vec#getEmbeddingDim", v);
    v = await word2vec.getVocabSize();
    debug("word2vec#getVocabSize %j", v);
    t.pass();
})

test("word2vec#sentence_vector", async (t) => {
    debug("word2vec#sentence_vector");
    let sen1 = "当时比特币价格由个位数向两位数上涨";
    let v = await word2vec.sentence_vector(sen1);
    debug("sentence_vector:", JSON.stringify(v));
    t.pass();
})