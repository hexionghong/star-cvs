/*  This file is part of the Vc library.

    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    Vc is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Vc is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Vc.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SSE_CASTS_H
#define SSE_CASTS_H

#include "intrinsics.h"
#include "types.h"

namespace Vc
{
namespace SSE
{
    template<typename To, typename From> static inline To mm128_reinterpret_cast(From v) CONST;
    template<typename To, typename From> static inline To mm128_reinterpret_cast(From v) { return v; }
    template<> inline _M128I mm128_reinterpret_cast<_M128I, _M128 >(_M128  v) CONST;
    template<> inline _M128I mm128_reinterpret_cast<_M128I, _M128D>(_M128D v) CONST;
    template<> inline _M128  mm128_reinterpret_cast<_M128 , _M128D>(_M128D v) CONST;
    template<> inline _M128  mm128_reinterpret_cast<_M128 , _M128I>(_M128I v) CONST;
    template<> inline _M128D mm128_reinterpret_cast<_M128D, _M128I>(_M128I v) CONST;
    template<> inline _M128D mm128_reinterpret_cast<_M128D, _M128 >(_M128  v) CONST;
    template<> inline _M128I mm128_reinterpret_cast<_M128I, _M128 >(_M128  v) { return _mm_castps_si128(v); }
    template<> inline _M128I mm128_reinterpret_cast<_M128I, _M128D>(_M128D v) { return _mm_castpd_si128(v); }
    template<> inline _M128  mm128_reinterpret_cast<_M128 , _M128D>(_M128D v) { return _mm_castpd_ps(v);    }
    template<> inline _M128  mm128_reinterpret_cast<_M128 , _M128I>(_M128I v) { return _mm_castsi128_ps(v); }
    template<> inline _M128D mm128_reinterpret_cast<_M128D, _M128I>(_M128I v) { return _mm_castsi128_pd(v); }
    template<> inline _M128D mm128_reinterpret_cast<_M128D, _M128 >(_M128  v) { return _mm_castps_pd(v);    }

    template<typename From, typename To> struct StaticCastHelper {};
    template<> struct StaticCastHelper<float       , int         > { static _M128I cast(const _M128  &v) { return _mm_cvttps_epi32(v); } };
    template<> struct StaticCastHelper<double      , int         > { static _M128I cast(const _M128D &v) { return _mm_cvttpd_epi32(v); } };
    template<> struct StaticCastHelper<int         , int         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<unsigned int, int         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<float       , unsigned int> { static _M128I cast(const _M128  &v) { return _mm_cvttps_epi32(v); } };
    template<> struct StaticCastHelper<double      , unsigned int> { static _M128I cast(const _M128D &v) { return _mm_cvttpd_epi32(v); } };
    template<> struct StaticCastHelper<int         , unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<unsigned int, unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<float       , float       > { static _M128  cast(const _M128  &v) { return v; } };
    template<> struct StaticCastHelper<double      , float       > { static _M128  cast(const _M128D &v) { return _mm_cvtpd_ps(v); } };
    template<> struct StaticCastHelper<int         , float       > { static _M128  cast(const _M128I &v) { return _mm_cvtepi32_ps(v); } };
    template<> struct StaticCastHelper<unsigned int, float       > { static _M128  cast(const _M128I &v) { return _mm_cvtepi32_ps(v); } };
    template<> struct StaticCastHelper<float       , double      > { static _M128D cast(const _M128  &v) { return _mm_cvtps_pd(v); } };
    template<> struct StaticCastHelper<double      , double      > { static _M128D cast(const _M128D &v) { return v; } };
    template<> struct StaticCastHelper<int         , double      > { static _M128D cast(const _M128I &v) { return _mm_cvtepi32_pd(v); } };
    template<> struct StaticCastHelper<unsigned int, double      > { static _M128D cast(const _M128I &v) { return _mm_cvtepi32_pd(v); } };

    template<> struct StaticCastHelper<unsigned short, float8        > { static  M256  cast(const _M128I &v) {
        return M256(_mm_cvtepi32_ps(_mm_unpacklo_epi16(v, _mm_setzero_si128())),
                    _mm_cvtepi32_ps(_mm_unpackhi_epi16(v, _mm_setzero_si128())));
    } };
//X         template<> struct StaticCastHelper<short         , float8        > { static  M256  cast(const _M128I &v) {
//X             const _M128I neg = _mm_cmplt_epi16(v, _mm_setzero_si128());
//X             return M256(_mm_cvtepi32_ps(_mm_unpacklo_epi16(v, neg)),
//X                         _mm_cvtepi32_ps(_mm_unpackhi_epi16(v, neg)));
//X         } };
    template<> struct StaticCastHelper<float8        , short         > { static _M128I cast(const  M256  &v) { return _mm_packs_epi32(_mm_cvttps_epi32(v[0]), _mm_cvttps_epi32(v[1])); } };
    template<> struct StaticCastHelper<float8        , unsigned short> { static _M128I cast(const  M256  &v) { return _mm_packs_epi32(_mm_cvttps_epi32(v[0]), _mm_cvttps_epi32(v[1])); } };

    template<> struct StaticCastHelper<float         , short         > { static _M128I cast(const _M128  &v) { return _mm_packs_epi32(_mm_cvttps_epi32(v), _mm_setzero_si128()); } };
    template<> struct StaticCastHelper<short         , short         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<unsigned short, short         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<float         , unsigned short> { static _M128I cast(const _M128  &v) { return _mm_packs_epi32(_mm_cvttps_epi32(v), _mm_setzero_si128()); } };
    template<> struct StaticCastHelper<short         , unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct StaticCastHelper<unsigned short, unsigned short> { static _M128I cast(const _M128I &v) { return v; } };

    template<typename From, typename To> struct ReinterpretCastHelper {};
    template<> struct ReinterpretCastHelper<float       , int         > { static _M128I cast(const _M128  &v) { return _mm_castps_si128(v); } };
    template<> struct ReinterpretCastHelper<double      , int         > { static _M128I cast(const _M128D &v) { return _mm_castpd_si128(v); } };
    template<> struct ReinterpretCastHelper<int         , int         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<unsigned int, int         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<float       , unsigned int> { static _M128I cast(const _M128  &v) { return _mm_castps_si128(v); } };
    template<> struct ReinterpretCastHelper<double      , unsigned int> { static _M128I cast(const _M128D &v) { return _mm_castpd_si128(v); } };
    template<> struct ReinterpretCastHelper<int         , unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<unsigned int, unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<float       , float       > { static _M128  cast(const _M128  &v) { return v; } };
    template<> struct ReinterpretCastHelper<double      , float       > { static _M128  cast(const _M128D &v) { return _mm_castpd_ps(v); } };
    template<> struct ReinterpretCastHelper<int         , float       > { static _M128  cast(const _M128I &v) { return _mm_castsi128_ps(v); } };
    template<> struct ReinterpretCastHelper<unsigned int, float       > { static _M128  cast(const _M128I &v) { return _mm_castsi128_ps(v); } };
    template<> struct ReinterpretCastHelper<float       , double      > { static _M128D cast(const _M128  &v) { return _mm_castps_pd(v); } };
    template<> struct ReinterpretCastHelper<double      , double      > { static _M128D cast(const _M128D &v) { return v; } };
    template<> struct ReinterpretCastHelper<int         , double      > { static _M128D cast(const _M128I &v) { return _mm_castsi128_pd(v); } };
    template<> struct ReinterpretCastHelper<unsigned int, double      > { static _M128D cast(const _M128I &v) { return _mm_castsi128_pd(v); } };

    template<> struct ReinterpretCastHelper<unsigned short, short         > { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<unsigned short, unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<short         , unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
    template<> struct ReinterpretCastHelper<short         , short         > { static _M128I cast(const _M128I &v) { return v; } };
} // namespace SSE
} // namespace Vc

#endif // SSE_CASTS_H
