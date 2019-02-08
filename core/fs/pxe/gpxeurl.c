/*
 * Dummy gPXE stub
 */

#include "core_pxe.h"

void __attribute__((weak)) gpxe_open(struct inode *inode, const char *url)
{
    (void)inode;
    (void)url;
}
