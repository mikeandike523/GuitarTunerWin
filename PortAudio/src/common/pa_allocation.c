 
#include "pa_allocation.h"
#include "pa_util.h"
#define PA_INITIAL_LINK_COUNT_    16
struct PaUtilAllocationGroupLink
{
struct PaUtilAllocationGroupLink *next;
void *buffer;
};
static struct PaUtilAllocationGroupLink *AllocateLinks( long count,
struct PaUtilAllocationGroupLink *nextBlock,
struct PaUtilAllocationGroupLink *nextSpare )
{
struct PaUtilAllocationGroupLink *result;
int i;
result = (struct PaUtilAllocationGroupLink *)PaUtil_AllocateMemory(
sizeof(struct PaUtilAllocationGroupLink) * count );
if( result )
{
result[0].buffer = result;
result[0].next = nextBlock;
for( i=1; i<count; ++i )
{
result[i].buffer = 0;
result[i].next = &result[i+1];
}
result[count-1].next = nextSpare;
}
return result;
}
PaUtilAllocationGroup* PaUtil_CreateAllocationGroup( void )
{
PaUtilAllocationGroup* result = 0;
struct PaUtilAllocationGroupLink *links;
links = AllocateLinks( PA_INITIAL_LINK_COUNT_, 0, 0 );
if( links != 0 )
{
result = (PaUtilAllocationGroup*)PaUtil_AllocateMemory( sizeof(PaUtilAllocationGroup) );
if( result )
{
result->linkCount = PA_INITIAL_LINK_COUNT_;
result->linkBlocks = &links[0];
result->spareLinks = &links[1];
result->allocations = 0;
}
else
{
PaUtil_FreeMemory( links );
}
}
return result;
}
void PaUtil_DestroyAllocationGroup( PaUtilAllocationGroup* group )
{
struct PaUtilAllocationGroupLink *current = group->linkBlocks;
struct PaUtilAllocationGroupLink *next;
while( current )
{
next = current->next;
PaUtil_FreeMemory( current->buffer );
current = next;
}
PaUtil_FreeMemory( group );
}
void* PaUtil_GroupAllocateMemory( PaUtilAllocationGroup* group, long size )
{
struct PaUtilAllocationGroupLink *links, *link;
void *result = 0;
if( !group->spareLinks )
{
links = AllocateLinks( group->linkCount, group->linkBlocks, group->spareLinks );
if( links )
{
group->linkCount += group->linkCount;
group->linkBlocks = &links[0];
group->spareLinks = &links[1];
}
}
if( group->spareLinks )
{
result = PaUtil_AllocateMemory( size );
if( result )
{
link = group->spareLinks;
group->spareLinks = link->next;
link->buffer = result;
link->next = group->allocations;
group->allocations = link;
}
}
return result;
}
void PaUtil_GroupFreeMemory( PaUtilAllocationGroup* group, void *buffer )
{
struct PaUtilAllocationGroupLink *current = group->allocations;
struct PaUtilAllocationGroupLink *previous = 0;
if( buffer == 0 )
return;
while( current )
{
if( current->buffer == buffer )
{
if( previous )
{
previous->next = current->next;
}
else
{
group->allocations = current->next;
}
current->buffer = 0;
current->next = group->spareLinks;
group->spareLinks = current;
break;
}
previous = current;
current = current->next;
}
PaUtil_FreeMemory( buffer );  
}
void PaUtil_FreeAllAllocations( PaUtilAllocationGroup* group )
{
struct PaUtilAllocationGroupLink *current = group->allocations;
struct PaUtilAllocationGroupLink *previous = 0;
while( current )
{
PaUtil_FreeMemory( current->buffer );
current->buffer = 0;
previous = current;
current = current->next;
}
if( previous )
{
previous->next = group->spareLinks;
group->spareLinks = group->allocations;
group->allocations = 0;
}
}
