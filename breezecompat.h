#pragma once

#if defined(BREEZE10_HAVE_KDECORATION3)
#include <KDecoration3/DecoratedClient>
#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationButton>
#include <KDecoration3/DecorationButtonGroup>
#include <KDecoration3/DecorationSettings>
#include <KDecoration3/DecorationShadow>
#else
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationButton>
#include <KDecoration2/DecorationButtonGroup>
#include <KDecoration2/DecorationSettings>
#include <KDecoration2/DecorationShadow>
#endif

namespace Breeze
{
#if defined(BREEZE10_HAVE_KDECORATION3)
using DecorationApi = KDecoration3::Decoration;
using DecoratedClientApi = KDecoration3::DecoratedClient;
using DecorationButtonApi = KDecoration3::DecorationButton;
using DecorationButtonGroupApi = KDecoration3::DecorationButtonGroup;
using DecorationButtonTypeApi = KDecoration3::DecorationButtonType;
using DecorationSettingsApi = KDecoration3::DecorationSettings;
using DecorationShadowApi = KDecoration3::DecorationShadow;
using BorderSizeApi = KDecoration3::BorderSize;
using ColorRoleApi = KDecoration3::ColorRole;
using ColorGroupApi = KDecoration3::ColorGroup;
#else
using DecorationApi = KDecoration2::Decoration;
using DecoratedClientApi = KDecoration2::DecoratedClient;
using DecorationButtonApi = KDecoration2::DecorationButton;
using DecorationButtonGroupApi = KDecoration2::DecorationButtonGroup;
using DecorationButtonTypeApi = KDecoration2::DecorationButtonType;
using DecorationSettingsApi = KDecoration2::DecorationSettings;
using DecorationShadowApi = KDecoration2::DecorationShadow;
using BorderSizeApi = KDecoration2::BorderSize;
using ColorRoleApi = KDecoration2::ColorRole;
using ColorGroupApi = KDecoration2::ColorGroup;
#endif

inline DecoratedClientApi *decorationClient(DecorationApi *decoration)
{
    return decoration ? decoration->client() : nullptr;
}
}
