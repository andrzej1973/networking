/***************************************************************************
 *
 *  Project
 *                            Socket Programming Example
 *
 * Copyright (C) 2019 - 2020, Andrzej Mazur, <andrzej@mazur.info>
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://mazur.info/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/



/****************************** Debug Macros ********************************/


/*
Debug macro definition

The do { ... } while (0) idiom ensures that the code acts like a statement (function call). 
The unconditional use of the code ensures that the compiler always checks that your debug 
code is valid — but the optimizer will remove the code when DEBUG is 0.
"fmt" is a printf format string. "..." is whatever extra arguments fmt requires 
(possibly nothing).
__VA_ARGS__ - means variable arguments
##__VA_ARGS__ - comma before the ‘##’ will be deleted. This does not happen if 
empty argument is passed, nor does it happen if the token preceding ‘##’ 
is anything other than a comma. 
see https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html for more info
*/

#ifndef SOCKETS_DEBUG_H_INCLUDED 
#define SOCKETS_DEBUG_H_INCLUDED


#define DEBUG  //comment this line to diable debug info



#ifdef DEBUG
#define debug_print(fmt, ...) \
    fprintf(stderr, "%s() in %s, line %i: " fmt "\n", \
        __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug_print(fmt, ...) ((void)0)
#endif


#endif