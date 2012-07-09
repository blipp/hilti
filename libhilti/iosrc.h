// $Id$
//
// I/O source provide a general abstraction for dealing with high-volume
// external input/output (well, input only at the moment). Each I/O source
// implements a similar interface.
//
// For now, we online provide one implementation for libpcap input. More to
// come.
//
// Todo: This interface is quite preliminary; we'll need to refine it when we
// add more sources. For example, we probably want some kind of dispatcher
// functions that select the right driver based on the source's type.

/// \addtogroup iosrc
/// @{

#ifndef HILTI_IOSRC_H
#define HILTI_IOSRC_H

#include "types.h"
#include "enum.h"

/// The type of an IOSource as one of the Hilti::IOSrc constants.
typedef hlt_enum hlt_iosrc_type;

#include "iosrc-pcap.h"

/// @}

#endif
