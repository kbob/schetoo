# 1 "read.c"
# 1 "/home/kbob/scheme2//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "read.c"
# 1 "read.h" 1



# 1 "io.h" 1



# 1 "/usr/include/setjmp.h" 1 3 4
# 26 "/usr/include/setjmp.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 313 "/usr/include/features.h" 3 4
# 1 "/usr/include/bits/predefs.h" 1 3 4
# 314 "/usr/include/features.h" 2 3 4
# 346 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 353 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 354 "/usr/include/sys/cdefs.h" 2 3 4
# 347 "/usr/include/features.h" 2 3 4
# 378 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4


# 1 "/usr/include/gnu/stubs-32.h" 1 3 4
# 8 "/usr/include/gnu/stubs.h" 2 3 4
# 379 "/usr/include/features.h" 2 3 4
# 27 "/usr/include/setjmp.h" 2 3 4



# 1 "/usr/include/bits/setjmp.h" 1 3 4
# 27 "/usr/include/bits/setjmp.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 28 "/usr/include/bits/setjmp.h" 2 3 4






typedef int __jmp_buf[6];
# 31 "/usr/include/setjmp.h" 2 3 4
# 1 "/usr/include/bits/sigset.h" 1 3 4
# 24 "/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 32 "/usr/include/setjmp.h" 2 3 4



struct __jmp_buf_tag
  {




    __jmp_buf __jmpbuf;
    int __mask_was_saved;
    __sigset_t __saved_mask;
  };




typedef struct __jmp_buf_tag jmp_buf[1];



extern int setjmp (jmp_buf __env) __attribute__ ((__nothrow__));






extern int __sigsetjmp (struct __jmp_buf_tag __env[1], int __savemask) __attribute__ ((__nothrow__));




extern int _setjmp (struct __jmp_buf_tag __env[1]) __attribute__ ((__nothrow__));
# 78 "/usr/include/setjmp.h" 3 4




extern void longjmp (struct __jmp_buf_tag __env[1], int __val)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));


# 114 "/usr/include/setjmp.h" 3 4

# 5 "io.h" 2
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 149 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 211 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 3 4
typedef unsigned int size_t;
# 323 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 3 4
typedef int wchar_t;
# 6 "io.h" 2
# 1 "/usr/include/stdio.h" 1 3 4
# 30 "/usr/include/stdio.h" 3 4




# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 35 "/usr/include/stdio.h" 2 3 4

# 1 "/usr/include/bits/types.h" 1 3 4
# 28 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;




__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;







__extension__ typedef long long int __quad_t;
__extension__ typedef unsigned long long int __u_quad_t;
# 131 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/typesizes.h" 1 3 4
# 132 "/usr/include/bits/types.h" 2 3 4


__extension__ typedef __u_quad_t __dev_t;
__extension__ typedef unsigned int __uid_t;
__extension__ typedef unsigned int __gid_t;
__extension__ typedef unsigned long int __ino_t;
__extension__ typedef __u_quad_t __ino64_t;
__extension__ typedef unsigned int __mode_t;
__extension__ typedef unsigned int __nlink_t;
__extension__ typedef long int __off_t;
__extension__ typedef __quad_t __off64_t;
__extension__ typedef int __pid_t;
__extension__ typedef struct { int __val[2]; } __fsid_t;
__extension__ typedef long int __clock_t;
__extension__ typedef unsigned long int __rlim_t;
__extension__ typedef __u_quad_t __rlim64_t;
__extension__ typedef unsigned int __id_t;
__extension__ typedef long int __time_t;
__extension__ typedef unsigned int __useconds_t;
__extension__ typedef long int __suseconds_t;

__extension__ typedef int __daddr_t;
__extension__ typedef long int __swblk_t;
__extension__ typedef int __key_t;


__extension__ typedef int __clockid_t;


__extension__ typedef void * __timer_t;


__extension__ typedef long int __blksize_t;




__extension__ typedef long int __blkcnt_t;
__extension__ typedef __quad_t __blkcnt64_t;


__extension__ typedef unsigned long int __fsblkcnt_t;
__extension__ typedef __u_quad_t __fsblkcnt64_t;


__extension__ typedef unsigned long int __fsfilcnt_t;
__extension__ typedef __u_quad_t __fsfilcnt64_t;

__extension__ typedef int __ssize_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


__extension__ typedef int __intptr_t;


__extension__ typedef unsigned int __socklen_t;
# 37 "/usr/include/stdio.h" 2 3 4
# 45 "/usr/include/stdio.h" 3 4
struct _IO_FILE;



typedef struct _IO_FILE FILE;





# 65 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 75 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 32 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 15 "/usr/include/_G_config.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 16 "/usr/include/_G_config.h" 2 3 4




# 1 "/usr/include/wchar.h" 1 3 4
# 83 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {

    unsigned int __wch;



    char __wchb[4];
  } __value;
} __mbstate_t;
# 21 "/usr/include/_G_config.h" 2 3 4

typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 53 "/usr/include/_G_config.h" 3 4
typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
# 33 "/usr/include/libio.h" 2 3 4
# 53 "/usr/include/libio.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stdarg.h" 1 3 4
# 40 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 54 "/usr/include/libio.h" 2 3 4
# 170 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 180 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 203 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 271 "/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 319 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 328 "/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 364 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 416 "/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
# 458 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
# 488 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
# 76 "/usr/include/stdio.h" 2 3 4
# 89 "/usr/include/stdio.h" 3 4


typedef _G_fpos_t fpos_t;




# 141 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/bits/stdio_lim.h" 1 3 4
# 142 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));














extern FILE *tmpfile (void) ;
# 186 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__)) ;

# 209 "/usr/include/stdio.h" 3 4





extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 243 "/usr/include/stdio.h" 3 4






extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes) ;




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 272 "/usr/include/stdio.h" 3 4

# 306 "/usr/include/stdio.h" 3 4



extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__));

# 328 "/usr/include/stdio.h" 3 4





extern int fprintf (FILE *__restrict __stream,
      __const char *__restrict __format, ...);




extern int printf (__const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      __const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

# 402 "/usr/include/stdio.h" 3 4





extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));
# 425 "/usr/include/stdio.h" 3 4
extern int fscanf (FILE *__restrict __stream, __const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf") ;


extern int scanf (__const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf") ;

extern int sscanf (__const char *__restrict __s, __const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__));
# 445 "/usr/include/stdio.h" 3 4








extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (__const char *__restrict __s,
      __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
# 476 "/usr/include/stdio.h" 3 4
extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")



     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")

     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (__const char *__restrict __s, __const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf")



     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
# 504 "/usr/include/stdio.h" 3 4









extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 547 "/usr/include/stdio.h" 3 4








extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 599 "/usr/include/stdio.h" 3 4





extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

# 657 "/usr/include/stdio.h" 3 4





extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);

# 717 "/usr/include/stdio.h" 3 4





extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 765 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, __const fpos_t *__pos);
# 788 "/usr/include/stdio.h" 3 4

# 797 "/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__)) ;

# 814 "/usr/include/stdio.h" 3 4





extern void perror (__const char *__s);






# 1 "/usr/include/bits/sys_errlist.h" 1 3 4
# 827 "/usr/include/stdio.h" 2 3 4
# 916 "/usr/include/stdio.h" 3 4

# 7 "io.h" 2
# 1 "/usr/include/sys/types.h" 1 3 4
# 29 "/usr/include/sys/types.h" 3 4

# 46 "/usr/include/sys/types.h" 3 4
typedef __loff_t loff_t;



typedef __ino_t ino_t;
# 62 "/usr/include/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;
# 100 "/usr/include/sys/types.h" 3 4
typedef __pid_t pid_t;
# 110 "/usr/include/sys/types.h" 3 4
typedef __ssize_t ssize_t;
# 133 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 74 "/usr/include/time.h" 3 4


typedef __time_t time_t;

# 92 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 104 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 134 "/usr/include/sys/types.h" 2 3 4
# 147 "/usr/include/sys/types.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 148 "/usr/include/sys/types.h" 2 3 4
# 195 "/usr/include/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 235 "/usr/include/sys/types.h" 3 4
typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 273 "/usr/include/sys/types.h" 3 4

# 8 "io.h" 2
# 1 "/usr/include/wchar.h" 1 3 4
# 40 "/usr/include/wchar.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stdarg.h" 1 3 4
# 41 "/usr/include/wchar.h" 2 3 4

# 1 "/usr/include/bits/wchar.h" 1 3 4
# 43 "/usr/include/wchar.h" 2 3 4
# 52 "/usr/include/wchar.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 352 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 3 4
typedef unsigned int wint_t;
# 53 "/usr/include/wchar.h" 2 3 4
# 104 "/usr/include/wchar.h" 3 4


typedef __mbstate_t mbstate_t;

# 129 "/usr/include/wchar.h" 3 4





struct tm;









extern wchar_t *wcscpy (wchar_t *__restrict __dest,
   __const wchar_t *__restrict __src) __attribute__ ((__nothrow__));

extern wchar_t *wcsncpy (wchar_t *__restrict __dest,
    __const wchar_t *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__));


extern wchar_t *wcscat (wchar_t *__restrict __dest,
   __const wchar_t *__restrict __src) __attribute__ ((__nothrow__));

extern wchar_t *wcsncat (wchar_t *__restrict __dest,
    __const wchar_t *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__));


extern int wcscmp (__const wchar_t *__s1, __const wchar_t *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern int wcsncmp (__const wchar_t *__s1, __const wchar_t *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

# 186 "/usr/include/wchar.h" 3 4



extern int wcscoll (__const wchar_t *__s1, __const wchar_t *__s2) __attribute__ ((__nothrow__));



extern size_t wcsxfrm (wchar_t *__restrict __s1,
         __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));

# 216 "/usr/include/wchar.h" 3 4








extern wchar_t *wcschr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));
# 234 "/usr/include/wchar.h" 3 4
extern wchar_t *wcsrchr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


# 246 "/usr/include/wchar.h" 3 4



extern size_t wcscspn (__const wchar_t *__wcs, __const wchar_t *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern size_t wcsspn (__const wchar_t *__wcs, __const wchar_t *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));
# 263 "/usr/include/wchar.h" 3 4
extern wchar_t *wcspbrk (__const wchar_t *__wcs, __const wchar_t *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));
# 274 "/usr/include/wchar.h" 3 4
extern wchar_t *wcsstr (__const wchar_t *__haystack, __const wchar_t *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));



extern wchar_t *wcstok (wchar_t *__restrict __s,
   __const wchar_t *__restrict __delim,
   wchar_t **__restrict __ptr) __attribute__ ((__nothrow__));


extern size_t wcslen (__const wchar_t *__s) __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

# 308 "/usr/include/wchar.h" 3 4

# 317 "/usr/include/wchar.h" 3 4
extern wchar_t *wmemchr (__const wchar_t *__s, wchar_t __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));



extern int wmemcmp (__const wchar_t *__restrict __s1,
      __const wchar_t *__restrict __s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern wchar_t *wmemcpy (wchar_t *__restrict __s1,
    __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));



extern wchar_t *wmemmove (wchar_t *__s1, __const wchar_t *__s2, size_t __n)
     __attribute__ ((__nothrow__));


extern wchar_t *wmemset (wchar_t *__s, wchar_t __c, size_t __n) __attribute__ ((__nothrow__));

# 348 "/usr/include/wchar.h" 3 4



extern wint_t btowc (int __c) __attribute__ ((__nothrow__));



extern int wctob (wint_t __c) __attribute__ ((__nothrow__));



extern int mbsinit (__const mbstate_t *__ps) __attribute__ ((__nothrow__)) __attribute__ ((__pure__));



extern size_t mbrtowc (wchar_t *__restrict __pwc,
         __const char *__restrict __s, size_t __n,
         mbstate_t *__p) __attribute__ ((__nothrow__));


extern size_t wcrtomb (char *__restrict __s, wchar_t __wc,
         mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));


extern size_t __mbrlen (__const char *__restrict __s, size_t __n,
   mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));
extern size_t mbrlen (__const char *__restrict __s, size_t __n,
        mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));

# 403 "/usr/include/wchar.h" 3 4



extern size_t mbsrtowcs (wchar_t *__restrict __dst,
    __const char **__restrict __src, size_t __len,
    mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));



extern size_t wcsrtombs (char *__restrict __dst,
    __const wchar_t **__restrict __src, size_t __len,
    mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));

# 445 "/usr/include/wchar.h" 3 4



extern double wcstod (__const wchar_t *__restrict __nptr,
        wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));





extern float wcstof (__const wchar_t *__restrict __nptr,
       wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));
extern long double wcstold (__const wchar_t *__restrict __nptr,
       wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));







extern long int wcstol (__const wchar_t *__restrict __nptr,
   wchar_t **__restrict __endptr, int __base) __attribute__ ((__nothrow__));



extern unsigned long int wcstoul (__const wchar_t *__restrict __nptr,
      wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));






__extension__
extern long long int wcstoll (__const wchar_t *__restrict __nptr,
         wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));



__extension__
extern unsigned long long int wcstoull (__const wchar_t *__restrict __nptr,
     wchar_t **__restrict __endptr,
     int __base) __attribute__ ((__nothrow__));

# 582 "/usr/include/wchar.h" 3 4



extern int fwide (__FILE *__fp, int __mode) __attribute__ ((__nothrow__));






extern int fwprintf (__FILE *__restrict __stream,
       __const wchar_t *__restrict __format, ...)
                                                           ;




extern int wprintf (__const wchar_t *__restrict __format, ...)
                                                           ;

extern int swprintf (wchar_t *__restrict __s, size_t __n,
       __const wchar_t *__restrict __format, ...)
     __attribute__ ((__nothrow__)) ;





extern int vfwprintf (__FILE *__restrict __s,
        __const wchar_t *__restrict __format,
        __gnuc_va_list __arg)
                                                           ;




extern int vwprintf (__const wchar_t *__restrict __format,
       __gnuc_va_list __arg)
                                                           ;


extern int vswprintf (wchar_t *__restrict __s, size_t __n,
        __const wchar_t *__restrict __format,
        __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) ;






extern int fwscanf (__FILE *__restrict __stream,
      __const wchar_t *__restrict __format, ...)
                                                          ;




extern int wscanf (__const wchar_t *__restrict __format, ...)
                                                          ;

extern int swscanf (__const wchar_t *__restrict __s,
      __const wchar_t *__restrict __format, ...)
     __attribute__ ((__nothrow__)) ;
# 654 "/usr/include/wchar.h" 3 4
extern int fwscanf (__FILE *__restrict __stream, __const wchar_t *__restrict __format, ...) __asm__ ("" "__isoc99_fwscanf")


                                                          ;
extern int wscanf (__const wchar_t *__restrict __format, ...) __asm__ ("" "__isoc99_wscanf")

                                                          ;
extern int swscanf (__const wchar_t *__restrict __s, __const wchar_t *__restrict __format, ...) __asm__ ("" "__isoc99_swscanf")


     __attribute__ ((__nothrow__)) ;
# 678 "/usr/include/wchar.h" 3 4









extern int vfwscanf (__FILE *__restrict __s,
       __const wchar_t *__restrict __format,
       __gnuc_va_list __arg)
                                                          ;




extern int vwscanf (__const wchar_t *__restrict __format,
      __gnuc_va_list __arg)
                                                          ;

extern int vswscanf (__const wchar_t *__restrict __s,
       __const wchar_t *__restrict __format,
       __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) ;





extern int vfwscanf (__FILE *__restrict __s, __const wchar_t *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfwscanf")


                                                          ;
extern int vwscanf (__const wchar_t *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vwscanf")

                                                          ;
extern int vswscanf (__const wchar_t *__restrict __s, __const wchar_t *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vswscanf")


     __attribute__ ((__nothrow__)) ;
# 734 "/usr/include/wchar.h" 3 4









extern wint_t fgetwc (__FILE *__stream);
extern wint_t getwc (__FILE *__stream);





extern wint_t getwchar (void);






extern wint_t fputwc (wchar_t __wc, __FILE *__stream);
extern wint_t putwc (wchar_t __wc, __FILE *__stream);





extern wint_t putwchar (wchar_t __wc);







extern wchar_t *fgetws (wchar_t *__restrict __ws, int __n,
   __FILE *__restrict __stream);





extern int fputws (__const wchar_t *__restrict __ws,
     __FILE *__restrict __stream);






extern wint_t ungetwc (wint_t __wc, __FILE *__stream);

# 849 "/usr/include/wchar.h" 3 4




extern size_t wcsftime (wchar_t *__restrict __s, size_t __maxsize,
   __const wchar_t *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));

# 889 "/usr/include/wchar.h" 3 4

# 9 "io.h" 2

# 1 "char.h" 1







    typedef char char_t;
    typedef int charint_t;
# 11 "io.h" 2

typedef struct instream instream_t;
typedef struct outstream outstream_t;

extern instream_t *make_file_instream (FILE *);
extern instream_t *make_readline_instream(void);
extern instream_t *make_string_instream (const wchar_t *, size_t);
extern void delete_instream (instream_t *);
extern wint_t instream_getwc (instream_t *);
extern wint_t instream_ungetwc (wint_t, instream_t *);

extern outstream_t *make_file_outstream (FILE *);
extern outstream_t *make_string_outstream(wchar_t *, size_t);
extern void delete_outstream (outstream_t *);
extern wint_t outstream_putwc (wchar_t, outstream_t *);
extern int outstream_printf (outstream_t *,
                                          const wchar_t *fmt, ...);
extern off_t outstream_pos (outstream_t *);
extern jmp_buf *outstream_set_limit (outstream_t *, off_t limit);
extern void outstream_clear_limit(outstream_t *);
# 5 "read.h" 2
# 1 "mem.h" 1



# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 5 "mem.h" 2
# 1 "/usr/include/stdint.h" 1 3 4
# 28 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/stdint.h" 2 3 4
# 49 "/usr/include/stdint.h" 3 4
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;





__extension__
typedef unsigned long long int uint64_t;






typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;



__extension__
typedef long long int int_least64_t;



typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;



__extension__
typedef unsigned long long int uint_least64_t;






typedef signed char int_fast8_t;





typedef int int_fast16_t;
typedef int int_fast32_t;
__extension__
typedef long long int int_fast64_t;



typedef unsigned char uint_fast8_t;





typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
__extension__
typedef unsigned long long int uint_fast64_t;
# 126 "/usr/include/stdint.h" 3 4
typedef int intptr_t;


typedef unsigned int uintptr_t;
# 138 "/usr/include/stdint.h" 3 4
__extension__
typedef long long int intmax_t;
__extension__
typedef unsigned long long int uintmax_t;
# 6 "mem.h" 2

# 1 "bool.h" 1



typedef enum bool { false = 0, true = 1 } bool;
# 8 "mem.h" 2
# 81 "mem.h"
typedef struct object *obj_t;
typedef struct obj_header obj_header_t;
typedef intptr_t word_t;
# 117 "mem.h"
static inline word_t obj_bits(const obj_t o)
{
    return (word_t)o;
}

static inline bool is_normal(const obj_t o)
{
    return (obj_bits(o) & 0x07) == 0x00;
}

static inline bool is_forward(const obj_t o)
{
    return (obj_bits(o) & 0x07) == 0x02;
}

static inline bool is_immediate(const obj_t o)
{
    return (obj_bits(o) & 0x07) == 0x06;
}

static inline bool is_special(const obj_t o)
{
    return (obj_bits(o) & 0x1F) == 0x16;
}

static inline bool is_null(const obj_t o)
{
    return o == ((obj_t)0x016);
}

static inline bool is_undefined(const obj_t o)
{
    return o == ((obj_t)0x116);
}

static inline bool is_EOF(const obj_t o)
{
    return o == ((obj_t)0x216);
}

static inline word_t obj_fixnum_value(const obj_t p)
{
    return obj_bits(p) >> 1;
}

static inline obj_header_t *obj_normal_ptr(obj_t o)
{
    return (obj_header_t *)o;
}

static inline obj_header_t *obj_fwd_ptr(obj_t o)
{
    return (obj_header_t *)(obj_bits(o) & ~0x07);
}

static inline void header_set_fwd_ptr(obj_header_t *dst, obj_t fwd)
{
    *(word_t *)dst = obj_bits(fwd) | 0x02;
}
# 203 "mem.h"
typedef size_t mem_size_op(const obj_header_t *);


typedef size_t mem_ptr_count_op(const obj_header_t *);


typedef void mem_move_op(const obj_header_t *src, obj_header_t *dst);


typedef obj_t mem_get_ptr_op(const obj_header_t *, size_t index);


typedef void mem_set_ptr_op(obj_header_t *, size_t index, obj_t);

typedef struct mem_end_marker { } mem_end_marker_t;

typedef struct mem_ops mem_ops_t;
struct mem_ops {
    word_t mo_start_marker;
    const wchar_t *mo_name;
    mem_ops_t *mo_super;
    mem_size_op *mo_size;
    mem_ptr_count_op *mo_ptr_count;
    mem_move_op *mo_move;
    mem_get_ptr_op *mo_get_ptr;
    mem_set_ptr_op *mo_set_ptr;
    mem_end_marker_t mo_end_marker;
};

struct obj_header {
    mem_ops_t *oh_ops;
};

static inline obj_header_t *obj_header(obj_t obj)
{
    return (obj_header_t *) obj;
}

static inline mem_ops_t *header_mem_ops(obj_header_t *header)
{
    return header->oh_ops;
}

static inline mem_ops_t *obj_mem_ops(obj_t obj)
{
    return header_mem_ops(obj_header(obj));
}

static inline bool is_primitive_obj(obj_header_t *header)
{
    return header->oh_ops->mo_start_marker == 0x316;
}

static inline bool is_record_instance(obj_header_t *header)
{
    return !is_primitive_obj(header);
}


extern void set_heap_size_bytes(size_t usable_size_bytes);


extern void init_heap(void);




    extern void check_obj(const obj_t);


extern obj_header_t *mem_alloc_obj(mem_ops_t *, size_t size_bytes);
# 6 "read.h" 2


extern bool read_stream(instream_t *in, obj_t *obj_out);
# 2 "read.c" 2

# 1 "/usr/include/assert.h" 1 3 4
# 68 "/usr/include/assert.h" 3 4



extern void __assert_fail (__const char *__assertion, __const char *__file,
      unsigned int __line, __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, __const char *__file,
      unsigned int __line,
      __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



# 4 "read.c" 2
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/limits.h" 1 3 4
# 11 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/limits.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/syslimits.h" 1 3 4






# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/limits.h" 1 3 4
# 122 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 123 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/limits.h" 2 3 4
# 8 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/syslimits.h" 2 3 4
# 12 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include-fixed/limits.h" 2 3 4
# 5 "read.c" 2

# 1 "/usr/include/stdlib.h" 1 3 4
# 33 "/usr/include/stdlib.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 34 "/usr/include/stdlib.h" 2 3 4


# 96 "/usr/include/stdlib.h" 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;







__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;


# 140 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__)) ;




extern double atof (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (__const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern float strtof (__const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern long double strtold (__const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern long int strtol (__const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern unsigned long int strtoul (__const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 207 "/usr/include/stdlib.h" 3 4


__extension__
extern long long int strtoll (__const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtoull (__const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 378 "/usr/include/stdlib.h" 3 4


extern int rand (void) __attribute__ ((__nothrow__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__));

# 469 "/usr/include/stdlib.h" 3 4


extern void *malloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;










extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__));

# 511 "/usr/include/stdlib.h" 3 4


extern void abort (void) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 530 "/usr/include/stdlib.h" 3 4

# 539 "/usr/include/stdlib.h" 3 4




extern void exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));
# 553 "/usr/include/stdlib.h" 3 4






extern void _Exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));






extern char *getenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern char *__secure_getenv (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 661 "/usr/include/stdlib.h" 3 4





extern int system (__const char *__command) ;

# 691 "/usr/include/stdlib.h" 3 4
typedef int (*__compar_fn_t) (__const void *, __const void *);
# 701 "/usr/include/stdlib.h" 3 4



extern void *bsearch (__const void *__key, __const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;



extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
# 720 "/usr/include/stdlib.h" 3 4
extern int abs (int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;



__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;







extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;




__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;

# 805 "/usr/include/stdlib.h" 3 4



extern int mblen (__const char *__s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int mbtowc (wchar_t *__restrict __pwc,
     __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__)) ;



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__));

extern size_t wcstombs (char *__restrict __s,
   __const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__));

# 912 "/usr/include/stdlib.h" 3 4

# 7 "read.c" 2
# 1 "/usr/include/string.h" 1 3 4
# 28 "/usr/include/string.h" 3 4





# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 34 "/usr/include/string.h" 2 3 4









extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

# 62 "/usr/include/string.h" 3 4


extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 94 "/usr/include/string.h" 3 4
extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 125 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

# 208 "/usr/include/string.h" 3 4

# 233 "/usr/include/string.h" 3 4
extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 260 "/usr/include/string.h" 3 4
extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 279 "/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 312 "/usr/include/string.h" 3 4
extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 340 "/usr/include/string.h" 3 4
extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 395 "/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 409 "/usr/include/string.h" 3 4


extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

# 449 "/usr/include/string.h" 3 4
extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 644 "/usr/include/string.h" 3 4

# 8 "read.c" 2

# 1 "roots.h" 1




# 1 "uniq.h" 1
# 6 "roots.h" 2
# 62 "roots.h"
typedef struct root_descriptor root_descriptor_t;
typedef obj_t root_constructor_t(void);

struct root_descriptor {
    const wchar_t *rd_name;
    const char *rd_func;
    obj_t *rd_root;
    root_constructor_t *rd_init;
    root_descriptor_t *rd_next;
};

extern void record_static_root(root_descriptor_t *);
extern void push_root(root_descriptor_t *);
extern void pop_root(const wchar_t *);
extern void pop_function_roots(const char *func_name);
extern root_descriptor_t *get_thread_roots(void);
extern void init_roots(void);
# 10 "read.c" 2
# 1 "scan.h" 1






typedef enum token_type {
    TOK_EXACT_NUMBER,
    TOK_SIMPLE,
    TOK_ABBREV,
    TOK_COMMENT,
    TOK_BEGIN_VECTOR,
    TOK_BEGIN_BYTEVECTOR,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_PERIOD,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_EOF,

    TOKEN_TYPE_COUNT
} token_type_t;

extern token_type_t yylex(obj_t *lvalp, instream_t *in);
extern const char *token_name(token_type_t tok);
# 11 "read.c" 2
# 1 "test.h" 1



# 1 "/usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h" 1 3 4
# 5 "test.h" 2
# 36 "test.h"
typedef enum test_phase {
    TP_READ,
    TP_EVAL,
} test_phase_t;

typedef struct test_case_descriptor test_case_descriptor_t;
struct test_case_descriptor {
    test_phase_t tcd_phase;
    const wchar_t *tcd_input;
    const wchar_t *tcd_expected;
    size_t tcd_expected_len;
    const char *tcd_file;
    int tcd_lineno;
    test_case_descriptor_t *tcd_next;
};
# 60 "test.h"
extern void register_test(test_case_descriptor_t *);
extern void self_test();
# 12 "read.c" 2
# 1 "types.h" 1




# 1 "obj_boolean.h" 1



# 1 "except.h" 1
# 18 "except.h"
typedef struct condition_type {
    const char *ct_name;
} condition_type_t;

extern condition_type_t condition, warning, serious, message, irritants, who;
extern condition_type_t error, violation, assertion, non_continuable;
extern condition_type_t implementation_restriction, lexical, syntax, undefined;

extern void assertion_failed(const char *file,
        int line,
        const char *fn,
        const char *expr) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));

extern void raise_error(obj_t who, const char *message, obj_t irritants, ...);

extern void raise(condition_type_t *, obj_t obj, const char *msg, ...);

extern void raise_continuable(condition_type_t *, obj_t obj, const char *msg);

extern void set_program_name(const char *);
# 5 "obj_boolean.h" 2


static inline obj_t make_boolean(bool value)
{
    return value ? ((obj_t)0x10E) : ((obj_t)0x00E);
}

static inline bool is_boolean(const obj_t o)
{
    return (obj_bits(o) & 0x1F) == 0x0E;
}

static inline bool boolean_value(obj_t boolean)
{
    ((is_boolean(boolean)) ? (void)0 : raise(&violation, (boolean), "boolean_value: must be Boolean"));
    return boolean == ((obj_t)0x10E);
}
# 6 "types.h" 2
# 1 "obj_character.h" 1







static inline obj_t make_character(char_t value)
{
    return (obj_t)(value << 8 | 0x06);
}

static inline bool is_character(const obj_t o)
{
    return (obj_bits(o) & 0x1F) == 0x06;
}

static inline char_t character_value(obj_t ch)
{
    ((is_character(ch)) ? (void)0 : raise(&violation, (ch), "character_value: must be character"));
    return obj_bits(ch) >> 8;
}
# 7 "types.h" 2
# 1 "obj_fixnum.h" 1





static inline obj_t make_fixnum(word_t value)
{
    return (obj_t)(value << 1 | 0x01);
}

static inline bool is_fixnum(obj_t obj)
{
    return (obj_bits(obj) & 0x01) == 0x01;
}

static inline word_t fixnum_value(obj_t n)
{
    ((is_fixnum(n)) ? (void)0 : raise(&violation, (n), "fixnum_value: must be fixnum"));
    return obj_bits(n) >> 1;
}
# 8 "types.h" 2
# 1 "obj_string.h" 1






extern obj_t make_string (size_t len, char_t fill);
extern obj_t make_string_uninitialized
                                         (size_t len);
extern obj_t make_string_from_chars
                                         (const char_t *value, size_t len);
extern bool is_string (obj_t);
extern size_t string_len (obj_t);
extern const char_t *string_value (obj_t);
extern void string_set_char (obj_t, size_t index, char_t wc);
extern void string_set_substring(obj_t string,
       size_t pos,
       size_t len,
       const char_t *substring);
extern int strings_cmp (obj_t str1, obj_t str2);

static inline bool strings_are_equal (obj_t str1, obj_t str2)
{
    return strings_cmp(str1, str2) == 0;
}
# 9 "types.h" 2

# 1 "obj_pair.h" 1





extern obj_t make_pair(obj_t car, obj_t cdr);
extern bool is_pair(obj_t);
extern obj_t pair_car(obj_t);
extern obj_t pair_cdr(obj_t);
extern void pair_set_car(obj_t pair, obj_t car);
extern void pair_set_cdr(obj_t pair, obj_t cdr);

static inline obj_t pair_caar(obj_t pair)
{
    return pair_car(pair_car(pair));
}

static inline obj_t pair_cadr(obj_t pair)
{
    return pair_car(pair_cdr(pair));
}

static inline obj_t pair_cdar(obj_t pair)
{
    return pair_cdr(pair_car(pair));
}

static inline obj_t pair_cddr(obj_t pair)
{
    return pair_cdr(pair_cdr(pair));
}

static inline obj_t pair_cadar(obj_t pair)
{
    return pair_car(pair_cdr(pair_car(pair)));
}

static inline obj_t pair_caddr(obj_t pair)
{
    return pair_car(pair_cdr(pair_cdr(pair)));
}

static inline obj_t pair_cadddr(obj_t pair)
{
    return pair_car(pair_cdr(pair_cdr(pair_cdr(pair))));
}

static inline obj_t pair_cdaddr(obj_t pair)
{
    return pair_cdr(pair_car(pair_cdr(pair_cdr(pair))));
}

static inline obj_t pair_cddddr(obj_t pair)
{
    return pair_cdr(pair_cdr(pair_cdr(pair_cdr(pair))));
}

static inline obj_t pair_cdadddr(obj_t pair)
{
    return pair_cdr(pair_car(pair_cdr(pair_cdr(pair_cdr(pair)))));
}
# 11 "types.h" 2


# 1 "obj_bytevector.h" 1





typedef unsigned char byte_t;

extern obj_t make_bytevector(size_t len, byte_t fill);
extern obj_t make_bytevector_uninitialized(size_t len);
extern bool is_bytevector(obj_t);
extern size_t bytevector_len(obj_t);
extern byte_t bytevector_get(obj_t, size_t index);
extern void bytevector_set(obj_t, size_t index, byte_t elem);
extern byte_t *bytevector_addr(obj_t);
# 14 "types.h" 2
# 13 "read.c" 2
# 101 "read.c"
static inline bool stack_is_empty(const obj_t stack)
{
    return is_null(stack);
}

static inline void stack_push(obj_t *stack, obj_t elem)
{
    *stack = make_pair(elem, *stack);
}

static inline obj_t stack_pop(obj_t *stack)
{
    obj_t elem = pair_car(*stack);
    *stack = pair_cdr(*stack);
    return elem;
}

static inline obj_t stack_top(obj_t stack)
{
    return pair_car(stack);
}
# 185 "read.c"
typedef enum char_type {
    CT_NONE = 0,
    CT_TERMINAL = 0x40,
    CT_NONTERMINAL = 0x80,
    CTMASK = 0xC0,
    SYMMASK = ~(CTMASK)
} char_type_t;

typedef uint_fast16_t exterminal_set_t;
# 205 "read.c"
typedef struct production {
    char p_lhs;
    const char *p_rhs;
    int p_action;
} production_t;





typedef struct token_pair {
    char tm_term;
    token_type_t tm_ttype;
} token_pair_t;




static const production_t grammar[] = {
    { 'p', "xp", },
    { 'p', "d", },
    { 'p', "", },

    { 'd', "N", },
    { 'd', "S", },
    { 'd', "(i)", (((0) << 8 | 0x816)) },
    { 'd', "[i]", (((0) << 8 | 0x816)) },
    { 'd', "Ve)", (((1) << 8 | 0x816)) },
    { 'd', "Bb)", (((2) << 8 | 0x816)) },
    { 'd', "Ad", (((5) << 8 | 0x816)) },

    { 'i', "dj", },
    { 'i', "xi", },
    { 'i', "", (((3) << 8 | 0x816)) },

    { 'j', "dj", },
    { 'j', "xj", },
    { 'j', ".ydy", (((4) << 8 | 0x816)) },
    { 'j', "", (((3) << 8 | 0x816)) },

    { 'e', "de", },
    { 'e', "xe", },
    { 'e', "", (((3) << 8 | 0x816)) },

    { 'b', "Nb", },
    { 'b', "xb", },
    { 'b', "", (((3) << 8 | 0x816)) },

    { 'x', ";d", (((6) << 8 | 0x816)) },

    { 'y', "xy", },
    { 'y', "", },
};
static const size_t grammar_size = sizeof grammar / sizeof *grammar;





static token_pair_t token_pairs[] = {
    { 'N', TOK_EXACT_NUMBER },
    { 'S', TOK_SIMPLE },
    { 'A', TOK_ABBREV },
    { ';', TOK_COMMENT },
    { 'V', TOK_BEGIN_VECTOR },
    { 'B', TOK_BEGIN_BYTEVECTOR },
    { '(', TOK_LPAREN },
    { ')', TOK_RPAREN },
    { '.', TOK_PERIOD },
    { '[', TOK_LBRACKET },
    { ']', TOK_RBRACKET },
    { '$', TOK_EOF },
};
static size_t token_pairs_size = sizeof token_pairs / sizeof *token_pairs;




static const uint_fast8_t NO_RULE = (255);







static uint_fast8_t charmap[256];
static const size_t charmap_size = sizeof charmap / sizeof *charmap;

static inline bool char_is_nonterminal(char c)
{
    return (charmap[(uint_fast8_t)c] & CTMASK) == CT_NONTERMINAL;
}

static char start_symbol;
static size_t epsilon;
static size_t terminals_size;
static size_t exterminals_size;
static size_t nonterminals_size;
# 319 "read.c"
static char symbols[(((12 + 1) + 8))]; static size_t symbols_size = 0; static size_t alloc_symbols(size_t n) { size_t index = symbols_size; symbols_size += n; ((symbols_size <= (((12 + 1) + 8))) ? (void) (0) : __assert_fail ("symbols_size <= (((12 + 1) + 8))", "read.c", 319, __PRETTY_FUNCTION__)); return index; } static inline char *next_symbols(size_t n) { return &symbols[alloc_symbols(n)]; } __attribute__((destructor)) static void verify_size_symbols(void) { if (symbols_size != (((12 + 1) + 8))) { fprintf(stderr, "read.c" ":%d: warning: " "array `" "symbols" "' allocated %d, " "used %zd %s.\n", 319, (((12 + 1) + 8)), symbols_size, symbols_size == 1 ? "entry" : "entries"); } };
static exterminal_set_t sym_first[(((12 + 1) + 8))]; static size_t sym_first_size = 0; static size_t alloc_sym_first(size_t n) { size_t index = sym_first_size; sym_first_size += n; ((sym_first_size <= (((12 + 1) + 8))) ? (void) (0) : __assert_fail ("sym_first_size <= (((12 + 1) + 8))", "read.c", 320, __PRETTY_FUNCTION__)); return index; } static inline exterminal_set_t *next_sym_first(size_t n) { return &sym_first[alloc_sym_first(n)]; } __attribute__((destructor)) static void verify_size_sym_first(void) { if (sym_first_size != (((12 + 1) + 8))) { fprintf(stderr, "read.c" ":%d: warning: " "array `" "sym_first" "' allocated %d, " "used %zd %s.\n", 320, (((12 + 1) + 8)), sym_first_size, sym_first_size == 1 ? "entry" : "entries"); } };
static exterminal_set_t follow[(8)]; static size_t follow_size = 0; static size_t alloc_follow(size_t n) { size_t index = follow_size; follow_size += n; ((follow_size <= (8)) ? (void) (0) : __assert_fail ("follow_size <= (8)", "read.c", 321, __PRETTY_FUNCTION__)); return index; } static inline exterminal_set_t *next_follow(size_t n) { return &follow[alloc_follow(n)]; } __attribute__((destructor)) static void verify_size_follow(void) { if (follow_size != (8)) { fprintf(stderr, "read.c" ":%d: warning: " "array `" "follow" "' allocated %d, " "used %zd %s.\n", 321, (8), follow_size, follow_size == 1 ? "entry" : "entries"); } };
static uint_fast8_t parsing_table[((12 * 8))]; static size_t parsing_table_size = 0; static size_t alloc_parsing_table(size_t n) { size_t index = parsing_table_size; parsing_table_size += n; ((parsing_table_size <= ((12 * 8))) ? (void) (0) : __assert_fail ("parsing_table_size <= ((12 * 8))", "read.c", 322, __PRETTY_FUNCTION__)); return index; } static inline uint_fast8_t *next_parsing_table(size_t n) { return &parsing_table[alloc_parsing_table(n)]; } __attribute__((destructor)) static void verify_size_parsing_table(void) { if (parsing_table_size != ((12 * 8))) { fprintf(stderr, "read.c" ":%d: warning: " "array `" "parsing_table" "' allocated %d, " "used %zd %s.\n", 322, ((12 * 8)), parsing_table_size, parsing_table_size == 1 ? "entry" : "entries"); } };
# 336 "read.c"
static void init_symbols(void)
{
    size_t i, j;
    const char *p;

    start_symbol = grammar[0].p_lhs;
    for (i = 0; i < charmap_size; i++)
 charmap[i] = CT_NONE;
    for (i = 0; i < grammar_size; i++)
 charmap[(uint_fast8_t)grammar[i].p_lhs] = CT_NONTERMINAL;
    for (i = 0; i < grammar_size; i++)
 for (p = grammar[i].p_rhs; *p; p++)
     if (charmap[(uint_fast8_t)*p] == CT_NONE)
  charmap[(uint_fast8_t)*p] = CT_TERMINAL;
    ((charmap['$'] == CT_NONE) ? (void) (0) : __assert_fail ("charmap['$'] == CT_NONE", "read.c", 350, __PRETTY_FUNCTION__));
    charmap['$'] = CT_TERMINAL;
    ((charmap['-'] == CT_NONE) ? (void) (0) : __assert_fail ("charmap['-'] == CT_NONE", "read.c", 352, __PRETTY_FUNCTION__));
    for (i = j = 0; i < token_pairs_size; i++, j++) {
 token_pair_t *tpp = &token_pairs[i];
 ((tpp->tm_ttype < token_pairs_size) ? (void) (0) : __assert_fail ("tpp->tm_ttype < token_pairs_size", "read.c", 355, __PRETTY_FUNCTION__));
 uint_fast8_t *cmp = &charmap[(uint_fast8_t)tpp->tm_term];
 ((*cmp == CT_TERMINAL) ? (void) (0) : __assert_fail ("*cmp == CT_TERMINAL", "read.c", 357, __PRETTY_FUNCTION__));
 *cmp |= tpp->tm_ttype;
 *next_symbols(1) = tpp->tm_term;
    }
    terminals_size = symbols_size;
    charmap['-'] = CT_TERMINAL | j++;
    epsilon = symbols_size;
    *next_symbols(1) = '-';
    exterminals_size = symbols_size;
    for (i = 0; i < charmap_size; i++)
 if (charmap[i] == CT_NONTERMINAL) {
     charmap[i] |= j++;
     *next_symbols(1) = i;
 }
    nonterminals_size = symbols_size - exterminals_size;
    ((symbols_size < CTMASK) ? (void) (0) : __assert_fail ("symbols_size < CTMASK", "read.c", 372, __PRETTY_FUNCTION__));
# 392 "read.c"
}


static inline size_t sym_index(char sym)
{
    uint_fast8_t cm = charmap[(size_t)sym];
    ((cm & CTMASK) ? (void) (0) : __assert_fail ("cm & CTMASK", "read.c", 398, __PRETTY_FUNCTION__));
    return cm & SYMMASK;
}


static inline size_t term_index(char term)
{
    uint_fast8_t cm = charmap[(size_t)term];
    ((cm & CT_TERMINAL) ? (void) (0) : __assert_fail ("cm & CT_TERMINAL", "read.c", 406, __PRETTY_FUNCTION__));
    return cm & SYMMASK;
}


static inline size_t nonterm_index(char nonterm)
{
    uint_fast8_t cm = charmap[(size_t)nonterm];
    (((cm & CTMASK) == CT_NONTERMINAL) ? (void) (0) : __assert_fail ("(cm & CTMASK) == CT_NONTERMINAL", "read.c", 414, __PRETTY_FUNCTION__));
    return (cm & SYMMASK) - exterminals_size;
}


static inline char terminal(size_t term_index)
{
    ((term_index < terminals_size) ? (void) (0) : __assert_fail ("term_index < terminals_size", "read.c", 421, __PRETTY_FUNCTION__));
    return symbols[term_index];
}


static inline char nonterminal(size_t nonterm_index)
{
    ((nonterm_index < nonterminals_size) ? (void) (0) : __assert_fail ("nonterm_index < nonterminals_size", "read.c", 428, __PRETTY_FUNCTION__));
    return symbols[nonterm_index + exterminals_size];
}
# 445 "read.c"
static exterminal_set_t first(const char *w);

static void init_first(void)
{
    size_t i, j;
    for (i = 0; i < exterminals_size; i++)
 *next_sym_first(1) = 1 << i;
    for (i = 0; i < nonterminals_size; i++)
 *next_sym_first(1) = 0;
    for (i = 0; i < grammar_size; i++)
 if (!*grammar[i].p_rhs)
     sym_first[sym_index(grammar[i].p_lhs)] |= 1 << epsilon;
    bool done = false;
    while (!done) {
 done = true;
 for (i = 0; i < grammar_size; i++) {
     const production_t *pp = &grammar[i];
     exterminal_set_t *x = &sym_first[sym_index(pp->p_lhs)];
     for (j = 0; pp->p_rhs[j]; j++) {
  exterminal_set_t *y = &sym_first[sym_index(pp->p_rhs[j])];
  if (*y & ~*x) {
      exterminal_set_t b = *x;
      *x |= *y;
      exterminal_set_t a = *x;
      ((b != a) ? (void) (0) : __assert_fail ("b != a", "read.c", 469, __PRETTY_FUNCTION__));
      ((a & ~b) ? (void) (0) : __assert_fail ("a & ~b", "read.c", 470, __PRETTY_FUNCTION__));
      done = false;
  }
  if (!(*y & 1 << epsilon))
      break;
     }
 }
    }
# 504 "read.c"
}


static exterminal_set_t first(const char *w)
{
    const char *p;
    exterminal_set_t f = 1 << epsilon;
    for (p = w; *p; p++) {
 exterminal_set_t yf = sym_first[sym_index(*p)];
 f |= yf;
 if (!(yf & 1 << epsilon)) {
     f &= ~(1 << epsilon);
     break;
 }
    }
    return f;
}
# 529 "read.c"
static void init_follow(void)
{
    int i, j;
    const char *p;

    for (i = 0; i < nonterminals_size; i++)
 *next_follow(1) = 0;
    follow[nonterm_index(start_symbol)] = 1 << sym_index('$');
    for (i = 0; i < grammar_size; i++)
 for (p = grammar[i].p_rhs; *p; p++) {
     char b = *p;
     const char *beta = p + 1;
     if (char_is_nonterminal(b))
  follow[nonterm_index(b)] |= first(beta) & ~(1 << epsilon);
 }
    bool done = false;
    while (!done) {
 done = true;
 for (i = 0; i < grammar_size; i++) {
     const production_t *pp = &grammar[i];
     char a = pp->p_lhs;
     exterminal_set_t *fa = &follow[nonterm_index(a)];
     for (j = strlen(pp->p_rhs); --j >= 0; ) {
  char b = pp->p_rhs[j];
  const char *beta = pp->p_rhs + j + 1;
  if (!(first(beta) & 1 << epsilon))
      break;
  if (char_is_nonterminal(b)) {
      exterminal_set_t *fb = &follow[nonterm_index(b)];
      if (*fa & ~*fb) {
   done = false;
   *fb |= *fa;
      }
  }
     }
 }
    }
# 578 "read.c"
}





static int pt_entry(char A, char a)
{
    int found = NO_RULE;
    size_t ia = sym_index(a);
    exterminal_set_t foA = follow[nonterm_index(A)];
    bool a_follows_A = (bool)(foA & 1 << ia);
    size_t i;
    for (i = 0; i < grammar_size; i++) {
 const production_t *pp = &grammar[i];
 if (pp->p_lhs == A) {
     exterminal_set_t fig = first(pp->p_rhs);
     if (fig & 1 << ia) {
  ((found == NO_RULE && "grammar not LL(1)") ? (void) (0) : __assert_fail ("found == NO_RULE && \"grammar not LL(1)\"", "read.c", 596, __PRETTY_FUNCTION__));
  found = i;
     }
     if (a_follows_A && (fig & 1 << epsilon)) {
  ((found == NO_RULE && "grammar not LL(1)") ? (void) (0) : __assert_fail ("found == NO_RULE && \"grammar not LL(1)\"", "read.c", 600, __PRETTY_FUNCTION__));
  found = i;
     }
 }
    }
    return found;
}

static uint_fast8_t parsing_table_entry(size_t i, size_t j)
{
    ((i < nonterminals_size) ? (void) (0) : __assert_fail ("i < nonterminals_size", "read.c", 610, __PRETTY_FUNCTION__));
    ((j < terminals_size) ? (void) (0) : __assert_fail ("j < terminals_size", "read.c", 611, __PRETTY_FUNCTION__));
    return parsing_table[i * terminals_size + j];
}







static void init_parsing_table(void)
{
    size_t i, j;

    for (i = 0; i < nonterminals_size; i++) {
 char A = nonterminal(i);
 for (j = 0; j < terminals_size; j++) {
     char a = terminal(j);
     *next_parsing_table(1) = pt_entry(A, a);
 }
    }
# 657 "read.c"
}

__attribute__((constructor))
static void init_parser(void)
{
    init_symbols();
    init_first();
    init_follow();
    init_parsing_table();
}



static uint_fast8_t get_rule(char symbol, size_t term)
{
    uint_fast8_t rule = NO_RULE;
    if (char_is_nonterminal(symbol))
 rule = parsing_table_entry(nonterm_index(symbol), term);
    return rule;
}





static obj_t parse(instream_t *in)
{
    obj_t actions = ((obj_t)0x016);
    obj_t yylval = ((obj_t)0x016);
    obj_t tmp = make_fixnum(TOK_EOF);
    obj_t stack = ((obj_t)0x016);
    stack_push(&stack, tmp);
    tmp = make_fixnum(sym_index(start_symbol));
    stack_push(&stack, tmp);
    int tok = yylex(&yylval, in);
    while (true) {
 int sym = fixnum_value(stack_pop(&stack));
 ((0 <= sym && sym < symbols_size) ? (void) (0) : __assert_fail ("0 <= sym && sym < symbols_size", "read.c", 694, __PRETTY_FUNCTION__));
 uint_fast8_t rule = get_rule(symbols[sym], tok);
 if (rule != NO_RULE) {
     const production_t *pp = &grammar[rule];
     int j;
     for (j = strlen(pp->p_rhs); --j >= 0; ) {
  tmp = make_fixnum(sym_index(pp->p_rhs[j]));
  stack_push(&stack, tmp);
     }
     if (pp->p_action)
  stack_push(&actions, (obj_t)pp->p_action);
 } else {
     if (sym == TOK_EOF)
  break;

     ((sym == tok && "syntax error") ? (void) (0) : __assert_fail ("sym == tok && \"syntax error\"", "read.c", 709, __PRETTY_FUNCTION__));
     if (!is_null(yylval))
  stack_push(&actions, yylval);
     if (!stack_is_empty(actions) &&
  fixnum_value(stack_top(stack)) == TOK_EOF)
  break;
     yylval = ((obj_t)0x016);
     tok = yylex(&yylval, in);
 }
    }
    return actions;
}


static obj_t build_vector(obj_t list)
{
# 741 "read.c"
    return ((obj_t)0x016);

}


static obj_t build_bytevec(obj_t list)
{
    root_descriptor_t auto_root_748 = { L"list", __func__, &list, ((void *)0), ((void *)0) }; push_root(&auto_root_748);;
    obj_t p = list;
    size_t i, size = 0;
    while (!is_null(p)) {
 size++;
 p = pair_cdr(p);
    }
    obj_t bvec = (make_bytevector(size, 0)); root_descriptor_t auto_root_755 = { L"bvec", __func__, &bvec, ((void *)0), ((void *)0) }; push_root(&auto_root_755);;;
    for (i = 0, p = list; i < size; i++) {
 bytevector_set(bvec, i, fixnum_value(pair_car(p)));
 p = pair_cdr(p);
    }
    pop_function_roots(__func__);
    return bvec;
}


static bool build(obj_t actions, obj_t *obj_out)
{
# 782 "read.c"
    obj_t vstack = ((obj_t)0x016);
    obj_t reg = ((obj_t)0x016);
    obj_t tmp = ((obj_t)0x016);
    while (!stack_is_empty(actions)) {
 obj_t op = stack_pop(&actions);
 switch ((word_t)op) {

 default:
     reg = make_pair(op, reg);
     continue;

 case (((0) << 8 | 0x816)):
     reg = make_pair(reg, stack_pop(&vstack));
     continue;

 case (((1) << 8 | 0x816)):
     reg = build_vector(reg);
     reg = make_pair(reg, stack_pop(&vstack));
     continue;

 case (((2) << 8 | 0x816)):
     reg = build_bytevec(reg);
     reg = make_pair(reg, stack_pop(&vstack));
     continue;

 case (((5) << 8 | 0x816)):
     tmp = make_pair(pair_cadr(reg), ((obj_t)0x016));
     tmp = make_pair(pair_car(reg), tmp);
     reg = make_pair(tmp, pair_cddr(reg));
     continue;

 case (((3) << 8 | 0x816)):
     stack_push(&vstack, reg);
     reg = ((obj_t)0x016);
     continue;

 case (((4) << 8 | 0x816)):
     stack_push(&vstack, pair_cdr(reg));
     reg = pair_car(reg);
     continue;

 case (((6) << 8 | 0x816)):
     reg = pair_cdr(reg);
     continue;
    }
    ((stack_is_empty(vstack)) ? (void) (0) : __assert_fail ("stack_is_empty(vstack)", "read.c", 827, __PRETTY_FUNCTION__));

    bool success = false;
    if (!is_null(reg)) {
 ((is_null(pair_cdr(reg))) ? (void) (0) : __assert_fail ("is_null(pair_cdr(reg))", "read.c", 831, __PRETTY_FUNCTION__));
 *obj_out = pair_car(reg);
 success = true;
    }
    return success;
}

bool read_stream(instream_t *in, obj_t *obj_out)
{
    return build(parse(in), obj_out);
}


__attribute__((constructor))
static void register_TP_READ_test_844(void)
{
    static test_case_descriptor_t tc = {
	(TP_READ),
	(L"(a b)"),
	(L"(a b)"),
	sizeof (L"(a b)") / sizeof *(L"(a b)") - 1,
	"read.c",
	844,
	((void *)0)
    };
    register_test(&tc);
};


__attribute__((constructor)) static void register_TP_EVAL_test_845(void) { static test_case_descriptor_t tc = { (TP_EVAL), (L"(pair? '(a b))"), (L"#t"), sizeof (L"#t") / sizeof *(L"#t") - 1, "read.c", 845, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_846(void) { static test_case_descriptor_t tc = { (TP_READ), (L"[a b]"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 846, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_EVAL_test_847(void) { static test_case_descriptor_t tc = { (TP_EVAL), (L"(pair? '[a b])"), (L"#t"), sizeof (L"#t") / sizeof *(L"#t") - 1, "read.c", 847, ((void *)0) }; register_test(&tc); };


__attribute__((constructor)) static void register_TP_READ_test_850(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#(a b)"), (L"#(a b)"), sizeof (L"#(a b)") / sizeof *(L"#(a b)") - 1, "read.c", 850, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_851(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#(a (b c))"), (L"#(a (b c))"), sizeof (L"#(a (b c))") / sizeof *(L"#(a (b c))") - 1, "read.c", 851, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_852(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#(a #(b c))"), (L"#(a #(b c))"), sizeof (L"#(a #(b c))") / sizeof *(L"#(a #(b c))") - 1, "read.c", 852, ((void *)0) }; register_test(&tc); };


__attribute__((constructor)) static void register_TP_READ_test_855(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#vu8()"), (L"#vu8()"), sizeof (L"#vu8()") / sizeof *(L"#vu8()") - 1, "read.c", 855, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_856(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#vu8(1 2)"), (L"#vu8(1 2)"), sizeof (L"#vu8(1 2)") / sizeof *(L"#vu8(1 2)") - 1, "read.c", 856, ((void *)0) }; register_test(&tc); };


__attribute__((constructor)) static void register_TP_READ_test_859(void) { static test_case_descriptor_t tc = { (TP_READ), (L"'a"), (L"(quote a)"), sizeof (L"(quote a)") / sizeof *(L"(quote a)") - 1, "read.c", 859, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_860(void) { static test_case_descriptor_t tc = { (TP_READ), (L"'(a b)"), (L"(quote (a b))"), sizeof (L"(quote (a b))") / sizeof *(L"(quote (a b))") - 1, "read.c", 860, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_861(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#('a '(a b))"), (L"#((quote a) (quote (a b)))"), sizeof (L"#((quote a) (quote (a b)))") / sizeof *(L"#((quote a) (quote (a b)))") - 1, "read.c", 861, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_862(void) { static test_case_descriptor_t tc = { (TP_READ), (L"('a b c)"), (L"((quote a) b c)"), sizeof (L"((quote a) b c)") / sizeof *(L"((quote a) b c)") - 1, "read.c", 862, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_863(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a 'b c)"), (L"(a (quote b) c)"), sizeof (L"(a (quote b) c)") / sizeof *(L"(a (quote b) c)") - 1, "read.c", 863, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_864(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a b 'c)"), (L"(a b (quote c))"), sizeof (L"(a b (quote c))") / sizeof *(L"(a b (quote c))") - 1, "read.c", 864, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_865(void) { static test_case_descriptor_t tc = { (TP_READ), (L"'''a"), (L"(quote (quote (quote a)))"), sizeof (L"(quote (quote (quote a)))") / sizeof *(L"(quote (quote (quote a)))") - 1, "read.c", 865, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_866(void) { static test_case_descriptor_t tc = { (TP_READ), (L"`a"), (L"(quasiquote a)"), sizeof (L"(quasiquote a)") / sizeof *(L"(quasiquote a)") - 1, "read.c", 866, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_867(void) { static test_case_descriptor_t tc = { (TP_READ), (L",a"), (L"(unquote a)"), sizeof (L"(unquote a)") / sizeof *(L"(unquote a)") - 1, "read.c", 867, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_868(void) { static test_case_descriptor_t tc = { (TP_READ), (L",@a"), (L"(unquote-splicing a)"), sizeof (L"(unquote-splicing a)") / sizeof *(L"(unquote-splicing a)") - 1, "read.c", 868, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_869(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#'a"), (L"(syntax a)"), sizeof (L"(syntax a)") / sizeof *(L"(syntax a)") - 1, "read.c", 869, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_870(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#`a"), (L"(quasisyntax a)"), sizeof (L"(quasisyntax a)") / sizeof *(L"(quasisyntax a)") - 1, "read.c", 870, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_871(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#,a"), (L"(unsyntax a)"), sizeof (L"(unsyntax a)") / sizeof *(L"(unsyntax a)") - 1, "read.c", 871, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_872(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#,@a"), (L"(unsyntax-splicing a)"), sizeof (L"(unsyntax-splicing a)") / sizeof *(L"(unsyntax-splicing a)") - 1, "read.c", 872, ((void *)0) }; register_test(&tc); };


__attribute__((constructor)) static void register_TP_READ_test_875(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#; asdf ghjk"), (L"ghjk"), sizeof (L"ghjk") / sizeof *(L"ghjk") - 1, "read.c", 875, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_876(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;()b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 876, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_877(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;(comment)b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 877, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_878(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#; \t(comment) b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 878, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_879(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;(\n)b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 879, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_880(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;\t()b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 880, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_881(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;((c)(d))b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 881, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_882(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(#;c a . b)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 882, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_883(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(#;c#;c a . b)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 883, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_884(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;c . b)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 884, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_885(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a #;c#;c . b)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 885, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_886(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a #;c#;c . #;c b)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 886, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_887(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a #;c . #;c #;c b)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 887, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_888(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a #;c#;c . #;c b #;c)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 888, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_889(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a . #;c#;c b#;c#;c)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 889, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_890(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#;c . #;c#;c b#;c#;c)"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 890, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_891(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a . #;()#;() b#;()#;())"), (L"(a . b)"), sizeof (L"(a . b)") / sizeof *(L"(a . b)") - 1, "read.c", 891, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_892(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a #;(b #;(c d) e) f)"), (L"(a f)"), sizeof (L"(a f)") / sizeof *(L"(a f)") - 1, "read.c", 892, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_893(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(a#!r6rs b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 893, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_894(void) { static test_case_descriptor_t tc = { (TP_READ), (L"#!r6rs(a b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 894, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_895(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(#!r6rs a b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 895, ((void *)0) }; register_test(&tc); };
__attribute__((constructor)) static void register_TP_READ_test_896(void) { static test_case_descriptor_t tc = { (TP_READ), (L"(#!r6\x33s a b)"), (L"(a b)"), sizeof (L"(a b)") / sizeof *(L"(a b)") - 1, "read.c", 896, ((void *)0) }; register_test(&tc); };
