const test = require("ava");
/**
 * Test Tokenizer
 */
const debug = require("debug")("synonyms:test:tokenzier");
const tokenizer = require("../tokenizer");

test("Synonyms:Tokenizer#cut", async (t)=> {
    debug("Synonyms:Tokenizer#cut");
    let s = "工信处女干事每月经过下属科室都要亲口交代24口交换机等技术性器件的安装工作。";
    let result = await tokenizer.seg(s);
    t.true(result.length === 20, "wordseg length incorrect");

    result = await tokenizer.seg(s, false);
    t.true(result.length === 15, "wordseg no stopwords legnth incorrect");
    debug("cut %s: \n %j, len: %s", s, result, result.length);

    result = await tokenizer.seg(s, false, false);
    debug("cut %s: \n %j, len: %s", s, result, result.length);
    t.true(result.length === 14, "wordseg no stopwords, no punt legnth incorrect");
    t.pass();
})