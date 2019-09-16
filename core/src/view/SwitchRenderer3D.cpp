/*
 * Renderer3DModule.cpp
 *
 * Copyright (C) 2009 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "mmcore/view/SwitchRenderer3D.h"
//#include "mmcore/view/CallRender3D.h"
#include "mmcore/param/EnumParam.h"
#include "mmcore/param/ButtonParam.h"
#include "vislib/sys/Log.h"


using namespace megamol::core;


/*
 * view::SwitchRenderer3D::SwitchRenderer3D
 */
view::SwitchRenderer3D::SwitchRenderer3D(void) : view::Renderer3DModule_2(),
        renderer1Slot("renderer1", "Call to the first renderer"),
        renderer2Slot("renderer2", "Call to the second renderer"),
        selectedRendererSlot("selection", "Enum which renderer is active"),
        switchRendererBtnSlot("switch", "Switches to the other renderer"),
        selection(0) {

    this->renderer1Slot.SetCompatibleCall<CallRender3D_2Description>();
    this->MakeSlotAvailable(&this->renderer1Slot);

    this->renderer2Slot.SetCompatibleCall<CallRender3D_2Description>();
    this->MakeSlotAvailable(&this->renderer2Slot);

    param::EnumParam *renSelEnum = new param::EnumParam(1);
    renSelEnum->SetTypePair(1, "Renderer 1");
    renSelEnum->SetTypePair(2, "Renderer 2");
    this->selectedRendererSlot << renSelEnum;
    this->MakeSlotAvailable(&this->selectedRendererSlot);
    this->selectedRendererSlot.ForceSetDirty();

    this->switchRendererBtnSlot << new param::ButtonParam(core::view::Key::KEY_S, core::view::Modifier::ALT);
    this->switchRendererBtnSlot.SetUpdateCallback(&SwitchRenderer3D::onSwitchRenderer);
    this->MakeSlotAvailable(&this->switchRendererBtnSlot);

}


/*
 * view::SwitchRenderer3D::~SwitchRenderer3D
 */
view::SwitchRenderer3D::~SwitchRenderer3D(void) {
    this->Release();
}


/*
 * view::SwitchRenderer3D::create
 */
bool view::SwitchRenderer3D::create(void) {
    // intentionally empty
    return true;
}


/*
 * view::SwitchRenderer3D::release
 */
void view::SwitchRenderer3D::release(void) {
    // intentionally empty
}


/*
 * view::SwitchRenderer3D::GetExtents
 */
bool view::SwitchRenderer3D::GetExtents(CallRender3D_2& call) {
    CallRender3D_2 *cr3d = this->callToRenderer(&call);
    if ((cr3d != NULL) && (*cr3d)(core::view::AbstractCallRender::FnGetExtents)) {
        call = *cr3d;
        return true;
    }
    return false;
}


/*
 * view::SwitchRenderer3D::Render
 */
bool view::SwitchRenderer3D::Render(CallRender3D_2& call) {
    CallRender3D_2 *cr3d = this->callToRenderer(&call);
    if ((cr3d != NULL) && (*cr3d)(core::view::AbstractCallRender::FnRender)) {
        call = *cr3d;
        return true;
    }
    return false;
}


/*
 * view::SwitchRenderer3D::onSwitchRenderer
 */
bool view::SwitchRenderer3D::onSwitchRenderer(param::ParamSlot& param) {
    int newVal = (this->selectedRendererSlot.Param<param::EnumParam>()->Value() == 2) ? 1 : 2;
    this->selectedRendererSlot.Param<param::EnumParam>()->SetValue(newVal);
    vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_INFO + 200,
        "Switching to renderer no. %d\n", newVal);
    return true;
}


/*
 * view::SwitchRenderer3D::whichRenderer
 */
int view::SwitchRenderer3D::whichRenderer(void) {
    if (this->selectedRendererSlot.IsDirty()) {
        this->selection = this->selectedRendererSlot.Param<param::EnumParam>()->Value();
        this->selectedRendererSlot.ResetDirty();

        if (this->selection == 1) {
            if (this->renderer1Slot.CallAs<CallRender3D_2>() == NULL) {
                if (this->renderer2Slot.CallAs<CallRender3D_2>() == NULL) {
                    this->selection = 0;
                } else {
                    this->selection = 2;
                }
            }
        } else if (this->selection == 2) {
            if (this->renderer2Slot.CallAs<CallRender3D_2>() == NULL) {
                if (this->renderer1Slot.CallAs<CallRender3D_2>() == NULL) {
                    this->selection = 0;
                } else {
                    this->selection = 1;
                }
            }
        }

    }

    return this->selection;
}


/*
 * view::SwitchRenderer3D::callToRenderer
 */
view::CallRender3D_2 *view::SwitchRenderer3D::callToRenderer(view::CallRender3D_2 *src) {
    ASSERT(src != NULL);
    view::CallRender3D_2 *c = NULL;
    switch (this->whichRenderer()) {
        case 1:
            c = this->renderer1Slot.CallAs<CallRender3D_2>();
            break;
        case 2:
            c = this->renderer2Slot.CallAs<CallRender3D_2>();
            break;
    }
    if (c != NULL) {
        *c = *src;
    }
    return c;
}
