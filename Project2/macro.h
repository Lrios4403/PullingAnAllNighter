#ifndef MACRO_H
#define MACRO_H	

#include <iostream>

#define SWITCHANDRUN(bname, func)	\
if(!bname)							\
{									\
	func							\
	bname = true;					\
}							

#endif