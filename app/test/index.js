const test = require("ava");
/**
 * Test
 */
const curdir = __dirname;
const path = require("path");
const debug = require("debug")("synonyms:test");
const synonyms = require(path.resolve(curdir, ".."));

test("Synonyms#init", async (t)=>{
    debug("Synonyms#init")
    t.pass();
})