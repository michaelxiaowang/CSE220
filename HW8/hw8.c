/*
Homework #8
name: MICHAEL_WANG
sbuid: 109055666
*/

#include "hw8.h"

size_t hw_strlen(const char *str) {
	const char *chrptr = str;
	size_t size = 0;
	if(str == NULL)
		return size;
	while(*chrptr != '\0') {
		size++;
		chrptr++;
	}
	return size;
}

int hw_strcmp(const char *str1, const char *str2) {
	const char *chrptr1 = str1, *chrptr2 = str2;
	if(str1 == str2) /*If str1 == str2, they are pointing to the same address, which accounts for NULL*/
		return 1;
	else if(str1 == NULL || str2 == NULL) /*If one str is NULL and the other isn't, they are not equal*/
		return 0;
	while(*chrptr1 != '\0' || *chrptr2 != '\0') { /*While the current index of either character is not '\0'*/
		if(*chrptr1 != *chrptr2) 
			return 0;
		chrptr1++;
		chrptr2++;
	}
	return 1;
}

char* hw_strncpy(char *dst, const char *src, size_t n) {
	int i = n;
	int dstindex = 0;
	const char *srcptr = src;
	if(dst == NULL || src == NULL)
		return NULL;
	if(n == 0)
		return dst;
	while (i > 0) {
		*(dst + dstindex) = *srcptr;
		srcptr++;
		dstindex++;
		i--;
		
	}
	return dst;
}

int hw_indexof(const char *str, char c) {
	const char *strptr = str;
	if(str == NULL)
		return -1;
	if(c == '\0')
		return hw_strlen(str);
	while(*strptr != '\0') {
		strptr++;
		if(*strptr == c)
			return strptr - str;
	}
		return -1;
}

void hw_reversestr(char *str) {
	char temp;
	int i;
	if(str == NULL)
		return;
	for(i = 0; i < hw_strlen(str)/2; i++) {
		temp = *(str + i);
		*(str + i) = *(str + (hw_strlen(str) - i - 1));
		*(str + (hw_strlen(str) - i - 1)) = temp;
	}
}

int hw_equalsIgnoreCase(const char *str1, const char *str2) {
	const char *chrptr1 = str1, *chrptr2 = str2;
	if(str1 == str2) /*If str1 == str2, they are pointing to the same address, which accounts for NULL*/
		return 1;
	else if(str1 == NULL || str2 == NULL) /*If one str is NULL and the other isn't, they are not equal*/
		return 0;
	while(*chrptr1 != '\0' || *chrptr2 != '\0') { /*While the current index of either character is not '\0'*/
		if(*chrptr1 != *chrptr2) {
			if(((int)*chrptr1 >= 65 && (int)*chrptr1 <= 90) || ((int)*chrptr1 >= 97 && (int)*chrptr1 <= 122)) { /*If the characters are not equal, is the character in str1 an alphabet character?*/
				if(!((int)*chrptr1 - 32 == (int)*chrptr2 || (int)*chrptr1 + 32 == (int)*chrptr2)) { /*If the character in str1 is an alphabet character, is the corresponding character in str2 not the uppercase or lowercase version?*/
					return 0;
				}
			}
			else {
				return 0;
			}
		}
		chrptr1++;
		chrptr2++;
	}
	return 1;
}

void hw_replaceall(char *str, const char *pattern, char replacement) {
	int i;
	char *strptr = str;
	if(str == NULL || pattern == NULL || replacement == '\0')
		return;
	while(*strptr != '\0') {
		for(i = 0; i < hw_strlen(pattern); i++) {
			if(*strptr == *(pattern + i))
				*strptr = replacement;
		}
		strptr++;
	}
}

char* hw_expandtabs(const char *str, size_t tabsize) {
	const char *strptr = str;
	int numtabs = 0, i;
	if(str == NULL)
		return NULL;
	while(*strptr != '\0') {
		if(*strptr == '\t')
			numtabs++;
		strptr++;
	}
	char *returnstr = malloc(hw_strlen(str) + (tabsize * numtabs) + 1);
	if(returnstr == NULL)
		return NULL;
	char *returnstrptr = returnstr;
	strptr = str;
	while(*strptr != '\0') {
		if(*strptr != '\t') {
			*returnstrptr = *strptr;
			returnstrptr++;
		}
		else {
			for(i = 0; i < tabsize; i++) {
				*returnstrptr = 32;
				returnstrptr++;
			}
		}
		strptr++;
	}
	*returnstrptr = '\0';
	return returnstr;
}

char** hw_split(const char *str, char c) {
	const char *strptr = str;
	if(str == NULL || c == '\0')
		return NULL;
	char *splitstr = malloc(hw_strlen(str) + 1);
	if(splitstr == NULL)
		return NULL;
	int numtokens = 0;
	while(*strptr != '\0') {
		if(*strptr == c) {
			numtokens++;
			while(*strptr == c) {
				strptr++;
			}
		}
		strptr++;
	}
	strptr = str;
	char **returnstr = malloc(sizeof(char *) * (numtokens));
	if(returnstr == NULL) {
		free(splitstr);
		return NULL;
	}
	int returnstrindex = 0;
	while(*strptr != '\0') {
		if(*strptr == c) {
			*splitstr = '\0';
			splitstr++;
			strptr++;
			while(*(strptr+1) == c) {
				*splitstr = '\0';
				splitstr++;
				strptr++;
			}
			*(returnstr + returnstrindex) = splitstr + 1;
			returnstrindex++;		
		}
		else {
			*splitstr = *strptr;
		}
		strptr++;
		splitstr++;
	}
	return returnstr;
}

int hw_findAndReplace(char **dst, const char *str, const char *find_str, const char* replace_str) {
	if(str == NULL || find_str == NULL || replace_str == NULL)
		return 0;

	int i;
	int numoccur = 0;
	int matched = 0;
	const char *strptr = str;
	/*finds number of occurences of find_str*/
	while(*strptr != '\0') {
		if(*strptr == *find_str) {
			matched = 1;
			for(i = 0; i< hw_strlen(find_str); i++) {
				if(*(find_str+i) != *strptr) {
					strptr-=i;
					matched = 0;
					break;
				}
				strptr++;
			}
			if(matched == 1) {
				matched = 0;
				numoccur++;
				strptr--;
			}
		}
		strptr++;
	}

	if(dst == NULL) {
		return 0;
	}
	*dst = malloc(hw_strlen(str) + 1 - (numoccur * hw_strlen(find_str)) + (numoccur * hw_strlen(replace_str)));
	if(*dst == NULL) {
		free(dst);
		return 0;
	}
	
	matched = 0;
	strptr = str;
	char *newptr = *dst;
	while(*strptr != '\0') {
		if(*strptr == *find_str) {
			matched = 1;
			for(i = 0; i< hw_strlen(find_str); i++) {

				if(*(find_str+i) != *strptr) {
					strptr -= i;
					int j;
					for(j = i; j > 0; j--) {
						*newptr = *strptr;
					}
					matched = 0;
					break;
				}
				strptr++;
			}
			if(matched == 1) {
				strptr--;
				matched = 0;
				hw_strncpy(newptr, replace_str, hw_strlen(replace_str));
				for(i = 1; i < hw_strlen(replace_str); i++) {
					newptr++;
				}
			}
		}
		else {
			*newptr = *strptr;
		}
		strptr++;
		newptr++;
	}
	str = *dst;
	return numoccur;

}

void hw_swapTokens(char *str, size_t i, size_t j, const char *delimiters) {
	/*if(str == NULL || i == j || delimiters == NULL)
		return;
	char *splitstr = malloc(hw_strlen(str) + 1);
	if(splitstr == NULL)
		return;
	hw_strncpy(splitstr, str, hw_strlen(str) + 1);
	char *splitstrptr = splitstr;
	int counter;
	while(*splitstrptr != '\0') {
		for(counter = 0; counter < hw_strlen(delimiters); counter++) {
			if(*splitstrptr == *(delimiters + counter)) {
				*splitstrptr = '\0';
			}
		}
		splitstrptr++;
	}
	splitstrptr = splitstr;
	int numtokens = 0;
	counter = 0;
	while(counter < hw_strlen(str)) {
		if(*splitstrptr != '\0') {
			numtokens++;
			splitstrptr++;
			counter++;
			while(*splitstrptr != '\0') {
				splitstrptr++;
				counter++;
			}
		}
		else {
			splitstrptr++;
			counter++;
		}
	}
	char **splitarray = malloc(sizeof(char *) * numtokens);
	if(splitarray == NULL) {
		free(splitstr);
		return;
	}
	int splitindex = 0;
	splitstrptr = splitstr;
	while(numtokens > 0) {
		if(*splitstrptr != '\0') {
			*(splitarray + splitindex) = splitstrptr;
			splitindex++;
			numtokens--;
			splitstrptr++;
			while(*splitstrptr != '\0') {
				splitstrptr++;
			}
		}
		else {
			splitstrptr++;
		}
	}
	if(i > (numtokens-1) || j > (numtokens - 1)) {
		free(splitarray);
		return;
	}
	else {

	}
	splitstrptr = splitstr;
	for(counter = 0; counter < hw_strlen(str) + 1; counter++) {
		*(str+counter) = *(splitstrptr+counter);
	}*/
}

int ec_findAndReplace(char **dst, const char *str, const char *find_str, const char* replace_str) {
	return 1;
}