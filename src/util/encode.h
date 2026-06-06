#ifndef _ENCODE_H_
#define _ENCODE_H_


#ifdef __cplusplus
extern "C" {
#endif

const char * map_encode(const char * dat);
const char * map_decode(const char * dat);

const char * hex_encode(const char * dat);
const char * hex_decode(const char * dat);

#ifdef __cplusplus
}
#endif

#endif /* _ENCODE_H_ */
