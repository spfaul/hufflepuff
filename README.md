# hufflepuff - File compression example via Huffman Coding

Compresses and decompresses **plain ascii files**.
> Inspired by [this](https://www.youtube.com/watch?v=JsTptu56GM8&ab_channel=TomScott) video from Tom Scott

## Usage
```
> sh build.sh
> ./build/huff ./examples/bee-movie.txt ./out.compress
> du -b ./examples/bee-movie.txt
115264  examples/bee-movie.txt
> du -b ./out.compress
74252   out.txt

100 - (74252 / 115264 * 100) = ~35.6% disk space saved
```