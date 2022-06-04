/*
    BVA
    Copyright (C) 2012 Norbert Manthey

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/ 
 


#ifndef _SORT_H
#define _SORT_H

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

template <typename T>
void effmergesort(T* a, int32_t n);

template <typename T>
void mergesort(T* a, int32_t n);

template <typename T>
void bubblesort( T* a, int32_t n);

template <typename T>
void insertionsort(T* a, int32_t n);

template <typename T>
void selectionsort(T *a,uint32_t n);

template <typename T>
inline void quicksort(T *a, int32_t n);


























template <typename T>
inline void effmergesort(T* a, int32_t n)
{
	int32_t m, s;
	
	T* b=new T[ n ];

	
	for (s=1; s<n; s+=s)
	{
		m = n;
		do {
			m = m - 2*s;	
			int32_t hi = (m+s > 0) ? m + s : 0;	
			
			int32_t i = (m > 0) ? m : 0;	
			int32_t j = hi;
			
			int32_t stopb = m + 2*s;	
			int32_t currentb = i;			
			
			
			while( i < hi && j < stopb)
			{
				if( a[i] < a[j] )
					b[currentb++] = a[i++];
				else
					b[currentb++] = a[j++];
			}
			
			for( ; i < hi; )
				b[currentb++] = a[i++];
				
			for( ; j< stopb; 	)
				b[currentb++] = a[j++];
				
		} while( m > 0 );
		
		
		T* tmp = a;
		a = b;
		b = tmp;
	}
	
	delete [] b;
}

template <typename T>
inline void mergesort(T* a, int32_t n)
{
	int32_t m, s;
	
	T* b=new int[ (n+1)/2 ];
	
	for (s=1; s<n; s+=s)
	{
		for (m=n-1-s; m>=0; m-=s+s)
		{
			int32_t lo = ((m-s+1) > 0) ? (m-s+1) : 0; 
			
			int32_t i, j, k;
			i=0; j=lo;
				
				while (j<=m)
					b[i++]=a[j++];
				i=0; k=lo;
				
				while (k<j && j<= m + s )
					if (b[i]<=a[j])
						a[k++]=b[i++];
					else
						a[k++]=a[j++];
				
				while (k<j)
					a[k++]=b[i++];
		}
	}
	
	delete [] b;
}

template <typename T>
inline void bubblesort( T* a, int32_t n)
{
	int32_t i,k,tmp;
	bool swap = true;
	for( i=0; swap && i < n-1 ;++i )
	{
		swap = false;
		for( k=0; k < n - 1; ++k )
		{
			if( a[k] > a[k+1] )
			{
				tmp = a[k];
				a[k]=a[k+1];
				a[k+1] = tmp;
				swap = true;
			}
		}
	}
}

template <typename T>
inline void insertionsort(T* a, int32_t n)
{
	for(int32_t i = 1 ; i < n; i++ )
	{
		T value = a[i];
		int32_t j = i-1;
		while (j >= 0 && a[j] > value )
		{
			a[j + 1] = a[j];
			j = j-1;
  }
		a[j+1] = value;
	}
}

template <typename T>
inline void selectionsort(T*a, uint32_t n)
{
	uint32_t i,j,minat;
	T min;
	for(i=0;i<(n-1);i++)
	{
		minat=i;
		min=a[i];

		for(j=i+1; j<n; j++) 
	 {
		 
			if(min>a[j])  
			{
				
				minat=j; 
				min=a[j];
			}
		}
		
		T temp=a[i] ;
		a[i]=a[minat]; 
		a[minat]=temp;
	}
}


template <typename T>
void _quicksort(T* a, int32_t left, int32_t right)
{
 T pivot;
 int32_t l_hold, r_hold, tmp;
 
 l_hold = left;
 r_hold = right;
 pivot = a[left];
 while (left < right)
 {
  while ((a[right] >= pivot) && (left < right))
   right--;
  if (left != right)
  {
   a[left] = a[right];
   left++;
  }
  while ((a[left] <= pivot) && (left < right))
   left++;
  if (left != right)
  {
   a[right] = a[left];
   right--;
  }
 }
 
 a[left] = pivot;
 tmp = left;
 left = l_hold;
 right = r_hold;
 
 if (left < tmp) _quicksort(a, left, tmp-1);
 if (right > tmp) _quicksort(a, tmp+1, right);
}

template <typename T>
inline void quicksort(T *a, int32_t n)
{
 _quicksort(a, 0, n - 1);
}
 
 



#endif
