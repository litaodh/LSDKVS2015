// Copyright 2019 SICK AG. All rights reserved.
#include "PFNC.h"

std::string pixelFormatToString(common::PixelFormat pixelFormat)
{
  switch (pixelFormat)
  {
  case PFNC_Mono8: return "Mono8";
  case PFNC_Mono12p: return "Mono12p";
  case PFNC_Mono16: return "Mono16";
  case PFNC_Coord3D_C8: return "Coord3D_C8";
  case PFNC_Coord3D_C12p: return "Coord3D_C12p";
  case PFNC_Coord3D_C16: return "Coord3D_C16";
  default: return "Unknown";
  }
}
