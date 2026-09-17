#pragma once

#include <KDecoration2/DecoratedClient>
#include <KDecoration2/Decoration>

#if defined(BREEZE10_HAVE_KDECORATION3)
#include <KDecoration3/DecoratedClient>
#include <KDecoration3/Decoration>
#endif

namespace Breeze
{
#if defined(BREEZE10_HAVE_KDECORATION3)
inline KDecoration3::DecoratedClient *decorationClient(KDecoration3::Decoration *decoration)
{
    return decoration ? decoration->client() : nullptr;
}
#else
inline KDecoration2::DecoratedClient *decorationClient(KDecoration2::Decoration *decoration)
{
    return decoration ? decoration->client() : nullptr;
}
#endif
}
