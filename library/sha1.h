#ifndef SHA1_H
#define SHA1_H

#include <string>
#include <openssl/sha.h>

std::string sha1(const std::string& in)
{
  const char lookup[] = "0123456789abcdef";
  //do a messy cast from char to unsigned char. Not sure yet what this will mean for wider character sets. Seems risky.
  const unsigned char* string_data = reinterpret_cast<const unsigned char*>(in.data());
  unsigned char hash[SHA_DIGEST_LENGTH];

  SHA1(string_data, in.size(), hash);

  std::string hash_string(SHA_DIGEST_LENGTH*2,'0');
  size_t string_index = 0;//hex values are packed in pairs within each element hash[i]
  for(size_t i = 0; i!=SHA_DIGEST_LENGTH; ++i)
  {
    hash_string[string_index++] = lookup[(hash[i]>>4)&0x0F];//unpack the first
    hash_string[string_index++] = lookup[ hash[i]    &0x0F];//unpack the second
  }
  return hash_string;
}
#endif
