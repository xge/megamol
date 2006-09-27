/*
 * ShallowRectangle.h  27.09.2006 (mueller)
 *
 * Copyright (C) 2006 by Universitaet Stuttgart (VIS). Alle Rechte vorbehalten.
 */

#ifndef VISLIB_SHALLOWRECTANGLE_H_INCLUDED
#define VISLIB_SHALLOWRECTANGLE_H_INCLUDED
#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */


#include "vislib/AbstractRectangle.h"


namespace vislib {
namespace math {


    /**
     * This class represents a shallow rectangle, that uses memory provided
     * by the caller.
     */
    template<class T> class ShallowRectangle 
            : public AbstractRectangle<T, T *> {

    public:

        /**
         * Construct a rectangle from an array holding its bounds. The array
         * 'bounds' holds in this order to following borders of the rectangle:
         * left, bottom, right, top.
         *
         * @return The left, bottom, right and top border of the rectangle in
         *         a consecutive order.
         */
        explicit inline ShallowRectangle(const T *bounds) {
            ::memcpy(this->bounds, bounds, 4 * sizeof(T));
        }

        /**
         * Copy ctor.
         *
         * @param rhs The object to clone.
         */
        inline ShallowRectangle(const ShallowRectangle& rhs) {
            ::memcpy(this->bounds, rhs.bounds, 4 * sizeof(T));
        }

        /** Dtor. */
        ~ShallowRectangle(void);

        /**
         * Assigment operator. This operator never creates an alias, even for
         * shallow rectangles!
         *
         * @param rhs The right hand side operand.
         *
         * @return *this.
         */
        inline ShallowRectangle& operator =(const ShallowRectangle& rhs) {
            AbstractRectangle<T, T[4]>::operator =(rhs);
            return *this;
        }

        /**
         * Assigment operator. This operator never creates an alias, even for
         * shallow rectangles!
         *
         * This assignment allows for arbitrary rectangle to rectangle
         * conversions.
         *
         * @param rhs The right hand side operand.
         *
         * @return *this.
         */
        template<class Tp, class Sp>
        inline ShallowRectangle& operator =(
                const AbstractRectangle<Tp, Sp>& rhs) {
            AbstractRectangle<T, T[4]>::operator =(rhs);
            return *this;
        }
    };


/*
 * vislib::math::ShallowRectangle<T>::~ShallowRectangle
 */
template<class T> ShallowRectangle<T>::~ShallowRectangle(void) {
}


} /* end namespace math */
} /* end namespace vislib */

#endif /* VISLIB_SHALLOWRECTANGLE_H_INCLUDED */
