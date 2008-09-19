/*
 * glfunctions.h
 *
 * Copyright (C) 2007 by Universitaet Stuttgart (VIS). Alle Rechte vorbehalten.
 */

#ifndef VISLIB_GLFUNCTIONS_H_INCLUDED
#define VISLIB_GLFUNCTIONS_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */
#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(push, off)
#endif /* defined(_WIN32) && defined(_MANAGED) */


#include "vislib/VersionNumber.h"


namespace vislib {
namespace graphics {
namespace gl {

    /**
     * Enables or disables VSync. This method can only be used when there is a
     * current open gl context.
     *
     * @param enabled Flag whether to enable (a value of 'true') or disable (a
     *                value of 'false') VSync.
     *
     * @return true if the operation completed successfully, false otherwise.
     */
    bool EnableVSync(bool enable = true);

    /**
     * Disables VSync. This is identically to calling 'EnableVSync(false)'.
     * This method can only be used when there is a current open gl context.
     *
     * @return true if the operation completed successfully, false otherwise.
     */
    inline bool DisableVSync(void) {
        // Note: not ordered correctly because we would need a forward 
        //       declaration
        return EnableVSync(false);
    }

    /**
     * Answer the open gl version number. Note that the version number uses 
     * only the first two or the first three number elements (where the third
     * element 'buildNumber' is used as release number).
     *
     * @return The open gl version number.
     */
    const VersionNumber& GLVersion(void);

    /**
     * Answer whether VSync is enabled or not. This method can only be used 
     * when there is a current open gl context.
     *
     * @param error Pointer to an bool receiving whether the function resulted
     *              in an error (returning 'true' and setting 'error' to
     *              'true').
     *
     * @return 'true' if VSync is enabled, 'false' if not.
     */
    bool IsVSyncEnabled(bool *error = NULL);

} /* end namespace gl */
} /* end namespace graphics */
} /* end namespace vislib */

#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(pop)
#endif /* defined(_WIN32) && defined(_MANAGED) */
#endif /* VISLIB_GLFUNCTIONS_H_INCLUDED */
