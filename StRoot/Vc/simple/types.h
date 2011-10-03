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

#ifndef VC_SIMPLE_TYPES_H
#define VC_SIMPLE_TYPES_H

namespace Vc
{
    namespace Simple
    {
        class VectorAlignedBase {};
        template<typename T> class Vector;
        template<typename T, typename Parent> struct VectorBase;
        template<typename T> class _Memory;

        namespace VectorSpecialInitializerZero { enum ZEnum { Zero }; }
        namespace VectorSpecialInitializerOne { enum OEnum { One }; }
        namespace VectorSpecialInitializerRandom { enum REnum { Random }; }
        namespace VectorSpecialInitializerIndexesFromZero { enum IEnum { IndexesFromZero }; }
    } // namespace Simple
} // namespace Vc

#endif // VC_SIMPLE_TYPES_H
