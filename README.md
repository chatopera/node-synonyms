# Synonyms
Chinese Synonyms for Natural Language Processing and Understanding.

最好的中文近义词工具包。

```synonyms```可以用于自然语言理解的很多任务：文本对齐，推荐算法，相似度计算，语义偏移，关键字提取，概念提取，自动摘要，搜索引擎等。

![](https://camo.githubusercontent.com/ae91a5698ad80d3fe8e0eb5a4c6ee7170e088a7d/687474703a2f2f37786b6571692e636f6d312e7a302e676c622e636c6f7564646e2e636f6d2f61692f53637265656e25323053686f74253230323031372d30342d30342532306174253230382e32302e3437253230504d2e706e67)

# Welcome
```
npm install node-synonyms
```

支持使用环境变量配置：
| 环境变量 | 描述 | 默认值 |
| --- | ---  | --- |
| SYN_MODEL_W2V_PATH | word2vec训练后得到的词向量文件 | node_modules/node-synonyms/data | 
| SYN_MODEL_NEARBY_PATH | 近义词文件 | node_modules/node-synonyms/data |
| SYN_WORDSEG_CUSTOM_DICT | 分词用户词典 | node_modules/node-synonyms/data/tokenizer/user.dict.utf8 | 
| SYN_WORDSEG_STOPWORD_DICT | 分词停用词词典 | node_modules/node-synonyms/data/tokenizer/stop_words.utf8 |
| SYN_WORDSEG_PUNCT_DICT | 分词标点词典 | node_modules/node-synonyms/data/tokenizer/punctuation.utf8  |tokenizer


# Word2vec
word2vec是用来训练词向量模型的工具，为了方便，将word2vec也放在代码库中。编译和使用word2vec：
```
cd app/word2vec/google
make clean
make
./word2vec
```

更多信息，参考 [word2vec/google/README](app/word2vec/google/README.txt)。

# Statement
Synonyms发布证书 GPL3.0。数据和程序可用于研究和商业产品，必须注明引用和地址，比如发布的任何媒体、期刊、杂志或博客等内容。

```
@online{Synonyms:hain2017,
  author = {Hai Liang Wang, Hu Ying Xi},
  title = {中文近义词工具包Synonyms},
  year = 2017,
  url = {https://github.com/huyingxi/Synonyms},
  urldate = {2017-09-27}
}
```