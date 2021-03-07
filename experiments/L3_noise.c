#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>



#define PAGE_BITS 12
#define LINE_BITS 6
#define SLICE_BITS 2
#define SET_BITS 13

#define PAGE_SIZE2 (1 << PAGE_BITS)
#define LINE_SIZE (1 << LINE_BITS)
#define CACHE_SLICES (1 << SLICE_BITS)
#define CACHE_SETS (1 << SET_BITS)


//get the paddress from the proc/pagmap info and the page offset from the virtual address offset
unsigned long long 
vtop(unsigned long long vaddr)
{
        char path[1024];
        int fd = open ("/proc/self/pagemap", O_RDONLY);
        if (fd < 0)
        {
                return -1;
        }

        unsigned long long paddr = -1;
        unsigned long long index = (vaddr / PAGE_SIZE2) * sizeof(paddr);
        if (pread (fd, &paddr, sizeof(paddr), index) != sizeof(paddr))
        {
                return -1;
        }
        close (fd);
        paddr &= 0x7fffffffffffff;
        return (paddr << PAGE_BITS) | (vaddr & (PAGE_SIZE2-1));
}

unsigned long long
vtop_other_procces(unsigned pid, unsigned long long vaddr)
{
	char path[1024];
	sprintf (path, "/proc/%u/pagemap", pid);
	int fd = open (path, O_RDONLY);
	if (fd < 0)
	{
		return -1;
	}

	unsigned long long paddr = -1;
	unsigned long long index = (vaddr / PAGE_SIZE2) * sizeof(paddr);
	if (pread (fd, &paddr, sizeof(paddr), index) != sizeof(paddr))
	{
		return -1;
	}
	close (fd);
	paddr &= 0x7fffffffffffff;
	return (paddr << PAGE_BITS) | (vaddr & (PAGE_SIZE2-1));
}

unsigned int
count_bits(unsigned long long n)
{
	unsigned int count = 0;
	while (n)
	{

		n &= (n-1) ;
		count++;
	}
	return count;
}

unsigned int
nbits(unsigned long long n)
{
	unsigned int ret = 0;
	n = n >> 1;
	while (n > 0)
	{
		n >>= 1;
		ret++;
	}
	return ret;
}

unsigned long  long
ptos(unsigned long long paddr, unsigned long long bits)
{
	unsigned long long ret = 0;
	unsigned long long mask[3] = {0x1b5f575440ULL, 0x2eb5faa880ULL, 0x3cccc93100ULL}; // according to Maurice et al.
	switch (bits)
	{
		case 3:
			ret = (ret << 1) | (unsigned long long)(count_bits(mask[2] & paddr) % 2);
		case 2:
			ret = (ret << 1) | (unsigned long long)(count_bits(mask[1] & paddr) % 2);
		case 1:
			ret = (ret << 1) | (unsigned long long)(count_bits(mask[0] & paddr) % 2);
		default:
		break;
	}
	return ret;
}

void
check2(unsigned long long virtual_addresses, unsigned int length)
{
	unsigned int cache_sets = 8192;

	unsigned long long paddr = 0, cacheset = 0, slice = 0;

	for (unsigned int i = 0; i < length; i++)
	{
		paddr = vtop (virtual_addresses);
		cacheset = (paddr >> LINE_BITS) & (CACHE_SETS - 1); //take the cache set as the number of bits for the cache while ignoring the line bits
		slice = ptos (paddr, SLICE_BITS);
		printf(" - element pfn: 0x%llx, cache set: %lli, slice: %lli\n", paddr, cacheset, slice);
	}
}

void
check(unsigned long long virtual_addresses, unsigned int length)
{
        unsigned int cache_sets = 8192;

        unsigned long long paddr = 0, cacheset = 0, slice = 0;

        for (unsigned int i = 0; i < length; i++)
        {
                paddr = vtop (virtual_addresses);
                cacheset = (paddr >> LINE_BITS) & (CACHE_SETS - 1); //take the cache set as the number of bits for the cache while ignoring the line bits
                slice = ptos (paddr, SLICE_BITS);
                printf("%lli\n",cacheset);
		printf("%lli\n",slice);
        }
}
volatile char buffer[4096];

int main(int ac, char **av) {
long seconds = time(NULL), timeUp = seconds + 30;
unsigned long long setnumber = vtop(buffer);
//printf("addres of buffer[800]:");
check(&buffer[800],1);
//printf("addres of buffer[1800]:");
//check(&buffer[1800],1);
fflush(stdout);
  while(seconds < timeUp)
 {
    for (int i = 0; i < 64000; i++)
      buffer[800] += i;

    for (int i = 0; i < 64000; i++)
      buffer[1800] += i;

    for (int i = 0; i < 64000; i++)
      buffer[800] += i;
    for (int i = 0; i < 64000; i++)
      buffer[800] += i;
    for (int i = 0; i < 64000; i++)
      buffer[800] += i;

    for (int i = 0; i < 64000; i++)
      buffer[1800] += i;
    for (int i = 0; i < 64000; i++)
      buffer[1800] += i;
    for (int i = 0; i < 64000; i++)
      buffer[1800] += i;
    seconds = time(NULL);
  }
 
}

