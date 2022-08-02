#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define two31 (1UL << 31)
#define two24 (1UL << 24)
#define two23 (1UL << 23)
#define two22 (1UL << 22)
#define two20 (1UL << 20)
#define two16 (1UL << 16)
#define two13 (1UL << 13)
#define two12 (1UL << 12)

#define mask24 ((1UL << 24) - 1UL)
#define mask23 ((1UL << 23) - 1UL)
#define mask22 ((1UL << 22) - 1UL)
#define mask16 ((1UL << 16) - 1UL)
#define mask13 ((1UL << 13) - 1UL)
#define mask12 ((1UL << 12) - 1UL)
#define mask9  ((1UL <<  9) - 1UL)

#define cfixxExitError 55

#define cfixxLookupStart 0x7f0104846000UL
#define cfixxColdStart 0x7f53feacb000UL

#define CFIXX_PROTECT_METADATA_MPROTECT
#define CFIXX_PROTECT_METADATA_PKEYS

void cfixxEnableMetadataWrites(void *thisPtr);
void cfixxDisableMetadataWrites(void *thisPtr);

void cfixxEnableMetadataWritesAll();
void cfixxDisableMetadataWritesAll();

//actual lookup table
//void **cfixxLookup = NULL;
static void **cfixxNextSecondLevel = NULL;
void **cfixxTableEnd = NULL;
static unsigned long cfixxSecondLevelTables = 100UL;

#ifdef CFIXX_PROTECT_METADATA_PKEYS
int pkey;
#endif

//trade some space for one fewer if check on the hot path
//this trade is only an issue if we want to "fail open"
//void **cfixxCold = NULL;




//__attribute__((constructor(0)))
void cfixxInitialization(){
  size_t len = two22*sizeof(void*) + two23*sizeof(void*)*cfixxSecondLevelTables;
  size_t len2 = two23*sizeof(void*);
  //cfixxLookup = (void **)mmap((void*)cfixxLookupStart, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(mmap((void*)cfixxLookupStart, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED){
    fprintf(stderr, "[CFIXX ERROR] Mapping lookup table failed\n");
    exit(cfixxExitError);
  }
  //cfixxCold   = (void **)mmap((void*)cfixxColdStart, len2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(mmap((void*)cfixxColdStart, len2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED){
    fprintf(stderr, "[CFIXX ERROR] Mapping cold path table failed\n");
    exit(cfixxExitError);
  }

  for(unsigned long i = 0; i < two22; ++i){
    ((void**)cfixxLookupStart)[i] = (void *)cfixxColdStart;
  }

  //since this is a void** compiler will adjust by sizeof(void*)
  cfixxNextSecondLevel = (void *)cfixxLookupStart + two22;
  //len was raw bytes, have to adjust - compiler will automatically do the size adjustment.  sigh.
  cfixxTableEnd = (void*)cfixxLookupStart + len/sizeof(void*);

  //fprintf(stderr, "[CFIXX Log] Enabled. lookup: %p\tlen: %lx\n", (void*)cfixxLookupStart, len);
  #ifdef CFIXX_PROTECT_METADATA_PKEYS
  pkey = pkey_alloc(0, PKEY_DISABLE_WRITE);
  #endif
  cfixxDisableMetadataWritesAll();
}

//__attribute__((noinline))
void *cfixxSlow(unsigned long idx1){
  //create the second level lookup table
  //void **tmp2 = (void **)mmap(NULL, two23*sizeof(void*), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
  //link the second level table into the first level
  
  //bump allocator fanciness
  if(cfixxNextSecondLevel >= cfixxTableEnd){
    fprintf(stderr, "[CFIXX ERROR]: Not enough second level tables.  Allocated %lu\n", cfixxSecondLevelTables);
    exit(cfixxExitError);
  }
  void **tmp2 = cfixxNextSecondLevel;
  ((void **)cfixxLookupStart)[idx1] = tmp2;
  cfixxNextSecondLevel += two23;

  return tmp2;
}

void cfixxFakeMetadataWrite(void *, void **);
void cfixxFakeMPXCheck(void *);



void cfixxSetVTablePtr(void *thisPtr, void *vtablePtr){
  cfixxEnableMetadataWritesAll();
  /*Do the 2 level lookup*/
  // get the pointer to the second level table from the top level
  unsigned long idx1 = (unsigned long)thisPtr >> 26 & mask22;
  void **level2 = ((void***)cfixxLookupStart)[idx1];

  //get the index in the second level
  unsigned long idx2 = (unsigned long)thisPtr >> 3 & mask23;

  //see if we are on the cold path
  if(level2 == (void*)cfixxColdStart){
    level2 = cfixxSlow(idx1);
  }

  //store the metadata
  level2[idx2] = vtablePtr;
  cfixxDisableMetadataWritesAll();

  return;
}

void *cfixxVTable(void *thisPtr){
  //do the multilevel lookup
  unsigned long idx1 = (unsigned long)thisPtr >> 26 & mask22;
  unsigned long idx2 = (unsigned long)thisPtr >> 3 & mask23;
  void **level2 = ((void ***)cfixxLookupStart)[idx1];
  //if the second level hasn't been allocated, this ends up on the cfixxCold table, 
  //which has all NULL entries
  void *ret = level2[idx2];
  
  //Fail open.  Until we recompile standard libraries, we coudl miss some constructors
  //of objects, but still see the virtual calls.
  /*if(!ret){
    return (void *)(*((size_t*)thisPtr));
  }
  */
  return ret;
}

void cfixxDtor(void *thisPtr){
  unsigned long idx1 = (unsigned long)thisPtr >> 26 & mask22;
  unsigned long idx2 = (unsigned long)thisPtr >> 3 & mask23;
  void **level2 = ((void ***)cfixxLookupStart)[idx1];
  level2[idx2] = NULL;
  return;
}

typedef struct MetadataAddresses {
  void *level1;
  void *level2;
} MetadataAddresses;

MetadataAddresses getMetadataAddresses(void* this_ptr) {
  size_t index1 = (size_t)this_ptr >> 26 & mask22;
  size_t index2 = (size_t)this_ptr >> 3 & mask23;
  size_t *level1 = (size_t*)(cfixxLookupStart + index1);
  size_t *level2_base = (size_t*)(((size_t*)cfixxLookupStart)[index1]);
  size_t *level2 = level2_base + index2;

  long pagesize = sysconf(_SC_PAGE_SIZE);
  MetadataAddresses ret;
  ret.level1 = (void*)(pagesize * ((size_t)level1/pagesize));
  ret.level2 = (void*)(pagesize * ((size_t)level2/pagesize));
  return ret;
}



void
cfixxEnableMetadataWrites(void *thisPtr)
{
  #ifdef CFIXX_PROTECT_METADATA_MRPOTECT
  MetadataAddresses adr = getMetadataAddresses(thisPtr);
  fprintf(stderr, "[CFIXX LOG] this ptr: %lu\n", (size_t)thisPtr);
  fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites specific: level 1: %lu\tlevel2: %lu\n", (size_t)adr.level1, (size_t)adr.level2);

  if (mprotect(adr.level1, sysconf(_SC_PAGE_SIZE), PROT_WRITE))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites specific - mprotect level1 failed with %s\n", strerror(errno));
    exit(cfixxExitError);
  }

  if (mprotect(adr.level2, sysconf(_SC_PAGE_SIZE), PROT_WRITE))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites specific - mprotect level2 failed with %s\n", strerror(errno));
    exit(cfixxExitError);
  }
  #endif
  #ifdef CFIXX_PROTECT_METADATA_PKEYS
  MetadataAddresses adr = getMetadataAddresses(thisPtr);
  fprintf(stderr, "[CFIXX LOG] this ptr: %lu\n", (size_t)thisPtr);
  fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites specific: level 1: %lu\tlevel2: %lu\n", (size_t)adr.level1, (size_t)adr.level2);

  if (pkey_mprotect(adr.level1, sysconf(_SC_PAGE_SIZE), PROT_WRITE, pkey))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites specific - mprotect level1 failed with %s\n", strerror(errno));
    exit(cfixxExitError);
  }

  if (pkey_mprotect(adr.level2, sysconf(_SC_PAGE_SIZE), PROT_WRITE, pkey))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites specific - mprotect level2 failed with %s\n", strerror(errno));
    exit(cfixxExitError);
  }
  #endif
}

void
cfixxDisableMetadataWrites(void *thisPtr)
{
  #ifdef CFIXX_PROTECT_METADATA
  MetadataAddresses adr = getMetadataAddresses(thisPtr);

  if (mprotect(adr.level1, sysconf(_SC_PAGE_SIZE), PROT_READ))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxDisableMetadataWrites specific - mprotect level1 failed\n");
    exit(cfixxExitError);
  }

  if (mprotect(adr.level2, sysconf(_SC_PAGE_SIZE), PROT_READ))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxDisableMetadataWrites specific - mprotect level2 failed\n");
    exit(cfixxExitError);
  }
  #endif
  #ifdef CFIXX_PROTECT_METADATA_PKEYS
  MetadataAddresses adr = getMetadataAddresses(thisPtr);

  if (pkey_mprotect(adr.level1, sysconf(_SC_PAGE_SIZE), PROT_READ, pkey))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxDisableMetadataWrites specific - mprotect level1 failed\n");
    exit(cfixxExitError);
  }

  if (pkey_mprotect(adr.level2, sysconf(_SC_PAGE_SIZE), PROT_READ, pkey))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxDisableMetadataWrites specific - mprotect level2 failed\n");
    exit(cfixxExitError);
  }
  #endif
}

void cfixxEnableMetadataWritesAll()
{
  #ifdef CFIXX_PROTECT_METADATA

  size_t tableLength = (size_t)cfixxTableEnd - (size_t)cfixxLookupStart;
  if (mprotect((void *)cfixxLookupStart, tableLength, PROT_WRITE))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites mprotect failed\n");
    exit(cfixxExitError);
  }
  #endif
  #ifdef CFIXX_PROTECT_METADATA_PKEYS
  size_t tableLength = (size_t)cfixxTableEnd - (size_t)cfixxLookupStart;
  if (pkey_mprotect((void *)cfixxLookupStart, tableLength, PROT_WRITE, pkey))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxEnableMetadataWrites mprotect failed\n");
    exit(cfixxExitError);
  }
  #endif
}

void cfixxDisableMetadataWritesAll()
{
  #ifdef CFIXX_PROTECT_METADATA
  size_t tableLength = (size_t)cfixxTableEnd - (size_t)cfixxLookupStart;
  if (mprotect((void *)cfixxLookupStart, tableLength, PROT_READ))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxDisableMetadataWrites mprotect failed\n");
    exit(cfixxExitError);
  }
  #endif
  #ifdef CFIXX_PROTECT_METADATA_PKEYS
  size_t tableLength = (size_t)cfixxTableEnd - (size_t)cfixxLookupStart;
  if (pkey_mprotect((void *)cfixxLookupStart, tableLength, PROT_READ, pkey))
  {
    fprintf(stderr, "[CFIXX ERROR] cfixxDisableMetadataWrites mprotect failed\n");
    exit(cfixxExitError);
  }
  #endif
}

__attribute__((section(".preinit_array"), used)) void (*_cfixx_preinit)(void) = cfixxInitialization;
