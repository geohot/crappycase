// this is free as long as you hate the companies that can't code

#include <stdio.h>
#include <sys/syscall.h>
#include <sys/attr.h>
//#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

// change this 
#define LOG_FILE "/tmp/crappycase.log"

#define qa(path) (syscall(SYS_access, path, 4)==0)

FILE* cc_logfile;

// replaces end with good and returns true if it did a good job
int find_replace(char* path) {
  char pathbuf2[0x400]; // buffer overflow, yay?
  strcpy(pathbuf2, path);
  char* a = strrchr(pathbuf2, '/'); a[0] = '\0';
  char* filename = a+1;
  //printf("trying to fix %s in directory %s\n", path, pathbuf2);

  DIR* dp = opendir(pathbuf2);
  struct dirent* ep;
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //printf("  %s\n", ep->d_name);
      if (strcasecmp(ep->d_name, filename) == 0) {
        //printf("  switch %s to %s\n", filename, ep->d_name);
        // i <3 pointer arithmetic
        strcpy(path + (filename-pathbuf2), ep->d_name);
        //printf("  new is %s\n", path);
        return qa(path);
      }
    }
    closedir(dp);
  }
  //printf("  can't be fixed\n");
  
  return 0;
}


// replace with existing file with different case if possible
// lol at brute force :)
char* fix_path(const char* name, void* p) {
  char* rp = malloc(0x400); //malloc(strlen(p)+1);
  char pathbuf[0x400]; // buffer overflow, yay?
  char* path = (char *)p;
  strcpy(rp, path); // syscalls rely on this...
  int didfix = 0;

  //printf("%s: (%d) %s\n", name, qa(rp), rp);

// do the magic
  if (rp[0] != '/') {
    getcwd(rp, 0x1000);
    strcat(rp, "/");
    strcat(rp, path);
  }
  char* pathptr = rp+1;
  while (pathptr=strchr(pathptr,'/')) {
    memcpy(pathbuf, rp, pathptr-rp);
    pathbuf[pathptr-rp] = '\0';
    //printf("checking %s: %d\n", pathbuf, qa(pathbuf));
    if (qa(pathbuf) == 0) {
      if (find_replace(pathbuf) == 0) {
        goto done;
      } else {
        //printf("replacing...%s %s %d\n", rp, pathbuf, strlen(pathbuf));
        memcpy(rp, pathbuf, strlen(pathbuf));
        didfix = 1;
      }
    }
    pathptr++;
  }

  if (qa(rp) == 0) {
    if (find_replace(rp) == 1) {
      didfix = 1;
    }
  }

done:
  if (didfix == 1) {
    //printf("%s: %s -> %s\n", name, path, rp);
// logging
    if (cc_logfile != NULL) {
      fprintf(cc_logfile, "%s: %s -> %s\n", name, path, rp);
      fflush(cc_logfile);
    }
  }
  return rp;
}


#define sa1(NAME) int NAME(void* a1) { char* rp = fix_path(""#NAME"", a1); int ret = syscall(SYS_##NAME, rp); free(rp); return ret;}
#define sa2(NAME) int NAME(void* a1, void* a2) { char* rp = fix_path(""#NAME"", a1); int ret = syscall(SYS_##NAME, rp, a2); free(rp); return ret;}
#define sa3(NAME) int NAME(void* a1, void* a2, void* a3) { char* rp = fix_path(""#NAME"", a1); int ret = syscall(SYS_##NAME, rp, a2, a3); free(rp); return ret;}
#define sa4(NAME) int NAME(void* a1, void* a2, void* a3, void* a4) { char* rp = fix_path(""#NAME"", a1); int ret = syscall(SYS_##NAME, rp, a2, a3, a4); free(rp); return ret;}
#define sa5(NAME) int NAME(void* a1, void* a2, void* a3, void* a4, void* a5) { char* rp = fix_path(""#NAME"", a1); int ret = syscall(SYS_##NAME, rp, a2, a3, a4, a5); free(rp); return ret;}
#define sa6(NAME) int NAME(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6) { char* rp = fix_path(""#NAME"", a1); int ret = syscall(SYS_##NAME, rp, a2, a3, a4, a5, a6); free(rp); return ret;}

#define sa3UNIX(NAME) int NAME##$UNIX2003(void* a1, void* a2, void* a3) { char* rp = fix_path(""#NAME"$UNIX2003", a1); int ret = syscall(SYS_##NAME, rp, a2, a3); free(rp); return ret; }

// 5  AUE_OPEN_RWTC ALL { int open(user_addr_t path, int flags, int mode); } 
sa3(open)
sa3UNIX(open)
// 9  AUE_LINK  ALL { int link(user_addr_t path, user_addr_t link); } 
sa2(link);
// 10  AUE_UNLINK  ALL { int unlink(user_addr_t path); } 
sa1(unlink)
// 12 AUE_CHDIR ALL { int chdir(user_addr_t path); } 
sa1(chdir)
// 14  AUE_MKNOD ALL { int mknod(user_addr_t path, int mode, int dev); } 
sa3(mknod)
// 15  AUE_CHMOD ALL { int chmod(user_addr_t path, int mode); } 
sa2(chmod)
// 16  AUE_CHOWN ALL { int chown(user_addr_t path, int uid, int gid); } 
sa3(chown)
// 33  AUE_ACCESS  ALL { int access(user_addr_t path, int flags); } 
sa2(access)


// 34  AUE_CHFLAGS ALL { int chflags(char *path, int flags); } 
sa2(chflags)
// 51 AUE_ACCT  ALL { int acct(char *path); } 
sa1(acct)
// 56  AUE_REVOKE  ALL { int revoke(char *path); } 
sa1(revoke)
// 57  AUE_SYMLINK ALL { int symlink(char *path, char *link); } 
sa2(symlink)
// 58  AUE_READLINK  ALL { int readlink(char *path, char *buf, int count); } 
sa3(readlink)
// 59 AUE_EXECVE  ALL { int execve(char *fname, char **argp, char **envp); } 
sa3(execve)
// 61 AUE_CHROOT  ALL { int chroot(user_addr_t path); } 
sa1(chroot)
// 132  AUE_MKFIFO  ALL { int mkfifo(user_addr_t path, int mode); }
sa2(mkfifo)
// 136 AUE_MKDIR ALL { int mkdir(user_addr_t path, int mode); } 
sa2(mkdir)
// 137 AUE_RMDIR ALL { int rmdir(char *path); } 
sa1(rmdir)
// 138 AUE_UTIMES  ALL { int utimes(char *path, struct timeval *tptr); }
//sa2(utimes)

// 157  AUE_STATFS  ALL { int statfs(char *path, struct statfs *buf); } 
sa2(statfs)
/*int statfs(char* a1, struct statfs* a2) { 
  printf("statfs hacking: %s\n", a1);
  int ret = syscall(SYS_statfs, a1, a2);
  return ret;
}*/
//log2(fstatfs)

// 188 AUE_STAT  ALL { int stat(user_addr_t path, user_addr_t ub); } 
sa2(stat)
// 190 AUE_LSTAT ALL { int lstat(user_addr_t path, user_addr_t ub); } 
sa2(lstat)
// 191 AUE_PATHCONF  ALL { int pathconf(char *path, int name); } 
sa2(pathconf)
// 200  AUE_TRUNCATE  ALL { int truncate(char *path, off_t length); } 
sa2(truncate)
// 205  AUE_UNDELETE  ALL { int undelete(user_addr_t path); } 
sa1(undelete)

// 216 AUE_MKCOMPLEX UHN { int mkcomplex(const char *path, mode_t mode, u_long type); }  { soon to be obsolete }
sa3(mkcomplex)
// 217 AUE_STATV UHN { int statv(const char *path, struct vstat *vsb); }   { soon to be obsolete }
sa2(statv)
// 218 AUE_LSTATV  UHN { int lstatv(const char *path, struct vstat *vsb); }  { soon to be obsolete }
sa2(lstatv)
// 220 AUE_GETATTRLIST ALL { int getattrlist(const char *path, struct attrlist *alist, void *attributeBuffer, size_t bufferSize, u_long options); } 
sa5(getattrlist)
/*int getattrlist(const char *path, struct attrlist *alist, void *attributeBuffer, size_t bufferSize, u_long options) {
  int ret = syscall(SYS_getattrlist, path, alist, attributeBuffer, bufferSize, options);

  uint32_t* weird = (uint32_t*)attributeBuffer;
  //printf("%d: %x %x %x and %x size %X\n", ret, weird[0], weird[1], weird[2], VOL_CAP_FMT_CASE_SENSITIVE|VOL_CAP_FMT_CASE_PRESERVING, bufferSize);
  printf("qattr(%d): %X %X %X %X %X\n", alist->bitmapcount, alist->commonattr, alist->volattr, alist->dirattr, alist->fileattr, alist->forkattr);
  int i;
  for (i = 0; i < weird[0]; i+= 4) {
    printf("%X ", weird[i/4]);
  }
  printf("\n");


  printf("getattrlist hacking: %s\n", path);
  return ret;
}*/
// 221 AUE_SETATTRLIST ALL { int setattrlist(const char *path, struct attrlist *alist, void *attributeBuffer, size_t bufferSize, u_long options); } 
sa5(setattrlist)

// 223  AUE_EXCHANGEDATA  ALL { int exchangedata(const char *path1, const char *path2, u_long options); }
sa3(exchangedata)
// 225  AUE_SEARCHFS  ALL { int searchfs(const char *path, struct fssearchblock *searchblock, uint32_t *nummatches, uint32_t scriptcode, uint32_t options, struct searchstate *state); } 
sa6(searchfs)
// 226  AUE_DELETE  ALL { int delete(user_addr_t path) NO_SYSCALL_STUB; }       { private delete (Carbon semantics) }
sa1(delete)
// 234  AUE_GETXATTR  ALL { user_ssize_t getxattr(user_addr_t path, user_addr_t attrname, user_addr_t value, size_t size, uint32_t position, int options); }
sa6(getxattr)
// 236  AUE_SETXATTR  ALL { int setxattr(user_addr_t path, user_addr_t attrname, user_addr_t value, size_t size, uint32_t position, int options); } 
sa6(setxattr)
// 238  AUE_REMOVEXATTR ALL { int removexattr(user_addr_t path, user_addr_t attrname, int options); } 
sa3(removexattr)
// 240  AUE_LISTXATTR ALL { user_ssize_t listxattr(user_addr_t path, user_addr_t namebuf, size_t bufsize, int options); } 
sa4(listxattr)
// 242  AUE_FSCTL ALL { int fsctl(const char *path, u_long cmd, caddr_t data, u_int options); } 
sa4(fsctl)
// 244  AUE_POSIX_SPAWN ALL { int posix_spawn(pid_t *pid, const char *path, const struct _posix_spawn_args_desc *adesc, char **argv, char **envp); } 
//sa5_2(posix_spawn)

// 277 AUE_OPEN_EXTENDED_RWTC  ALL { int open_extended(user_addr_t path, int flags, uid_t uid, gid_t gid, int mode, user_addr_t xsecurity) NO_SYSCALL_STUB; } 
sa6(open_extended)
// 279 AUE_STAT_EXTENDED ALL { int stat_extended(user_addr_t path, user_addr_t ub, user_addr_t xsecurity, user_addr_t xsecurity_size) NO_SYSCALL_STUB; } 
sa4(stat_extended)
// 280 AUE_LSTAT_EXTENDED  ALL { int lstat_extended(user_addr_t path, user_addr_t ub, user_addr_t xsecurity, user_addr_t xsecurity_size) NO_SYSCALL_STUB; } 
sa4(lstat_extended)
// 282 AUE_CHMOD_EXTENDED  ALL { int chmod_extended(user_addr_t path, uid_t uid, gid_t gid, int mode, user_addr_t xsecurity) NO_SYSCALL_STUB; } 
sa5(chmod_extended)
// 284 AUE_ACCESS_EXTENDED ALL { int access_extended(user_addr_t entries, size_t size, user_addr_t results, uid_t uid) NO_SYSCALL_STUB; } 
//sa4(access_extended)
// 291 AUE_MKFIFO_EXTENDED ALL { int mkfifo_extended(user_addr_t path, uid_t uid, gid_t gid, int mode, user_addr_t xsecurity) NO_SYSCALL_STUB; } 
sa4(mkfifo_extended)
// 292 AUE_MKDIR_EXTENDED  ALL { int mkdir_extended(user_addr_t path, uid_t uid, gid_t gid, int mode, user_addr_t xsecurity) NO_SYSCALL_STUB; }
sa4(mkdir_extended)

// 345  AUE_STATFS64  ALL { int statfs64(char *path, struct statfs64 *buf); }
sa2(statfs64)
//log2(fstatfs64)


// 380  AUE_MAC_EXECVE  ALL { int __mac_execve(char *fname, char **argp, char **envp, struct mac *mac_p); }
sa4(__mac_execve)
// 398  AUE_OPEN_RWTC ALL { int open_nocancel(user_addr_t path, int flags, int mode) NO_SYSCALL_STUB; } 
sa3(open_nocancel)

#include <dlfcn.h>

int PBHGetVolParmsSync(uint32_t *blk) {
  void* iokit = dlopen("/System/Library/Frameworks/CoreServices.framework/CoreServices", RTLD_LAZY);
  int (*PBH)(uint32_t *) = dlsym(iokit, "PBHGetVolParmsSync");
  uint8_t* info = (uint8_t*)(blk[8]);
  int ret = PBH(blk);
  info[0x16] &= ~0x40;
  printf("I AM THE FUCKING WALRUS: %X %X  %X\n", blk[8], blk[9], info[0x16]);
  return ret;
}


__attribute__((constructor))
static void initializer(void) {
  printf("hookers are hired\n");
  cc_logfile = fopen(LOG_FILE, "a");
  if (cc_logfile != NULL) {
    fprintf(cc_logfile, "------------------------\n"); fflush(cc_logfile);
  }
}

__attribute__((destructor))
static void finalizer(void) {
  printf("closing log\n");
  if (cc_logfile != NULL) {
    fprintf(cc_logfile, "************************\n");
    fclose(cc_logfile);
  }
}

