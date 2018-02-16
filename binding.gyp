{
  "targets": [
    {
      "target_name": "word2vec",
      "sources": [ "app/word2vec/addon.cc", "app/word2vec/reader.cc" ],
      "include_dirs": [
      ],
      'cflags_cc!': [ '-fno-rtti', '-std=c++14'],
      "libraries": [
      ],
      "conditions": [
                  [ 'OS=="mac"', {
                      "include_dirs": [
                        "<!(node -e \"require('nan')\")"],
                      "libraries": [
                      ],
                      "xcode_settings": {
                          'OTHER_CPLUSPLUSFLAGS' : ['-frtti', '-std=c++14','-stdlib=libc++', '-v'],
                          'OTHER_LDFLAGS': ['-stdlib=libc++', '-ObjC'],
                          'MACOSX_DEPLOYMENT_TARGET': '10.7',
                          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                      }
                  }],
                  [ 'OS=="linux"', {
                      "include_dirs": [
                        "<!(node -e \"require('nan')\")"
                      ],
                      'cflags_cc': [ '-fno-exceptions', '-std=c++14'],
                      "libraries": [
                      ]
                  }]
        ]
    }
  ]
}
