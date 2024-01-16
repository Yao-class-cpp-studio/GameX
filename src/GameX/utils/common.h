#pragma once
#include <grassland/grassland.h>

#include <set>

#define GAMEX_DECLARE_CLASS(ClassName)             \
  class ClassName;                                 \
  typedef std::unique_ptr<ClassName> U##ClassName; \
  typedef ClassName *P##ClassName;

#define GAMEX_CLASS(ClassName)                     \
  class ClassName;                                 \
  typedef std::unique_ptr<ClassName> U##ClassName; \
  typedef ClassName *P##ClassName;                 \
  class ClassName
