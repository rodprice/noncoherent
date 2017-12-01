
#include "pmtk.h"
#include "util.h"
#include "ringbuffer.h"


/* Convert an ASCII character into its hex equivalent */
static uint8_t ascii2hex(char ch) {
  if ((48 <= ch) && (ch <58))
    return ch-48;
  if ((65 <= ch) && (ch < 71))
    return ch - 55;
  if ((97 <= ch) && (ch < 107))
    return ch - 87;
  return 0xFF;
}

/* Convert a number between 0 and 15 into ascii hex */
static uint8_t hex2ascii(uint8_t byte) {
  if (byte < 10)
    return byte + 48;
  if (byte < 16)
    return byte + 55;
  return 0xFF;
}

/* Convert two consecutive hex characters into a number */
static uint8_t hex2byte(ringbuffer *rb, uint8_t n) {
  uint8_t lsb, msb;
  lsb = ascii2hex(rbpeek(rb, n));
  msb = ascii2hex(rbpeek(rb, n+1));
  return (msb << 4) + lsb;
}

/* Convert a number into two consecutive characters */
static uint8_t byte2hex(ringbuffer *rb, uint8_t byte) {
  uint8_t code = 0;
  char lsb, msb;
  lsb = hex2ascii(byte & 0x0F);
  msb = hex2ascii(byte >> 4);
  code |= rbput(rb, msb);
  code |= rbput(rb, lsb);
  return code == 0;
}

/* Compute checksum for a PMTK sentence */
uint8_t pmtk_checksum(ringbuffer *rb) {
  uint8_t i, n, ch, sum = 0;
  n = rblen(rb);
  if (n > 0) {
    if (rbpeek(rb, 0) != '$')
      return 0xFF;
    for (i=1; i<n; i++) {
      ch = rbpeek(rb, i);
      if (ch == '*')
        return sum;
      sum ^= ch;
    }
  }
  return 0xFF;
}

/* Confirm that the checksum in a sentence is correct */
bool pmtk_checksum_confirm(ringbuffer *rb) {
  uint8_t i, n, msb, lsb, sum, star = 0;
  /* find out where the '*' is */
  n = rblen(rb);
  for (i=0; i<n; i++) {
    if (rbpeek(rb, i) == '*') {
      star = i;
      break;
    }
  }
  /* if no '*' or less than two chars left, bail */
  if (star == 0 || (n-star < 3))
    return false;
  /* convert the next two chars into a number */
  msb = ascii2hex(rbpeek(rb, star+1));
  lsb = ascii2hex(rbpeek(rb, star+2));
  sum = (msb << 4) + lsb;
  /* see if the checksums match */
  return sum == pmtk_checksum(rb);
}

bool pmtk_make_sentence_header(ringbuffer *rb, char* type) {
  /* PMTK string should be the only thing in the buffer */
  if (!rbempty(rb))
    return false;
  if (rbconcat(rb, "$PMTK", 5) == error) return false;
  if (rbconcat(rb, type, 3)    == error) return false;
  if (rbput(rb, ',')           == error) return false;
  return true;
}

bool pmtk_make_sentence_footer(ringbuffer *rb) {
  uint8_t sum;
  if (rbput(rb,'*') == error) return false;
  sum = pmtk_checksum(rb);
  if (byte2hex(rb, sum) == error) return false;
  if (rbput(rb, '\r')   == error) return false;
  if (rbput(rb, '\n')   == error) return false;
  return true;
}
