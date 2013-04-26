{
  "targets": [
    {
      "target_name": "native",
      "sources": [ "src/native.cpp", "src/hashtable.cpp" ],
      "cflags": [ "-std=c++11" ],
      "conditions": [
         ["OS=='mac'", {"cflags": ["-Wno-c++11-extensions"]}]
      ]     
    }
  ]
}