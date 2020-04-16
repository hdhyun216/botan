#include <stdint.h>
#include <stdio.h>

static const uint64_t B[] = {0x5555555555555555,
                             0x3333333333333333,
                             0x0F0F0F0F0F0F0F0F,
                             0x00FF00FF00FF00FF,
                             0x0000FFFF0000FFFF,
};
static const uint32_t S[] = {1, 2, 4, 8, 16};

uint32_t compact(uint64_t x)
   {
   x &= 0x5555555555555555;
   x = (x ^ (x >>  1)) & 0x3333333333333333;
   x = (x ^ (x >>  2)) & 0x0f0f0f0f0f0f0f0f;
   x = (x ^ (x >>  4)) & 0x00ff00ff00ff00ff;
   x = (x ^ (x >>  8)) & 0x0000ffff0000ffff;
   x = (x ^ (x >>  16)) & 0xFFFFffff;
return x;
   }

uint32_t compact_32_4x8(uint32_t x)
   {
   x &= 0x0f0f0f0f;
   //x = (x ^ (x >>  2)) & 0x0f0f0f0f;
   x = (x ^ (x >>  4)) & 0x00ff00ff;
   x = (x ^ (x >>  8)) & 0x0000ffff;
   x = (x ^ (x >>  16)) & 0xFFFFffff;
   return x;
   }

uint32_t encode_4x8(uint32_t x)
   {
   x &= 0xf;
   x = (x | (x << 16)) & 0x80007;
   x = (x | (x << 8)) & 0x80403;
   x = (x | (x << 4)) & 0x804021;
   x = (x | (x << 2)) & 0x810081;
   x = (x | (x << 1)) & 0x1010101;

   return x;
   }

uint32_t compact_4x8(uint32_t x)
   {
   x &= 0x1010101;
   x = (x | (x << 1)) & 0x810081; 
   x = (x | (x << 2)) & 0x804021;
   x = (x | (x << 4)) & 0x80403;
   x = (x | (x << 8)) & 0x80007;
   x = (x | (x << 16)) & 0xF;
   return x;
   }

uint64_t encode(uint32_t x32)
   {
   uint64_t x = x32;
   x = (x ^ (x << 16)) & 0x0000FFFF0000FFFF;
   x = (x ^ (x <<  8)) & 0x00ff00ff00ff00ff;
   x = (x ^ (x <<  4)) & 0x0f0f0f0f0f0f0f0f;
   x = (x ^ (x <<  2)) & 0x3333333333333333;
   x = (x ^ (x <<  1)) & 0x5555555555555555;
   return x;
   }

uint32_t bit_permute_step(uint32_t x, uint32_t m, int shift) {
    uint32_t t;
    t = ((x >> shift) ^ x) & m;
    x = (x ^ t) ^ (t << shift);
    return x;
    }

int main()
   {


#if 0
   uint64_t x = 0xABCDEF00; // Interleave lower 16 bits of x and y, so the bits of x
uint64_t y = 0x69696939; // are in the even positions and bits from y in the odd;
uint64_t z = 0; // z gets the resulting 32-bit Morton Number.
                // x and y must initially be less than 65536.
printf("%016lX\n", compact(encode(x)));
printf("%016lX\n", compact(encode(y)));

x = (x | (x << S[4])) & B[4];
x = (x | (x << S[3])) & B[3];
x = (x | (x << S[2])) & B[2];
x = (x | (x << S[1])) & B[1];
x = (x | (x << S[0])) & B[0];

y = (y | (y << S[4])) & B[4];
y = (y | (y << S[3])) & B[3];
y = (y | (y << S[2])) & B[2];
y = (y | (y << S[1])) & B[1];
y = (y | (y << S[0])) & B[0];


printf("%016lX\n", x);
printf("%016lX\n", y);

z = x | (y << 1);

printf("%016lX\n", z);

printf("%08X\n", compact(z));
printf("%08X\n", compact(z >> 1));
#endif

uint32_t I[8] = { 0 };

const uint32_t x = 0xCAFEBABE;

uint32_t d = x;
d = bit_permute_step(d, 0x00aa00aa, 7);  // Bit index swap 0,3
d = bit_permute_step(d, 0x0000cccc, 14);  // Bit index swap 1,4
d = bit_permute_step(d, 0x00f000f0, 4);  // Bit index swap 2,3
d = bit_permute_step(d, 0x0000ff00, 8);  // Bit index swap 3,4

printf("d = %08X\n", d);

   printf("%08X\n", compact_32_4x8(x));
   printf("%08X\n", compact_32_4x8(x >> 1));
   printf("%08X\n", compact_32_4x8(x >> 2));
   printf("%08X\n", compact_32_4x8(x >> 3));
   printf("%08X\n", compact_32_4x8(x >> 4));
   printf("%08X\n", compact_32_4x8(x >> 5));
   printf("%08X\n", compact_32_4x8(x >> 6));
   printf("%08X\n", compact_32_4x8(x >> 7));
   // FIXME specialize this:
   // Something like this??
   // I[0] = compact(x & 0x80808080);
   // I[1] = compact((x << 1) & 0x80808080);
   for(size_t i = 0; i != sizeof(uint32_t); i++)
      {
      printf("I[0] gets x %d\n", 7+8*i);
      printf("I[1] gets x %d\n", 6+8*i);
      printf("I[2] gets x %d\n", 5+8*i);
      printf("I[3] gets x %d\n", 4+8*i);
      printf("I[4] gets x %d\n", 3+8*i);
      printf("I[5] gets x %d\n", 2+8*i);
      printf("I[6] gets x %d\n", 1+8*i);
      printf("I[7] gets x %d\n", 0+8*i);
      I[0] = (I[0] << 1) | ((x >> (7+8*i)) & 1);
      I[1] = (I[1] << 1) | ((x >> (6+8*i)) & 1);
      I[2] = (I[2] << 1) | ((x >> (5+8*i)) & 1);
      I[3] = (I[3] << 1) | ((x >> (4+8*i)) & 1);
      I[4] = (I[4] << 1) | ((x >> (3+8*i)) & 1);
      I[5] = (I[5] << 1) | ((x >> (2+8*i)) & 1);
      I[6] = (I[6] << 1) | ((x >> (1+8*i)) & 1);
      I[7] = (I[7] << 1) | ((x >> (0+8*i)) & 1);
      }

   printf("%08X %08X %08X %08X %08X %08X %08X %08X\n",
          I[0], I[1], I[2], I[3], I[4], I[5], I[6], I[7]);

   printf("%08X\n", compact_4x8(x));
   printf("%08X\n", compact_4x8(x >> 1));
   printf("%08X\n", compact_4x8(x >> 2));
   printf("%08X\n", compact_4x8(x >> 3));

   uint32_t f = 0;
   for(size_t i = 0; i != 8; ++i)
      f |= encode_4x8(I[i]) << (7-i);
   printf("%08X\n", f);
   }
