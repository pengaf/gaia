#include "utility.h"
#include <type_traits>

BEGIN_GAIA

template<typename Enum_t>
struct is_bitmask_enum : std::false_type {};

#define ENABLE_BITMASK(Enum_t) template<> struct is_bitmask_enum<Enum_t> : std::true_type {};

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t> operator|(Enum_t a, Enum_t b) noexcept
{
    return static_cast<Enum_t>(static_cast<std::underlying_type_t<Enum_t>>(a) | static_cast<std::underlying_type_t<Enum_t>>(b));
}

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t> operator&(Enum_t a, Enum_t b) noexcept
{
    return static_cast<Enum_t>(static_cast<std::underlying_type_t<Enum_t>>(a) & static_cast<std::underlying_type_t<Enum_t>>(b));
}

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t> operator^(Enum_t a, Enum_t b) noexcept
{
    return static_cast<Enum_t>(static_cast<std::underlying_type_t<Enum_t>>(a) ^ static_cast<std::underlying_type_t<Enum_t>>(b));
}

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t> operator~(Enum_t val) noexcept
{
    return static_cast<Enum_t>(~static_cast<std::underlying_type_t<Enum_t>>(val));
}

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t&> operator|=(Enum_t& a, Enum_t b) noexcept 
{
    return a = a | b; 
}

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t&> operator&=(Enum_t& a, Enum_t b) noexcept 
{
    return a = a & b; 
}

template<typename Enum_t>
constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t&> operator^=(Enum_t& a, Enum_t b) noexcept 
{
    return a = a ^ b; 
}

//template<typename Enum_t>
//constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, bool>
//HasFlag(Enum_t src, Enum_t flag) noexcept
//{
//    return (static_cast<std::underlying_type_t<Enum_t>>(src) & static_cast<std::underlying_type_t<Enum_t>>(flag)) != 0;
//}
//

//template<typename Enum_t>
//constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t>
//AddFlag(Enum_t src, Enum_t flag) noexcept { return src | flag; }
//

//template<typename Enum_t>
//constexpr std::enable_if_t<is_bitmask_enum<Enum_t>::value, Enum_t>
//RemoveFlag(Enum_t src, Enum_t flag) noexcept { return src & ~flag; }

END_GAIA