/*
 * QuadBufferStereoView.cpp
 *
 * Copyright (C) 2010 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "mmcore/view/special/QuadBufferStereoView.h"
#include "mmcore/view/CallRenderViewGL.h"
#include "mmcore/utility/log/Log.h"

using namespace megamol::core;


/*
 * view::special::QuadBufferStereoView::QuadBufferStereoView
 */
view::special::QuadBufferStereoView::QuadBufferStereoView(void)
        : AbstractStereoView(), hasQuadBuffer(false) {
    // Intentionally empty
}


/*
 * view::special::QuadBufferStereoView::~QuadBufferStereoView
 */
view::special::QuadBufferStereoView::~QuadBufferStereoView(void) {
    this->Release();
}


/*
 * view::special::QuadBufferStereoView::Render
 */
void view::special::QuadBufferStereoView::Render(const mmcRenderViewContext& context) {
    CallRenderViewGL *crv = this->getCallRenderView();
    if (crv == NULL) return;
    crv->SetTime(static_cast<float>(context.Time));
    crv->SetInstanceTime(context.InstanceTime);
    crv->SetGpuAffinity(context.GpuAffinity);

    if (this->hasQuadBuffer) {
        thecam::Projection_type proj = this->getProjectionType();
        bool switchEyes = this->getSwitchEyes();

        crv->SetProjection(proj,switchEyes
            ? thecam::Eye::left
            : thecam::Eye::right);
        crv->SetOutputBuffer(GL_BACK_RIGHT, this->getViewportWidth(), this->getViewportHeight());
        ::glDrawBuffer(GL_BACK_RIGHT);
        (*crv)(view::CallRenderViewGL::CALL_RENDER);

        crv->SetProjection(proj, switchEyes
            ? thecam::Eye::right
            : thecam::Eye::left);
        crv->SetOutputBuffer(GL_BACK_LEFT, this->getViewportWidth(), this->getViewportHeight());
        ::glDrawBuffer(GL_BACK_LEFT);
        (*crv)(view::CallRenderViewGL::CALL_RENDER);

        ::glDrawBuffer(GL_BACK); // force overlays and gui on both buffers for less brain-pain

    } else {
        crv->SetOutputBuffer(GL_BACK, this->getViewportWidth(), this->getViewportHeight());
        (*crv)(view::CallRenderViewGL::CALL_RENDER);
    }
}


/*
 * view::special::QuadBufferStereoView::create
 */
bool view::special::QuadBufferStereoView::create(void) {
    GLboolean stereo;
    ::glGetBooleanv(GL_STEREO, &stereo);
    this->hasQuadBuffer = (stereo == GL_TRUE);

    if (this->hasQuadBuffer) {
        megamol::core::utility::log::Log::DefaultLog.WriteMsg(megamol::core::utility::log::Log::LEVEL_INFO,
           "Stereo Buffers supported!\n");
    } else {
        megamol::core::utility::log::Log::DefaultLog.WriteMsg(megamol::core::utility::log::Log::LEVEL_WARN,
            "No Stereo Buffers for you!\n");
    }
    return true;
}


/*
 * view::special::QuadBufferStereoView::release
 */
void view::special::QuadBufferStereoView::release(void) {
    // Intentionally empty
}
