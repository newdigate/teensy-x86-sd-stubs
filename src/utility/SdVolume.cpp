/* Arduino SdFat Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "SdFat.h"

uint8_t SdVolume::allocContiguous(uint32_t count, uint32_t* curCluster) {
  return 1;
}
//------------------------------------------------------------------------------
uint8_t SdVolume::cacheFlush(void) {
  return 1;
}
//------------------------------------------------------------------------------
uint8_t SdVolume::cacheRawBlock(uint32_t blockNumber, uint8_t action) {
  return 1;
}
//------------------------------------------------------------------------------
// cache a zero block for blockNumber
uint8_t SdVolume::cacheZeroBlock(uint32_t blockNumber) {

  return 1;
}
//------------------------------------------------------------------------------
// return the size in bytes of a cluster chain
uint8_t SdVolume::chainSize(uint32_t cluster, uint32_t* size) const {

  return 1;
}
//------------------------------------------------------------------------------
// Fetch a FAT entry
uint8_t SdVolume::fatGet(uint32_t cluster, uint32_t* value) const {

  return 1;
}
//------------------------------------------------------------------------------
// Store a FAT entry
uint8_t SdVolume::fatPut(uint32_t cluster, uint32_t value) {

  return 1;
}
//------------------------------------------------------------------------------
// free a cluster chain
uint8_t SdVolume::freeChain(uint32_t cluster) {
  // clear free cluster location
  allocSearchStart_ = 2;

  do {
    uint32_t next;
    if (!fatGet(cluster, &next)) return false;

    // free cluster
    if (!fatPut(cluster, 0)) return false;

    cluster = next;
  } while (!isEOC(cluster));

  return 1;
}

