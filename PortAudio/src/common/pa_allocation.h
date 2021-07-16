#ifndef PA_ALLOCATION_H
#define PA_ALLOCATION_H
#ifdef __cplusplus
extern "C"
{
#endif  
typedef struct
{
long linkCount;
struct PaUtilAllocationGroupLink *linkBlocks;
struct PaUtilAllocationGroupLink *spareLinks;
struct PaUtilAllocationGroupLink *allocations;
}PaUtilAllocationGroup;
PaUtilAllocationGroup* PaUtil_CreateAllocationGroup( void );
void PaUtil_DestroyAllocationGroup( PaUtilAllocationGroup* group );
void* PaUtil_GroupAllocateMemory( PaUtilAllocationGroup* group, long size );
void PaUtil_GroupFreeMemory( PaUtilAllocationGroup* group, void *buffer );
void PaUtil_FreeAllAllocations( PaUtilAllocationGroup* group );
#ifdef __cplusplus
}
#endif  
#endif  
