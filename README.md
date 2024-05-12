This program implements a simple image compression algorithm using a quadtree data structure.
It reads an image file in PPM format, compresses it using a quadtree, and optionally applies 
horizontal or vertical mirroring. It can also decompress compressed images back to their original 
form.

Compression
To compress an image, run the program with the following arguments:
  ./quadtree -c <factor> <input_file> <output_file> (compression factor is an integer)
  
Mirroring
To compress and mirror an image horizontally or vertically, run the program with the following arguments:
  ./quadtree -m <type> <factor> <input_file> <output_file> (type is either 'h' or 'v')

Decompression
To decompress a compressed image, run the program with the following arguments:
  ./quadtree -d <input_file> <output_file>
