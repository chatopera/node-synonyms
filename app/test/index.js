const test = require("ava");
/**
 * Test
 */
const curdir = __dirname;
const path = require("path");
const debug = require("debug")("synonyms:test");
const synonyms = require(path.resolve(curdir, ".."));

test("Synonyms#display", async (t)=>{
    debug("Synonyms#display");
    synonyms.display("飞机");
    t.pass();
})

test("Synonyms#seg", async (t)=>{
    let sen1 = "移动互联网";
    let words = await synonyms.seg(sen1, true, true);
    debug("synonyms#seg %j", words);
    t.pass();
})

test("word2vec#compare", async (t)=>{
    let sen1 = "移动互联网";
    let sen2 = "互联网";
    let v = await synonyms.compare(sen1, sen2);
    debug("synonyms#compare %j", v);
    t.pass();
})

test("word2vec#vector", async (t)=>{
    let word = "股市";
    let v = await synonyms.vector(word);
    debug("synonyms#vector %j", v);
    t.pass();
})

test("word2vec#nearby", async (t)=>{
    let word = "股市";
    let v = await synonyms.nearby(word);
    debug("synonyms#nearby %j", v);
    t.pass();
})