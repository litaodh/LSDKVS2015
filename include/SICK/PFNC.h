// Copyright 2019 SICK AG. All rights reserved.

#pragma once

#include "BufferInfoVLB.h"

// From GenICam Pixel Format Naming Convention we get the
// pixel format values, imported from "PFNC.h" available at emva.org
const common::PixelFormat PFNC_Mono8 = 0x01080001;
const common::PixelFormat PFNC_Mono12p = 0x010C0047;
const common::PixelFormat PFNC_Mono16 = 0x01100007;
const common::PixelFormat PFNC_Coord3D_C8 = 0x010800B1;
const common::PixelFormat PFNC_Coord3D_C12p = 0x010C00DA;
const common::PixelFormat PFNC_Coord3D_C16 = 0x011000B8;

std::string pixelFormatToString(common::PixelFormat pixelFormat);
