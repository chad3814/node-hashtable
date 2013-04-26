{
  "targets": [
    {
      "target_name": "native",
      "sources": [ "src/native.cpp", "src/hashtable.cpp" ],
      "cflags": [ "-std=c++11" ],
      "conditions": [
         ["OS=='mac'", {"cflags": ["-Wc++11-extensions"]}]
      ]     
    }
  ]
}