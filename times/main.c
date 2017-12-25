//
// Tomás Oliveira e Silva, AED, November 2017
//
// sorting algorithms
//
// modify the code to:
// 1) measure the execution time of each sorting function
// 2) count the number of comparisons made by each sorting algorithm
// 3) for a wide range of array sizes, say, starting at 10 and ending at 1000000, on a logarithmic scale,
//    report the average and, if possible, the standard deviation, of at least 100 sorting attempts for each value of n
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef int T;
typedef void (*sort_function)(int *,int,int);
int counter;

////////////////////////////////////////////////////////////////////////////
double elapsed_time(void)
{
	static struct timespec last_time,current_time;

	last_time = current_time;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&current_time) != 0)  // the first argument could also be CLOCK_REALTIME
		return -1.0; // clock_gettime() failed!!!
	return ((double)current_time.tv_sec - (double)last_time.tv_sec) +
		1.0e-9 * ((double)current_time.tv_nsec - (double)last_time.tv_nsec);
}
////////////////////////////////////////////////////////////////////////////
//(void)elapsed_time();
//dt = elapsed_time();

void show(T *data,int first,int one_after_last)
{
  int i;

  //printf("[%2d,%2d]",first,one_after_last - 1);
  //for(i = first;i < one_after_last;i++)
  //  printf(" %5d",data[i]);
  //printf("\n");
}

//
// TP.08.1
//

void bubble_sort(T *data,int first,int one_after_last)
{
  int i,i_low,i_high,i_last;

  i_low = first;
  i_high = one_after_last - 1;
  while(i_low < i_high)
  {
    for(i = i_last = i_low;i < i_high;i++)
      if(data[i] > data[i + 1])
      {
        T tmp = data[i];
        data[i] = data[i + 1];
        data[i + 1] = tmp;
        i_last = i;
      }
    i_high = i_last;
  }
}

void shaker_sort(T *data,int first,int one_after_last)
{
  int i,i_low,i_high,i_last;

  i_low = first;
  i_high = one_after_last - 1;
  while(i_low < i_high)
  {
    // up pass
    for(i = i_last = i_low;i < i_high;i++)
      if(data[i] > data[i + 1])
      {
        T tmp = data[i];
        data[i] = data[i + 1];
        data[i + 1] = tmp;
        i_last = i;
      }
    i_high = i_last;
    // down pass
    for(i = i_last = i_high;i > i_low;i--)
      if(data[i] < data[i - 1])
      {
        T tmp = data[i];
        data[i] = data[i - 1];
        data[i - 1] = tmp;
        i_last = i;
      }
    i_low = i_last;
  }
}


//
// TP.08.2
//

void insertion_sort(T *data,int first,int one_after_last)
{
  int i,j;

  for(i = first + 1;i < one_after_last;i++)
  {
    T tmp = data[i];
    for(j = i;j > first && tmp < data[j - 1];j--)
      data[j] = data[j - 1];
    data[j] = tmp;
  }
}

void shell_sort(T *data,int first,int one_after_last)
{
  int i,j,h;

  for(h = 1;h < (one_after_last - first) / 3;h = 3 * h + 1)
    ;
  while(h >= 1)
  { // for each stride h, use insertion sort
    for(i = first + h;i < one_after_last;i++)
    {
      T tmp = data[i];
      for(j = i;j - h >= first && tmp < data[j - h];j -= h)
        data[j] = data[j - h];
      data[j] = tmp;
    }
    h /= 3;
  }
}


//
// TP.08.3
//

void quick_sort(T *data,int first,int one_after_last)
{
  int i,j,one_after_small,first_equal,n_smaller,n_larger,n_equal;
  T pivot,tmp;

  if(one_after_last - first < 20)
    insertion_sort(data,first,one_after_last);
  else
  {
    //
    // select pivot (median of three, the pivot's position will be one_after_last-1)
    //
#   define POS1  (first)
#   define POS2  (one_after_last - 1)
#   define POS3  ((first + one_after_last) / 2)
#   define TEST(pos1,pos2)  do if(data[pos1] > data[pos2])                                      \
                               { tmp = data[pos1]; data[pos1] = data[pos2]; data[pos2] = tmp; } \
                               while(0)
    TEST(POS1,POS2);  // bitonic
    TEST(POS1,POS3);  // sort of
    TEST(POS2,POS3);  // 3 items
#   undef POS1
#   undef POS2
#   undef POS3
#   undef TEST
    //
    // 3-way partition. At the end of the while loop the items will be partitioned as follows:
    // |first  "smaller part"|one_after_small  "larger part"|first_equal  "equal part"|one_after_last
    //
    one_after_small = first;
    first_equal = one_after_last - 1;
    pivot = data[first_equal];
    i = first;
    while(i < first_equal)
      if(data[i] < pivot)
      { // place data[i] in the "smaller than the pivot" part of the array
        tmp = data[i];
        data[i] = data[one_after_small]; // tricky! this does the right thing when
        data[one_after_small] = tmp;     //   i == one_after_small and when i > one_after_small
        i++;
        one_after_small++;
      }
      else if(data[i] == pivot)
      { // place data[i] in the "equal to the pivot" part of the array
        first_equal--;
        tmp = data[i];               // this is known to be the pivot, but we do it in this way
        data[i] = data[first_equal]; //   to make life easier to those that need to adapt this
        data[first_equal] = tmp;     //   code so that it deals with more complex data items
      }
      else
      { // data[i] becomes automatically part of the "larger than the pivot" part of the array
        i++;
      }
    n_smaller = one_after_small - first;
    n_larger = first_equal - one_after_small;
    n_equal = one_after_last - first_equal;
    j = (n_equal < n_larger) ? n_equal : n_larger;
    for(i = 0;i < j;i++)
    { // move the "equal to the pivot" part of the array to the middle
      tmp = data[one_after_small + i];
      data[one_after_small + i] = data[one_after_last - 1 - i];
      data[one_after_last - 1 - i] = tmp;
    }
    //
    // recurse
    //
    quick_sort(data,first,first + n_smaller);
    quick_sort(data,first + n_smaller + n_equal,one_after_last);
  }
}


//
// TP.08.4
//

void merge_sort(T *data,int first,int one_after_last)
{
  int i,j,k,middle;
  T *buffer;

  if(one_after_last - first < 40) // do not allocate less than 40 bytes
    insertion_sort(data,first,one_after_last);
  else
  {
    middle = (first + one_after_last) / 2;
    merge_sort(data,first,middle);
    merge_sort(data,middle,one_after_last);
    buffer = (T *)malloc((size_t)(one_after_last - first) * sizeof(T)) - first; // no error check!
    i = first;  // first input (first half)
    j = middle; // second input (second half)
    k = first;  // merged output
    while(k < one_after_last)
      if(j == one_after_last || (i < middle && data[i] <= data[j]))
        buffer[k++] = data[i++];
      else
        buffer[k++] = data[j++];
    for(i = first;i < one_after_last;i++)
      data[i] = buffer[i];
    free(buffer + first);
  }
}


//
// TP.08.5
//

void heap_sort(T *data,int first,int one_after_last)
{
  int i,j,k,n;
  T tmp;

  data += first - 1;          // adjust pointer (data[first] becomes data[1])
  n = one_after_last - first; // number of items to sort
  //
  // phase 1. heap construction
  //
  for(i = n / 2;i >= 1;i--)
    for(j = i;2 * j <= n;j = k)
    {
      k = (2 * j + 1 <= n && data[2 * j + 1] > data[2 * j]) ? 2 * j + 1 : 2 * j;
      if(data[j] >= data[k])
        break;
      tmp = data[j];
      data[j] = data[k];
      data[k] = tmp;
    }
  //
  // phase 2. sort
  //
  while(n > 1)
  {
    tmp = data[1]; // largest
    data[1] = data[n];
    data[n--] = tmp;
    for(j = 1;2 * j <= n;j = k)
    {
      k = (2 * j + 1 <= n && data[2 * j + 1] > data[2 * j]) ? 2 * j + 1 : 2 * j;
      if(data[j] >= data[k])
        break;
      tmp = data[j];
      data[j] = data[k];
      data[k] = tmp;
    }
  }
}


//
// TP.08.6
//

void rank_sort(T *data,int first,int one_after_last)
{
  int i,j,*rank;
  T *buffer;

  rank = (int *)malloc((size_t)(one_after_last - first) * sizeof(int)) - first; // no error check!
  for(i = first;i < one_after_last;i++)
    rank[i] = first;
  for(i = first + 1;i < one_after_last;i++)
    for(j = first;j < i;j++)
      rank[(data[i] < data[j]) ? j : i]++;
  buffer = (T *)malloc((size_t)(one_after_last - first) * sizeof(T)) - first; // no error check!
  for(i = first;i < one_after_last;i++)
    buffer[i] = data[i];
  for(i = first;i < one_after_last;i++)
    data[rank[i]] = buffer[i];
  free(buffer + first);
  free(rank + first);
}

void selection_sort(T *data,int first,int one_after_last)
{
  int i,j,k;

  for(i = one_after_last - 1;i > first;i--)
  {
    for(j = first,k = 1;k <= i;k++)
      if(data[k] > data[j])
        j = k;
    if(j < i)
    {
      T tmp = data[i];
      data[i] = data[j];
      data[j] = tmp;
    }
  }
}

int main(void)
{
# define max_n  1000000
# define n_tests 100
  static struct
  {
    sort_function function;
    char *name;
  }
  functions[] =
  {
    { bubble_sort   ,"bubble_sort"    },
    { shaker_sort   ,"shaker_sort"    },
    { insertion_sort,"insertion_sort" },
    { shell_sort    ,"shell_sort"     },
    { quick_sort    ,"quick_sort"     },
    { merge_sort    ,"merge_sort"     },
    { heap_sort     ,"heap_sort"      },
    { rank_sort     ,"rank_sort"      },
    { selection_sort,"selection_sort" }
  };
  int i,j,k,n,first,one_after_last;
  T master[max_n],data[max_n];

  srand((unsigned int)time(NULL));
  for(n = 10;n < max_n;n=n*10)
  {
    // generate n random numbers
    for(i = 0;i < n;i++)
      master[i] = (T)((int)rand() % max_n);
    // test all sorting functions
    first = 0;
    one_after_last = n;
	printf("n= %d\n", n);
    for(j = 0;j < n_tests;j++)
    {
	//fprintf(stderr,"%4d[%4d,%4d] \r",n,first,one_after_last);
      for(k = 0;k < (int)(sizeof(functions) / sizeof(functions[0]));k++)
      {
        (void)elapsed_time();
		// all functions use the same unsorted data
        for(i = 0;i < first;i++)
          data[i] = (T)(-1);
        for(;i < one_after_last;i++)
          data[i] = master[i];
        for(;i < n;i++)
          data[i] = (T)(-1);
        // sort
        (*functions[k].function)(data,first,one_after_last);
        // test
        if(data[first] < (T)(0))
        {
          //fprintf(stderr,"%s() failed for n=%d, first=%d, and one_after_last=%d (access error)\n",functions[k].name,n,first,one_after_last);
          exit(1);
        }
        for(i = first + 1;i < one_after_last;i++)
          if(data[i] < data[i - 1])
          {
            //show(data,first,one_after_last);
            //fprintf(stderr,"%s() failed for n=%d, first=%d, and one_after_last=%d (sort error for i=%d)\n",functions[k].name,n,first,one_after_last,i);
            exit(1);
          }
		double dt = elapsed_time();
		printf("%s %0.10f\n",functions[k].name,dt);
      }
      // change index range
      first = (int)rand() % (1 + (3 * n) / 4);
      do
        one_after_last = (int)rand() % (1 + n);
      while(one_after_last <= first);
    }
  }
  //
  // done
  //
  printf("No errors found :-)\n");
  return 0;
# undef max_n
# undef n_tests
}
