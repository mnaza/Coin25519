/* Taken from silc, and the code and all changes to it are in public domain. --Pieter */
/* Taken from public domain libtomcrypt library and the code and all changes
   to it are in public domain. -Pekka */
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */

#define _SHA256_C_ 1

#include <stdint.h>
#include <string.h>

#include "sha256.h"

#define TRUE 1
#define FALSE 0

static inline void get32(uint32_t *ptr, const unsigned char *data)
{
  *ptr = 0;
  for (int i=0; i<4; i++)
  {
    *ptr |= (uint32_t)(data[i]) << (24-8*i);
  }
}

static inline void put32(uint32_t val, unsigned char *data)
{
  for (int i=0; i<4; i++)
  {
    data[3-i] = val & 0xFF;
    val >>= 8;
  }
}

static inline void put64(uint64_t val, unsigned char *data)
{
  for (int i=0; i<8; i++)
  {
    data[7-i] = val & 0xFF;
    val >>= 8;
  }
}

#define GET32(val,data) get32(&(val),data)
#define PUT32(val,data) put32((val),data)
#define PUT64(val,data) put64((val),data)

/* Various logical functions */
#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         (((x) >> (n)) | (x) << ((32-(n))))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

/* Transform 512-bits */
void static sha256_transform(uint32_t *state, unsigned char *buf)
{
  uint32_t S[8], W[64], t0, t1;
  int i;

  /* copy state into S */
  for (i = 0; i < 8; i++) {
    S[i] = state[i];
  }

  /* copy the state into 512-bits into W[0..15] */
  for (i = 0; i < 16; i++)
    GET32(W[i], buf + (4 * i));

  /* fill W[16..63] */
  for (i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#define RND(a,b,c,d,e,f,g,h,i,ki)               \
  t0 = h + Sigma1(e) + Ch(e, f, g) + ki + W[i]; \
  t1 = Sigma0(a) + Maj(a, b, c);                \
  d += t0;                                      \
  h  = t0 + t1;

  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],0,0x428a2f98);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],1,0x71374491);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],2,0xb5c0fbcf);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],3,0xe9b5dba5);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],4,0x3956c25b);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],5,0x59f111f1);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],6,0x923f82a4);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],7,0xab1c5ed5);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],8,0xd807aa98);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],9,0x12835b01);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],10,0x243185be);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],11,0x550c7dc3);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],12,0x72be5d74);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],13,0x80deb1fe);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],14,0x9bdc06a7);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],15,0xc19bf174);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],16,0xe49b69c1);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],17,0xefbe4786);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],18,0x0fc19dc6);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],19,0x240ca1cc);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],20,0x2de92c6f);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],21,0x4a7484aa);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],22,0x5cb0a9dc);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],23,0x76f988da);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],24,0x983e5152);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],25,0xa831c66d);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],26,0xb00327c8);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],27,0xbf597fc7);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],28,0xc6e00bf3);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],29,0xd5a79147);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],30,0x06ca6351);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],31,0x14292967);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],32,0x27b70a85);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],33,0x2e1b2138);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],34,0x4d2c6dfc);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],35,0x53380d13);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],36,0x650a7354);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],37,0x766a0abb);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],38,0x81c2c92e);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],39,0x92722c85);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],40,0xa2bfe8a1);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],41,0xa81a664b);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],42,0xc24b8b70);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],43,0xc76c51a3);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],44,0xd192e819);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],45,0xd6990624);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],46,0xf40e3585);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],47,0x106aa070);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],48,0x19a4c116);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],49,0x1e376c08);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],50,0x2748774c);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],51,0x34b0bcb5);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],52,0x391c0cb3);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],53,0x4ed8aa4a);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],54,0x5b9cca4f);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],55,0x682e6ff3);
  RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],56,0x748f82ee);
  RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],57,0x78a5636f);
  RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],58,0x84c87814);
  RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],59,0x8cc70208);
  RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],60,0x90befffa);
  RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],61,0xa4506ceb);
  RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],62,0xbef9a3f7);
  RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],63,0xc67178f2);

#undef RND

  /* feedback */
  for (i = 0; i < 8; i++) {
    state[i] = state[i] + S[i];
  }
}

int sha256_init(sha256_state * md)
{
  md->length = 0;
  md->curlen = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;
  return TRUE;
}

#if !defined(MIN)
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

int sha256_process(sha256_state * md, const unsigned char *in,
                   unsigned long inlen)
{
  unsigned long n;
  int block_size = sizeof(md->buf);

  if (md->curlen > block_size)
    return FALSE;

  while (inlen > 0) {
    if (md->curlen == 0 && inlen >= block_size) {
      sha256_transform(md->state, (unsigned char *)in);
      md->length     += block_size * 8;
      in             += block_size;
      inlen          -= block_size;
    } else {
      n = MIN(inlen, (block_size - md->curlen));
      memcpy(md->buf + md->curlen, in, (size_t)n);
      md->curlen     += n;
      in             += n;
      inlen          -= n;
      if (md->curlen == block_size) {
        sha256_transform(md->state, md->buf);
        md->length += block_size * 8;
        md->curlen = 0;
      }
    }
  }
  return TRUE;
}

int sha256_done(sha256_state * md, unsigned char *out)
{
  int i;

  if (md->curlen >= sizeof(md->buf))
    return FALSE;

  /* increase the length of the message */
  md->length += md->curlen * 8;

  /* append the '1' bit */
  md->buf[md->curlen++] = (unsigned char)0x80;

  /* if the length is currently above 56 bytes we append zeros
   * then compress.  Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  if (md->curlen > 56) {
    while (md->curlen < 64) {
      md->buf[md->curlen++] = (unsigned char)0;
    }
    sha256_transform(md->state, md->buf);
    md->curlen = 0;
  }

  /* pad upto 56 bytes of zeroes */
  while (md->curlen < 56) {
    md->buf[md->curlen++] = (unsigned char)0;
  }

  /* store length */
  PUT64(md->length, md->buf + 56);
  sha256_transform(md->state, md->buf);

  /* copy output */
  for (i = 0; i < 8; i += 2) {
    PUT32(md->state[i], out + (4 * i));
    PUT32(md->state[i + 1], out + (4 * (i + 1)));
  }

  return TRUE;
}

int sha256(const unsigned char *in, unsigned long inlen, unsigned char *out)
{
  sha256_state state;
  sha256_init(&state);
  sha256_process(&state, in, inlen);
  sha256_done(&state, out);
  return TRUE;
}
