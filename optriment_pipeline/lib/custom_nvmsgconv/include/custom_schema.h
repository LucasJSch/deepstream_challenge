
#ifndef NVCUSTOMMSGCONV_H_
#define NVCUSTOMMSGCONV_H_

#include <glib.h>
#include <unordered_map>

using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

struct NvDsScheinkObject
{
  std::unordered_map<std::string, int> zoneCounts;
};

#ifdef __cplusplus
}
#endif
#endif /* NVCUSTOMMSGCONV_H_ */
