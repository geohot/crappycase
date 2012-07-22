#include <Foundation/Foundation.h>

int main(int argc, char *argv[]) {

  if (access("testCase.m", 4) == 0) printf("passed\n");
  else printf("FAILED\n");
  if (access("/Users/geohot/downloads/Diablo III Setup.app/Contents/Resources/Installer Tome 1.mpq", 4) == 0) printf("passed\n");
  else printf("FAILED\n");

  return 0;

  NSAutoreleasePool *pool = [NSAutoreleasePool new];
  NSString *path = [NSString stringWithCString:"/Applications" encoding:NSASCIIStringEncoding];
  NSURL *filesystem = [NSURL fileURLWithPath:path isDirectory:YES];
  NSNumber *caseSensitiveFS;
  BOOL hasCaseSensitiveResource;

  printf("got filesystem ptr\n");

  hasCaseSensitiveResource = [filesystem getResourceValue:&caseSensitiveFS forKey:NSURLVolumeSupportsCaseSensitiveNamesKey error:NULL];

  printf("got case sensitive resource\n");

  if (hasCaseSensitiveResource) {
    if ([caseSensitiveFS intValue] == 1) {
      printf("yes case sensitive\n");
    } else {
      printf("no case insensitive\n");
    }
  } else {
    printf("fuck you\n");
  }

  [pool drain];
  return 0;
}

