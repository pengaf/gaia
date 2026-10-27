#pragma once
#include <cstdint>
#include <cfloat>
#include "pafcore/utility.h"

#define BEGIN_GAIA namespace gaia {
#define END_GAIA }

#define GAIA_ARG(T, name, ...)				                            \
public:                                                                 \
    inline auto& name(const T& new_##name)                              \
    {                                                                   \
        this->m_##name = new_##name;								    \
        return *this;                                                   \
    }                                                                   \
    inline auto& name(T&& new_##name)                                   \
    {                                                                   \
        this->m_##name = std::move(new_##name);                         \
        return *this;                                                   \
    }                                                                   \
    inline const T& name() const noexcept                               \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
    inline T& name() noexcept                                           \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
protected:                                                              \
    T m_##name{__VA_ARGS__}


#define GAIA_ARG_VAL(T, name, ...)				                        \
public:                                                                 \
    inline auto& name(T new_##name)                                     \
    {                                                                   \
        this->m_##name = std::move(new_##name);						    \
        return *this;                                                   \
    }                                                                   \
    inline T name() const                                               \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
protected:                                                              \
    T m_##name{__VA_ARGS__}


#define GAIA_ARG_PTR(T, name, ...)				                        \
public:                                                                 \
    inline auto& name(T* new_##name)                                    \
    {                                                                   \
        this->m_##name = new_##name;								    \
        return *this;                                                   \
    }                                                                   \
    inline const T* name() const noexcept                               \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
    inline T* name() noexcept                                           \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
protected:                                                              \
    T* m_##name{__VA_ARGS__}


#define GAIA_ARG_RO(T, name, ...)					                    \
public:                                                                 \
    inline const T& name() const noexcept                               \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
 protected:                                                             \
  T m_##name{__VA_ARGS__}


#define GAIA_ARG_RO_VAL(T, name, ...)					                \
public:                                                                 \
    inline T name() const                                               \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
 protected:                                                             \
  T m_##name{__VA_ARGS__}


#define GAIA_ARG_RO_PTR(T, name, ...)					                \
public:                                                                 \
    inline const T* name() const noexcept                               \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
    inline T* name() noexcept                                           \
    {                                                                   \
        return this->m_##name;                                          \
    }                                                                   \
protected:                                                              \
    T* m_##name{__VA_ARGS__}


#ifndef GAIA_PLATFORM_WEB
#define GAIA_PLATFORM_WEB				1
#endif

#ifndef GAIA_PLATFORM_WIN32
#define GAIA_PLATFORM_WIN32				2
#endif

#ifndef GAIA_PLATFORM_ANDROID
#define GAIA_PLATFORM_ANDROID			3
#endif

#ifndef GAIA_PLATFORM_WAYLAND
#define GAIA_PLATFORM_WAYLAND			4
#endif

#ifndef GAIA_PLATFORM_XCB
#define GAIA_PLATFORM_XCB				5
#endif

#ifndef GAIA_PLATFORM
#define GAIA_PLATFORM					GAIA_PLATFORM_WIN32
#endif

#define GAIA_HIGH_PRECISION_POSITION_2D		0
#define GAIA_HIGH_PRECISION_POSITION_3D		0

#define GAIA_ASSERT PAF_ASSERT
